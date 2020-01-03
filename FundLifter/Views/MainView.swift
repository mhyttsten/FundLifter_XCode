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
        PortfolioListView().environmentObject(self.settings)

        NavigationLink(destination: TestView()) { Text("TestView") }

        Text("\(settings.message)").bold()  // .padding(.bottom, 50)
        Text("\(settings.fundDBCreationTime)")
        Button(action: {
          self.show_modal = true
        }) {
          Text("Refresh DB")
        }.sheet(isPresented: self.$show_modal) {
          UpdateDBModalView().environmentObject(self.settings)
        }
      }
      .navigationBarTitle(Text("Main: \(dateLastFridaysAsYYMMDD()[0])"))
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

  var body: some View {
    List {
      ForEach(settings.portfolios) { dp4W in
        self.link(dp4W: dp4W, destination: PortfolioView(portfolioName: dp4W.id))
      }
    }
    .navigationBarTitle(Text("Menu"), displayMode: .large)
  }

  private func link<Destination: View>(dp4W: DP4WModel, destination: Destination) -> some View {
    return NavigationLink(destination: PortfolioView(portfolioName: dp4W.displayName).environmentObject(self.settings)) {
      DP4WRowView(displayName: dp4W.displayName, dp4ModelKey: dp4W.id).environmentObject(self.settings)
    }
  }
  
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

//struct MainView: View {
//  @EnvironmentObject var settings: AppDataObservable
//
//  @State private var show_modal: Bool = false
//
//  var body: some View {
//    NavigationView {
//      VStack {
//        List {
//          ForEach(settings.portfolios) { dp4W in
//            NavigationLink(destination: PortfolioView(portfolioName: dp4W.displayName).environmentObject(self.settings)) {
//              DP4WRowView(displayName: dp4W.displayName, dp4ModelKey: dp4W.id).environmentObject(self.settings)
//            }
//          }
//        }
//
//        NavigationLink(destination: TestView(portfolioName: D_FundInfo.TYPE_PPM)) {
//          Text("TestView").bold()
//        }
//
//        Text("\(settings.message)").bold()  // .padding(.bottom, 50)
//        Text("\(settings.fundDBCreationTime)")
//        Button(action: {
//          self.show_modal = true
//        }) {
//          Text("Refresh DB")
//        }.sheet(isPresented: self.$show_modal) {
//          UpdateDBModalView().environmentObject(self.settings)
//        }
//      }
//      .navigationBarTitle(Text("Main: \(dateLastFridaysAsYYMMDD()[0])"))
//      // Cannot do this because of bug when pushing 'Back'
////              .navigationBarItems(trailing:
////                NavigationLink(destination: LogView()) {
////                  Text("Help")
////                })
//      }.onAppear {
//        self.settings.initialize()
//    }
//  }
//}
//
//struct MainView_Previews: PreviewProvider {
//    static var previews: some View {
//        MainView()
//    }
//}
