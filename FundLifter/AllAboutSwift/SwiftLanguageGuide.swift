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
 - Types (including protocols of course) start with capital letter

Protocols:
 - Collection: startIndex, endIndex, index(before:) (after:) (_:offsetBy)
    So works for String, Array, Dictionary, Set, etc
 - RangeReplaceableCollection: insert(_:at:) (contentsOf:at:), remove(at:), removeSubrange(_:)
 - Equatable: == operator overloaded
   a == a (reflexivity)
   a == b implies b == a (symmetry)
   a == b && b == c implies a == c (transitivity)
 - Error (empty protocol)
 - IteratorProtocol
 - Sequence
 - Hashable: Need to have hashValue: Int property
   hashValue must not be the same over different program executions
   Also requires conformance to Equatable
 - CaseIterable (i.e. allows you to iterate over enum cases: Enumb.allCases)
 - ExpressibleByArrayLiteral, ExpressibleByDictionaryLiteral, ExpressibleByStringLiteral

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
     // [weak self] is called a capture list
     // When you use capture lists, you must use the 'in' closure syntax (even if you don't specify params & return)
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
 
 
 // Capture lists
 var a = 0
 var b = 0
 let closure = { [a] in print(a, b) }
 a = 10
 b = 10
 closure()
 // Prints "0 10". Value of 'a' is captured at closure declaration
 // Not inconsisten with '[weak self]', closure is capturing reference
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
 Question: I think not, below reference says === is used for class reference equality (so you can overload ==)
    https://developer.apple.com/documentation/swift/equatable
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
 lazy is useful for properties that captures self
 If not lazy is used, you cannot reference self, since initialization may not be finished

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
 
    // Must have name 'projectedValue'
    // Provides access door beyond just wrapped value type
    var projectedValue: <Anything>  // Could be self, or any other complex type

    // Must have name 'wrappedValue', can be stored or computed property.
    // Responsible for defining and managing the underlaying storage needed by wrapped value.
    var wrappedValue : Int {
       get { return number }
       set { number = min(newValue, 12) }
    }
    init() { ... }
    init(wrappedValue: Int) { ... }
    init(wrappedValue: Int, somethingElse: Int) { ... }
 }
 struct A {
    // Compiler generates private _a1, _a2, _a3 and creates get/set to redirect
    // Wrapped value can willSet, didSet blocks (but not get/set)
    @TwelveOrLess var a0: Int       // TwelveOrLess.init()
    @TwelveOrLess var a1: Int = 10  // TwelveOrLess.init(Int)
    @TwelveOrLess(wrappedValue: 10, somethingElse: 1) var a2: Int
    @TwelveOrLess(somethingElse: 1) var a3: Int = 10
 }
 var a = A()
 a.a1 = 10     // Uses wrappedValue
 print(a.$a1)  // Uses projectedValue
 
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

 Question:
 init!(...) { ... }: Explicitly unwrapped failable initializer (? used when calling a init? knowing it's ok?)
 
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
 
 var things = [Any]()  // Could use 'AnyType' to restrict all elements to be of class types
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
 Protocols - Blueprint methods & properties for implementation in class, struct, enum
 
 1st class Types in Swift
 
 Class can have a superclass and also implement any number of protocols
 
 protocol P {
    var a1: Int { get set }  // Must be a stored property
    var a2: Int { get }      // Implementer can do computed/stored, and also choose to provide setter
    static var a3: String { get set }  // Implementer must declare static or class
 
    static func f1()
    func f2(a: Int) -> Double
    mutating func f3()  // Implementer does not need to specify 'mutating' keyword
 
    init(a: Int)  // Implementer must implement
       // Classes, unless 'final', must prefix specify 'required init' so subclasses also reimplement
 }
 
 Class C0: P  { init() { ... } }  // Question: Why not required here?
 Class C1: C0 { required override init() { ... } }
 
 Question: Design patterns (Decorator, Delegate)
 
 protocol P: AnyObject { }  // AnyObject: Must be implemented as a class
 
 // Make SomeClass conform to P protocol (even if you don't have source code to SomeClass)
 extension SomeClass: P { // Implement protocol P }
 
 If a class/struct/enum already implements protocol requirement, make it conform through empty extension
 extension SomeStruct: TextRepresentable { }
 
 Generics - Conditionally conforming to a Protocol
 // Make Array conform to TextRepresentable when it's elements do
 extension Array: TextRepresentable where Element: TextRepresentable {
    var textualDescription: String { ... }
 }
 
 Protocol Composition
 // One of the types could be a class/struct/enum type
 func wishHappyBirthday(to: Named & Aged) -> String {
    return "Happy \(to.age) year birthday \(to.name)"
 }

 Protocol Extensions
 Can provide: methods, inits, subscripts, and computed properties
 Can of course not add new requirements or add protocol conformances

 extension RandomNumberGenerator {  // A protocol that define random()
    func randomBool() -> Bool { return random() > 0.5 }
 }
 
 extension Collection where Element: Equatable {
    func allEqual() -> Bool { }
 }
 // allEqual is only available if elements conform to Equatable (which gives == and !=)

 Do not learn: Optional Protocl Requirements
 You can have optional protocol requirements for interworking with Objective-C
 Used if you have a class that inherits and Objective-C class
 @objec protocol P {
    @objc optional func f()
 }
 I won't document the details here...
 
 ***********************************************************
 Generics
 
 func swapValues<T>(_ a: inout T, _ b: inout T) { ... }

 struct Stack<Element> {
    var items = [Element]()
    mutating func push(_ a: Element) ..
 }
 
 extension Stack {  // No need for <Element>, as is already known in struct declaration
    var topItem: Element? { ... }
 }

 Type Constraints
 Type parameter must be instance of a struct/class/enum or conform to protocol
 func f<T: SomeClass, U: SomeProtocol>(...) { ... }
 
 Associated Types (really should be in Protocol section)
 // Allow protocol to use an unspecified type which is provided by implementation
 //    as either a concrete type (Int) or a generic type (<Element>)
 protocol Container {
     associatedtype Item: Equatable  // Item is unspecified at this type but must conform to Equatable
     mutating func append(_ item: Item)
     var count: Int { get }
     subscript(i: Int) -> Item { get }
 }
 
 protocol SuffixableContainer: Container {
     associatedtype Suffix: SuffixableContainer where Suffix.Item == Item
     func suffix(_ size: Int) -> Suffix
 }
 extension Stack: SuffixableContainer {
    // In other words, the method returns a Stack (SuffixableContainer) that needs
    //    to have same element types as itself
    func suffix(_ size: Int) -> Stack { ... }
 }
 extension IntStack: SuffixableContainer {
    func suffix(_ size: Int) -> Stack<Int> { ... }
 }

 Generic Where Clause
 func allItemsMatch<C1: Container, C2: Container>(_ sc1: C1, _ sc2: C2) -> Bool
    where C1.Item == C2.Item, C1.Item: Equatable {
 }

 Generic Where Clauses
 
 extension Stack where Element: Equatable {
    func isTop(_ item: Element) -> Bool { ... }
 }
 extension Container where Item: Equatable {
    func startsWith(_ item: Item) -> Bool { ... }
 }
 extension Container where Item == Double {
    func average() -> Double { ... }
 }
 
 protocol Container {
    associatedtype Item
    mutating func append(_ item: Item)
    associatedtype Iterator: IteratorProtocol where Iterator.Element == Item
    func makeIterator() -> Iterator { ... }
 }
 
 protocol ComparableContainer: Container where Item: Comparable { }
 
 extension Container {
    // indices parameter must be a Sequence of Int values
    subscript<Indices: Sequence>(indices: Indices) -> [Item]
       where Indices.Iterator.Element == Int {
    }
 }
 
 func max<T>(_ x: T, _ y: T) -> T where T: Comparable { ... }
 Question: Why not func max<T: Comparable> is that for parameters and T where T: Comparable is for return?
 
 ***********************************************************
 Opaque Types
 
 protocol Fighter: Equatable { }
 struct XWing: Fighter { }
 struct YWing: Fighter { }
 func launchFighter() -> Fighter { XWing() }
 
 Error: “Protocol 'Fighter' can only be used as a generic constraint
    because it has Self or associated type requirements.”
    Self is the problem here
 let a1 = launchFighter()
 let a2 = launchFighter()
 a1 == a2  // They don't need to be the same type even?
 
 Opaque types solve this problem because compiler knows exact type returned
    This also means we can only return one type also
 func launchFighter() -> some Fighter { XWing() }
 
 So it creates a compile time check rather than a runtime one (like Java.Comparable)
 
 Using Generics: Now the caller choose returned type
 func launchFighter<T: Fighter>() -> T { T() }  // Argument type == return type (strictly)
 let f1: XWing = launchFighter()
 let f2: YWing = launchFighter()
 
 So
    - Generics work: Work when caller select returned type
    - Opaque result types: When function implementation decides returned type
    - Above, we used Equatable as example but same question is relevant for protocols with associatedtype in them
 
 ***********************************************************
 Automatic Reference Counting - ARC
 
 ARC only applied to classes
 
 Strong reference: I am keeping a firm hold on this memory segment
    let c2 = c1  // Ref count for object that c1 pointed is now 2

 Strong reference cycle == memory leak
 Solution: Use weak or unowned references
    var weak variable: Type?  // Must be var (since runtime can change it)
 Rule: When all strong references to an instance, it is deallocated and weak references are set to nil
 
 Case #1: Weak references: Do not keep a strong hold on instance it refers to
 class Person {
   var apartment: Apartment?
 }
 class Apartment {
   weak var tenant: Person?
 }
 var p = Person()
 var a = Apartment()
 p.apartment = a
 a.tenant = p
 p = nil  // p is deallocated and a.tenant == nil
 a = nil  // a is deallocated
 Property observers are not called when ARC sets a weak reference to nil

 Case #2: Unowned references: Do not keep a strong hold on instance it refers to
 Use if objects have same lifetime, i.e you know reference is valid
 Unowned is not optional / set to nil, accessing invalid gives runtime error
 class Customer {
    var card: CreditCard?
 }
 class CreditCard {
    unowned let customer: Customer
    init(customer: Customer) {
       self.customer = customer
 }
 var c = Customer()
 c.card = CreditCard(customer: c)
 c = nil  // Both c and credit card becomes are deallocated
 You can also say unowned(unsafe), then Swift will not do runtime checks for validity
    Then accessing variable is an undefined operation
 
 Case #3: Unowned with implicitly unwrapped optional properties
 class Country {
    var capitalCity: City!  // Has default value nil, but does not need unwrapping
    init() {
       capitalCity = City(country: self)
    }
 }
 class City {
    unowned let country: Country
    init(country: Country) {
       self.country = country
    }
 }
 
 Closures, like classes, are reference types
 Memory leak: Closure stored as a property and body captures the instance (self)
 lazy var someClosure = {
     [unowned self, weak delegate = self.delegate]
     (index: Int, stringToProcess: String) -> String in
     // closure body goes here
 }
 Or if parameter and return types are inferred
 lazy var someClosure = {
     [unowned self, weak delegate = self.delegate] in
     // closure body goes here
 }
 Use unowned if closure and instance will deallocated at the same time
 Use weak if captured reference may become nil (weak are always optional type)
   
 ***********************************************************
 Memory Safety
 
 Conflicting access to memory can happen from single thread
 Swift guarantees you'll get a compile or runtime error
 
 TODO: Thread sanitizer - https://developer.apple.com/documentation/code_diagnostics/thread_sanitizer
 
 Instantaneous access - access that cannot conflict
 All the below is instantaneous access
 func oneMore(than number: Int) -> Int { return number + 1 }
 var myNumber = 1
 myNumber = oneMore(than: myNumber)
 
 Long-term access: where there could be conflicts

 Overlapping access - could cause conflicts, primarily:
 - inout parameters
 - Methods or mutating methods of a structure
 
 A function has long-term access to all its in-out parameters
 A mutating function has write access to self for its duration
 - Write access starts after all non-in-out parameters have been evaluated
 - And lasts until function returns
 var stepSize = 1  // global
 func increment(_ number: inout Int) { number += stepSize }
 increment(&stepSize)  // Same memory addess involved in read and write
 
 Swift checks for memory exclusivity, which is stricter than memory safety
 
 ***********************************************************
 Access Control
 
 Modules & Source files has access controls
 
 A module == unit of code distribution available through 'import' keyword
 XCode: Each build target (app or framework) becomes a separate module in Swift
 
 Default access level is 'internal'
   public: Available across modules and source files
   internal: Only within the module
   fileprivate: Only within the source file
   private: Only within enclosing declaration
 
 
 Unit Test Target
 - All symbols are available if you mark import with @testable and model was 'test' compiled
 
 Specify access level before declaration
 Default is 'internal'

 Access level of a nested type is the same as its containing type, unless the containing type is public
 Nested types defined within a public type have an automatic access level of internal
 If you want them public, you need to specify public
 private class SomePrivateClass {  // explicitly private class
    func somePrivateMethod() {} // implicitly private class member
 }

 A func access level is the most restrictive of its contained scope, parameters, and return type
 You must also mark function access level with this one
 In general this is the way it works for eveything
 
 public struct TrackedString {
    public private(set) var numberOfEdits = 0  // get is public, set is private
 }

 ***********************************************************
 Advanced Operators
 
 Bitwise NOT
    let initialBits: UInt8 =            0b00001111
    let invertedBits = ~initialBits  //   11110000

 Bitwise AND
    let f1: UInt8 =    0b11111100
    let f2: UInt8 =    0b00111111
    let f3 = f1 & f2 //  00111100
 
 Bitwise OR
    let f1: UInt8 =    0b10110010
    let f2: UInt8 =    0b01011110
    let f3 = f1 | f2  // 11111110
 
 Bitwise XOR: 1 if different 0 otherwise
    let f1: UInt8 =   0b00010100
    let f2: UInt8 =   0b00000101
    let f3 = f1 ^ f2 // 00010001
 
 Bitshifts
    For UInt, bits are discarded if shifted out of scope
       let shiftBits: UInt8 = 0b10000001
       shiftBits << 1        // 00000010
 
    For Int (signed) - Two complement representations
       MSB is the sign digit
       0b00000100 == 4
       0b11111100 == -4  // 1sign, 1111100==124 --> 128-124=4, so -4

 Overflow
    let a = Int16.max
    a += 1  // Runtime overflow error
    But you can use &+, &*, and &- to overflow instead of runtime error
    
 Operator precedence
    2 + 3 % 4 * 5  == 2 + ((3 % 4) * 5)  // % and * have same precedence --> left 2 right evaluation
 
 Operator Methods
 struct Vector2D: Equatable {  // Equatable: ==
     var x = 0.0, y = 0.0
     static func + (left: Vector2D, right: Vector2D) -> Vector2D {
       return Vector2D(x: left.x + right.x, y: left.y + right.y)
     }
     static prefix func - (vector: Vector2D) -> Vector2D {
        return Vector2D(x: -vector.x, y: -vector.y)
     }
     static func += (left: inout Vector2D, right: Vector2D) {
        left = left + right
     }
     // We let Swift standard library implement != by negating ==
     // There is no need to implement == if all members are Equatable type
     static func == (left: Vector2D, right: Vector2D) -> Bool {
       return (left.x == right.x) && (left.y == right.y)
     }
     // Define your own operators - there are rules: https://docs.swift.org/swift-book/ReferenceManual/LexicalStructure.html#ID418
     static prefix func +++ (vector: inout Vector2D) -> Vector2D {
       vector += vector
       return vector
     }
     // Define new operator with precedence specification
     infix operator +-: AdditionPrecedence
     static func +- (left: Vector2D, right: Vector2D) -> Vector2D { ... }
 
     You also have postfix, e.g force-unwrap: a!
 }

 
 ***********************************************************
 ***********************************************************
 ***********************************************************
 Language Guide

 ***********************************************************
 Expressions
 
 #file: String
 #line: Int
 #column: Int
 #function: String
 #dsohandle: UnsafeRawPointer  # The dynamic shared object handle in use where it appears
 
 Key Paths syntax: \type name.path, or \.path if type can be inferred

 struct A { var a0: Int }
 let s = A(a0: 10)
 let path = \A.someValue  # Gets replaced with KeyPath instantiation at compile time
 let v = s[keyPath: path]
 
 var cv = (a: 1, b: 2)
 cv[keyPath: \.self] = (a: 10, b: 20)  // Sames as cv = (a: 10, b: 20)
 
 let g = ["hello", "hola"]
 let m0 = g[keyPath: \[String].[1]]  // Requirement: String must conform to Hashable protocol
 let i = 1
 let m1 = \[String].[index]  // index is captured using value semantics
 
 Using Objective-C runtime features in Swift
    #keyPath    // Access property name
    #selector   // Refer to a method, setter, or getter from @objc
    https://developer.apple.com/documentation/swift/using_objective-c_runtime_features_in_swift

 ***********************************************************
 Statements

 // Statemens only included at compiile time if compilation_condition is true
 #if compilation_condition
    // statements
 #elseif ...
    // statements
 #else
    // statements
 #endif
 All statements are parsed even if not compiled with exception of swift(...) version
 compliation_condition can be below, combining with: &&, ||, and !
    - true / false Bool literals
    - Identifier specified -D command line flag
    - Platform conditions:
      os(macOS | iOS | watchOS | tvOS | Linux)
      arch(i386 | x86_64 | arm | arm64)  // i386 return true for 32-bit iOS simulator
      swift(>= version | < version)
      compiler(>= version | < version)
      canImport(moduleName)
      targetEnvironment(simulator | macCatalyst)

 #sourceLocation(file: String, line: Int)  // Changes #file and #line
 #sourceLocation()  // Resets back, i.e. undoes the one above)
 
 #error(_: String)   // Emit error at compile time and terminates process
 #warning(_: String) // Emits warning during compile time
 
 // Choose to execute code if a platform is available at runtime
 if/guard #available(platformName version, ..., *) { ... }
    platformName can be: { iOS, iOSApplicationExtension, macOS, macOSApplicationExtension, watchOS, tvOS }

 ***********************************************************
 Declarations
 
 Methods with special names
    @dynamicCallable   // Documented in Attributes chapter
    struct Repeater {
       // Argument must conform to ExpressibleByArrayLiteral
       //    or ExpressibleByDictionaryLiteral, where Key must conform to ExpressibleByStringLiteral
       // Return can be anything
       dynamicallyCall(withArguments: [Int])
       dynamicallyCall(withKeywordArguments: KeyValuePairs<String, Int>)

       func callAsFunction(_ number: Int, scale: Int)
    }
    var r = Repeater()
    r(4, scale: 10)  // callAsFunction
    r(1, 2, 3)    // dynamicallyCall
    r(a: 1, b: 2) // dynamicallyCall
 
    Keyword: 'rethrow' is used for a function f0, instead of throw, if
       a function parameter f1 to f0 throws its original exception
       func f0(f1: () throws -> Void) rethrows {
          try f1()  // Works with rethrows
       }
 
    // This function will never return (or throw an exception)
    // All code after calling this method is dead
    func noReturn() -> Never { }

    Introduces new grouping for infix operators
    Standard library does this a lot
    precedencegroup name {
       higherThan:    lowerGroupNames
       lowerThan:     higherGroupNames
       associativity: associativity
       assignment:    assignment
    }
    You can also specify left/right/none
 
    Access control levels: public, internal, fileprivate, private
       open: Used freely within module, can be subclassed in other modules (that import it)
 
 ***********************************************************
 Attributes
 
 Two types: Applies to declarations or types
 Attributes provide additional information about the declaration or type
 
 @attributeName
 @attributeName(attributeArguments)

 @available(platformName, versionNumber, *)
 @available(swift versionNumber)
    platformName: {iOS|macOS|watchOS|tvOS}[ApplicationExtension]01  // So 8 combos
       * = all platforms
 Quite complicated, for details see: https://docs.swift.org/swift-book/ReferenceManual/Attributes.html
 
 @discardableResult  // Apply to code declaration to not issue a warning if return value is not used
 @dynamicCallable    // See before
 
 @dynamicMemberLookup
 Used to create wrapper type from e.g other languages
 
 @dynamicMemberLookup
 struct DynamicStruct {
     let d = ["someDynamicMember": 325]
     subscript(dynamicMember: String) -> Int { return dictionary[dynamicMember] ?? 1054 }
 }
 let s = DynamicStruct()
 print(s.someDynamicMember)
 
 struct Point { var x, y: Int }
 @dynamicMemberLookup
 struct PassthroughWrapper<Value> {
     var v: Value
     subscript<T>(dynamicMember member: KeyPath<Value, T>) -> T {
         get { return v[keyPath: member] }
     }
 }
 let wrapper = PassthroughWrapper(value: Point(x: 381, y: 431))
 print(wrapper.x)
 
 @frozen
 Only allowed when compiling in Library Evolution Mode
    Swift compiler: -enable-library-evolution
    Xcode: BUILD_LIBRARY_FOR_DISTRIBUTION to Yes
 Disallows changing enum or struct declaration
 All non-frozen enum/struct in LEM can be changed and still not break code that depends on the library
 Compiler enables this through indirection and runtime lookups
 @frozen artifacts must be public or marked with @usableFromInline
 
 @inlinable
 Allows compiler to inline this code, which must be public (function, method, computed property, ...)
 This code can interact with other public code or internal code marked as @usableFromInline
 It cannot interact with private or fileprivate code
 
 @nonobjc
 Suppresses the implicit @objc attribute
 Makes code unavailable in Objective-C code
 Used to resolve circularity for bridging methods
 
 @NSApplicationMain
 Marks a class as the application delegate
 If you don't use this attribute, supply a main.swift file at top level:
    import AppKit
    NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
 
 @NSCopying
 Mark on a stored property of a class
 Causes property setter to use 'copyWithZone(_:)'
 Type of property must conform to NSCopying protocol
 
 @NSManaged (also implies @objc)
 For a class that inherits from @NSManagedObject
 Applied to method or stored variable to indicate Core Data provides its implementation at runtime
 
 @objc - used to make symbol available from Objective-C
 @objcMember - normally you use @objc instead
 
 @propertyWrapper - Applied to class, struct, or enum
 When applied it creates a new custom attribute with same name as type
 Apply new attribute to a property to wrap access through the wrapper type
    Local or global variables cannot use pws
 
 @requires_stored_property_inits
 Requires all stored properties to have default values
 Inferred if class inherits NSManagedObject
 
 @testable
 Apply to import declaration to open up access controls for testing purposes
 Imported module must be compiled with testing enabled
 For entities, makes internal -> public
 For classes & class members, makes internal/public -> open
    
 @UIApplicationMain
 Apply to class to indicate it's the apps delegate
 Can instead provide main.swift at top-level that calls UIApplicationMain(_:_:_:_:) function
   For examle, if app uses a custom subclass of UIApplication as its principle class
   Then call UIApplicationMain(_:_:_:_:) function instead of using @UIApplicationMain
 
 @usableFromInline
 Allow code to be used in inlinable code defined in the same module
 Must have internal access level
 If used on class/struct/enum all dependent types must be public or @usableFromInline
 
 @warn_unqualified_access
 No need for you to use
 Use this attribute to help discourage ambiguity between functions with the same name
 that are accessible from the same scope.
 

 @autoenclosure
 Apply to a functions parameter type
 Encloses expressions passed as this parameter in a closure that takes no parameter and return expression type
 Delays execution of expression until closure is called
 func serve(customer customerProvider: @autoclosure () -> String) {
     print("Now serving \(customerProvider())!")   // Expression execution
 }
 serve(customer: customersInLine.remove(at: 0))  // No expression execution
 Useful to delay expensive expressions, e.g. in assert that is only evaluated on debug builds

 @convention(swift | block | c)
 Apply this to the type of a function to indicate its calling conventions
 
 @escaping
 Apply to function type parameter ftp of a function f0, where the parameter will outlive the execution of f0
 E.g f0 stores ftp in an array variable of a scope outside f0
 
 @unknown
 Apply to a case of a switch that is not expected to be triggered when code was compiled
 
 ***********************************************************
 Patterns
 
 if case let Puppy.mastiff(droolRating, weight) = fido {
 ==
 switch fido {
 case let Puppy.mastiff(droolRating, weight):
 
 // let allows you to bind subsequent associated values
 if case Puppy.mastiff(let droolRating, let weight) = fido {
 if case let Puppy.mastiff(characteristics) = fido {  // Bind tuple into characteristics
 
 if case let Puppy.mastiff(droolFactor, weight)? = potentialPup {  // Matches non-nil optionals
 if case Puppy.mastiff = fido {
 
 for case let Puppy.mastiff(droolRating, weight) in rescuePups where droolRating > 5 && weight > 15.0 {

 if let rescueCenter = nearest(), case let puppies = rescueCenter.puppies, puppies.count < 10 {

 // Only prints non-nils
 let arrayOfOptionalInts: [Int?] = [nil, 2, 3, nil, 5]
 for case let number? in arrayOfOptionalInts { print("Found a \(number)") }

 TODO: Read more https://appventure.me/guides/pattern_matching/intro.html
 
 Question:
    if case let x? = someOptional { print(x) }
    What is the deal with 'case' here, isn't this same as if let x=someOptional { ... }
 
 ***********************************************************
 ***********************************************************
 ***********************************************************
 
 Compiler:
   Source code
   ->Parse->AST(w/o semantic & type information
   ->Semantic Analysis->Fully type checked AST
   ->Clang importer
   ->SIL generation
   ->SIL guaranteed transformations (dataflow diagnostics)->canonical SIL
   ->SIL optimizations(ARC optimizations, etc)
   ->LLVM IR Generation
 
 Standard Library
    Introduces fundamental data types (Int, Double, Array, etc)
    Has access to compiler builtins, i.e to generate SIL instructions and data types

 Swift Package Manager - https://swift.org/package-manager/
    Clearly designed to work well with GitHub
    Package
       - Swift source files and Package.swift (manifest file)
       - A package has 1 or more targets
       - A target (library(import) | executable) specifies 1 product and may declare 1 or more dependencies
       - Dependency: Module required by package code (URL + version)
         Package manager downloads and builds all dependencies
         In this process, it will find out their names, which can be resolved in 'target' of Package.swift
       - Layout: Package.swift (root), Sources/<TargetName>/...
       - $ git clone https://github.com/apple/example-package-dealer.git
         $ cd example-package-dealer
         $ swift run Dealer  # runs the Product, must be a executable, resolving dependencies first
         $ swift build SomeModule  # builds the module/executable, resolving dependencies first
       - Directory .build is used to process all this

    Using system modules - This is just import, you need #if/#else when using random/arc... code also
       #if os(Linux)
       import Glibc     // Enables the random() function
       #else
       import Darwin.C  // Enables the arc4random_uniform(UInt32(count - i)) function
       #endif
 
    LLVM
       SSA - Static Single Assignment
       ELF - Executable and Linkable Format (object file format)
       DWARF - Debugging data format designed along ELF but is independent of object file formats
 
       Darwin
 
 

 */


protocol Fighter: Equatable { init() }
struct XWing: Fighter { }
struct YWing: Fighter { }
func launchFighter1() -> some Fighter { XWing() }
func launchFighter2() -> some Fighter { XWing() }
func launchFighter3<T: Fighter>() -> T { T() }

enum NTE: Int {
    case one, two
    case three, four
  }

func slg() {
  print("***************** WE ARE IN slg")
  let f1: XWing = launchFighter3()
  let f2: XWing = launchFighter3()
  print(f1 == f2)

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
