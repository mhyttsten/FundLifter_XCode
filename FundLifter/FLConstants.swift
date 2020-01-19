//
//  Constants.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/27/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation


public struct FLConstants {
 
  public static let PORTFOLIO_ARCS = "All Categories"
  
  public static let PORTFOLIO_FILENAME_GCS = "backend/fundinfo-portfolios.bin"
  public static let DB_FILENAME_GCS = "backend/fundinfo-db-master.bin"
  public static let DB_FILENAME_LOCAL_ZIP = "fundinfo-db-master.bin.zip"
  public static let DB_FILENAME_LOCAL = "fundinfo-db-master.bin"
  public static let DB_ZIPARCHIVE = "FundList"
  public static let PORTFOLIO_LOCAL = "Portfolio.bin"
  public static func urlDocuments() -> URL { return FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first! }
  public static func urlDBZip() -> URL { return urlDocuments().appendingPathComponent(DB_FILENAME_LOCAL_ZIP) }
  public static func urlDBZipArchiveName() -> URL { return urlDocuments().appendingPathComponent(DB_ZIPARCHIVE) }
  public static func urlDB() -> URL { return urlDocuments().appendingPathComponent(DB_FILENAME_LOCAL) }
  public static func urlPortfolio() -> URL { return urlDocuments().appendingPathComponent(PORTFOLIO_LOCAL) }
  
  public static func fileDocuments() -> String { return NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0] }
  public static func fileLogFile() -> String { fileDocuments().appending("/FundLifterLog.txt") }
  public static func fileDB() -> String { fileDocuments().appending("/\(DB_FILENAME_LOCAL)") }
  public static func filePortfolio() -> String { fileDocuments().appending("/\(PORTFOLIO_LOCAL)") }
  
}
