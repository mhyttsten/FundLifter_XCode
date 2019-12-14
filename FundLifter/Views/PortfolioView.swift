//
//  PortfolioView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct PortfolioView: View {
  @EnvironmentObject var settings: AppDataObservable
  @State private var show_modal: Bool = false
  public var portfolioName: String
  public var funds: [DP4WModel] { AppDataObservable._portfolios[portfolioName]!.map {
    DataModelsCalculator.getDP4WModelForFund(fund: $0)
    }
  }
  
  init(portfolioName: String) {
    self.portfolioName = portfolioName
  }
  
  var body: some View {
    NavigationView {
      VStack {
        List {
          ForEach(funds)	 { fundDP4WModel in
            DP4WRowView(title: fundDP4WModel._name, dp4ModelKey: fundDP4WModel.id)
//            DP4WRowView(entry: fundDP4WModel)
          }
        }
        Spacer()

        Button(action: {
          self.show_modal = true
        }) {
          Text("Update Portfolio")
        }.sheet(isPresented: self.$show_modal) {
          PortfolioSelectionView(portfolioName: self.portfolioName).environmentObject(self.settings)
        }
      }
      .navigationBarTitle(Text("\(portfolioName): \(dateLastFridayAsYYMMDD(inclusive: false))"))
//      Text("End of list")
    }.onAppear {
    }
  }
  
}

struct PortfolioView_Previews: PreviewProvider {
    static var previews: some View {
      PortfolioView(portfolioName: "PPM")
    }
}
