//
//  Logging.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/9/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation


var logFilePath : String {
  let documents = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0]
  let filePath = documents.appending("/FundLifterLog.txt")
  return filePath
}

func logFileAppend(s: String) {
  let now = Date()
  let df = DateFormatter()
  df.dateFormat = "yyyy-MM-dd HH:mm:ss"
  let nowStr = df.string(from: now)
  let str = "\(nowStr): \(s)\n"
  
  let prevDataFull = logFileRead()
  let lastIndex = prevDataFull.count < 10000 ?
    prevDataFull.endIndex : prevDataFull.index(prevDataFull.startIndex, offsetBy: 10000)
  let prevData = prevDataFull[..<lastIndex]
  print(str)
  let logString = "\(str)\(prevData)"
  
  let d = Data(logString.utf8)
  let filemgr = FileManager.default
  FileManager.default.createFile(atPath: logFilePath, contents: nil, attributes: nil)
  if !filemgr.fileExists(atPath: logFilePath) {
    fatalError("Expected file to exist")
  }
  
  if let fh = FileHandle(forWritingAtPath: logFilePath) {
    fh.seekToEndOfFile()
    fh.write(d)
    if (try? fh.synchronize()) == nil {
      print("Synchronize failed")
    }
  } else {
    fatalError("Could not create FileHandle object")
  }
}

func logFileRead() -> String {
  if let text = NSData(contentsOfFile: logFilePath) {
    let data = text.subdata(with: NSRange(text.startIndex..<text.endIndex))
    let str = String(decoding: data, as: UTF8.self)
    return str
  }
  return ""
}
