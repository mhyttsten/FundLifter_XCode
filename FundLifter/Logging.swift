//
//  Logging.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/9/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

fileprivate var logFilePath : String {
  let documents = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0]
  let filePath = documents.appending("/FundLifterLog.txt")
  return filePath
}

fileprivate var logFileData: String = ""
func logFileAppend(s: String) {
  logFileAppendStdout(s: s)
}
func logFileAppendStdout(s: String) {
  let now = Date()
  let df = DateFormatter()
  df.dateFormat = "yyyy-MM-dd HH:mm:ss"
  let nowStr = df.string(from: now)
  let logStr = "\(nowStr): \(s)"
  print("\(logStr)")

}
func logFileAppendFile(s: String) {
  let now = Date()
  let df = DateFormatter()
  df.dateFormat = "yyyy-MM-dd HH:mm:ss"
  let nowStr = df.string(from: now)
  let logStr = "\(nowStr): \(s)\n"
  
  logFileData = "\(logStr)\(logFileData)"
  
  // Create an empty file
  let filemgr = FileManager.default
  FileManager.default.createFile(atPath: logFilePath, contents: nil, attributes: nil)
  if !filemgr.fileExists(atPath: logFilePath) {
    fatalError("Expected file to exist")
  }

  // Write all log info to file
  let d = Data(logFileData.utf8)
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

fileprivate func logFileRead() -> String {
  if let text = NSData(contentsOfFile: logFilePath) {
    let data = text.subdata(with: NSRange(text.startIndex..<text.endIndex))
    let str = String(decoding: data, as: UTF8.self)
    return str
  }
  return ""
}
