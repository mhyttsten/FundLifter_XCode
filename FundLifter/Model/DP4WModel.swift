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
  
  public var dp0M: Double? = nil
  public var dp0MValidityPercent: Double = 0.0

  public var dp0W: Double? = nil
  public var dp0WValidityPercent: Double = 0.0
  public var dp1W: Double? = nil
  public var dp1WValidityPercent: Double = 0.0
  public var dp2W: Double? = nil
  public var dp2WValidityPercent: Double = 0.0
  public var dp3W: Double? = nil
  public var dp3WValidityPercent: Double = 0.0
  
  public var dp1M: Double? = nil
  public var dp1MValidityPercent: Double = 0.0
  public var dp2M: Double? = nil
  public var dp2MValidityPercent: Double = 0.0
  public var dp3M: Double? = nil
  public var dp3MValidityPercent: Double = 0.0
  
  public var dpY2D: Double? = nil
  public var dpY2DValidityPercent: Double = 0.0
}
