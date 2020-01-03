//
//  TestView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/20/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct TestView: View {
  
  @EnvironmentObject var settings: AppDataObservable
  
  var body: some View {
    Text("TestView")
  }
}

struct TestView_Previews: PreviewProvider {
    static var previews: some View {
      TestView()
    }
}
