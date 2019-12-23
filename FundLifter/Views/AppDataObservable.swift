import SwiftUI
import Combine
import Firebase
import Zip

public class AppDataObservable: ObservableObject {
  @Published var message = "Initializing"
  @Published var fundDBCreationTime = "No Fund DB File"

  @Published var portfolios = [DP4WModel]()
  @Published var dp4ModelHM = [String: DP4WModel]()  // Funds as well as portfolios
 
  public static var _allFunds = [D_FundInfo]()
  public static var _type2Funds = [String: [D_FundInfo]]()
  public static var _typeAndName2Fund = [String: D_FundInfo]()
  public static var _portfolios = [String: [D_FundInfo]]()

  public func initialize() {
    DispatchQueue.global(qos: .userInteractive).async { [weak self] in
      guard let self=self else {
        return
      }
      
      DispatchQueue.main.async { [weak self] in
        self?.fundDBCreationTime = DBUpdateObservable.getFundDBCreationTime()
      }
      
      // Read DB file data
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
          self?.message = s
        }
        return
      }

      // Initialize data structures
      let data = data1!
      // print("Data read: \(data.count)")
      
      let _ = FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: 1024)
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
      
      let portfoliosReadStr = PortfolioIO.read()

      DispatchQueue.main.async { [weak self] in
        // Create DP4W for all funds
        for fi in AppDataObservable._allFunds {
          self?.dp4ModelHM[fi.typeAndName] = DataModelsCalculator.getDP4WModelForFund(fund: fi)
        }
        
        // Initialize status message on # funds & portfolios
        self?.message = "Funds: \(AppDataObservable._allFunds.count), Portfolios: \(portfoliosReadStr)"
        
        // Display the portfolios
        for p in AppDataObservable._portfolios.keys.sorted() {
          let dp4ModelP = DataModelsCalculator.getDP4WModelForPortfolio(name: p, funds: AppDataObservable._portfolios[p]!)
          self?.portfolios.append(dp4ModelP)
          self?.dp4ModelHM[p] = dp4ModelP
//          print("\(p): \(self?.dp4ModelHM[p]!.dpWs.count)")
        }
      }
    }
  }
}
