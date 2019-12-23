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
  
  public var id = ""
  public var displayName = ""
  
  public var dpWs = [Double?]()
  public var dpWsValidityPercent = [Double?]()
  public var dpMs = [Double?]()
  public var dpMsValidityPercent = [Double?]()
  public var dpY2D: Double? = nil
  public var dpY2DValidityPercent: Double = 0.0
}
