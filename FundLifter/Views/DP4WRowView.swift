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
        Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp0M))")
          .font(.title)
          .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp0M, percent: settings.dp4ModelHM[key]!.dp0MValidityPercent))
      }
      VStack {
        HStack {
          Spacer()
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp0W))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp0W, percent: settings.dp4ModelHM[key]!.dp0WValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp1W))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp1W, percent: settings.dp4ModelHM[key]!.dp1WValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp2W))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp2W, percent: settings.dp4ModelHM[key]!.dp2WValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp3W))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp3W, percent: settings.dp4ModelHM[key]!.dp3WValidityPercent))
        }
        HStack {
          Spacer()
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp0M))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp0M, percent: settings.dp4ModelHM[key]!.dp0MValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp1M))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp1M, percent: settings.dp4ModelHM[key]!.dp1MValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp2M))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp2M, percent: settings.dp4ModelHM[key]!.dp2MValidityPercent))
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dp3M))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dp3M, percent: settings.dp4ModelHM[key]!.dp3MValidityPercent))
          Text("|")
            .font(.body)
            .foregroundColor(.black)
          Text("\(doubleOptToString(value: settings.dp4ModelHM[key]!.dpY2D))")
            .font(.body)
            .foregroundColor(doubleOptToColor(value: settings.dp4ModelHM[key]!.dpY2D, percent: settings.dp4ModelHM[key]!.dpY2DValidityPercent))
        }
      }
    }.onAppear {
    }
  }


}

struct Fund4WView_Previews: PreviewProvider {
  static var previews: some View {
    DP4WRowView(displayName: "PPM", dp4ModelKey: "PPM")
    }
}
