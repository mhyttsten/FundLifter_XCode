//
//  TopFundsView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 1/3/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct TopFundsView: View {
  
  @EnvironmentObject var settings: AppDataObservable
  @State var quantity: Int = 4
  @State var isReturns = false // toggle state
  @State var coverageMessage = "Not calculated"
  @State private var show_modal: Bool = false
  
  public var title: String = ""
  public var dp4ws = [DP4WModel]()
  public var showTopFundsButton = false
  public var showUpdatePortfolioButton = false
  public var rowLimit: Int? = nil
  
  public init(title: String,
              dp4ws: [DP4WModel],
              showTopFundsButton: Bool,
              showUpdatePortfolioButton: Bool,
              rowLimit: Int? = 50) {
    self.title = title
    self.dp4ws = dp4ws
    self.showUpdatePortfolioButton = showUpdatePortfolioButton
    self.showTopFundsButton = showTopFundsButton
    self.rowLimit = rowLimit
  }
  
  var body: some View {
    VStack {
      if showTopFundsButton {
        HStack {
          NavigationLink(destination: TopFundsView(title: title,
                                                   dp4ws: settings.pubType2DP4Funds[title]!,
                                                   showTopFundsButton: false,
                                                   showUpdatePortfolioButton: false)) {
            Text("Top Funds").bold()
          }
          Spacer()
        }
      }
      Spacer()
      
      Text("\(self.title)")
        .font(.title)
        .bold()
      Text("Coverage: \(coverageMessage)")

      Toggle(isOn: $isReturns) {
        Text("Use returns")
      }
      
      Stepper(onIncrement: {
        if self.quantity <= 9 {
          self.quantity += 1
//          self.initializeFundList()
        }
      }, onDecrement: {
        if self.quantity > 1 {
          self.quantity -= 1
//          self.initializeFundList()
        }
      }, label: { Text("Duration: \(getListDuration())") })
      
      List {
        ForEach(getFundList())   { dp4w in
          DP4WRowView2(displayName: dp4w.displayName, model: dp4w)
        }
      }
      Spacer()
      
      if showUpdatePortfolioButton {
        Button(action: {
          self.show_modal = true
        }) {
          Text("Update Portfolio")
        }.sheet(isPresented: self.$show_modal) {
          PortfolioSelectionView(portfolioName: self.title).environmentObject(self.settings)
        }
      }
    }
    .onAppear {
      print("TestView.onAppear")
//      self.initializeFundList()
    }
  }
  
  func getListDuration() -> String {
    if quantity <= 4 {
      return "\(quantity) Weeks"
    }
    if quantity >= 5 && quantity <= 8 {
      return "\(quantity-4) Months"
    }
    if quantity == 9 {
      return "Year to Date"
    }
    return "ERROR"
  }
  
  func getFundList() -> [DP4WModel] {
    var incl = [DP4WModel]()
    var excl = [DP4WModel]()

    if quantity <= 4 {
      (incl, excl) = DataModelsCalculator.getPosition(dps: dp4ws,
                                                      weekCount: quantity, monthCount: nil,
                                                      isReturn: isReturns)
    }
    if quantity >= 5 && quantity <= 8 {
      (incl, excl) = DataModelsCalculator.getPosition(dps: dp4ws,
                                                      weekCount: nil, monthCount: quantity-4,
                                                      isReturn: isReturns)
    }
    if quantity == 9 {
      (incl, excl) = DataModelsCalculator.getPositionY2D(dps: dp4ws)
    }
    let countIncl = incl.count
    let countExcl = excl.count
    let countTotal = countIncl + countExcl
    let coverageMessage = "\(doubleOptToString(value: Double(Double(countIncl)/Double(countTotal)*100)))%"
      + " (\(countExcl)/\(countTotal) excluded)"
    
    if rowLimit != nil && incl.count > rowLimit! {
      incl = Array(incl[0..<rowLimit!])
    }
    
    DispatchQueue.main.async {
      self.coverageMessage = coverageMessage
//      self.fundList.removeAll()
      for (idx,f) in incl.enumerated() {
        var f = f
        f.displayName = "\(idx): \(f.displayName)"
//        self.fundList.append(f)
      }
    }
    return incl
  }
}


