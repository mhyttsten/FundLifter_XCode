
import Foundation

// https://docs.swift.org/swift-book/GuidedTour/GuidedTour.html#//apple_ref/doc/uid/TP40014097-CH2-ID1

public func aSwiftTour() {
      let c0 = 5  // can be assigned only once, value does not need to be known at compile time
      var v0 = 5  // type cannot be changed, this is inferred as int
      var v1: Double  // We can leave unassigned but then we must specify type
      var v2: Double = 70  // Also specify explicit type
      var v3: String = "a" + String(70)  // Never any implicit type conversion

      var ov0: String? = "hello"
      var ov1: String? = nil
      if let v0 = ov0 {
            print(v0)  // executed if optionalValue0 != nil
      }
      let v4 = ov1 ?? "yes"  // v1 is "yes" if ov1 is nil

      print("Hello \(c0)")  // Embed variables in strings
      print("""
            "hello"
            how are you
            """)

      var l0 = ["a", "b", "c"]
      l0.append("d")
      print(l0.contains("a"))
      print(l0)
      l0 = []
      var l1 = [String]()
      var d0 = ["a": 1, "b": 2, "c": 3]
      print(d0)
      d0 = [:]
      var d1 = [String: Float]()

      for s in l0 {
            if s == "hello" {
                  print("equal")
            }
      }
      for (k, v) in d0 {
      }
      for i in 0..<4 {
      }
      for i in 0...4 {
      }
      while v0 < 50 {
      }
      repeat {
      } while v0 < 50

      var switch0 = "is it pepper"
      switch (switch0) {
      case "a", "b":
            print("1st one")
      case let x where x.hasSuffix("pepper"):
            print("it was \(x)")
      default:
            print("unknown")
      }

      func foo1(arg1: String, arg2: Int) -> String {   // Ordinary function
            return "Hello \(arg1) integer \(arg2)"
      }

      foo1(arg1: "name", arg2: 10)

      func foo2(arg1: [Int]) -> (min: Int, max: Int) {  // Return tuples
            return (0, 10)
      }

      func foo3() {   // Nested functions
            var y = 1

            func tmp() {
                  y += 1
            }

            tmp()
      }

      func foo4() -> ((Int) -> Int) {  // Return a function
            func addOne(a: Int) -> Int {
                  return a + 1
            }

            return addOne
      }

      func foo5(f: (Int) -> Int) {
      }  // Take function as an argument

      foo5(f: foo4())

      // Functions are a special case of closures = blocks of code that can be called later
      var l2 = [Int]()
      l2.map({ (a: Int) -> Int in return a + 1 })
      l2.map({ a in return a + 1 })  // Type inference
      l2.sorted {
            $0 > $1
      }  // So compact

      class C0 {  // Use var, let, func as usual in here
            var _a: Int

            init(a: Int) {
                  // super.init()  // In case you need to initialize parent
                  _a = a;
                  // Or self._a = a
            }

            deinit {
            }              // destructor

            var p0: Int? {
                  get {
                        return _a * 3
                  }
                  set {
                        _a = newValue! / 3
                  }
                  // set(a) { _a = a }  // Also valid syntax
            }
            var p1: Int? {
                  willSet {
                        _a = newValue!
                  }
                  // willSet(a) { _a = a }  // Also valid syntax
                  didSet {
                  }
            }
      }

      var class0 = C0(a: 10)
      // Inheritence class Hello: <Parent>
      //    Must use 'override func <name>' when overriding functions
      //    Call super.init(), or other Parent methods
      var class1: C0? = C0(a: 10)
      var class1_v = class1?._a  // If class1 is nil then everything following it is ignored
      // Type of class1_v should be Int? I guess
      // Structs are very similar to classes
      //    Main difference is that they are always copied-by-value when passed around

      enum Rank: Int {  // You can also omit in case raw type is not needed
            case ace = 1  // Starts with 0 by default, enums can also use String or Float
            case jack, queen, king

            func print() -> String {
                  switch self {
                  case .ace: return "ace"
                  case .jack: return "jack"
                  default: return "error"
                  }
            }
      }

      let r0 = Rank.ace
      let r0_valaue = r0.rawValue
      let r1 = Rank(rawValue: 3)  // r1: Rank? in case arg does not match an element

      enum E1 {  // You can omit the type if not relevant
            case result(String, String)
            case failure(String)
      }

      let r2 = E1.result("a1", "a2")
      switch r2 {
      case let .result(a1, a2): print("\(a1) \(a2)")
      default: print("Error")
      }

      // This works because of extension Int below
      print(7._a)

      // Errors, adopt Error protocol
      enum PrinterError: Error {
            case outOfPaper, noToner
      }

      func foo10() throws -> String {
            throw PrinterError.outOfPaper
      }

      do {
            var result10 = try foo10()
      } catch PrinterError.outOfPaper {
      } catch let printerError as PrinterError {
      } catch {
      }
      let printerResult = try? foo10()  // Returns nil if foo throws error

      func foo2() {
            defer {  }  // Code block is executed right before function exit, even if error is thrown
      }
}

// Protocols and extensions must be in main scope
protocol BaseThing {
      var _a: String { get }  // What does this mean, only get allowed?
      mutating func foo()
}

class A0: BaseThing {
      var _a: String = "a"

      func foo() {
      }  // No need for mutating, methods of a class can always mutate
}

struct A1: BaseThing {
      var _a: String = "a"

      mutating func foo() {
      }
}

extension Int: BaseThing {  // You can extend any primitive of class type
      var _a: String {
            return "Hello"
      }

      func foo() {
            print("Hello")
      }
}


