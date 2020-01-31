//
//  MainView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct MainView: View {
  @EnvironmentObject var settings: AppDataObservable
  
  @State private var show_modal: Bool = false
  
  var body: some View {
    NavigationView {
      VStack {
        HStack {
          NavigationLink(destination: TopFundsView(
            title: "Indexes",
            rowType: .INDEXES,
            dp4ws: self.settings.pubIndexes,
            showTypePrefix: false,
            showTopFundsButton: false,
            showUpdatePortfolioButton: false,
            rowLimit: nil)) {
            Text("Indexes").bold()
          }
          Spacer()
        }

        PortfolioListView().environmentObject(self.settings)

        NavigationLink(destination: TestView()) { Text("TestView") }

        Text("\(settings.pubMessage)").bold()  // .padding(.bottom, 50)
        Text("\(settings.pubFundDBCreationTime) [\(dateLastFridaysAsYYMMDD()[0])]")
        Button(action: {
          self.show_modal = true
        }) {
          Text("Refresh DB")
        }.sheet(isPresented: self.$show_modal) {
          UpdateDBModalView().environmentObject(self.settings)
        }
      }
      .navigationBarTitle(Text("Portfolios"))
      // Cannot do this because of bug when pushing 'Back'
      //              .navigationBarItems(trailing:
      //                NavigationLink(destination: LogView()) {
      //                  Text("Help")
      //                })
      .navigationBarItems(leading: profileButton)  // ??? Cannot do this because of bug when pushing 'Back'
      .navigationBarItems(trailing: profileButton) // ??? Cannot do this because of bug when pushing 'Back'
      .onAppear {
        print("*** MainSwift.onAppear")
        self.settings.initialize()
      }
    }
  }
  
  private var profileButton: some View {
      Button(action: { }) {
          Image(systemName: "person.crop.circle")
      }
  }
}

struct PortfolioListView: View {
  @EnvironmentObject var settings: AppDataObservable

//  NavigationLink(destination: TopFundsView(portfolioName: portfolioName)) {
//    Text("Top Funds").bold()
//  }
  var body: some View {
    List {
      ForEach(settings.pubPortfolios) { dp4w in
        self.link(dp4w: dp4w,
                  destination: TopFundsView(
                    title: dp4w.id,
                    rowType: .FUNDS,
                    dp4ws: self.settings.pubPortfolio2DP4Funds[dp4w.id]!,
                    showTypePrefix: false,
                    showTopFundsButton: D_FundInfo.PORTFOLIO_TYPES.contains(dp4w.id),
                    showUpdatePortfolioButton: true))
      }
    }
//    .navigationBarTitle(Text("Menu"), displayMode: .large)
  }

  private func link<Destination: View>(dp4w: DP4WModel, destination: Destination) -> some View {
    return NavigationLink(destination: destination)  {
      DP4WRowView(displayName: dp4w.displayName, dp4ModelKey: dp4w.id).environmentObject(self.settings)
    }
  }

  // This is how cool you can go
  //  private func link<Destination: View>(icon: String, label: String, destination: Destination) -> some View {
  //    return NavigationLink(destination: destination) {
  //      HStack {
  //        Image(systemName: icon)
  //        Text(label)
  //      }
  //    }
}

struct MainView_Previews: PreviewProvider {
    static var previews: some View {
        MainView()
    }
}
