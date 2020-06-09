//
//  FileUtils.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 5/21/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation

func fileRead(fileName:String)-> String!{
  do {
    let contents = try String(contentsOfFile: fileName, encoding: .utf8)
    return contents
  } catch {
    print("File Read Error for file \(fileName)")
    return nil
  }
}

func tabSeparate(data: String, separator: String) -> [[String]] {
  var result: [[String]] = []
  let rows = data.components(separatedBy: "\n")
  for row in rows {
    let columns = row.components(separatedBy: separator)
    result.append(columns)
  }
  return result
}

func tabFilterCells(data: [[String]], cellsContaining: String) -> [[String]] {
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
