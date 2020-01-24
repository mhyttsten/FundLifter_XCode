//
//  TestView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/20/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct TestView: View {
  var body: some View {
    Text("TestView")
  }
}

struct TestView2: View {
  var t: TestView?
  
  var body: some View {
    Text("TestView2")
  }
}


//struct TestView<T: View>: View {
//  @EnvironmentObject var settings: AppDataObservable
//  var view: T
//
//  init() { }
//
//  init(callback: (String)->T) {
//
//  }
//
//  var body: some View {
//    Text("TestView")
//  }
//}

struct TestView_Previews: PreviewProvider {
    static var previews: some View {
      TestView()
    }
}
