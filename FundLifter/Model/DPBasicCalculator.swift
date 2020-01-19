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
    var rv = DP4WModel(fqName: fund.typeAndName, displayName: fund._nameMS, subTitle: fund._indexName)

    let weeksYYMMDD = dateLastFridaysAsYYMMDD()
    let wDPDs = getDPDayValuesForDates(fund: fund, datesYYMMDD: weeksYYMMDD)
    for i in 0..<6 {
      if wDPDs[i] != nil {
        rv.dpWs.append(wDPDs[i]!._r1w)
        rv.dpWsValidityPercent.append(1.0)
        
        if i==0 && wDPDs[i]!._rYTDFund != nil {
          rv.dpY2D = wDPDs[i]!._rYTDFund
          rv.dpY2DValidityPercent = 1.0
        }
        
      } else {
        rv.dpWs.append(nil)
        rv.dpWsValidityPercent.append(nil)
      }
    }
        
    let monthsYYMMDD = dateLastMonthsAsYYMMDD()
    let mDPDs = getDPDayValuesForDates(fund: fund, datesYYMMDD: monthsYYMMDD)
    for i in 0..<6 {
      if let r1m = mDPDs[i]?._r1m {
        rv.dpMs.append(r1m)
        rv.dpMsValidityPercent.append(1.0)
      } else {
        rv.dpMs.append(nil)
        rv.dpMsValidityPercent.append(nil)
      }
    }
    
    return rv
  }
  
  public static func getDP4WModelForPortfolio(name: String, funds: [D_FundInfo]) -> DP4WModel {
//    print("getDP4WModelForPortfolio: \(name)")

    let datesWeeks =  dateLastFridaysAsYYMMDD()
//    print("   DatesWeeks: \(datesWeeks)")
    let datesMonths = dateLastMonthsAsYYMMDD()
//    print("   DatesMonths: \(datesMonths)")

    var wVal = [[Double?]]()
    var mVal = [[Double?]]()
    var yVal = [[Double?]]()
    
    for fi in funds {
      let wDPDs = getDPDayValuesForDates(fund: fi, datesYYMMDD: datesWeeks)
      guard wDPDs.count > 0 else {
        continue
      }
      
//      print("   \(fi.typeAndName)")
//      print("      wDPDs[\(wDPDs.count)]: \(wDPDs.map { $0 != nil ? "(\($0!.description))" : "nil" })")
      let wValF = wDPDs.map {  $0?._r1w != nil ? $0!._r1w : nil }
      wVal.append(wValF)

      let mDPDs = getDPDayValuesForDates(fund: fi, datesYYMMDD: datesMonths)
//      print("      mDPDs[\(mDPDs.count)]: \(mDPDs.map { $0 != nil ? "(\($0!.description))" : "nil" })")
      let mValF = mDPDs.map {  $0?._r1w != nil ? $0!._r1m : nil }
      mVal.append(mValF)
      
      var yv = wDPDs[0]?._rYTDFund
      if yv == nil {
        yv = mDPDs[0]?._rYTDFund
      }
      yVal.append([yv])
    }

    var (vWAverage, vWPercentValid) = getAccumulated(values: wVal)
    var (vMAverage, vMPercentValid) = getAccumulated(values: mVal)
    var (vYAverage, vYPercentValid) = getAccumulated(values: yVal)

    // If we have no funds registered for portfolio, fill in nils
    if funds.count == 0 {
      precondition(datesWeeks.count == datesMonths.count, "Count mismatch")
      var dNils = [Double?]()
      for _ in 0..<datesWeeks.count { dNils.append(nil) }
      let dZeroes = Array(repeating: 0.0, count: datesWeeks.count)
      vWAverage = dNils
      vWPercentValid = dZeroes
      vMAverage = dNils
      vMPercentValid = dZeroes
      vYAverage = [nil]
      vYPercentValid = [0.0]
    }

    var dp4w = DP4WModel(fqName: name, displayName: name, subTitle: "N/A")
    dp4w.dpWs = vWAverage
//    print("      fdpWs[\(dp4w.dpWs.count)]: \(dp4w.dpWs)")
    dp4w.dpWsValidityPercent = vWPercentValid
//    print("      fdpWsVP[\(dp4w.dpWsValidityPercent.count)]: \(dp4w.dpWsValidityPercent)")
    dp4w.dpMs = vMAverage
//    print("      fdpMs[\(dp4w.dpMs.count)]: \(dp4w.dpMs)")
    dp4w.dpMsValidityPercent = vMPercentValid
//    print("      fdpMsVP[\(dp4w.dpMsValidityPercent.count)]: \(dp4w.dpMsValidityPercent)")
    if vYAverage.count >= 1 {
      dp4w.dpY2D = vYAverage[0]
      dp4w.dpY2DValidityPercent =  vYPercentValid[0]
    }
//    print("      fdpY2D: \(dp4w.dpY2D)")
//    print("      fdpY2DVP: \(dp4w.dpY2DValidityPercent)")

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
    
    var findex = 0
    for d in datesYYMMDD {
      var found = false

      for i in findex..<dpds.count {
        if dpds[i]._dateYYMMDD == d {
          r.append(dpds[i])
          findex += 1
          found = true
          break
        } else if (dpds[i]._dateYYMMDD < d) {
          r.append(nil)
          found = true
          break
        }
      }
      
      if !found {
        r.append(nil)
      }
    }
    return r
  }
}
