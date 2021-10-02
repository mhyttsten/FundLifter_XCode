////
////  Fund4wView.swift
////  FundLifter
////
////  Created by Magnus Hyttsten on 11/11/19.
////  Copyright © 2019 Magnus Hyttsten. All rights reserved.
////
//
import SwiftUI

struct DP4WRowView2: View {
  @EnvironmentObject var settings: AppDataObservable
  var displayName: String = ""
  var model: DP4WModel

  init(displayName: String, model: DP4WModel) {
    self.displayName = displayName
    self.model = model
  }
  
  func getString(isW: Bool=false, isM: Bool=false, isY: Bool=false, index: Int?=nil) -> String {
    
//    if model.displayName.starts(with: "28: Core Ny Teknik A") {
//      print("Hello")
//    }
    
    var v: Double? = nil
    var vpos: Int? = nil

    if isW {
      v = model.dpWs[index!]
      if model.dpWsPos.count > index! {
        vpos = model.dpWsPos[index!]
      }
    } else if isM {
      v = model.dpMs[index!]
      if model.dpMsPos.count > index! {
        vpos = model.dpMsPos[index!]
      }
    } else if isY {
      v = model.dpY2D
    } else {
      fatalError("Unexpected category")
    }
    // print("Fund. Index: \(index), \(model.displayName), \(model.dpWs)")
    var s = doubleOptToString(value: v)
    if let vpos=vpos {
      s += ":\(vpos)"
    }
    return s
  }
  
  func getColor(isW: Bool=false, isM: Bool=false, isY: Bool=false, index: Int?=nil) -> Color {
    var v: Double? = nil
    var vper: Double? = nil
    
    if isW {
      v = model.dpWs[index!]
      vper = model.dpWsValidityPercent[index!]
    } else if isM {
      v = model.dpMs[index!]
      vper = model.dpMsValidityPercent[index!]
    } else if isY {
      v = model.dpY2D
      vper = model.dpY2DValidityPercent
    } else {
      fatalError("Unexpected category")
    }
    let s = doubleOptToColor(value: v, percent: vper)
    return s
  }

  var body: some View {
    VStack(alignment: .trailing) {
      HStack {
        Text(displayName).font(.body)
        Spacer()
        Text("Y2D").font(.body)
        Text("\(getString(isY:true))")
          .font(.body)
          .foregroundColor(getColor(isY: true))
      }
      VStack {
        HStack {
          Text("\(model.subTitle)")
          Spacer()
        }
        HStack {
          Text("Ranking: \(doubleOptToString(value: model.ranking))")
          Spacer()
        }

        HStack {
          Text("Ws").font(.subheadline)
          Spacer()
          Text("\(getString(isW: true, index: 0))")
            .font(.subheadline)
            .foregroundColor(getColor(isW: true, index: 0))
          Text("\(getString(isW: true, index: 1))")
            .font(.subheadline)
            .foregroundColor(getColor(isW: true, index: 1))
          Text("\(getString(isW: true, index: 2))")
            .font(.subheadline)
            .foregroundColor(getColor(isW: true, index: 2))
          Text("\(getString(isW: true, index: 3))")
            .font(.subheadline)
            .foregroundColor(getColor(isW: true, index: 3))
        }
        HStack {
          Text("Ms").font(.subheadline)
          Spacer()
          Text("\(getString(isM: true, index: 0))")
            .font(.subheadline)
            .foregroundColor(getColor(isM: true, index: 0))
          Text("\(getString(isM: true, index: 1))")
            .font(.subheadline)
            .foregroundColor(getColor(isM: true, index: 1))
          Text("\(getString(isM: true, index: 2))")
            .font(.subheadline)
            .foregroundColor(getColor(isM: true, index: 2))
          Text("\(getString(isM: true, index: 3))")
            .font(.subheadline)
            .foregroundColor(getColor(isM: true, index: 3))
        }
      }
    }.onAppear {
//      print("Printing settings.dp4ModelHM: \(self.settings.dp4ModelHM[self.key]!)")
    }
  }
}

//struct Fund4WView2_Previews: PreviewProvider {
//  static var previews: some View {
////    DP4WRowView2(displayName: "PPM", dp4ModelKey: "PPM")
//  }
//}
