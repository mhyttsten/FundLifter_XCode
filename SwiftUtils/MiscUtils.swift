//
//  MiscUtils.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 8/5/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation

// Split an e.g. CSV data to table
func txtSeparate(data: String, separator: String) -> [[String]] {
  var result: [[String]] = []
  let rows = data.components(separatedBy: "\n")
  for row in rows {
    let columns = row.components(separatedBy: separator)
    result.append(columns)
  }
  return result
}

func txtFilterCells(data: [[String]], cellsContaining: String) -> [[String]] {
  var dataM = data
  for i in 0..<data.count {
    var idx = 0
    while idx < dataM[i].count {
      let cell = dataM[i][idx]
      if cell == cellsContaining {
        dataM[i].remove(at: idx)
      } else {
        idx += 1
      }
    }
  }
  return dataM
}
