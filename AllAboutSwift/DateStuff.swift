//
//  DateStuff.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/29/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

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


