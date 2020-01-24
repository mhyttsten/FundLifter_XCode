////
////  Fund4wView.swift
////  FundLifter
////
////  Created by Magnus Hyttsten on 11/11/19.
////  Copyright © 2019 Magnus Hyttsten. All rights reserved.
////
//
import SwiftUI

struct DP4WRowView: View {
    
  @EnvironmentObject var settings: AppDataObservable
  var displayName: String = ""
  var key: String = ""

  init(displayName: String, dp4ModelKey: String) {
    self.displayName = displayName
    self.key = dp4ModelKey
    print("DP4RowView.init: \(dp4ModelKey)")
  }

  var body: some View {
    VStack(alignment: .trailing) {
      HStack {
        Text(displayName).font(.body)
        Spacer()
        Text("Y2D").font(.body)
        Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpY2D))")
          .font(.body)
          .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpY2D, percent: settings.pubDP4ModelHM[key]!.dpY2DValidityPercent))
      }
      VStack {
        HStack {
          Text("Ws").font(.subheadline)
          Spacer()
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpWs[0]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpWs[0], percent: settings.pubDP4ModelHM[key]!.dpWsValidityPercent[0]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpWs[1]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpWs[1], percent: settings.pubDP4ModelHM[key]!.dpWsValidityPercent[1]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpWs[2]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpWs[2], percent: settings.pubDP4ModelHM[key]!.dpWsValidityPercent[2]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpWs[3]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpWs[3], percent: settings.pubDP4ModelHM[key]!.dpWsValidityPercent[3]))
        }
        HStack {
          Text("Ms").font(.subheadline)
          Spacer()
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpMs[0]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpMs[0], percent: settings.pubDP4ModelHM[key]!.dpMsValidityPercent[0]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpMs[1]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpMs[1], percent: settings.pubDP4ModelHM[key]!.dpMsValidityPercent[1]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpMs[2]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpMs[2], percent: settings.pubDP4ModelHM[key]!.dpMsValidityPercent[2]))
          Text("\(doubleOptToString(value: settings.pubDP4ModelHM[key]!.dpMs[3]))")
            .font(.subheadline)
            .foregroundColor(doubleOptToColor(value: settings.pubDP4ModelHM[key]!.dpMs[3], percent: settings.pubDP4ModelHM[key]!.dpMsValidityPercent[3]))
        }
      }
    }.onAppear {
//      print("Printing settings.dp4ModelHM: \(self.settings.dp4ModelHM[self.key]!)")
    }
  }
}

struct Fund4WView_Previews1: PreviewProvider {
  static var previews: some View {
    DP4WRowView(displayName: "PPM", dp4ModelKey: "PPM")
    }
}
