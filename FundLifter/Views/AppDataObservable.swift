import SwiftUI
import Combine
import Firebase
import Zip

public class AppDataObservable: ObservableObject {
  @Published var pubMessage = "Initializing"
  @Published var pubFundDBCreationTime = "No Fund DB File"
  @Published var pubPortfolios = [DP4WModel]()
  @Published var pubDP4ModelHM = [String: DP4WModel]()  // Funds as well as portfolios
  @Published var pubType2DP4Funds = [String: [DP4WModel]]()
  @Published var pubPortfolio2DP4Funds = [String: [DP4WModel]]()

  public static var _allFundsDP4 = [DP4WModel]()
  public static var _allFunds = [D_FundInfo]()
  public static var _type2Funds = [String: [D_FundInfo]]()
  public static var _typeAndName2Fund = [String: D_FundInfo]()
  public static var _portfolios = [String: [D_FundInfo]]()

  private var isInitialized = false
  public func initialize(forceReinitialization: Bool = false) {
    if isInitialized && !forceReinitialization {
      return
    }
    isInitialized = true
    
    DispatchQueue.global(qos: .userInteractive).async { [weak self] in
      guard let self=self else {
        return
      }
      
      DispatchQueue.main.async { [weak self] in
        self?.pubFundDBCreationTime = DBUpdateObservable.getFundDBCreationTime()
      }
            
      // Read DB file data
      print("Reading DB file at: \(FLConstants.urlDB())")
      print("File originating from GCS: \(FLConstants.DB_FILENAME_GCS)")
      let (exists, data1, errorStr) = FLBinaryIOUtils.readFile(url: FLConstants.urlDB())
      if data1 == nil {
        var s = ""
        if !exists {
          s = "*** DB File does not exist"
          print(s)
          logFileAppend(s: s)
          s = "No file"  // For the UI
        } else if errorStr == nil {
          s = errorStr!
        }
        DispatchQueue.main.async { [weak self] in
          self?.pubMessage = s
        }
        return
      }

      // Initialize data structures
      let data = data1!
      // print("Data read: \(data.count)")
      
      let _ = getHexDump(data: data, sindex: 0, eindex: 1024)
      // print("Hex dump\n\(s)")
      
      var cindex = 0
      while cindex < data.count {
        let fundInfo = D_FundInfo()
        
        // var startTag = ""
        (cindex, _) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("startTag: \(startTag) |\(cindex)") }
        
        // var recordLength = 0
        (cindex, _) = FLBinaryIOUtils.readInt32Java(data: data, sindex: cindex)
        // recordLength += 6  // Record length in file does not include startTag.  Adding its length to match cindex at end
        // print("recordLength: \(recordLength) |\(cindex)")
        
        // var notUsed = false
        (cindex, _) = FLBinaryIOUtils.readBoolJava(data: data, sindex: cindex)
        // print("notUsed: \(notUsed) |\(cindex)")
        
        var url = ""
        (cindex, url) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("url: \(url) |\(cindex)")
        fundInfo._url = url
        
        var isValid = false
        (cindex, isValid) = FLBinaryIOUtils.readBoolJava(data: data, sindex: cindex)
        // print("isValid: \(isValid) |\(cindex)")
        fundInfo._isValid = isValid
        
        var errorCode = 0
        (cindex, errorCode) = FLBinaryIOUtils.readInt32Java(data: data, sindex: cindex)
        // print("errorCode: \(errorCode) |\(cindex)")
        fundInfo._errorCode = errorCode
        
        var lastExtractInfo = ""
        (cindex, lastExtractInfo) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("lastExtractInfo: \(lastExtractInfo) |\(cindex)")
        fundInfo._lastExtractInfo = lastExtractInfo
        
        var type = ""
        (cindex, type) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("type: \(type) |\(cindex)")
        fundInfo._type = type
        
        var nameMS = ""
        (cindex, nameMS) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("nameMS: \(nameMS) |\(cindex)")
        fundInfo._nameMS = nameMS
        
        var nameOrig = ""
        (cindex, nameOrig) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("nameOrig: \(nameOrig) |\(cindex)")
        fundInfo._nameOrig = nameOrig
        
        var dateUpdated = ""
        (cindex, dateUpdated) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("dateUpdated: \(dateUpdated) |\(cindex)")
        fundInfo._dateUpdated = dateUpdated
        
        var dateUpdateAttempted = ""
        (cindex, dateUpdateAttempted) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("dateUpdateAttempted: \(dateUpdateAttempted) |\(cindex)")
        fundInfo._dateUpdateAttempted = dateUpdateAttempted
        
        var msRating = 0
        (cindex, msRating) = FLBinaryIOUtils.readInt32Java(data: data, sindex: cindex)
        // print("msRating: \(msRating) |\(cindex)")
        fundInfo._msRating = msRating
        
        var ppmNumber = ""
        (cindex, ppmNumber) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("ppmNumber: \(ppmNumber) |\(cindex)")
        fundInfo._ppmNumber = ppmNumber
        
        var categoryName = ""
        (cindex, categoryName) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("categoryName: \(categoryName) |\(cindex)")
        fundInfo._categoryName = categoryName
        
        var indexName = ""
        (cindex, indexName) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("indexName: \(indexName) |\(cindex)")
        fundInfo._indexName = indexName
        
        var currencyName = ""
        (cindex, currencyName) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
        // print("currencyName: \(currencyName) |\(cindex)")
        fundInfo._currencyName = currencyName
        
        var dpDayCount = 0
        while true {  // Existance of DP end tag creates break
          var tag = ""
          (cindex, tag) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
          // print("tag: \(tag) |\(cindex)")
          
          if tag == "DPYS" {
            var fundDPYear = D_FundDPYear()
            var year = 0
            (cindex, year) = FLBinaryIOUtils.readInt16Java(data: data, sindex: cindex)
            // print("year: \(year) |\(cindex)")
            fundDPYear._year = year
            
            var fund: Double? = nil
            (cindex, fund) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("fund: \(fund) |\(cindex)")
            fundDPYear._yieldFund = fund
            
            var category: Double? = nil
            (cindex, category) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("category: \(category) |\(cindex)")
            fundDPYear._yieldCategory = category
            
            var index: Double? = nil
            (cindex, index) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("index: \(index) |\(cindex)")
            fundDPYear._yieldIndex = index
            
            fundInfo._dpYears.append(fundDPYear)
          } else if tag == "DPDS" {
            var fundDPDay = D_FundDPDay()
            
            var dateYYMMDD = ""
            (cindex, dateYYMMDD) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
            // print("dateYYMMDD: \(dateYYMMDD) |\(cindex)")
            fundDPDay._dateYYMMDD = dateYYMMDD
            
            var dateYYMMDD_Actual = ""
            (cindex, dateYYMMDD_Actual) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
            // print("dateYYMMDD_Actual: \(dateYYMMDD_Actual) |\(cindex)")
            fundDPDay._dateYYMMDD_Actual = dateYYMMDD_Actual
            
            var currency = ""
            (cindex, currency) = FLBinaryIOUtils.readUTFJava(data: data, sindex: cindex)
            // print("currency: \(currency) |\(cindex)")
            fundDPDay._currency = currency
            
            var r1d: Double? = nil
            (cindex, r1d) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r1d: \(r1d) |\(cindex)")
            fundDPDay._r1d = r1d
            
            var r1w: Double? = nil
            (cindex, r1w) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r1w: \(r1w) |\(cindex)")
            fundDPDay._r1w = r1w
            
            var r1m: Double? = nil
            (cindex, r1m) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r1m: \(r1m) |\(cindex)")
            fundDPDay._r1m = r1m
            
            var r3m: Double? = nil
            (cindex, r3m) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r3m: \(r3m) |\(cindex)")
            fundDPDay._r3m = r3m
            
            var r6m: Double? = nil
            (cindex, r6m) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r6m: \(r6m) |\(cindex)")
            fundDPDay._r6m = r6m
            
            var r1y: Double? = nil
            (cindex, r1y) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r1y: \(r1y) |\(cindex)")
            fundDPDay._r1y = r1y
            
            var r3y: Double? = nil
            (cindex, r3y) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r3y: \(r3y) |\(cindex)")
            fundDPDay._r3y = r3y
            
            var r5y: Double? = nil
            (cindex, r5y) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r5y: \(r5y) |\(cindex)")
            fundDPDay._r5y = r5y
            
            var r10y: Double? = nil
            (cindex, r10y) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("r10y: \(r10y) |\(cindex)")
            fundDPDay._r10y = r10y
            
            var rYTDFund: Double? = nil
            (cindex, rYTDFund) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("rYTDFund: \(rYTDFund) |\(cindex)")
            fundDPDay._rYTDFund = rYTDFund
            
            var rYTDCategory: Double? = nil
            (cindex, rYTDCategory) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("rYTDCategory: \(rYTDCategory) |\(cindex)")
            fundDPDay._rYTDCategory = rYTDCategory
            
            var rYTDIndex: Double? = nil
            (cindex, rYTDIndex) = FLBinaryIOUtils.readFloatAsDoubleJava(data: data, sindex: cindex)
            // print("rYTDIndex: \(rYTDIndex) |\(cindex)")
            fundDPDay._rYTDIndex = rYTDIndex
            
            fundInfo._dpDays.append(fundDPDay)
            dpDayCount += 1
          } else if tag == "DFIE" {
            break
          }
        }  // End of while, retrieving DPYear and DPDay

        
        // Validate DPD date sequence, in decending order starting from lowest index
        // This takes like a minute!
        // Run it now and again, but it should be validated by server already
//        var cdate = dateLastFridayAsYYMMDD()
//        for dpd in fundInfo._dpDays {
//          if !dateIsFriday(dateYYMMDD: dpd._dateYYMMDD) {
//            fatalError("*** For \(fundInfo.typeAndName), dpd was not a friday: \(dpd._dateYYMMDD)")
//          }
//          if dpd._dateYYMMDD > cdate {
//            fatalError("*** For \(fundInfo.typeAndName), dpd was not strictly decending: \(dpd._dateYYMMDD)")
//          }
//          cdate = dpd._dateYYMMDD
//          cdate = dateLastFridayAsYYMMDD(startAt: dateFromYYMMDD(dateYYMMDD: cdate), inclusive: false)
//        }
//        print("Did fund: \(fundInfo.typeAndName)")
        
        if let _ = AppDataObservable._typeAndName2Fund[fundInfo.typeAndName] {
          let s = "Duplicate fund found in database: \(fundInfo.typeAndName)"
          logFileAppend(s: s)
          fatalError(s)
        }
        print("Adding typeAndName: \(fundInfo.typeAndName)")
        AppDataObservable._typeAndName2Fund[fundInfo.typeAndName] = fundInfo
        AppDataObservable._allFunds.append(fundInfo)
        if !AppDataObservable._type2Funds.keys.contains(fundInfo._type) {
          AppDataObservable._type2Funds[fundInfo._type] = [D_FundInfo]()
        }
        AppDataObservable._type2Funds[fundInfo._type]!.append(fundInfo)
      } // End of while, retrieving FundInfo
      
      AppDataObservable._allFunds.sort { $0.typeAndName < $1.typeAndName }
      print("Done reading all funds, total: \(AppDataObservable._allFunds.count)")
      print("   nonNullCounts: \(FLBinaryIOUtils.nonNullCounts), nullCounts: \(FLBinaryIOUtils.nullCounts)")
      
      for fi in AppDataObservable._allFunds {
        let fdm = DataModelsCalculator.getDP4WModelForFund(fund: fi)
        AppDataObservable._allFundsDP4.append(fdm)
      }
      
      // Populates: AppDataObservable._portfolios[type]!.append(fund)
      // let portfoliosReadStr = PortfolioIO.read()

      // GCS way of doing things
      AppDataObservable.initializeEmptyPortfolios()
      CoreIO.gcsRead(fromFile: FLConstants.PORTFOLIO_FILENAME_GCS) {
        
        var message = "Initializing"
        if let error=$1 {
          message = "Error GCS fetching portfolio: \(error)"
        }
        else if let data=$0 {
          // Assign: AppDataObservable._portfolios[type]
          let (error, portfolioCount) = portfolioDecode(data: data)
          if let error=error {
            message = "Error decoding portfolio: \(error)"
          } else {
            message = "Funds: \(AppDataObservable._allFunds.count), Portfolios: \(portfolioCount)"
          }
        }
        
        // All _ datastructures initialize, initialize the @Published ones
        DispatchQueue.main.async { [weak self] in
          // Create DP4W for all funds
          for fi in AppDataObservable._allFundsDP4 {
            self?.pubDP4ModelHM[fi.id] = fi
          }
        
          // Initialize status message on # funds & portfolios
          self?.pubMessage = message
        
          // Display the portfolios
          for p in AppDataObservable._portfolios.keys.sorted() {
            let dp4ModelP = DataModelsCalculator.getDP4WModelForPortfolio(name: p, funds: AppDataObservable._portfolios[p]!)
            self?.pubPortfolios.append(dp4ModelP)
            self?.pubDP4ModelHM[p] = dp4ModelP
            
            if self?.pubPortfolio2DP4Funds.keys.contains(p) == false {
              self?.pubPortfolio2DP4Funds[p] = [DP4WModel]()
            }
            
            for fi in AppDataObservable._portfolios[p]! {
              let dp4w = self!.pubDP4ModelHM[fi.typeAndName]!
              self?.pubPortfolio2DP4Funds[p]!.append(dp4w)
            }
          }
        
          // Type 2 [DP4WModel]
          for fi in AppDataObservable._allFunds {
            if self?.pubType2DP4Funds.keys.contains(fi._type) == false {
              self?.pubType2DP4Funds[fi._type] = [DP4WModel]()
              print("AppDataObservable, adding portfolio id: \(fi._type)")
            }
            guard let a = self?.pubDP4ModelHM[fi.typeAndName] else {
              fatalError("Fund expected to exist: \(fi.typeAndName)")
            }
            self?.pubType2DP4Funds[fi._type]!.append(a)
          }
        }  // End: DispatchQueue.main.async
      }  // End: GCS portfolio file fetched closure
    }  // End: DispatchQueue.global
  }  // End: initialize function
  
  public static func initializeEmptyPortfolios() {
    // Initialize all portfolios
    AppDataObservable._portfolios[FLConstants.PORTFOLIO_ARCS] = [D_FundInfo]()
    for t in D_FundInfo.PORTFOLIO_TYPES {
      AppDataObservable._portfolios[t] = [D_FundInfo]()
    }
  }
}

