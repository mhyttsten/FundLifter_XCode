//
//  FileUtils.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 5/21/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation

public class FileOSUtils {
  public static func fileRead(fileName:String)-> String!{
    do {
      let contents = try String(contentsOfFile: fileName, encoding: .utf8)
      return contents
    } catch {
      print("File Read Error for file \(fileName)")
      return nil
    }
  }
}


