//
//  DP4ModelPositionCalculator.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/23/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

public struct DP4WModelPosition {
  public let dp4Model: DP4WModel

  // Return fields
  public var eachReturn = [Double]()
  public var totalReturn = 0.0
  public var averageReturn = 0.0
  public var averageReturnDiscounted = 0.0

  // Position fields
  public var eachPosition = [Int]()
  public var averagePosition = 0.0
  
  public var description: String {
    return "dp4Model: \(dp4Model.id)..." +
      "\n   eachReturn: \(doublesToString(value: eachReturn))" +
      "\n   tr: \(doubleToString(value: totalReturn))" +
      ", ar: \(doubleToString(value: averageReturn))" +
      ", ard: \(doubleToString(value: averageReturnDiscounted))" +
      "\n   eachPosition: \(eachPosition)" +
      "\n   ap: \(doubleToString(value: averagePosition))"
  }
}

extension DataModelsCalculator {
  
  public static func getPosition(dps: [DP4WModel], weekCount: Int?=nil, monthCount: Int?=nil, isReturn: Bool) -> ([DP4WModelPosition], [DP4WModel]) {
    let count = weekCount != nil ? weekCount! : monthCount!
    precondition(count > 0 && count <= 6, "Count interval mismatch")
    let (incl, excl) = filterOut(dps: dps, weekCount: weekCount, monthCount: monthCount)
    
    var winners = [DP4WModelPosition]()
    winners = incl.map { (dp: DP4WModel)->DP4WModelPosition in
      // print("Fund: \(dp.displayName) \(doubleOptsToString(value: dp.dpWs))")
      var eachReturn = [Double]()
      for i in 0..<count {
        if weekCount != nil  { eachReturn.append(dp.dpWs[i]!) }
        if monthCount != nil { eachReturn.append(dp.dpMs[i]!) }
      }
      let totalReturn = eachReturn.reduce(0.0, +)

      var discount = 1.0
      let discountedReturn = eachReturn.reduce(0.0) {
        let rv = $0 + $1*discount
        discount *= 0.9
        return rv
      }
      let averageReturnDiscounted = discountedReturn / Double(eachReturn.count)

      let averageReturn = totalReturn / Double(eachReturn.count)
      
      let rv = DP4WModelPosition(dp4Model: dp,
                                 eachReturn: eachReturn,
                                 totalReturn: totalReturn,
                                 averageReturn: averageReturn,
                                 averageReturnDiscounted: averageReturnDiscounted)
      return rv
    }
    
    // Done if calculating return over count
    if isReturn {
      winners.sort { $0.averageReturnDiscounted > $1.averageReturnDiscounted }
      return (winners, excl)
    }
    
    // Allocate space for each position
    for i in 0..<winners.count {
      winners[i].eachPosition = Array.init(repeating: 0, count: count)
    }
    
    // We're counting position over count
    // Assign eachPosition[idx] == You biggness compared to the others
    for rindex in 0..<count {
//      print("rindex: \(rindex)")
      // Create [(value, indexInWinners)]
      var index = 0
      var returnSorted: [(Double, Int)] = winners.map {
        let rv = ($0.eachReturn[rindex], index)
        index += 1
        return rv
      }
      // Sort in value-decending order
      returnSorted.sort { $0.0 > $1.0 }
//      print("   rs: \(returnSorted)")
//      print("   winners have count: \(winners.count)")
      // Assign your ordered position for this week/month
      for (idx,rse) in returnSorted.enumerated() {
//        print("   for winners[\(rse.1)] updating eachPosition[\(rindex)] to \(idx)")
        winners[rse.1].eachPosition[rindex] = idx+1
//        print("   -> done")
      }
    }
    // Calculate average position
    winners = winners.map {
      var e = $0
      e.averagePosition = Double($0.eachPosition.reduce(0, +)) / Double($0.eachPosition.count)
      return e
    }
    // Sort ascending order (lower positions are winners)
    winners.sort { $0.averagePosition < $1.averagePosition }
    
    // Return
    return (winners, excl)
  }
    
  // ******************************************************
  
  private static func filterOut(dps: [DP4WModel], weekCount: Int?=nil, monthCount: Int?=nil) -> ([DP4WModel], [DP4WModel]) {
    precondition(weekCount != nil || monthCount != nil, "One of week or month must be non-nil")
    var rvIncl = [DP4WModel]()
    var rvExcl = [DP4WModel]()
    for d in dps {
      
//      if d.displayName.contains("landsbanken Global Aktie B") {
//        print("hello")
//      }
      
      var include = true
      for i in 0..<(weekCount != nil ? weekCount! : monthCount!) {
        if weekCount != nil && (d.dpWs.count < weekCount! || d.dpWs[i] == nil) {
          include = false
          break
        }
        
        if monthCount != nil && (d.dpMs.count < monthCount! || d.dpMs[i] == nil) {
          include = false
          break
        }
      }  // Done iterating through all weeks/months
      if include {
        rvIncl.append(d)
      } else {
        rvExcl.append(d)
      }
    }  // Done iterating through all funds
    return (rvIncl, rvExcl)
  }
  
    public static func testPosition() {
  //    print("\n*****************************************\nTesting Position")

  //    var dp01 = DP4WModel(fqName: "FQName01", displayName: "DisplayName01")
  //    dp01.dpWs = [1, 1, 3, 2, 8, 8]
  //    dp01.dpWsValidityPercent = [1, 1, 1, 1, 1, 1]
  //    var dp02 = DP4WModel(fqName: "FQName02", displayName: "DisplayName02")
  //    dp02.dpWs = [2, 1, 2, 3, 9, 9]
  //    dp02.dpWsValidityPercent = [1, 1, 1, 1, 1, 1]
  //    var dp03 = DP4WModel(fqName: "FQName03", displayName: "DisplayName03")
  //    dp03.dpWs = [3, 1, 2, 2, 2, 2]
  //    dp03.dpWsValidityPercent = [1, 1, 0, 1, 1, 1]
  //
  //    let (modelPos, model) = DataModelsCalculator.getPosition(
  //      dps: [dp01, dp02, dp03], weekCount: 4, monthCount: nil, isReturn: false)
      
      let (modelPos, model) = DataModelsCalculator.getPosition(
        dps: AppDataObservable._allFundsDP4, weekCount: 1, monthCount: nil, isReturn: false)
      
      print("Printing ModelPosition Results: \(modelPos.count)")
      for (idx,v) in modelPos.enumerated() {
        print(idx)
        print(v.description)
        if idx > 3 { break }
      }
      print("\nPrinting Exclusions: \(model.count)")
  //    for v in model { print(v.description) }
      print("Done Testing Position\n***************************")
    }
}
