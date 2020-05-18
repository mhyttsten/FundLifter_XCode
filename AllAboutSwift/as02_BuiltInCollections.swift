
//----------------------------------------------------------------------------
func as02_BuiltInCollections() {

    func f0() {
        print("--- f0()")
        let fibs = [0, 1, 1, 2, 3, 5]
        var mutableFibs = [0, 1, 1, 2, 3, 5]
        mutableFibs.append(8)
        mutableFibs.append(contentsOf: [13, 21])
        // mutableFibs // [0, 1, 1, 2, 3, 5, 8, 13, 21]
        var mutableFibs2 = mutableFibs  // Creates copy, chaging mutableFibvs does not affect mutableFibs2
        let mutableFibs3 = mutableFibs  // Creates copy, chaging mutableFibvs does not affect mutableFibs3
        mutableFibs2.append(contentsOf: [13, 21])
        mutableFibs.append(12)
        print(mutableFibs)
        print(mutableFibs2)
        print(mutableFibs3)

        func f1<T>(count: Int) -> [T] {
            var result: [T] = []
            result.reserveCapacity(count)
            return result
        }
    }
    f0()

    func f1() {
        print("--- f1()")
        // for x in array
        // for x in array.dropFirst()
        // for x in array.dropLast(5)
        let array = [1, 2, 3]
        for (num, element) in array.enumerated() { print("\(num) \(element)")}

        let d = ["one": 1, "two": 2]
        for (e) in d.enumerated() { print("\(e)")}
        for (num, e) in d.enumerated() { print("\(num) \(e.key) \(e.value)")}
    }
    f1()

    func transformingArrays() {
        print("--- transformingArrays")

        // map, flatMap
        // filter
        // reduce
        // forEach
        // sort(by:), sorted(by:), lexiographicallyPrecedes(_:by:), partition(by:)
        // firstIndex(where:), lastIndex(where:), first(where:), last(where:), contains(where:)
        // min(by:), max(by:)
        // elementsEqual(_:by:), starts(with:by:)
        // split(whereSeparator:)
        // prefix(while:)
        // drop(while:)
        // removeAll(where:)

        // Map
        var fibs = [0, 1, 1, 2, 3, 5]
        print(fibs.map(){ f in f*f })
        print(fibs.map{ f in f*f })
        print(fibs.map{ $0 * $0 })

        // FirstIndex and Sort
        print(fibs.firstIndex {$0 == 2})
        print(fibs.firstIndex {$0 == 15})
        print(fibs.sort { $0 > $1 })  // sort mutates the array, this is descending order
        print(fibs)  // Print mutated array

        var fibs2 = fibs.dropFirst()
        for (a1, a2) in zip(fibs, fibs2) { print("\(a1) \(a2)") }

        print(fibs.split(where: !=)) // See Array extension
        print(fibs.split(where: !=).flatMap{ $0 })

        // Filter
        let nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        nums.filter { num in num % 2 == 0 } // [2, 4, 6, 8, 10]
        nums.filter { $0 % 2 == 0 } // [2, 4, 6, 8, 10]

        print((1..<10).map { $0 * $0 }.filter { $0 % 2 == 0 })

        // Reduce. It is possible to implment both map & filter using reduce (see book page 37)
        fibs = [0, 1, 1, 2, 3, 5]
        let sum = fibs.reduce(0) { total, num in total + num } // 12
        fibs.reduce(0, +) // 12
        fibs.reduce("") { str, num in str + "\(num), " } // 0, 1, 1, 2, 3, 5,

        // Flatmap
        let suits = ["♠", "♥", "♣", "♦"]
        let ranks = ["J", "Q", "K", "A"]
        print(suits.flatMap { suit in ranks.map { rank in (suit, rank) } })
 
        // for where
        // for each
        let indices = 0..<16
        for idx in indices where indices[idx] == 4 { print("\(idx) \(indices[idx])")}
        (1..<4).forEach { number in
            if number >= 2 {return}  // Side effect, just exists closure not forEach
            print(number)
        }
    }
    transformingArrays()

    func arraySlices() {
        print("--- arraySlides")

        let fibs = [0, 1, 1, 2, 3, 5, 8]
        let slice = fibs[2...]
        print("\(slice) \(type(of: slice))")
        print("\(slice.startIndex)") // Obs == 2
        let sliceArray = Array(slice)
    }
    arraySlices()

    func dictionaries() {
        print("--- dictionaries")
        enum Setting {
            case text(String)
            case enabled(Bool)
        }
        var d: [String: Setting] = ["name": .text("My Text"), "wifi": .enabled(true)]
        d["name"] = .text("New Name")
        d.updateValue(.text("New Name2"), forKey: "name")
        print("\(type(of: d["name"]))")  // Optional<Setting> as key may not exist
        var d2: [String: Setting] = [:]
        d.merge(d2, uniquingKeysWith: { $1 })  // Overwrite d with keys that are equal in d2

        let thing = [("h",1), ("e", 1), ("l", 1), ("l", 1), ("o", 1)]
        var d3 = Dictionary(thing, uniquingKeysWith: +)
        print(d3)
        print(d3.filter { $0.value > 1})
        print(d3.map { $0.value + 1 })
        print(d3)  // When using map you need to assign result to lvalue

        // Dictionaries require Key to conform to Hashable protocol
        // Swift does a good job of making types automatically be hashable
        // To customize:
        //    - Your type must implement Equatable
        //    - Then implement hash(into:) required by Hashable protocol
        //    - Feed your data to Hasher
        //    - Two equal instances must yield the same hashvalue
        //    - Same hashvalue does not require equality
        //    - Standard library will yield different hashvalues on different program invocations
        //      Disable this during test with SWIFT_DETERMINISTIC_HASHING=1
    }
    dictionaries()

    func sets() {
        print("--- sets")
        // Operations are specific SetAlgebra protocol
        var s1: Set = [ 1, 1, 2, 2]
        var s2: Set = [ 2, 3, 3]
        var s3: Set = [ 3, 3]
        var s4 = s1
        print("\(s1.contains(2))")

        print(s1.isDisjoint(with: s2))
        print(s1.isDisjoint(with: s3))
        print(s1.subtract(s2)) // Modifies s1, does not return updated set
        print(s1)  // s1 is now update
        print(s4.intersection(s2))
        print(s4.union(s2))  // Returns updated set
        print(s4)
        print(s4.formUnion(s2))  // Modifies s4, does not return updated set
        print(s4)

        // IndexSet, more efficient to store ranges
        print("****** ERROR")
//        var indices = IndexSet(0..<10)  // s
//        indices.insert(integersIn: 20..<25)
//        print(indices)

        // CharacterSet, more efficient to store character ranges
    }
    sets()

    func ranges() {
        print("--- ranges")
        //
        let a = Array(0..<10)
        let lowercaseLetters = Character("a")...Character("z)")
        let fromZero = 0...
        let upToZ = ..<Character("z")
        let upTo10 = ..<10
        // Can only iterate over 'strideable' (Character ranges are not, because of Unicode)
        // Create sequences e.g. over floats using: stride(from:to:by) and stride(from:through:by)

        // 5..<5: Half-open range (can only contain -1 maximum of available range)
        // 0...Int.max: Closed range (can contain full range)


    }
    ranges()
}

// ----- transformingArrays()
extension Array {
    func split(where condition: (Element, Element) -> Bool) -> [[Element]] {
        var result: [[Element]] = self.isEmpty ? [] : [[self[0]]]
        for (previous, current) in zip(self, self.dropFirst()) {
            if condition(previous, current) {
                result.append([current]) }
            else{
                result[result.endIndex-1].append(current)
            }
        }
        return result
    }
}



