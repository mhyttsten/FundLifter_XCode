//
//  Fund4wView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct DP4WRowView: View {
  @EnvironmentObject var settings: AppDataObservable
  var displayName: String = ""
  var key: String = ""

  init(displayName: String, dp4ModelKey: String) {
    self.displayName = displayName
    self.key = dp4ModelKey
  }
  
  var body: some View {
    VStack(alignment: .trailing) {
      HStack {
        Text(displayName).font(.title)
        Spacer()
        Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[0]))")
          .font(.title)
          .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[0], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[0]))
      }
      VStack {
        HStack {
          Spacer()
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[0]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[0], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[0]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[1]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[1], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[1]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[2]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[2], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[2]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpWs[3]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpWs[3], percent: settings.dp4ModelHM[key]!.dpWsValidityPercent[3]))
        }
        HStack {
          Spacer()
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[1]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[1], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[1]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[2]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[2], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[2]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[3]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[3], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[3]))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpMs[4]))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpMs[4], percent: settings.dp4ModelHM[key]!.dpMsValidityPercent[4]))

          Text("|")
            .font(.body)
            .foregroundColor(.black)
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpY2D))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpY2D, percent: settings.dp4ModelHM[key]!.dpY2DValidityPercent))
        }
      }
    }.onAppear {
//      print("Printing settings.dp4ModelHM: \(self.settings.dp4ModelHM[self.key]!)")
    }
  }
}

struct Fund4WView_Previews: PreviewProvider {
  static var previews: some View {
    DP4WRowView(displayName: "PPM", dp4ModelKey: "PPM")
    }
}
