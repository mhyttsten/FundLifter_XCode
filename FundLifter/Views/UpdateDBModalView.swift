//
//  ModelLogView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/12/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI
import Combine
import Firebase
import Zip

class TimerObservable: ObservableObject  {
    @Published var message = "Waiting for timer"
  
  init() {
    DispatchQueue.main.asyncAfter(deadline: .now() + 10.0) {
      self.message = "10s Have Passed"
    }
  }
}

public class DBUpdateObservable: ObservableObject {
  @Published var message = "Before network call"
  
  public static func getFundDBCreationTime() -> String {
    do {
      let fileAttributes = try FileManager.default.attributesOfItem(atPath: FLConstants.fileDB())
      let creationDate = fileAttributes[FileAttributeKey.creationDate] as! Date
      let df = DateFormatter()
      df.dateFormat = "yyyy-MM-dd HH:mm:ss"
      let nowStr = df.string(from: creationDate)
      return "\(nowStr)"
    } catch {
      let s = "Exception: \(error)"
      print(s)
      return s
    }
  }
    
  init() {
    print("*** DBUpdateObservable.init()")
    let storage = Storage.storage()
    let storageRef = storage.reference(withPath: FLConstants.DB_FILENAME_GCS)
    

    // Store GCS file directly onto file system
    let _ = storageRef.write(toFile: FLConstants.urlDBZip()) { url, error in
      if let error = error {
        let s = "Netoworking Error: \(error)"
        print(s)
        self.message = s
        logFileAppend(s: s)
        return
      } else {
        print("Local file: \(url!)")
        do {
          try Zip.unzipFile(FLConstants.urlDBZip(), destination: FLConstants.urlDocuments(), overwrite: true, password: nil)
          print("Unzipped file")
          let fileManager = FileManager.default
          do { try fileManager.removeItem(at: FLConstants.urlDB()) } catch {}
          print("Removed old fund DB file")
          try fileManager.moveItem(at: FLConstants.urlDBZipArchiveName(), to: FLConstants.urlDB())
          print("Moved zip archive name to url name")
          try fileManager.removeItem(at: FLConstants.urlDBZip())
          print("Removed zip file name")
        } catch {
          let s = "Error fetching DB file:\(error)"
          print(s)
          self.message = s  // TODO: Why is this not forcing us to be on main thread?
          logFileAppend(s: s)
          return
        }
        let s = "New DB file fetched"
        print(s)
        
        DispatchQueue.main.async { [self] in
          self.message = s
        }
      }
    }
  }
}

struct UpdateDBModalView: View {
  @EnvironmentObject var settings: AppDataObservable
  @Environment(\.presentationMode) var presentationMode
//  @ObservedObject var data = TimerObservable()
  @ObservedObject var data = DBUpdateObservable()

  var body: some View {
    VStack {
      Text("\(data.message)")
        .padding(.top, 50)
      Spacer()
      Button(action: {
        self.presentationMode.wrappedValue.dismiss()
      }) {
        Text("Dismiss")
      }.padding(.top, 50)
    }.onAppear {
//      print("UpdateDBModalView.onAppear")
//      DispatchQueue.main.async { [self] in
//        self.data.message = "***"
//      }
//
//      print("...will now call fetch")
////      self.data.fetchNewFundDBFile(settings: self.settings)
//      print("...returned from fetch")
      self.settings.pubFundDBCreationTime = DBUpdateObservable.getFundDBCreationTime()
//      print("...done with everything")
    }
  }
}

struct UpdateDBModalView_Previews: PreviewProvider {
    static var previews: some View {
        UpdateDBModalView()
    }
}




