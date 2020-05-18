// Timing
//let now = Date()
//let every6Hours = TimeInterval(6 * 60 * 60)
//guard now > (lastCleanDate + oneWeek) else { return }
// DispatchQueue.main.asyncAfter(deadline: .now() + 10.0) {

//let policyTable = qtable.reduce([String: [GridMaze.Action]]()) { (dict, entry) -> [String: [GridMaze.Action]] in
//  let max = entry.value.max(by: { $0.qvalue < $1.qvalue })!.qvalue
//  var mlist = [GridMaze.Action]()
//  let preferredActions = entry.value.reduce([GridMaze.Action]()) { list, entry in
//    if entry.qvalue == max {
//      mlist.append(entry.action)
//    }
//  }
//
//  var dict = dict
//  dict[entry.key] = mlist
//  return dict
//}
