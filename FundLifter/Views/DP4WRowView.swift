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
  var title: String = ""
  var key: String = ""
//  var entry: DP4WModel
//
//  init(entry: DP4WModel) {
//    self.entry = entry
//  }

  init(title: String, dp4ModelKey: String) {
    self.title = title
    self.key = dp4ModelKey
  }
  
  var body: some View {
    VStack(alignment: .trailing) {
      HStack {
        Text(title).font(.title)
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

//  var body: some View {
//    VStack(alignment: .trailing) {
//      Text(settings._name).font(.title)
//      HStack {
//        Text(entry._name).font(.title)
//        Spacer()
//        Text("\(doubleOptToString(value: entry.dp0M))")
//          .font(.title)
//          .foregroundColor(doubleOptToColor(value: entry.dp0M, percent: entry.dp0MValidityPercent))
//      }
//      VStack {
//        HStack {
//          Spacer()
//          Text("\(doubleOptToString(value: entry.dp0W))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp0W, percent: entry.dp0WValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp1W))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp1W, percent: entry.dp1WValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp2W))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp2W, percent: entry.dp2WValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp3W))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp3W, percent: entry.dp3WValidityPercent))
//        }
//        HStack {
//          Spacer()
//          Text("\(doubleOptToString(value: entry.dp0M))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp0M, percent: entry.dp0MValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp1M))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp1M, percent: entry.dp1MValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp2M))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp2M, percent: entry.dp2MValidityPercent))
//          Text("\(doubleOptToString(value: entry.dp3M))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dp3M, percent: entry.dp3MValidityPercent))
//          Text("|")
//            .font(.body)
//            .foregroundColor(.black)
//          Text("\(doubleOptToString(value: entry.dpY2D))")
//            .font(.body)
//            .foregroundColor(doubleOptToColor(value: entry.dpY2D, percent: entry.dpY2DValidityPercent))
//        }
//      }
//    }.onAppear {
//    }
//  }
}

struct Fund4WView_Previews: PreviewProvider {
  static var previews: some View {
    DP4WRowView(title: "PPM", dp4ModelKey: "PPM")
    }
}
