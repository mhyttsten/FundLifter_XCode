import Foundation


// Use as reference type
// We never modify this, but we're shuffling around it in many data structures
public class D_FundInfo {
  public static let D_FLOAT_NULL = Float.greatestFiniteMagnitude
  
  public static let TYPE_PPM = "PPM"
  public static let TYPE_SEB = "SEB"
  public static let TYPE_SPP = "SPP"
  public static let TYPE_VGD = "VGD"
  public static let TYPES = [ TYPE_SEB, TYPE_SPP, TYPE_VGD, TYPE_PPM ]
  
  public var id : String { "\(_type).\(_nameMS)" }
  public var typeAndName: String { "\(_type).\(_nameMS)" }
  public var _url = ""
  public var _isValid = true
  public var _errorCode = 0
  public var _lastExtractInfo = ""
  public var _type = ""
  public var _nameMS = ""
  public var _nameOrig = ""
  public var _dateUpdated = ""
  public var _dateUpdateAttempted = ""
  public var _msRating = 0
  public var _ppmNumber = ""
  public var _categoryName = ""
  public var _indexName = ""
  public var _currencyName = ""
  
  public var _dpYears = [D_FundDPYear]()
  public var _dpDays = [D_FundDPDay]()
}

public struct D_FundDPYear {
  public var _year = 0
  public var _yieldFund: Double?
  public var _yieldCategory: Double?
  public var _yieldIndex: Double?
}

public struct D_FundDPDay {
  public var _dateYYMMDD = ""
  public var _dateYYMMDD_Actual = ""
  public var _currency = ""
  public var _r1d: Double?
  public var _r1w: Double?
  public var _r1m: Double?
  public var _r3m: Double?
  public var _r6m: Double?
  public var _r1y: Double?
  public var _r3y: Double?
  public var _r5y: Double?
  public var _r10y: Double?
  public var _rYTDFund: Double?
  public var _rYTDCategory: Double?
  public var _rYTDIndex: Double?
  
  public var description: String {
    return "\(_dateYYMMDD), 1w: \(doubleOptToString(value: _r1w)), 1m: \(doubleOptToString(value: _r1m))"
    
  }
}

