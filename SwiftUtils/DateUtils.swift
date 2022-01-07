//
//  DateUtils.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 7/13/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation
import SwiftUI

func weekOfYear(date: Date) -> Int {
  let calendar = Calendar.current
  let woy = calendar.dateComponents([.weekOfYear], from: date)
  return woy.weekOfYear!
}

func string2Date(dateString: String, dateFormat: String) -> Date {
  let dateFormatter = DateFormatter()
//  dateFormatter.locale = Locale(identifier: "en_US_POSIX") // set locale to reliable US_POSIX
  dateFormatter.timeZone = TimeZone(identifier: "UTC")
  dateFormatter.dateFormat = dateFormat  // E.g: "yyyy-MM-dd'T'HH:mm:ssZ"
  let date = dateFormatter.date(from: dateString)!
  return date
}

public func doubleToString(value: Double) -> String {
  return String(format: "%0.2f", value)
}

public func doublesToString(value: [Double]) -> String {
  var s = ""
  for v in value {
    s += String(format: "%0.2f", v) + ", "
  }
  return s
}
public func doubleOptsToString(value: [Double?]) -> String {
  var s = ""
  for v in value {
    s += doubleOptToString(value: v) + ", "
  }
  return s
}
public func doubleOptToString(value: Double?) -> String {
  return value != nil ? String(format: "%0.2f", value!) : "-"
}

public func doubleOptToColor(value: Double?, percent: Double?) -> Color {
  if value == nil || percent == nil || percent! < 1.0 {
    return Color.yellow
  }
  if value! < 0 {
    return Color.red
  }
  return Color.green
}

//--------------------------------------------------

public func getHexDump(data: Data, sindex: Int, eindex: Int) -> String {
  var rv = ""
  var cindex = 0
  while eindex > cindex {
    let lindex = (cindex + 16) <= eindex ? cindex+16 : eindex
    let elem = data[cindex..<lindex]
    let remainder = 16-(lindex-cindex)
    
    let s = elem.map { String(format: "%02hhx ", $0) }.joined()
    rv = "\(rv)\(s)"
    if remainder > 0 {
      for _ in 0..<remainder {
        rv = "\(rv)   "
      }
    }
    
    if (lindex>cindex) {
      for i in cindex..<lindex {
        var char = "."
        if elem[i]>=0x20 && elem[i]<=127 {
          char = String(UnicodeScalar(elem[i]))
        }
        rv = "\(rv)\(char)"
      }
    }
    if remainder > 0 {
      for _ in 0..<remainder {
        rv = "\(rv) "
      }
    }
    rv = "\(rv)\n"
    
    cindex = lindex
  }
  return rv
}

//--------------------------------------------------

public func dateAsYYMMDD(date: Date) -> String {
  let df = DateFormatter()
  df.dateFormat = "yyMMdd"
  return df.string(from: date)
}

public func dateFromYYMMDD(dateYYMMDD: String) -> Date {
  let dateYYYYMMDD = "\(20)\(dateYYMMDD)"
  let dateFormatter = DateFormatter()
  dateFormatter.dateFormat = "yyyyMMdd"
  // dateFormatter.locale = Locale(identifier: "en_US_POSIX") // set locale to reliable US_POSIX
  guard let date = dateFormatter.date(from: dateYYYYMMDD) else {
    fatalError("Could not create date from dateYYYYMMDD: \(dateYYYYMMDD)")
  }
  return date
}

//--------------------------------------------------

fileprivate var fridaysAsYYMMDD: [String]? = nil
public func dateLastFridaysAsYYMMDD() -> [String] {
  if fridaysAsYYMMDD == nil {
    let ds = dateLastFridays(count: 6, inclusive: false).map { dateAsYYMMDD(date: $0) }
    fridaysAsYYMMDD = ds
  }
  return fridaysAsYYMMDD!
}

