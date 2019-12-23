//
//  TestView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/20/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI


// Name: YTD[#]
// 1M[#], ..., 4m[#]
// Score is 1.0, 0.8, 0.8^2, ...
// Quantity == 0, the YTD, other entries are nil
// If nil, then use average between last 2 months with YELLOW
// At end print how many were disqualified
// Also print stats on how many 1M are missing, i.e # disqualified from list
struct TestView: View {
  @State var quantity: Int = 6
  public var portfolioName: String
  
  var body: some View {
    VStack {
      //        Text(/*@START_MENU_TOKEN@*/"Hello, World!"/*@END_MENU_TOKEN@*/)
      Stepper("Top Funds for Last: \(getTimeDescription())", value: $quantity, in: 1...13)
    }
    .onAppear {
      print("TestView.onAppear")
      
    }
  }
  
  func getTimeDescription() -> String {
    if quantity <= 6 {
      return "\(quantity) Weeks"
    }
    if quantity >= 7 && quantity <= 12 {
      return "\(quantity-6) Months"
    }
    if quantity == 13 {
      return "Year to Date"
    }
    fatalError("Should not arrive here")
  }
}


struct TestView_Previews: PreviewProvider {
    static var previews: some View {
      TestView(portfolioName: D_FundInfo.TYPE_PPM)
    }
}
