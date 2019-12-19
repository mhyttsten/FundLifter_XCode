//
//  DataModelsCalculators.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/29/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

public class DataModelsCalculator {
  
  public static func getDP4WModelForFund(fund: D_FundInfo, startDate: Date=Date()) -> DP4WModel {
    var rv = DP4WModel(fqName: fund.typeAndName, displayName: fund._nameMS)

    let weeksYYMMDD = dateLastFridaysAsYYMMDD(count: 4, inclusive: false)
    let wDPDs = getDPDayValuesForDates(fund: fund, datesYYMMDD: weeksYYMMDD)
    if let v = wDPDs[0]?._r1w { rv.dp0W = v; rv.dp0WValidityPercent = 1.0 }
    if let v = wDPDs[1]?._r1w { rv.dp1W = v; rv.dp1WValidityPercent = 1.0 }
    if let v = wDPDs[2]?._r1w { rv.dp2W = v; rv.dp2WValidityPercent = 1.0 }
    if let v = wDPDs[3]?._r1w { rv.dp3W = v; rv.dp3WValidityPercent = 1.0 }

    let monthsYYMMDD = dateLastMonthsAsYYMMDD(count: 4, inclusive: false)
    let mDPDs = getDPDayValuesForDates(fund: fund, datesYYMMDD: monthsYYMMDD)
    if let v = mDPDs[0]?._r1m {rv.dp0M = v; rv.dp0MValidityPercent = 1.0 }
    if let v = mDPDs[1]?._r1m {rv.dp1M = v; rv.dp1MValidityPercent = 1.0 }
    if let v = mDPDs[2]?._r1m {rv.dp2M = v; rv.dp2MValidityPercent = 1.0 }
    if let v = mDPDs[3]?._r1m {rv.dp3M = v; rv.dp3MValidityPercent = 1.0 }
    
    if let v = wDPDs[0]?._rYTDFund {rv.dpY2D = v; rv.dpY2DValidityPercent = 1.0 }
    
    return rv
  }
  
  public static func getDP4WModelForPortfolio(name: String, funds: [D_FundInfo]) -> DP4WModel {
    print("getDP4WModelForPortfolio: \(name)")

    let datesWeeks =  dateLastFridaysAsYYMMDD(count: 4, inclusive: false)
    print("...DatesWeeks: \(datesWeeks)")
    let datesMonths = dateLastMonthsAsYYMMDD(count: 4, inclusive: false)
    print("...DatesMonths: \(datesMonths)")

    var wVal = [[Double?]]()
    var mVal = [[Double?]]()
    var yVal = [[Double?]]()
    for fi in funds {
      let wDPDs = getDPDayValuesForDates(fund: fi, datesYYMMDD: datesWeeks)
      guard wDPDs.count > 0 else {
        continue
      }
      
      print("\(fi.typeAndName)")
      print("...wDPDs: \(wDPDs.map { $0 != nil ? "(\($0!.description))" : "nil" })")
      let wValF = wDPDs.map {  $0?._r1w != nil ? $0!._r1w : nil }
      wVal.append(wValF)

      let mDPDs = getDPDayValuesForDates(fund: fi, datesYYMMDD: datesMonths)
      print("...mDPDs: \(mDPDs.map { $0 != nil ? "(\($0!.description))" : "nil" })")
      let mValF = mDPDs.map {  $0?._r1w != nil ? $0!._r1m : nil }
      mVal.append(mValF)
      
      let yv = wDPDs[0]?._rYTDFund
      yVal.append([yv])
    }

    let (vWAverage, vWPercentValid) = getAccumulated(values: wVal)
    let (vMAverage, vMPercentValid) = getAccumulated(values: mVal)
    let (vYAverage, vYPercentValid) = getAccumulated(values: yVal)

    var dp4w = DP4WModel(fqName: name, displayName: name)
    guard vWAverage.count == 4 else {
      return dp4w
    }
    
    if vWAverage.count >= 1 {
      dp4w.dp0W = vWAverage[0]
      dp4w.dp0WValidityPercent =  vWPercentValid[0]
    }
    if vWAverage.count >= 2 {
      dp4w.dp1W = vWAverage[1]
      dp4w.dp1WValidityPercent =  vWPercentValid[1]
    }
    if vWAverage.count >= 3 {
      dp4w.dp2W = vWAverage[2]
      dp4w.dp2WValidityPercent =  vWPercentValid[2]
    }
    if vWAverage.count >= 4 {
      dp4w.dp3W = vWAverage[3]
      dp4w.dp3WValidityPercent =  vWPercentValid[3]
    }
    
    if vMAverage.count >= 1 {
      dp4w.dp0M = vMAverage[0]
      dp4w.dp0MValidityPercent =  vMPercentValid[0]
    }
    if vMAverage.count >= 2 {
      dp4w.dp1M = vMAverage[1]
      dp4w.dp1MValidityPercent =  vMPercentValid[1]
    }
    if vMAverage.count >= 3 {
      dp4w.dp2M = vMAverage[2]
      dp4w.dp2MValidityPercent =  vMPercentValid[2]
    }
    if vMAverage.count >= 4 {
      dp4w.dp3M = vMAverage[3]
      dp4w.dp3MValidityPercent =  vMPercentValid[3]
    }
    
    if vYAverage.count >= 1 {
      dp4w.dpY2D = vYAverage[0]
      dp4w.dpY2DValidityPercent =  vYPercentValid[0]
    }
    
    return dp4w
  }
  
  private static func getAccumulated(values: [[Double?]]) -> ([Double?], [Double]) {
    if values.count == 0 || values[0].count == 0 {
      return ([Double?](), [Double]())
    }
    
    var vAverage = [Double?]()
    var vValid = [Int]()
    var vNull = [Int]()
    var vPercent = [Double]()
    for _ in (0..<values[0].count) {
      vAverage.append(nil)
      vValid.append(0)
      vNull.append(0)
      vPercent.append(1.0)
    }
    
    for v in values {
      for (idx,d) in v.enumerated() {
        if let d=d { // d != nil {
          if let va=vAverage[idx] {
            vAverage[idx] = va+d
          } else {
            vAverage[idx] = d
          }
          vValid[idx] += 1
        } else {
          vNull[idx] += 1
        }
      }
    }
       
    // Average and calculate validities
    for i in (0..<values[0].count) {
      if vValid[i] > 0 {
        precondition(vAverage[i] != nil, "Cannot have valids but no doubles")
        vAverage[i] = vAverage[i]! / Double(vValid[i])
      } else {
        precondition(vAverage[i] == nil, "No entries requires a nil")
      }
      vPercent[i] = Double(vValid[i]) / Double(vValid[i] + vNull[i])
    }
    
    return (vAverage, vPercent)
  }
  
  public static func getDPDayValuesForDates(fund: D_FundInfo, datesYYMMDD: [String]) -> [D_FundDPDay?] {
    var r = [D_FundDPDay?]()
    let dpds = fund._dpDays
    for d in datesYYMMDD {
      for i in (0..<dpds.count) {
        if dpds[i]._dateYYMMDD == d {
          r.append(dpds[i])
          break
        } else if (dpds[i]._dateYYMMDD < d) {
          r.append(nil)
          break
        }
      }
    }
    return r
  }
}