fileprivate var monthsAsYYMMDD: [String]? = nil
public func dateLastMonthsAsYYMMDD() -> [String] {
  if monthsAsYYMMDD == nil {
    let ds = dateLastMonths(count: 6, inclusive: false).map { dateAsYYMMDD(date: $0) }
    monthsAsYYMMDD = ds
  }
  return monthsAsYYMMDD!
}

// *****************************************************************

private func dateLastMonths(count: Int, startAt: Date=Date(), inclusive: Bool=true) -> [Date] {
  let calendar = Calendar.current
  var cdate = calendar.startOfDay(for: startAt)  // Weird, returns 10am for Hawaii (so midnight Hawaii == UTC+10?)
  
  var rv = [Date]()
  var count = count
  while count > 0 {
    cdate = dateGetClosestFridayFromDate(fromDate: cdate, inclusive: inclusive)
    rv.append(cdate)
    cdate = Calendar.current.date(byAdding: .month, value: -1, to: cdate)!
    count -= 1
  }
  
  return rv
}

fileprivate func dateLastFridays(count: Int, startAt: Date=Date(), inclusive: Bool=true) -> [Date] {
  precondition(count>0, "Count was 0")
  var r = [Date]()
  let lastFriday = dateLastFriday(startAt: startAt, inclusive: inclusive)
  r.append(lastFriday)
  var cdate = lastFriday
  for _ in (1..<count) {
    let d = dateLastFriday(startAt: cdate, inclusive: false)
    r.append(d)
    cdate = d
  }
  return r
}

func dateIsFriday(dateYYMMDD: String) -> Bool {
  let date = dateFromYYMMDD(dateYYMMDD: dateYYMMDD)
  let dow = Calendar.current.component(.weekday, from: date)
  return dow == 6
}

func dateLastFridayAsYYMMDD(startAt: Date=Date(), inclusive: Bool=true) -> String {
  let lastFriday = dateLastFriday(startAt: startAt, inclusive: inclusive)
  return dateAsYYMMDD(date: lastFriday)
}
fileprivate func dateLastFriday(startAt: Date=Date(), inclusive: Bool=true) -> Date {
  let dowFriday = 6
//  var date = Date()
//  startOfDay
//  let calendar = Calendar.current.date(bySettingHour: 0, minute: 0, second: 0, of: Date())!
  
  let calendar = Calendar.current
  var d2 = calendar.startOfDay(for: startAt)  // Weird, returns 10am for Hawaii (so midnight Hawaii == UTC+10?)

//  let yy = calendar.component(.year, from: d2)
//  let mm = calendar.component(.month, from: d2)
//  let dd = calendar.component(.day, from: d2)
  let dow = calendar.component(.weekday, from: d2)
  if dow == dowFriday && inclusive {
    return d2
  }
  while true {
    d2 = Calendar.current.date(byAdding: .day, value: -1, to: d2)!
    let dow = calendar.component(.weekday, from: d2)
    if dow == dowFriday {
      break
    }
  }
  return d2
}

fileprivate func dateGetClosestFridayFromDate(fromDate: Date=Date(), inclusive: Bool=true) -> Date {
  let fromDate = Calendar.current.startOfDay(for: fromDate)
  let dlf = dateLastFriday(startAt: fromDate, inclusive: inclusive)
  let dnf = Calendar.current.date(byAdding: .weekday, value: 7, to: dlf)!

  var now = Date()
  now = Calendar.current.startOfDay(for: now)
  
  var rv = fromDate
  if dnf > now {
    rv = dlf
  } else {
    let dlfDiff = abs(fromDate.timeIntervalSince(dlf))
    let dnfDiff = abs(fromDate.timeIntervalSince(dnf))
    if dlfDiff < dnfDiff {
      rv = dlf
    } else {
      rv = dnf
    }
  }
  
  // If the computed Friday is today and we're not inclusive, then use last friday!
  if rv == now && !inclusive {
    rv = Calendar.current.date(byAdding: .weekday, value: -7, to: rv)!
  }
  
  return rv
}
  
