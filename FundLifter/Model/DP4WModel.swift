//
//  Fund4WModel.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/11/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Foundation

public struct DP4WModel: Identifiable {
  
  init(fqName: String, displayName: String) {
    self.displayName = displayName
    self.id = fqName
  }
  
  public var description: String {
    return "\(id): \(displayName)" +
      "\n   dpWs: \(doubleOptsToString(value: dpWs))" +
      "\n   dpWsVP: \(doubleOptsToString(value: dpWsValidityPercent))" +
      "\n   dpMs: \(doubleOptsToString(value: dpMs))" +
      "\n   dpMsVP: \(doubleOptsToString(value: dpMsValidityPercent))" +
      "\n   dpY2D:\(doubleOptToString(value: dpY2D))" +
      "\n   dpY2DVP:\(String(format: "%0.2f", dpY2DValidityPercent))"
  }
  
  public var id = ""
  public var displayName = ""
  public var ranking: Double?
  
  public var dpWs = [Double?]()
  public var dpWsValidityPercent = [Double?]()
  public var dpWsPos = [Int]()
  
  public var dpMs = [Double?]()
  public var dpMsValidityPercent = [Double?]()
  public var dpMsPos = [Int]()
  
  public var dpY2D: Double? = nil
  public var dpY2DValidityPercent: Double = 0.0
}