public func portfolioDecode(data: Data) -> (String?, Int) {
  var cindex = 0
  var portfolioCount = 0
  while cindex < data.count {
    var type = ""
    (cindex, type) = FLBinaryIOUtils.readUTFSwift(data: data, sindex: cindex)
    print("type: \(type)")
    
    var count = 0
    (cindex, count) = FLBinaryIOUtils.readIntSwift(data: data, sindex: cindex)
    var error = false
    for _ in 0..<count {
      var typeAndName = ""
      (cindex, typeAndName) = FLBinaryIOUtils.readUTFSwift(data: data, sindex: cindex)
      print("...typeAndName: \(typeAndName)")
      if let fund = AppDataObservable._typeAndName2Fund[typeAndName] {
        if !error {
          AppDataObservable._portfolios[type]!.append(fund)
        }
      } else {
        logFileAppend(s: "*** Portfolio error, fund not found: \(typeAndName)")
        print("*** portfolioDecode, fund not found: \(typeAndName)")
        AppDataObservable._portfolios[type] = [D_FundInfo]()
        error = true
      }
    }
    portfolioCount += 1
  }
  return (nil, portfolioCount)
}

public func portfolioEncode() -> Data {
  var data = Data()
  
  for t in AppDataObservable._portfolios.keys {
    data.append(FLBinaryIOUtils.writeUTFSwift(string: t))
    // print(getHexDump(data: data, sindex: 0, eindex: data.count))
    
    let funds = AppDataObservable._portfolios[t]!
    data.append(FLBinaryIOUtils.writeIntSwift(integer: funds.count))
    // print(getHexDump(data: data, sindex: 0, eindex: data.count))
    
    for f in funds {
      data.append(FLBinaryIOUtils.writeUTFSwift(string: f.typeAndName))
      // print(FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: data.count))
    }
  }
  return data
}

