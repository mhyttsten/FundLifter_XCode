/*
 Document: https://docs.swift.org/swift-book/LanguageGuide/BasicOperators.html
 Todo: https://docs.swift.org/swift-book/LanguageGuide/CollectionTypes.html
 
 ***********************************************************
 The Basics
 
Simple types: Int, Float, Bool
Primary collection types: Array, Set, Dictionary
Advanced types: Tuples
 
 Optionals: At the heart of many advanced Swift features

 let a=0.0, b=1.0
 var red, green, blue: Double
 var/let names are unicode, but cannot start with a number
 You can use a Swift reserved word for let/var if you enclose it in `

 Best practice:
 
 Comments: /* /* */ */ can be nested
 Semicolon: Supported but not required unless >1 stmt on a line

 Integer: 8, 16, 32, 64 signed an unsigned, e.g. Int32
 - Int: Platforms native word-size
 - Int32.min, Int32.max
 - Float (32-bit, >=6-decimal digits)
 - Double (64-bit, 15-decimal digits) also default type inference: let pi = 3.14
 
 Best practice:
 - Don't provide explicit type in let/var declarations (let a: String = "")
 - Use Int event if it's unsigned, only use UInt if for specfic reasons
 - Use Double and not Float unless you have specific reasons
   One reason to use default Int/Double = default type inference types
 - Use tuples only for simple things (common: return value). Use class/struct for complex things
 - Implicitly unwrapped variables - probably never use them
 */

func slg_01() {
  let a = 3 + 0.14; print(a)  // a will be a Double
  // Error: let tooBig = Int.max + 1

}

/*
 Integer literals prefix: No prefix (decimal), 0b (binary), 0o (octal), 0x (hex)
 Float literals prefix (must have a number on each side of decimal point):
 - Decimal=no prefix. Optional exponent with 'e'. let decimalDouble = 12.1875, let exponentDouble = 1.21875e1
 - 0x=hex prefix. Must have exponent 'e'.
    - Must always have a number on each side of decimal point
    - Can also have optional exponent
    - 0xFp-2 means 15 x 2-2, or 3.75: let hexadecimalDouble = 0xC.3p0
 let paddedDouble = 000123.456
 let oneMillion = 1_000_000
 let justOverOneMillion = 1_000_000.000_000_1
 
 Type conversion needed:
 let twoThousand: UInt16 = 2_000
 let one: UInt8 = 1
 let twoThousandAndOne = twoThousand + UInt16(one)
 
 Floats are always truncated: let a = Int(3.75) becomes 3
 
 ****
 
 typealias AudioSample = UInt16
 print(AudioSample.min)

 Bool, true/false

 Tuples:
 let httpError = (404, "Not found")
 let (code, _) = httpError
 print(httpError.0)
 let httpStatus = (statusCode: 200, desc: "Ok")
 print(httpStatus.statusCode)
 
 Optionals:
 let a = "123"
 let b = Int(a)  // This Int initializer returns type Int?
 var c: Int?  // Will assign nil as default value
 Force-unwrap: print(c!)
 Optinal binding:
    if let/var d = Int(a) { print(d) }  // d is only available in if-block
    else { print("No value") }
    if let a1=Int(a), let b1=Int(b), a < b && b > 100 { ... }

    Implicitly unwrapping
    let a: String! = "Un implicitly unwrapped optional string"
    let b: String = a  // b is not an option, a is implicitly unwrapped
    You can still use a as an optional: if let c = a  { ... }

 Error Handling
 func f() throws { ... }
 do {  // You must always encode 'try' in a 'do' block
    try f()
   // No error was thrown
 } catch [type] { }
[type] can be:
 - Nothing
 - A.b (type)
 - A.b(let c)  // error with values

 Assertions: Only debug builds
    assert(age>=0, "Not expected negative")
    assert(age>=0)
    assertionFailure("Software error")
 Precondition: Debug as well as Production builds
    Same signatures as above
 Fatal error
    fatalError("Unsupported")
 Build flags:
    -Ounchecked: Preconditions are not checked, but fatalError still halts
 
***********************************************************
 Basic Operators
 
 
 
 */
