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
  @State var quantity: Int = 4
  @State var isReturns = false // toggle state
  @State var durationMessage = "Error"
  @State var funds = [DP4WModel]()
  public var portfolioName: String
  
  var body: some View {
    VStack {
//      Toggle("Use returns", isOn: $isReturns)

      Toggle(isOn: $isReturns) {
        Text("Use returns\(self.initializeFundList())")
      }
      
      Stepper(onIncrement: {
        if self.quantity <= 9 {
          self.quantity += 1
          self.initializeFundList()
//          DispatchQueue.main.async { self.durationMessage = "\(self.quantity)" }
        }
      }, onDecrement: {
        if self.quantity > 1 {
          self.quantity -= 1
          self.initializeFundList()
        }
      }, label: { Text("Top Funds for Last: \(durationMessage)") })
      
      Spacer()
      List {
        ForEach(funds)   { fund in
          DP4WRowView2(displayName: fund.displayName, model: fund)
        }
      }
      Spacer()
    }
    .onAppear {
      print("TestView.onAppear")
      self.initializeFundList()
    }
  }
  
  func initializeFundList() -> String {
    var rv = "Error"
    var incl = [DP4WModel]()
    if quantity <= 4 {
      // print("Trying to find pn: \(portfolioName)")
      let dps = settings.portfolio2DP4Funds[portfolioName]!
      (incl, _) = DataModelsCalculator.getPosition(dps: dps,
                                                   weekCount: quantity, monthCount: nil,
                                                   isReturn: isReturns)
      rv = "\(quantity) Weeks"
    }
    if quantity >= 5 && quantity <= 8 {
      (incl, _) = DataModelsCalculator.getPosition(dps: settings.portfolio2DP4Funds[portfolioName]!,
                                                   weekCount: nil, monthCount: quantity-4,
                                                   isReturn: isReturns)
      rv = "\(quantity-4) Months"
    }
    if quantity == 9 {
      (incl, _) = DataModelsCalculator.getPositionY2D(dps: settings.portfolio2DP4Funds[portfolioName]!)
      rv = "Year to Date"
    }
    if incl.count > 50 {
      incl = Array(incl[0..<50])
    }
    
    DispatchQueue.main.async {
      self.funds.removeAll()
      for (idx,f) in incl.enumerated() {
        var f = f
        f.displayName = "\(idx): \(f.displayName)"
        self.funds.append(f)
      }
      self.durationMessage = rv
    }
    return ""
  }
}

struct TestView_Previews: PreviewProvider {
    static var previews: some View {
      TestView(portfolioName: D_FundInfo.TYPE_PPM)
    }
}
