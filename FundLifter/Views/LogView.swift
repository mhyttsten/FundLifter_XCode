//
//  LogView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct LogView: View {
  var logContent = ""
  
  var body: some View {
    HStack {
      Text("Log Content")
        .font(.title)
      Text(logContent)
        .font(.body)
    }
  }
}

struct LogView_Previews: PreviewProvider {
    static var previews: some View {
        LogView()
    }
}
