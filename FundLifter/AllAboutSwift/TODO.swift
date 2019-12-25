/*

L00: Intro
 - Swift: Subscript [r,c] instead of [r][c]. Shallow vs deep-copy semantics for classes, arrays, and structs

L01_01:
 - Below: Sutton is 4x4 with R=..., vtable duals vs inplace
 - Describe the problem: Maze that consists of 5x5 cells, let's call each cell a state.
   Robot with energy level 3. Moving to another state give energy -1. Bounce off wals.
   (Show image. Robot is always able to get out, but must chose the most efficient path).
   (Obviously, you can find the solution just by looking the maze, but this is the basis for much more complex scenariosis)
   We want to create a map of this maze, so if we are dropped somewhere we'd know how to best get out
   Our strategy is to give each state in the maze a score that specifies how good it is to be in that location
   Then, given a state we can chose the action to move left/up/down/right to get to the adjacent state with the best score
   Let's call this map a VTable, which is short for value-table
 - So how can we create this map?
   Now start to show code,
   Create Maze, Get VTable ((i)
   Print VTable, all states have value 0.0
   Update algorithm for vtable - Equal probability for all directions, what else could we do?
 - Look at vtable, would this b e good map? Discussion points:
   + S[1][1]=-6.83, but S[0][2]=-7.59 both have 2 steps to G
     Compare to game [1][1] gives you access to 2 better paths, [0][2] only to 1

 L01_02:
 - We did x number of iterations, was that really necessary How can we detect at which k we solved the problem?
 - Do grid of 5x5 to get a middle/middle one. Also do R=1 when reaching Goal state, -1 for rest
 - Introduce StopCondition w iterations, and stopOnConvergence set to true
 - Do the run, see that it terminates before 20 iterations
 - Introduce qtable, it's another map, deduced from vtable
   (we need to construct qtable from mazeEnv, not only vtable, to account for bouncing)
 - Print vtable, qtable, and policy, discuss
 - Extend to 17x17: iterations 200, see similarity of suggested policy
 - Now the punchline: We start of with a policy of 25% probability in each direction (because we have no other information)
   But result is a new policy that specifies how we should move about
   In our case, this new policy solves the maze optimally but this is not always the case
   That's what we'll look at in next lesson

L02:
 - Bottom right gives highes reward from anywhere except closest to top/left
 - Introduce policy improvement / iteration into the mix (and introduce definition of policy evaluation, also called prediction)
 - Create the code to solve tha problem
 - Maybe solve Cliff Gridworld?

L03:
 - Let's move from V to Qtable. We always need to create a QTable from VTable, why don't we just work with QTables in the first place?
   No reason, let's do QTables instead

L04:
 - Probabilistic target states: Frozen lake.
 - Also learn how to build a maze from the base components (SPACE, GOAL, etc)

 L05:
 - When DP cannot be used: We do not have a good model of the environment
 - E.g: how many states are there in tic tac toe? Black jack if you have (Sutton's configuration)

 L06:
 - MC requires full episode. So really, it means we must follow a policy that will lead to a terminal state.
   Will our G upper-left, bottom-right do this, not necessarily but probably. Games? Yes. Other things maybe not true.
   Why called MC?
   MC does not build on the value of another state/value (it uses the full sequence or Rs)

Backburner - Things to try out
 - Tic-Tac-Toe
 - CliffWorld: Safer path is selected based on epsilon value causing fall
 - Labyrinth
 - Wind labyrinth
 - Jacks Car Rental (4.1)
 - How many ways to place x given y (tic-tac-toe, chess)


 
 // Reinforcement Learning things
 //
 // Questions
 // - createQTableFromVTable is not correct, it is always assuming a 1.0 probability of a next state (given a from state)
 // - QTable for DP
 //
 // - I am deeply unsatisfied with DP policy iteration algorithm in Sutton:
 //   a. It says evaluation should stop once delta-v across all states is small enough.
 //      I would have preferred if e.g a policy derived from each sweep was equivalent, it is a hard measure
 //      If a delta value used is big enough, we may not have converged the evaluation
 //   b. If evaluation has not converged then the optimal policy theorem in iteration step is wrong, as we could
 //      have two non-converged evaluation rounds yield the same policy (which would not be optimal)
 //
 // Notes
 // - Evaluation does not terminate based on policy equality after each step, need to use < delta_value
 //   (discussed with Oscar)
 // - At each policy iteration step you have 3 options (discussed with Oscar):
 //   1) Max, resolve ties arbitrarily and set winner to 1.0 probability
 //   2) Max, let all go into next evaluation round, each with 1.0 / count probability
 //   3) Keep all, but with adjusted probability distribution
 //
 // ----- Other things...
 // Solve Cliff with Q-learning and SARSA as in book
 // Look for other mazes in book

 /* RL QUESTIONS
 - SARSA, Expected SARSA, Q-Learning: What's off-policy about these? They all use target policy to generate behavior
 */

 /* SWIFT QUESTIONS

 // Error cannot return ArraySlice for [String]
 var a = ["a", "b", "c", "d"]
 func foo() -> [String] {
    return a[0..<2]
 }
 
 - Any more beautiful way to write this
   if rv.dpY2D == nil && wDPDs[i]!._rYTDFund != nil {
     rv.dpY2D = wDPDs[i]!._rYTDFund
   }
- Any more beautiful way to write this
   let mValF = mDPDs.map {  $0?._r1w != nil ? $0!._r1m : nil }
- How to init an array of Doubles containing nils? Below does not work
   var dZeroes = Array(repeating: Double(nil), count: datesWeeks.count)
 - What does this do if wDPDs[0] is nil?
   rv.dpWs.append(wDPDs[0]?._r1w)

- Assertion: Swift creates implicit init that requires unassigned properties as argument
  BUT: 1. There can be no private properties (even if they have default value) (complains 'privat init'). Why?
       2. Order must be the same as property declarations       
 
- SwiftUI allows you to have properties without any explicit init and then you initialize
  by doing TestView(portfolioName: D_FundInfo.TYPE_PPM). Is this a SwiftUI thing or a Swift thing?

 - print("\(.player(0).playerSymbol)")  // Ambiguous
 - if dict[key] != nil { ... }: Still does a full copy just to check if key exists I think
   Or should I assume that compiler optimizations would manage it? If so, is this better?
   if let _ = dict[key] { ... } better or worse?
 - if qtable[gameState.description] == nil {  // Do we need to do .description or type inference using desc?
 - Is this correct?
   No?
      let legalActions = gameState.legalActions
      qtable[gameState.description] = legalActions.map { ActionValue(action: $0, value: 0.0) }
   Yes?
      qtable[gameState.description] = gameState.legalActions.map { ActionValue(action: $0, value: 0.0) }
 - Bloated and requires double O(1)
      action = qtable[gameState.description]![Int.random(in: 0..<qtable[gameState.description]!.count)]


 - B
 - I guess this will copy if actionValueLists and content are non-class type: for (ci,actionValueList) in r.enumerated() {
 - Best way to do constants (that might have same value so enum cannot be used).
   a. If you use 'static var' in class you always need to prefix with class name (even if you use constant within
   scope of class)
   b. Enum cannot be used (solves the scoping problem with static variables, but two cannot have same value

 - This does not work, why not?
   switch(s) {
     case (0,1) && action==1:

 - Can't convert value of type (Int,Int) to Bool (at first case)
         var reward = 0;
         switch (newState._row, newState._col) {
         case (0,0) || (SimpleMazeEnv.ROW_COUNT-1, SimpleMazeEnv.COL_COUNT-1):
             reward = 1
         default:
             reward = -1
         }

 - var a = Array<Array<State>>()
    a.reserveCapacity(rowCount)
    for (ri,r) in a.enumerated() {
         for ci in 0..<colCount {
             if (ri,ci) == (0,0) {
                 r.append(GOAL(r: rTopLeft))  //Cannot use mutating member on immutable value 'r'
             }
         }
    }

 - What's an instance of Void?
   var a = [Int: Void]()
   a[10] = ???

 - Performance and Arrays, are tuples structs. How do you know what is performant code?
   Idea: Wrap non-reference structures into reference ones? I.e. easily wrap and map Array methods from a class context?
   I now need to write code like this. Will that not be a huge performance hit (argument & return value)
      var listOfMismatchPairs = areTriplesEqual(t1:_mazeMapPrev!, t2:siVIVs)  // ([TripleSIVIV],[TripleSIVIV])
      listOfMismatchPairs = resolveMismatches(listOfMismatchPairs)

 - Idea: Only require parameter name specification if it difference from a variable name passed?

 - Cannot call a static function from a non-static one in a class, what's up with that?
 */

 /* GOOD STUFF
 //        _qtable = [[[Float]]](
 //                repeating: [[Float]](
 //                        repeating: [Float](repeating: 0.0, count: 4),
 //                        count:_colCount),
 //                count: _rowCount)
 // Print enum
         for (ai,a) in Action.allCases.enumerated() {
             print("\(a)[\(a.rawValue)]", terminator: "")
             if (ai < Action.allCases.count-1) {
                 print(", ", terminator: "")
             }
         }

 */



 
 
*/
