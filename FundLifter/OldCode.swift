
//
//  Fund4wView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

//import SwiftUI
//
//struct DP4WRowView: View {
//  @EnvironmentObject var settings: AppDataObservable
//  var displayName: String = ""
//  var key: String = ""
//
//  init(displayName: String, dp4ModelKey: String) {
//    self.displayName = displayName
//    self.key = dp4ModelKey
//  }
//
//  var body: some View {
//    VStack(alignment: .trailing) {
//      HStack {
//        Text(displayName).font(.body)
//        Spacer()
//        Text("Y2D").font(.body)
//        Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpY2D))")
//          .font(.body)
//          .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpY2D, percent: settings.dp4ModelHM[key]!.dpY2DValidityPercent))
//      }
//      VStack {
//        HStack {
//          Text("Ws").font(.subheadline)
//          Spacer()
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[0]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[0], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[0]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[1]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[1], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[1]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[2]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[2], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[2]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[3]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[3], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[3]))
//        }
//        HStack {
//          Text("Ms").font(.subheadline)
//          Spacer()
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[0]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[0], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[0]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[1]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[1], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[1]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[2]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[2], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[2]))
//          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[3]))")
//            .font(.subheadline)
//            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[3], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[3]))
//        }
//      }
//    }.onAppear {
////      print("Printing settings.dp4ModelHM: \(self.settings.dp4ModelHM[self.key]!)")
//    }
//  }
//}
//
//struct Fund4WView_Previews: PreviewProvider {
//  static var previews: some View {
//    DP4WRowView(displayName: "PPM", dp4ModelKey: "PPM")
//    }
//}



//----------------------------------------------
// Loading object from GCS bare metal using HTTP (but do not seem to get all data)
//  let HTTPS_URI = "https://storage.cloud.google.com/ql-magnushyttsten.appspot.com/backend/fundinfo-db-master.bin?alt=media"
//  getGCSUsingHTTP() {
//    guard let url = URL(string: HTTPS_URI) else {
//      let s = "DBU, error at URL"
//      print(s)
//      return
//    }
//
//    URLSession.shared.dataTask(with: url) {
//      (data, response, error) in
//      //      do {
//      print("Response: \(response)")
//
//      if error != nil {
//        let s = "DBU, got error: \(error!)"
//        print(s)
//        self.message = s
//        return
//      }
//
//      guard let data=data else {
//        let s = "DBU, error at data"
//        print(s)
//        self.message = s
//        return
//      }
//
//      let s = "DBU, data success: \(data.count)"
////      self.message = s
//      print("Here is the data: \(data)")
//      print(s)
//
//      //        DispatchQueue.main.async {
//      //          self.todos = todos
//      //        }
//      //      } catch {
//      //        let s = "Failed To decode: \(error)"
//      //        messageUpdate = s
//      //        print(s)
//      //      }
//    }
//    .resume()
