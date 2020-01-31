//
//  FundView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 1/25/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct FundView: View {
  public var fund: D_FundInfo
  
  init(typeAndName: String) {
    self.fund = AppDataObservable._typeAndName2Fund[typeAndName]!
  }
  
  var body: some View {
    Text("Hello World from fund: \(fund.typeAndName)")
  }
}

