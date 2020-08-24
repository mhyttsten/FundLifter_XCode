//
//  CoreIO.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 1/10/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation
import Firebase

public class FileGCSUtils {
  
  public static func gcsRead(fromFile: String, done: @escaping(Data?, Error?)->Void) {
    let storage = Storage.storage()
    let storageRef = storage.reference(withPath: fromFile)
    storageRef.getData(maxSize: 20 * 1024 * 1024) { data, error in
      if let error=error {
        done(nil, error)
      } else {
        done(data, nil)
      }
    }
  }
  
  public static func gcsWrite(targetFile: String, data: Data, done: @escaping(StorageMetadata?, Error?)->Void) {
    let storage = Storage.storage()
    let storageRef = storage.reference(withPath: targetFile)
    let _ = storageRef.putData(data, metadata: nil) { (metadata, error) in
      if let error=error {
        done(nil, error)
      } else {
        done(metadata, nil)
      }
    }
  }
}
