//
//  DP4ModelPositionCalculator.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/23/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

public struct DP4WModelPosition: Identifiable {
  public var dp4Model: DP4WModel
  public var id: String
  public var isReturn = true

  // Return fields
  public var y2dReturn = 0.0
  public var eachReturn = [Double]()
  public var totalReturn = 0.0
  public var averageReturn = 0.0
  public var averageReturnDiscounted = 0.0

  // Position fields
  public var eachPosition = [Int]()
  public var averagePosition = 0.0
  public var averagePositionDiscounted = 0.0
  
  public var description: String {
    return "dp4Model: \(dp4Model.id)..." +
      "\n   eachReturn: \(doublesToString(value: eachReturn))" +
      "\n      tr: \(doubleToString(value: totalReturn))" +
      ", ar: \(doubleToString(value: averageReturn))" +
      ", ard: \(doubleToString(value: averageReturnDiscounted))" +
      "\n   eachPosition: \(eachPosition)" +
      "\n      ap: \(doubleToString(value: averagePosition))" +
      "\n      apd: \(doubleToString(value: averagePositionDiscounted))"
  }
}

extension DataModelsCalculator {

  public static func getPositionY2D(dps: [DP4WModel]) -> ([DP4WModel], [DP4WModel]) {
    var (incl, excl) = filterOut(dps: dps, weekCount: nil, monthCount: nil, y2d: true)
    incl.sort { $0.dpY2D! > $1.dpY2D! }
    return (incl, excl)
  }
  
  public static func getPosition(dps: [DP4WModel], weekCount: Int?=nil, monthCount: Int?=nil, isReturn: Bool) -> ([DP4WModel], [DP4WModel]) {
    let count = weekCount != nil ? weekCount! : monthCount!
    precondition(count > 0 && count <= 6, "Count interval mismatch")
    var (incl, excl) = filterOut(dps: dps, weekCount: weekCount, monthCount: monthCount, y2d: false)
    guard incl.count > 0 else { return ([DP4WModel](), excl) }
    
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
                                 id: dp.id,
                                 eachReturn: eachReturn,
                                 totalReturn: totalReturn,
                                 averageReturn: averageReturn,
                                 averageReturnDiscounted: averageReturnDiscounted)
      return rv
    }

    // If Return is what we care about
    if isReturn {
      winners.sort { $0.averageReturnDiscounted > $1.averageReturnDiscounted }
      incl.removeAll()
      for w in winners {
        var w=w
        w.dp4Model.ranking = w.averageReturnDiscounted
        incl.append(w.dp4Model)
      }
      return (incl, excl)
    }
    
    // Below is code to manage position
    
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

      var discount = 1.0
      let discountedReturn = $0.eachPosition.reduce(0.0) {
        let cacc = $0
        let cval = Double($1)
        let thispart = cval * discount
        let rv = cacc + thispart
        discount /= 0.9
        return rv
      }
      e.averagePositionDiscounted = discountedReturn / Double($0.eachPosition.count)

      return e
    }
    
    // Sort ascending order (lower positions are winners)
    winners.sort { $0.averagePositionDiscounted < $1.averagePositionDiscounted }

    // Assign back to DP4WModel
    incl.removeAll()
    for w in winners {
      var w = w
      w.dp4Model.ranking = w.averagePositionDiscounted
      if weekCount != nil {
        w.dp4Model.dpWsPos = w.eachPosition
      } else {
        w.dp4Model.dpMsPos = w.eachPosition
      }
      incl.append(w.dp4Model)
    }
    
    // Return
    return (incl, excl)
  }
    
  // ******************************************************
  
  private static func filterOut(dps: [DP4WModel],
                                weekCount: Int?=nil,
                                monthCount: Int?=nil,
                                y2d: Bool=false) -> ([DP4WModel], [DP4WModel]) {
    precondition(weekCount != nil || monthCount != nil || y2d, "One of week or month must be non-nil")
    var rvIncl = [DP4WModel]()
    var rvExcl = [DP4WModel]()
    for d in dps {
      var include = true

      // Year to Date
      if y2d == true {
        if d.dpY2D == nil {
          include = false
        }
      }
      // Week or Month
      else {
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
      }
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
        dps: AppDataObservable._allFundsDP4, weekCount: 4, monthCount: nil, isReturn: false)
      
      print("Printing ModelPosition Results: \(modelPos.count)")
      for (idx,v) in modelPos.enumerated() {
        print(idx)
        print(v.description)
        if idx > 3 { break }
      }
      print("\nPrinting Exclusions: \(model.count)")
  //    for v in model { print(v.description) }
      print("Done Testing Position\n***************************")
      for (idx,v) in model.enumerated() {
        print(idx)
        print(v.description)
        if idx > 3 { break }
      }
    }
}
