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
  
  public var funds: [DP4WModel] {
    return getFundList()
  }

  public enum ViewType {
    case TYPE
    case PORTFOLIO
  }
  public var portfolioName: String
  public var type: ViewType = .TYPE
  public var title: String = ""
  
  public func hasTopFunds() -> Bool {
    if type == .PORTFOLIO && D_FundInfo.PORTFOLIO_TYPES.contains(portfolioName) {
      return true
    } else {
      return false
    }
  }
  
  public func isTopFunds() -> Bool {
    if type == .TYPE && D_FundInfo.PORTFOLIO_TYPES.contains(portfolioName) {
      return true
    } else {
      return false
    }
  }

  public init(portfolioName: String, type: ViewType) {
    self.portfolioName = portfolioName
    self.type = type
    if isTopFunds() {
      self.title = "Top Funds: \(portfolioName)"
    } else {
      self.title = "\(portfolioName)"
    }
  }
  
  var body: some View {
    VStack {
      if hasTopFunds() {
        HStack {
          NavigationLink(destination: TopFundsView(portfolioName: portfolioName, type: .TYPE)) {
            Text("Top Funds").bold()
          }
          .navigationBarTitle("\(portfolioName)")
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
        ForEach(funds)   { fund in
          DP4WRowView2(displayName: fund.displayName, model: fund)
        }
      }
      Spacer()
      
      if !isTopFunds() {
        Button(action: {
          self.show_modal = true
        }) {
          Text("Update Portfolio")
        }.sheet(isPresented: self.$show_modal) {
          PortfolioSelectionView(portfolioName: self.portfolioName).environmentObject(self.settings)
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
    var dps = type == .TYPE ? settings.pubType2DP4Funds[portfolioName]! : settings.pubPortfolio2DP4Funds[portfolioName]!

    if quantity <= 4 {
      (incl, excl) = DataModelsCalculator.getPosition(dps: dps,
                                                      weekCount: quantity, monthCount: nil,
                                                      isReturn: isReturns)
    }
    if quantity >= 5 && quantity <= 8 {
      (incl, excl) = DataModelsCalculator.getPosition(dps: dps,
                                                   weekCount: nil, monthCount: quantity-4,
                                                   isReturn: isReturns)
    }
    if quantity == 9 {
      (incl, excl) = DataModelsCalculator.getPositionY2D(dps: dps)
    }
    let countIncl = incl.count
    let countExcl = excl.count
    let countTotal = countIncl + countExcl
    let coverageMessage = "\(doubleOptToString(value: Double(Double(countIncl)/Double(countTotal)*100)))%"
      + " (\(countExcl)/\(countTotal) excluded)"
    
    if incl.count > 50 {
      incl = Array(incl[0..<50])
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


