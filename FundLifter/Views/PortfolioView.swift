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
  public var funds: [DP4WModel] {
    AppDataObservable._portfolios[portfolioName]!.map {
      var dp4 = self.settings.dp4ModelHM[$0.typeAndName]!
      if portfolioName == FLConstants.PORTFOLIO_ARCS {
        dp4.displayName = $0.typeAndName
      }
      return dp4
    }
  }
  
  var body: some View {
    VStack {
      HStack {
        NavigationLink(destination: TopFundsView(portfolioName: portfolioName)) {
          Text("Top Funds").bold()
        }
        .navigationBarTitle("\(portfolioName)")
        Spacer()
      }
      List {
        ForEach(funds)	 { fundDP4WModel in
          DP4WRowView(displayName: fundDP4WModel.displayName, dp4ModelKey: fundDP4WModel.id)
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
    }.onAppear {
    }
  }
}

struct PortfolioView_Previews: PreviewProvider {
    static var previews: some View {
      PortfolioView(portfolioName: "PPM")
    }
}
