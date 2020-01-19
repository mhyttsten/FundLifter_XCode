//
//  FLUtils.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/27/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

public struct FLBinaryIOUtils {

  public static func readBoolJava(data: Data, sindex: Int) -> (Int, Bool) {
    let array = Array(data[sindex..<sindex+1]).withUnsafeBytes { $0.load(as: Bool.self) }
    return (sindex+1, Bool(array))
  }
  
  public static func readInt16Java(data: Data, sindex: Int) -> (Int, Int) {
    let array = Array(data[sindex..<sindex+2]).withUnsafeBytes { $0.load(as: UInt16.self) }
    return (sindex+2, Int(array.bigEndian))
  }

  public static func readInt32Java(data: Data, sindex: Int) -> (Int, Int) {
    let array = Array(data[sindex..<sindex+4]).withUnsafeBytes { $0.load(as: UInt32.self) }
    return (sindex+4, Int(array.bigEndian))
  }
  
  public static func readUTFJava(data: Data, sindex: Int) -> (Int, String) {
    let (offset, int16) = readInt16Java(data: data, sindex: sindex)
    let str = String(data: data[offset..<(offset+int16)], encoding: .utf8)!
    return (offset+int16, str)
  }

  public static var nullCounts = 0
  public static var nonNullCounts = 0
  public static func readFloatAsDoubleJava(data: Data, sindex: Int) -> (Int, Double?) {
    let bytes:Array<UInt8> = [data[sindex+3], data[sindex+2], data[sindex+1], data[sindex]]
    var f:Float = 0.0
    memcpy(&f, bytes, 4)
    
    if f == Float.greatestFiniteMagnitude {
      nullCounts += 1
      return (sindex+4, nil)
    }
    nonNullCounts += 1
    let d = Double(f)
    return (sindex+4, d)
  }

  public static func readIntSwift(data: Data, sindex: Int) -> (Int, Int) {
    let array = Array(data[sindex..<sindex+8]).withUnsafeBytes { $0.load(as: Int.self) }
    return (sindex+8, Int(array.littleEndian))
  }

  public static func readUTFSwift(data: Data, sindex: Int) -> (Int, String) {
    let (offset, int32) = readIntSwift(data: data, sindex: sindex)
    let str = String(data: data[offset..<(offset+int32)], encoding: .utf8)!
    return (offset+int32, str)
  }
    
  public static func writeIntSwift(integer: Int) -> Data {
    var integer2 = integer
    let integerData = Data(bytes: &integer2, count: MemoryLayout.size(ofValue: integer2))
    return integerData
  }

  public static func writeUTFSwift(string: String) -> Data {
    let dataStr: Data? = string.data(using: .utf8)
    let dataInt = writeIntSwift(integer: dataStr!.count)
    var data = Data()
    data.append(dataInt)
    data.append(dataStr!)
    return data
  }

  public static func readFile(url: URL) -> (Bool, Data?, String?) {
    var data: Data? = nil
    do {
      do {
        if try !url.checkResourceIsReachable() {
          return (false, nil, nil)
        }
      } catch {
        return (false, nil, nil)
      }
      
      let file: FileHandle? = try FileHandle(forReadingFrom: url)
      if file != nil {
        data = file?.readDataToEndOfFile()
        file?.closeFile()
        return (true, data, nil)
      }
      return (true, nil, "FileHandle nil")
    } catch {
      let s = "Exception: \(error)"
      print(s)
      logFileAppend(s: s)
      return (true, nil, s)
    }
  }
}
