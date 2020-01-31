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
 - Readability ALWAYS has precedence over brevity (so group multiple && || expressions for example)
 - [Element] preferred over Array<Element>
 -

Protocols:
 - Collection: startIndex, endIndex, index(before:) (after:) (_:offsetBy)
    So works for String, Array, Dictionary, Set, etc
 - RangeReplaceableCollection: insert(_:at:) (contentsOf:at:), remove(at:), removeSubrange(_:)
 - Equatable: == operator overloaded
   a == a (reflexivity)
   a == b implies b == a (symmetry)
   a == b && b == c implies a == c (transitivity)
 - Error (empty protocol)
 
 - Hashable: Need to have hashValue: Int property
   hashValue must not be the same over different program executions
   Also requires conformance to Equatable
 - CaseIterable (i.e. allows you to iterate over enum cases: Enumb.allCases)

 Questions:
   - var str2 = str  // "str2 will have it's own storage", isn't COW implemented here? (from #3-String under Substrings)
   - StringProtocol: Both String and Substring implement it. What is it?
   -
 
 
 */

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
 
 Addition works for strings: "a" + "b"
 Modulo: -9 % 4 == -1
 Not equal to: !=

 (3, "apple") < (3, "bird") == true because "apple" < "bird"
 ("blue", false) < ("purple", true)  // Error, < does not work for Bool

 a ?? b  // Unwraps a if it contains a value, or b if it's nil

 a...b // Range between a and b inclusive
 a..<b // Includes a but not b
 for name in names[2...]  // All elements from index 2 to end
 for name in names[...2]
 for name in names[..<2]
 let r = ...5
 print(r.contains(-1)  // true

 a || b && !c  // && and || are evaluated left to right (so no built-in priority)
 // But you should use ( ) for readability which ALWAYS has precedence over brevity
 
 ***********************************************************
 Strings and Characters
 
 let/var determines if you can change string content
 
 String, Character types
 Allows access in various Unicode representations
 (Bridged with Foundations NSString, which also extends String to bring NSString methods to it)
 
 let theSame = "Hello how are you"
 let theSame = """
 Hello how are you
 """
 // """ lines don't contain any characters
 // You can use unescaped " inside """ block
 // Spaces before closing """ in all lines are not considered part of string (== nice source formatting)
 // You can use \ to make 2 lines below to a single line
 // Line feeds after starting and before ending """ are line feeds in resulting string
 
 // Escape: \0 (null), \\ (backslash), \t (tab), \n (lf), \r (cr), \" ("), \' (')
 //         \u{n}  1-8 digit hexadecimal number for Unicode character
 // String interpolation: \(<expression>)
 
 Extended string delimeters: Does not intepret escape or string interpolations
 #"Line1\nLine2"#  // Prints \n literarily and not a lf
 #"Line1\#nLine2"# // Prints lf
 let ml = #"""
 Lets print """
 """#
 
 let empty1 = ""
 let empty2 = String()
 empty1.isEmpty    // true
 empty1 == empty2  // true
 
 String is a value type, but fully optimized to only copy when necessary
 
 let chars: [Character] = ["a", "b"]
 let str = String(chars)
 += and + works to append strings
 let char: Character "c"
 str.append(char)

 String uses Unicode scalar values internally
 A 21-bit number, eg: U+0061 for "a" or U+1F425 for a chick emoji
 Not all 21-bit number are assign a character, some are reserved or used for UTF-16 encodings
 A character is a extended grapheme cluster (one or more Unicode scalars that when combined is a single character)
 These are valid Swift Character instances resulting in the same symbol: é
  "\u{E9}" == é (precomposed)
    "\u{65}\u{301}" == e followed by  ́ (decomposed)

 Counting string characters: str.count
 Note that appending characters to the string might not increase count - see decomposed above
 The count property requires String implementation to iterate through entire string
 NSString and String counts may return different values (NSString uses UTF-16)
 
 Indexing string - String.Index type specifies position of each character
 Size of a Character can vary so you cannot index using integers
 - for i in str.indices { print(str[i]) }
 - In empty string: startIndex == endIndex
 - str[str.startIndex]: Index of first character position in string
 - str[str.endIndex]:   Index AFTER last character index
 - str.index(before:) (after:) (_:offsetBy)  // Last skips multiple characters

 Inserting & removing: String implements RangeReplaceableCollection
 insert(_:at:) (contentsOf:at:), remove(at:)
 let range = str.index(str.endIndex, offsetBy: -4)..<str.endIndex
 str.removeSubrange(range)

 Substring is a separate type which points into the String instance
 (String must be keps in memory for as long as there are Substrings referring to it)
 
 String & Character equality tested using == and !=
    Characters are equals if they have same linguistic meaning and appearance, but not necessarily bytes
 String.hasPrefix(_:), hasSuffix(_:)
 
 Unicode scalars are encoded in an encoding form. You should select the biggest you need for your strings
 - str.utf8: String.UTF8View = [UInt8]
 - str.utf16: String.UTF16View == [UInt16]
 - str.unicodeScalars: UnicodeScalarView == [UInt32] (21-bit unicode scalars equivalent to UTF-32 encoding form)
 
 ***********************************************************
 Collection Types: Arrays (ordered), Sets (unordered), Dictionaries (unordered key:value)
    Implemented as generic collections
 
 let/var determines if you can change collection content
 
 *** Array (bridged to Foundation's NSArray)
   - Array<Element>
   - [Element]  // preferred

 var a = [Int]()
 a.append(3)
 a = []  // Ok, type is known

 var a1 = Array(repeating: 0.0, count: 3)
 var a3 = a1 + a2
 
 var a1: [String] = ["a", "b"]
 var a1 = ["a", "b"]  // preferred
 
 .count, .isEmpty
 .append(_:), +=, +
 .insert(_:at:)
 .remove(at:), .removeLast()  // Returns removed element
 [<index:Int>]  // Both r- and l-value
    a[0...1] = ["a1", "b1"]

 for e in array { }
 for (index, e) in array.enumerated() { }
 
 ** Sets (bridged to NSSet)
 
 - Use instead of array when order is irrelevant and uniqueness required
 - Elements must be Hashable, hash value is an Int
 
 - Swift basic types String, Int, Double, Bool are Hashable
   Also Enumeration case values without associated values
 
 var s = Set<Character>()
 s = []  // Ok, we know the type already
 var s: Set = ["AB", "BC"]  // Set required, otherwise Array assumed
     var s: Set = ["A"]  // Character or String?
 .count, .isEmpty
 .insert(_:)
 .remove(_:)
 .contains(_:)
 for a in s {}
 for a in s.sorted() {}   // Sorted uses '<' operator
 .intersection(_:), .symmetricDifference(_:), .union(_:), .subtracting(_:)
   Symmetric difference are all elements that are not shared
 ==, isSubsetOf(of:), isSupersetOf(of:), isStrictSubsetOf(of:), .isStrictSupersetOf(of:)
Superset, Subset, Disjoint (no common members)
 
 ** Dictionaries (bridged to NSDictionary)
 - Key must conform to Hashable protocol
 var d = [String: Int]()
 d = ["hello": 1, "goodbye": 2 ]
 d["hello"] = 10
 d["hello"] = nil  // Remove key from dictionary
 d = [:]  // Empty it, type already known

 .count, .isEmpty
 []  // As both r- and l-value, r-value return Value? type
 .updateValue(_:forKey:)  // Updates and returns Value? type (optional)
 
 for (k,v) in dict {}
 for k in dict.keys {}   // Could also use sorted()
 for v in dict.values {} // Could also use sorted()
 let asarray = [String](dict.keys)
  
 ***********************************************************
 Control Flows

 for a in array { }
 for (k,v) in dict { }
 for _ in 1...5 { }
 for i in stride(from: 0, to: 60, by: 5) { }       // 0, .., 55
 for i in stride(from: 0, through: 60, by: 5) { }  // 0, .., 60

  while <conditional> { }
 
  repeat {
  } while <conditional>
 
  if ...
 
  switch point {
  case (_,0), (0,_):      // On x or y axis
  case (1, let dist),  (let dist, 1):
  case (-1...1, -1...1):  // In a box
  case (let x,1):
  case let (x, y) where x == -y:
  default:
     break  // Break for clarity but could be left out
  }
  - Cases must match every value or a default must be provided
  - No breaks needed, no automatic fallthrough multiple cases
 
  continue // Start next iteration
  break  // Terminates loop (transfers execution to after loops '}') or switch statement
  fallthrough  // Last statement of a switch-case, causing execution of next case (even if it doesn't match)
 
  You can label all loops and switch statements
  <label_name>: <loop/switch>
    Now you can break or continue to this particular label
 
  guard <condition> { ... } else {
     // You must return, break, continue, throw, fatalError(_:file:line:)
     // to exit block where guard statement occurs
  }
  
  if #available(iOS 10, macOS 10.12, *) {
      // Use iOS 10 APIs on iOS, and use macOS 10.12 APIs on macOS
      // For any other platform (*), execute on minimal version given by deployment target
      // https://docs.swift.org/swift-book/ReferenceManual/Attributes.html#ID348
  } else {
      // Fall back to earlier iOS and macOS APIs
  }
 
 ***********************************************************
 Functions
 
 Void type is an empty tuple: ()
 func a()  // Type is () -> Void
 
 // alabel = used when you call function
 // pname  = used within the function
 // Omit argument label by putting '_' there
 func a(alabel pname: String) -> (min: Int, max: Int) {  // Optinal to specify tuple names
    (0, 10)  // No return needed for single expression
 }
 let b = a()
 print("\(b.min) \(b.max)")

 Best practice:
    - Put default function parameter at the end of the parameter list
 TODO:
    - Language reference shows compiler optimizations

 Variadic parameter - Variable argument list
 func a(_ pname: Double...) {  }  // pname.count, pname[...] }
 a(0.0, 1.0, 2.0)
 A function can have only one variadic parameter
 
 Function parameter are constants (let) by default
 
 // inout parameter cannot be variadic or have default values
 func a(param: inout Int)
 var b=10
 a(&b)  // Changes b
 
 Functions are variables
 var a1: (Int, Int) -> String = a
 var a2 = a  // Infer type
 
 Nested functions
 - Hidden by enclosing scope by default
 - A nested function can be returned enclosing one
 QUESTION: What about variable scope binding in this case?
 QUESTION: What about naming conventions: CAMEL stopCar, stop_car, for different declarations (and members), __x__, etc

 ***********************************************************
 Closures
 
 Closures "close over the constants and variables from the context in which they are defined"
 They take 3 forms:
 1. Global functions: Have a name and do not capture any values
 2. Nested functions: Have a name and capture values from their enclosing function (parameter, variables, ...)
 3. Closure expressions: Unnamed that capture values from surrounding context

 Functions and Closures are reference types
 
 { (parameters) -> returnType in
    statements
 }

 // sorted argument returns if 1st argument should be before 2nd
 var names = ["a", "b"]

 // Conclusion multi-line closure requires 'in' form?
 1. names = names.sorted(by: backward)  // backward is a func (String, String) -> Book
 2. names = names.sorted(by: { (s1: String, s2: String) -> Bool in
     return s1 > s2
    })
 3. names = names.sorted(by: { (s1, s2) -> Bool in return s1 > s2 })  // Partial closure type specification
 4. names = names.sorted(by: { s1, s2 in return s1 > s2 })
 5. names = names.sorted(by: { s1, s2 in s1 > s2 })
 6. names = names.sorted(by: { $0 > $1 })
 7. names = names.sorted(by: >)  // Because String overloads >
 8. names = names.sorted() { $0 > $1 }  // If by: closure is final argument
 9. names = names.sorted { $0 > $1 }    // If by: closure is only argument

 // Capturing context of enclosing function
 func makeIncrementer(forIncrement amount: Int) -> () -> Int {
     var runningTotal = 0
     func incrementer() -> Int {
         runningTotal += amount
         return runningTotal
     }
     return incrementer
 }
 var mi1 = makeIncrementer(10)
 var mi2 = mi1  // Share runningTotal, functions/closures are reference types
 var mi3 = makeIncrementer(20)  // Its own runningTotal
 QUESTION: So we're copying entire stack-frame of makeIncrementer to closure I guess
 
 1. Assigning a closure to a property of a class instance
 2. And it captures a class instance (by referring to its instance or members)
 3. You create a strong reference cycle between the closure and the instance
 QUESTION: What if the closure captures the instance but is not a property of the instance?
 
 Escaping closure:
    A closure passed as an argument to a function
    But is also called after the function returns (so either returned by function or assigned to variable outside it)
    Specify @escaping to indicate closure is allowed to escape
    func a(c: @escaping ()->Void) { ... }
 
 QUESTION: But why?
 var __closureEscaping: (()->Void)? = nil
 func closureEscaping(a: @escaping ()->Void) { __closureEscaping = a }
 func closureNotEscaping(a: ()->Void) { a() }
 class ClosureEscapes {
   var x = 10
   func doIt() {
     closureEscaping { [weak self] ()-> Void in self?.x = 10 }  // Works
     closureEscaping { self.x = 10 }  // But so does this, what is value of @escaping when I provide closure to closureEscaping?
     closureNotEscaping { x = 10 }
   }
 }
 
 AutoClosures - Wraps an argument expression into a closure of type ()->ExpressionType
    Syntactic sugar, write the expression, not the closure overhead code
    Common to call functions with ac but not define them (assert, ...)
 var customersInLine = ["a", "b"]
 func serve(customer customerProvider: @autoclosure () -> String) { print("\(customerProvider())") }
 serve(customer: customersInLine.remove(at: 0))  // Statement here automatically converts to a closure
 An AC can also escape, use syntax @autoclosure, @escaping
*/

var __closureEscaping: (()->Void)? = nil
func closureEscaping(a: @escaping ()->Void) { __closureEscaping = a }
func closureNotEscaping(a: ()->Void) { a() }
class ClosureEscapes {
  var x = 10
  func doIt() {
    closureEscaping { [weak self] ()-> Void in self?.x = 10 }  // Works
    closureEscaping { self.x = 10 }  // But so does this, what is value of @escaping when I provide closure to closureEscaping?
    // Also, changing 'class' to 'struct' requires 'mutating func doIt()...'
    //    And [weak self] does not work (needs class)
    //    So how can we have conditional self? What does that even mean for the closure for value-semantic structs?
    closureNotEscaping { x = 10 }
  }
}

/*
 ***********************************************************
 Enumerations
 
 Enums can be of String, Character, any Int, any Double
 Also supports:
   - Computed properties
   - Instance methods
   - Initializers (to provide initial case value)
   - Be extended, i.e supports: Properties, Methods, Initialization, Extensions, Protocols

 TODO: See Properties, Methods, Initialization, Extensions, Protocols
 
 enum A {
    case a0, ..., a9
    case 10
 }
 let a: A = .a0
 switch a {
   case .a0:
     print("a0")
 }
 
 enum Beverage: CaseIterable {
   case coffee, tea, juice
 }
 let choiceCount = Beverage.allCases.count
 for b in Beverage.allCases { print("\(b)")
 
 // Associated Values: Store additional information with enum cases
 enum Barcode {
     case upc(Int, Int, Int, Int)
     case qrCode(String)
 }
 var productBarcode = Barcode.upc(8, 85909, 51226, 3)
 productBarcode = .qrCode("ABCDEFGHIJKLMNOP")
 switch productBarcode {
 case .upc(let numberSystem, let manufacturer, let product, let check):
     print("UPC: \(numberSystem), \(manufacturer), \(product), \(check).")
 case .qrCode(let productCode):
     print("QR code: \(productCode).")
 }
 // Or if all values are var or let
 switch productBarcode {
 case let .upc(numberSystem, manufacturer, product, check):
     print("UPC : \(numberSystem), \(manufacturer), \(product), \(check).")
 case let .qrCode(productCode):
     print("QR code: \(productCode).")
 }
 
 // Raw Values: All cases have the same type. Value of case never changes
 enum ASCIIControlCharacter: Character {
     case tab = "\t"
     case lineFeed = "\n"
     case carriageReturn = "\r"
 }
 
 // venus == 2
 // Had mercury not been set, it's value would have been 0
 // If raw value type is String, raw value is same as case name
 enum Planet: Int {
     case mercury = 1, venus, earth, mars, jupiter, saturn, uranus, neptune
 }
 print(Planet.venus.rawValue)
 let possiblePlanet: Planet? = Planet(rawValue: 5)  // Optional since can fail

 // Recursive enums: An enum that has the enum type as associated value for 1 or more cases
 enum ArithmeticExpression {
     case number(Int)
     indirect case addition(ArithmeticExpression, ArithmeticExpression)
     indirect case multiplication(ArithmeticExpression, ArithmeticExpression)
 }
 // Or to enable redirection for all associated values
 indirect enum ArithmeticExpression {
     case number(Int)
     case addition(ArithmeticExpression, ArithmeticExpression)
     case multiplication(ArithmeticExpression, ArithmeticExpression)
 }
 let five = ArithmeticExpression.number(5)
 let four = ArithmeticExpression.number(4)
 let sum = ArithmeticExpression.addition(five, four)
 let product = ArithmeticExpression.multiplication(sum, ArithmeticExpression.number(2))
 
 // Enums can contain methods
 enum CompassPoint {
     case north, south, east, west
     mutating func turnNorth() {
         self = .north
     }
 }
 
 Enums can only contain Computed Properties
 
 ***********************************************************
 Structures and Classes
 
 Stuff classes have that structures don't: Inheritence, Type casting, Deinitializers, Reference Counting

 TODO: See Inheritance, Type Casting, Deinitialization, and Automatic Reference Counting.
 
 Best practice:
 - Use struct and enums when you can, classes come with increased complexity
 
 Struct automatically gets a default property member initializer.
    If you have an uninitialized property, you must provide its value in the init(...)
 Classes don't get default property member initializers.

 Struct and enum types are 'Value Types'
 - Deep-copy semantics upon assignment or sending to function
 - Collections (String, Array, Set, Dictionary) are structs but implement CopyOnlyOnModification
 
 Class is a 'Reference Type"
 - let c = SomeClassType()
   c.member = 10  // You can still do this in class type, the reference cannot be changed
   For a struct you can neither change content or reassign the variable

 Equality: == and != for class types work on the pointers
 In general (struct only?) you must define what these means for each type
 
 TODO: Interact with pointers directly
    https://developer.apple.com/documentation/swift/swift_standard_library/manual_memory_management
 
 ***********************************************************
 Properties
 
 For class, struct, and enum
 
 *** Stored Properties: var, let
 
 let structv = Struct(...)
 struct.a = 10  // Not allowed, both variable and properties are constants
 let classv = Class(...)
 classv.a = 10  // Totally ok, properties are not protected
 
 class A {
    lazy var data = ...  // Initializer called on 1st access, must be 'var'
       // No protection against multi-threaded access
 }

 *** Computed Properties
 
 struct A {
    var center: Int {
       get { ... }  // 'return' not needed if return value is single experssion
       set(v)  {  Use v as r-value  }  // Also skip '(v)' and use reserved word 'newValue'
    }
    var center2: Int {  // Has only getter. Not 'var' becuase value could change across calls
       // ...
    }
    var center3: Int {
       willSet(v) { ... }  // If only willSet then 'newValue'
       didSet { ... }  // 'oldValue'
    }
 }

 *** Property Wrappers

 Created using a class, struct, or enum
 @propertyWrapper
 struct TwelveOrLess {
    private var number = 0
    private var somethingElse = 0
    var projectedValue: <Anything>  // Could be self, or any other complex type
    var wrappedValue : Int {
       get { return number }
       set { number = min(newValue, 12) }
    }
    init() { ... }
    init(wrappedValue: Int) { ... }
    init(wrappedValue: Int, somethingElse: Int) { ... }
 }
 struct A {
    @TwelveOrLess var a1: Int = 10  // Will call TwelveOrLess.init()
    @TwelveOrLess(wrappedValue: 10, somethingElse: 1) var a2: Int
    @TwelveOrLess(somethingElse: 1) var a3: Int = 10
 }
 var a = A()
 a.a1 = 10   // Uses wrappedValue
 a.$a1 = ... // Uses projectedValue
 
 *** Global and Local Variables
 
 Can also use use computed and observed values
 
 Global varibles: Are always evaluated lazily (without need for lazy keyword)
 Local variables: Are never computed lazily
 
 *** Type Properties: 'static'
 
 Must always be initialized at declaration, no init() can initialize it
 
 static var ...  // Both stored and computed properties are supported
 
 For class computed properties, you can use 'class' instead of 'static'
    This allows subclasses to override superclass behavior
 
 ***********************************************************
 Methods
 
 Classes, structs, and enums can have methods
 There are instance methods and type methods

 Structs and enums are value types
    - You need to use 'mutating func' to modify instance properties
    - You can also modify self
 enum Switch {
    case on, off
    mutating func switch() {
       switch self {
          case .on:
             self = .off
          case .off:
             self = .on
       }
    }
 }
 
 Type methods are defined like type properties using 'static' and 'class' (before 'func' keyword)
    - Like instance methods, you can use 'self' to refer to type properties and methods
 
 @discardableResult  // Specify that it's not a mistake to ignore the return value of the method
 mutating func a() -> Bool { ... }
 
 ***********************************************************
 Subscripts
 
 Can be defined for classes, structs, and enums
 subscript(row: Int, col: Int) -> Int {   // You define argument and return value
    get { }  // Can be used as r-value if defined
    set { }  // Can be used as l-value if defined (this one uses default 'newValue')
 }
 E.g: a[0, 1] = 10
 
 Type subscripts using 'static' or 'class' before 'subscript'
 
 ***********************************************************
 Inheritance
 
 Only classes can inherit
 A class that don't inherit from any class is called a Base class
 
 class Subclass: Superclass {
    override func makeNoise() { super.makeNoise() }

    // You can make a Superclass read-only property a rw one
    //    But you cannot make a rw-one a r-only one
    override var description {
       super.description + ": Subclass"
    }
 }
 
 To disable ability to override, use:
    final class
    final var
    final func
    final class func
    final subscript
 
 ***********************************************************
 Initialization
 
 All stored properties must be initialized
 No property observers are called during initialization
 
 It is ok to initialize constant properties 'let' in init
 
 struct Celsius {
    var tinc1: Double
    var tinc2: Double?  // No initialization needed, will default to nil
    init(_ celsius: Double) {
       tinc1 = celsius
    }
 }
 let t = Celsius(37.0)
 
 No init(...) is needed if you provide default values for all properties
 
 Structs get default init(...) with property names as argument names for all properites
    You can provide for any number of properties, as long as the others have default init values
 
 You can override inits, but you must use override init(...) of course
 
 Value Types Initialization (structs and enums)
    - You can call self.init(...) from within an init(...). I.e reuse initialization code
 
 *** Class Type Initialization
    - All properties of entire hierarchy must be init'ed
 
 Designated Initializers
    - "Delegates initialization up"
    - Fully initializes subclass properties and call superclass
    - A class must have at least 1
    - Must initialize all properties of its class before calling "up"
    - Must call "up" before initializing an inherited property
 Convenience Initializers
    - "Delegates initialization across"
    - convenience init(...)
    - Can call other convenience initializers but chain must eventually call a designated initializer
 
 ... And here there are lots of details around this, that I'm not gonna read in details
 
 *** Failable initializers
 
 Can be created for classes, structs, and enums

 init?(...) {
    if error {
       return nil
    }
    // No error, no need to return a value
 }
 let a: Int? = Int(exactly: 3.14)  // a will be nil

 All Enum init?(rawValue: xyz) are failable
 
 Question: What is the init!(...) { ... } initializer, don't understand this
 
 required init(...)  // All subclasses must also implement this initializer (no override needed)
 
 var a: Bool = {  // Closure is called when the class, struct, enum is created
    // This is a closure to initialize a, in here you cannot access any other properties (or even self)
 }()  // We're calling this closure, otherwise we'd be assigning a closure to a...
 
 ***********************************************************
 Deinitialization
 
 deinit {  // Only possible on class types
    // Properties still available in this block
 }

 ***********************************************************
 Optional Chaining
 
 Optional chaining: Process to access things on a optional that might be nil
 
 Result of optional chaining call is result of expression wrapped in an optional
 
 class A {
    var va0: B?
 }
 class B {
    var vb0: Int
    var doit() { }  // Return type is Void == ()
 }
 var v = A()
 if let rc = v.va0?.vb0 { // not nil }
 
 v.va0?.vb0 = 10  // Will fail if va0 is nil. If so, non of r-value is evaluated
 // Remember result of optional chaining is a wrapped optional, so:
 if (v.va0?.vb0 = 10) != nil {  // set successfully }
 if v.va0?.doit() != nil {  // doit called, since it's within optional chaining, it returns Void?  }

 someArray?[0] = 10  // Retrieve index 0 if someArray is non-nil
 
 var a0: Int?
 var a1: Int?
 if let a0=a0, let a1=a1 {
    // a0, a1 are Int
 }
 // a0, a1 are Optional<Int>

 // Multi-level optional chaining is still optional (not optional of optional)
 if let bv = john.residence?.address?.buildingIdentifier()?.hasPrefix("The") {}
 
 ***********************************************************
 Error handling - interoperates with NSError in Cocoa
 
 Error type, something that conforms to Error protocol (empty)
 
 Common to model errors using enums
 enum VendingMachineError: Error {
     case invalidSelection
     case insufficientFunds(coinsNeeded: Int)
     case outOfStock
 }
 
 Swift does not unwind stack, so throw is comparable to a return statement
 init can also throw

 enum Error2: Error { case e1, e2 }
 func f0() throws -> String {
   throw Error2.e2
   // return "Success"
 }
 func f1() throws -> String {
   try f0()
 }
 func f3() {
   do {
     let a = try f1()
     print("f1 Success: \(a)")
   }
   catch Error2.e1 { print("e1 caught") }
   catch is Error2 { print("Error2 for sure") }
   catch { print("Default catch for: \(error)") }  // Only default can use 'error'
   
   if let _ = try? f1() { }  // Convert exception to an optional value (returning nil if thrown)
   let _ = try! f1()  // Runtime error if it throws
 }

 Add a defer {} in any block
 - Called before exiting the block: works not only for errors but also break, return, etc
 
 ***********************************************************
 Type Casting

 if a is <Type> { }
 if let a as? <Type> { }
 let b  = a as! <Type>

 Any: Represent instance of any type (including optional), including function types
 AnyObject: Represent instance of class type
 
 var things = [Any]()
 things.append(0)
 things.append(0.0)
 things.append("hello")
 for t in things {
    switch t {
       case 0 as Int:
       case 0 as Double:
       case let someInt as Int where someInt > 0:
       case is Double:
       case let sc as (String)->String:
    }
 }
 var oi: Int? = 3
 things.append(oi)  // Gives warning if optional is used where Any is expected
 things.append(oi as Any)  // No warning
 
 ***********************************************************
 Nested Types
 
 You can nest type declarations to any level:
    - Enums can declare structs
    - Structs can declare enums
    - Etc

 ***********************************************************
 Extensions - Adds new functionality to a type, but they cannot override existing functionality
 
 Works for: Class, Struct, Enum, or Protocol types
  
 'Retroactive Modelling' - Extending types for which you don't have the source code

 Extensions in Swift can:
   - Add computed instance properties and computed type properties
   - Define instance methods and type methods
   - Add convenience initializers
   - Define subscripts
   - Define and use new nested types
   - Make an existing type conform to a protocol

 extension SomeType [: SomeProtocol, AnotherProtocol] {
 
 extension Int {
    var km: Int { return self * 1_000 }
    var m:  Int { return self }
 
    func repeatMe(task: ()->Void) { for _ in 0..<self { task() } }
    mutating func square() { self = self * self }
 }
 let far = 10.km + 42200.m
 3.repeatMe { print("Hello") }  // Prints hello 3 times
 var si = 2
 si.square()
 print(si)  // 4
 
 Subscripts, e.g  123456789[0]  could become '1'
 
 Adding convenience inits
 extension Rect {
    init(...) {
       ...
       self.init(...)
    }
 }
 
 extension Int {
    enum Kind { case negative, zero, positive }
    var kind: Kind {
       switch self {... }
    }
 }
 
 ***********************************************************
 Protocols
 
 


*/

enum NTE: Int {
    case one, two
    case three, four
  }

func slg() {
  print("***************** WE ARE IN slg")
  print(NTE.one.rawValue)
  print(NTE.three.rawValue)

  
  var a0: Int? = 10
  if let a0=a0 {
    print(type(of: a0))
  }
  print(type(of: a0))
  
  
  //*************************
  print("Chapter 01")
  let a = 3 + 0.14; print(a)  // a will be a Double
  // Error: let tooBig = Int.max + 1
  
  //*************************
  print("Chapter: Closures")

}
