import Foundation

// From: https://www.raywenderlich.com/5370-grand-central-dispatch-tutorial-for-swift-4-part-1-2

// Dispatch queues are thread-safe which means that you can access them from multiple threads simultaneously
// Queues can be either serial or concurrent
// 1. Main queue
// 2. Global queues: concurrent queues that are shared by the whole system, QoS:
//    User-interactive: User is seeing results from this on its screen
//    User-initiated: User requested this info
//    Utility: Computations, IO, ...
//    Background: User does not see this
// 3. Custom queues: queues that you create which can be serial or concurrent

// DispatchQueue.sync(execute:)
// DispatchQueue.async(execute:)
// Argument is DispatchWorkItem (that you can configure with QoS etc)

class EncloserClass {
  func encloserFunc() {
    // Example 1
    DispatchQueue.global(qos: .userInitiated).async { [weak self] in
      guard let self = self else {
        return
      }
      // Do some stuff, e.g. calculate an image
      DispatchQueue.main.async { [weak self] in
        // Update UI elements, i.e. display the image
        print(self.debugDescription)  // Just here to surpress warning
      }
    }
    
    // Example 2: Timer
    DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) { [weak self] in
      print(self.debugDescription)  // Just here to surpress warning
    }
    // Why not Timer:
    // - This is easier to read
    // - Timer runs on a run loop, so you'd have to make sure it runs on the correct run loop
  }
}

// Example 3: Reader/Writer
class EncloserClass2 {
  let concurrentPhotoQueue = DispatchQueue(label: "com.pf.queue", attributes: .concurrent)

  var unsafeStrings: [String] = []
  var safeStrings: [String] {
    var stringsCopy: [String]!
    concurrentPhotoQueue.sync {
      stringsCopy = self.unsafeStrings
    }
    return stringsCopy
  }

  func addString(_ string: String) {
    concurrentPhotoQueue.async(flags: .barrier) { [weak self] in
      guard let self = self else {
        return
      }
      self.unsafeStrings.append(string)
      DispatchQueue.main.async { [weak self] in
        print(self.debugDescription) // Just here to surpress warning
        // Update UI elements, i.e. display the image
      }
    }
  }
}

// Example: Sleep for 5s
// usleep(5 * 1000000)

