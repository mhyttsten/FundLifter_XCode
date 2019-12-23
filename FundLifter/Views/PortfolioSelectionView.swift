//
//  PortfolioView.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/29/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import SwiftUI

struct PortfolioSelectionView: View {
  @EnvironmentObject var settings: AppDataObservable
  @Environment(\.presentationMode) var presentationMode
  @State var items: [String] = [String]()
  @State var selections: [String] = [String]()
  public var portfolioName: String
  public var useFQName: Bool = false
  
  init(portfolioName: String) {
    self.portfolioName = portfolioName
    if portfolioName == FLConstants.PORTFOLIO_ARCS {
      useFQName = true
    }
  }

  var body: some View {
    VStack {
      List {
        ForEach(self.items, id: \.self) { item in
          MultipleSelectionRow(title: item, isSelected: self.selections.contains(item)) {
            if self.selections.contains(item) {
              self.selections.removeAll(where: { $0 == item })
            }
            else {
              self.selections.append(item)
            }
          }
        }
      }
      Spacer()
      Button(action: {
        AppDataObservable._portfolios[self.portfolioName] = [D_FundInfo]()
        for fname in self.selections {
          let typeAndName = self.useFQName ? fname : "\(self.portfolioName).\(fname)"
          let fundInfo = AppDataObservable._typeAndName2Fund[typeAndName]!
          AppDataObservable._portfolios[self.portfolioName]!.append(fundInfo)
        }
        AppDataObservable._portfolios[self.portfolioName]!.sort { (a1: D_FundInfo, a2: D_FundInfo)->Bool in
          return a1.typeAndName < a2.typeAndName
        }
        let s = PortfolioIO.write()
        let s1 = "Writing portfolio file, result is: \(s)"
        print(s1)
        logFileAppend(s: s1)
        
        DispatchQueue.main.async {
          let pname = self.portfolioName
          let dp4ModelP = DataModelsCalculator.getDP4WModelForPortfolio(name: pname, funds: AppDataObservable._portfolios[pname]!)
          self.settings.dp4ModelHM[pname] = dp4ModelP
        }
        self.presentationMode.wrappedValue.dismiss()
      }) {
        Text("Done")
      }.padding(.top, 50)
    }.onAppear {
      print("onAppear")
      DispatchQueue.main.async { [self] in
        // Add portfolio funds in beginning of list and selected
        for fi in AppDataObservable._portfolios[self.portfolioName]! {
          let name = self.useFQName ? fi.typeAndName : fi._nameMS
          self.items.append(name)
          self.selections.append(name)
        }

        // Then add all other funds of type 'portfolioName', eg 'PPM'
        if self.portfolioName == FLConstants.PORTFOLIO_ARCS {
          for fi in AppDataObservable._allFunds {
            if !self.items.contains(fi.typeAndName) {
              self.items.append(fi.typeAndName)
            }
          }
        } else {
          for fi in AppDataObservable._type2Funds[self.portfolioName]! {
            if !self.items.contains(fi._nameMS) {
              self.items.append(fi._nameMS)
            }
          }
        }
      }
    }
  }
}

struct MultipleSelectionRow: View {
    var title: String
    var isSelected: Bool
    var action: () -> Void

    var body: some View {
        Button(action: self.action) {
            HStack {
                Text(self.title)
                if self.isSelected {
                    Spacer()
                    Image(systemName: "checkmark")
                }
            }
        }
    }
}



