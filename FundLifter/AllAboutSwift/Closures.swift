//
//  Closures.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/30/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation


// In place sort
//AppDataObservable._portfolios[self.portfolioName]!
//   .sort { (a1: D_FundInfo, a2: D_FundInfo)->Bool in
//  return a1._nameMS < a2._nameMS
//}

// Obs: You could get warning unless you use $0 in closure
// let a: [(Double, Int)] = winners.map { (a: DP4WModelPosition)->(Double,Int) in { ... }
// let a: [(Double, Int)] = winners.map { ... }
// let a: [(Double, Int)] = winners.map { (a: DP4WModelPosition)->(Double,Int) in { ... }
// let a: [(value:Double, idx:Int)] = winners.map { (a: DP4WModelPosition)->(Double,Int) in { ... }

