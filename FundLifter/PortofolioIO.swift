//
//  PortofolioIO.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/29/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation


public struct PortfolioIO {

  private static func initializeEmptyPortfolios() {
    // Initialize all portfolios
    AppDataObservable._portfolios[FLConstants.PORTFOLIO_ARCS] = [D_FundInfo]()
    for t in D_FundInfo.TYPES {
      AppDataObservable._portfolios[t] = [D_FundInfo]()
    }
  }
  
  public static func read() -> String {
    PortfolioIO.initializeEmptyPortfolios()

    // Populate with what we have on disk
    let (exists, data1, errorStr) = FLBinaryIOUtils.readFile(url: FLConstants.urlPortfolio())
    if !exists {
      return "No file"
    }
    precondition(data1 != nil || errorStr != nil, "One of data or errorStr expected")
    if let e=errorStr {
      return e
    }
    
    let data = data1!
    
    print("Portfolio binar data and decoding process")
    print(FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: data.count))
    
    var cindex = 0
    var portfolioCount = 0
    while cindex < data.count {
      var type = ""
      (cindex, type) = FLBinaryIOUtils.readUTFSwift(data: data, sindex: cindex)
      print("type: \(type)")
      
      var count = 0
      (cindex, count) = FLBinaryIOUtils.readIntSwift(data: data, sindex: cindex)
      for _ in 0..<count {
        var typeAndName = ""
        (cindex, typeAndName) = FLBinaryIOUtils.readUTFSwift(data: data, sindex: cindex)
        print("...typeAndName: \(typeAndName)")
        guard let fund = AppDataObservable._typeAndName2Fund[typeAndName] else {
          initializeEmptyPortfolios()
          logFileAppend(s: "*** Portfolio error, fund not found: \(typeAndName)")
          return "Error(no fund)"
        }
        AppDataObservable._portfolios[type]!.append(fund)
        portfolioCount += 1
      }
    }
    return "\(portfolioCount)"
  }
  
  public static func write() -> String {
    var data = Data()
    
    for t in AppDataObservable._portfolios.keys {
      data.append(FLBinaryIOUtils.writeUTFSwift(string: t))
      print(FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: data.count))

      let funds = AppDataObservable._portfolios[t]!
      data.append(FLBinaryIOUtils.writeIntSwift(integer: funds.count))
      print(FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: data.count))

      for f in funds {
        data.append(FLBinaryIOUtils.writeUTFSwift(string: f.typeAndName))
        print(FLBinaryIOUtils.getHexDump(data: data, sindex: 0, eindex: data.count))
      }
    }
    
    do {
      try data.write(to: FLConstants.urlPortfolio())
    } catch {
      return "\(error)"
    }
    return "\(AppDataObservable._portfolios.keys.count)"
  }
}