//func urlDownload() {
//  // Create destination URL
//  let documentsUrl:URL =  FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
//  print("Documents Url: \(documentsUrl)")
//  if FileManager.default.fileExists(atPath: documentsUrl.path) == false {
//    print("...*** Documents URL does not exist")
//    return
//  }
//  print("...documents url existed")
//
//  let destinationFileUrl = documentsUrl.appendingPathComponent("downloadedFile.jpg")
//  print("Destination file Url: \(destinationFileUrl)")
//  do {
//    try FileManager.default.removeItem(at: documentsUrl)
//  } catch {
//    print("*** ERROR")
//  }
//  if true {
//    return
//  }
//
//  // Delete file if it already exists
//  print("Checking if file exists")
//  if FileManager.default.fileExists(atPath: documentsUrl.path) {
//    print("...File existed, will now delete it")
//    do { try FileManager.default.removeItem(at: documentsUrl) } catch {
//      print("...*** Error deleting file: \(error)")
//      return
//    }
//  }
//  print("Ok, file does not exist")
//
//  //Create URL to the source file you want to download
//  let fileURL = URL(string: "https://www.wikipedia.org/")
//
//  let sessionConfig = URLSessionConfiguration.default
//  let session = URLSession(configuration: sessionConfig)
//  let request = URLRequest(url: fileURL!)
//
//  let task = session.downloadTask(with: request) { (tempLocalUrl, response, error) in
//    if let tempLocalUrl = tempLocalUrl, error == nil {
//      if let statusCode = (response as? HTTPURLResponse)?.statusCode {
//        print("Successfully downloaded. Status code: \(statusCode)")
//      }
//
//      do {
//        print("Temporary URL is: \(tempLocalUrl)")
//        print("Will now move the file to: \(destinationFileUrl)")
//        try FileManager.default.moveItem(at: tempLocalUrl, to: destinationFileUrl)
//      } catch (let writeError) {
//        print("...*** Error moving the file: \(writeError)")
//      }
//
//    } else {
//      print("Error took place while downloading a file. Error description: %@", error?.localizedDescription);
//    }
//  }
//  task.resume()
//}
