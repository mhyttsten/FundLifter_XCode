//
//  main.swift
//  FileUtils
//
//  Created by Magnus Hyttsten on 5/21/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//
import Foundation

func diff_ls_content() {
  var fileContent = [String: [String:Int]]()
  
  let fm = FileManager.default
  do {
    // Specify directory to look for comparsion files as cmdline argument
    let fnames = try fm.contentsOfDirectory(atPath: CommandLine.arguments[1])
    for fname in fnames {
      let fname = "\(CommandLine.arguments[1])/\(fname)"
      if let fcontent = fileRead(fileName: fname) {
        let fcontent = fcontent
          .replacingOccurrences(of: "\r", with: "\n")
          .replacingOccurrences(of: "\n\n", with: "\n")
        var tabular = tabSeparate(data: fcontent, separator: " ")
        tabular = tabFilterCells(data: tabular, cellsContaining: "")
        
        fileContent[fname] = [String:Int]()
        var countSkip = 0
        var count9 = 0
        var count11 = 0
        for r in tabular {
          if r.count == 9 {
            fileContent[fname]![r[8]] = 0
            count9 += 1
          } else if r.count == 11 {
            fileContent[fname]![r[10]] = 0
            count11 += 1
          } else {
            countSkip += 1
          }
        }
        print("Done with file: \(fname). Counts - 9: \(count9), 11: \(count11), skip: \(countSkip)")
      } else {
        fatalError("Error, could not read filename: \(fname)")
      }
    }
  } catch {
    fatalError("Failed to read directory: \(CommandLine.arguments[1])")
  }
  
  // Initialize count for all files
  for fname in fileContent.keys {
    for cell in fileContent[fname]!.keys {
      var count = 0
      for fnameIter in fileContent.keys {
        if let _=fileContent[fnameIter]?[cell] {
          count += 1
        }
      }
      fileContent[fname]![cell] = count
    }
  }
  
  print()
  
  let allCount = fileContent.keys.count
  print("Exists in all build targets")
  for fname in fileContent.keys {
    for cell in fileContent[fname]!.keys.sorted() {
      if fileContent[fname]?[cell] == allCount {
        print("   \(cell)")
      }
    }
    break
  }
  
  for fname in fileContent.keys {
    print("\nExists only in: \(fname)")
    for cell in fileContent[fname]!.keys.sorted() {
      if fileContent[fname]?[cell] == 1 {
        print("   \(cell)")
      }
    }
  }
}



