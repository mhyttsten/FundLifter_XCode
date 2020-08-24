//
//  DateStuff.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/29/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation


// Strip Minutes and Seconds
//calendar = Calendar.current
//let components = calendar.dateComponents([.year, .month, .day, .hour], from: date)
//let finalDate = calendar.date(from:components)

public func foo() {
  let now = Date()
  let df = DateFormatter()
  df.dateFormat = "yyyy-MM-dd HH:mm:ss"
  let nowStr = df.string(from: now)
  print(nowStr)

  let date = Date()
  let calendar = Calendar.current
  let yy = calendar.component(.year, from: date)
  let mm = calendar.component(.month, from: date)
  let dd = calendar.component(.day, from: date)
  print("\(yy) \(mm) \(dd)")
}


//  let lastIndex = prevDataFull.count < 10000 ?
//    prevDataFull.endIndex : prevDataFull.index(prevDataFull.startIndex, offsetBy: 10000)
//  let prevData = prevDataFull[..<lastIndex]
  // print(logStr)
