/*
//----
func l01q() {

//    let mazeEnv = create_TLAndBRCornerGoals(
//            rowCount: 5,
//            colCount: 5,
//            rewardTopLeft: 0,
//            rewardBottomRight: 6,
//            rewardSpaces: -1,
//            rewardBounceBack: -1)
//    l01q_01(mazeEnv: mazeEnv, maxIterationCount: 10, printIterationInterval: 1, convergedAtLessThanPercent: 0.1)

    // Example #0 - Sutton's first
    // Example #1
//    var mazeDef1 = createMatrix_RxC_WithSpaces(rowCount:5, colCount: 5, rewardSpaces: -1, rewardBounceBack: -1)
//    mazeDef1[4][4] = GOAL(reward: 0.0)
//    mazeDef1[1][3] = WALL()
//    mazeDef1[2][3] = WALL()
//    mazeDef1[3][3] = SPACE(reward: -9)
//    mazeDef1[4][3] = SPACE(reward: -10)
    // Example #2 - Cliff, with 50% falling off when entering edge state
    var mazeDef = createMatrix_RxC_WithSpaces(rowCount:6, colCount: 6, rewardSpaces: -1, rewardBounceBack: -1)
    mazeDef[5][5] = END(reward: 0.0)
    mazeDef[4][1].setJumpProbabilities(jumpProbabilities: [(.Relative(1,0),0.5), (.Welcome,0.5)])
    mazeDef[5][1] = HOLE(reward: -100)
    mazeDef[4][2].setJumpProbabilities(jumpProbabilities: [(.Relative(1,0),0.5), (.Welcome,0.5)])
    mazeDef[5][2] = HOLE(reward: -100)
    mazeDef[4][3].setJumpProbabilities(jumpProbabilities: [(.Relative(1,0),0.5), (.Welcome,0.5)])
    mazeDef[5][3] = HOLE(reward: -100)
    mazeDef[4][4].setJumpProbabilities(jumpProbabilities: [(.Relative(1,0),0.5), (.Welcome,0.5)])
    mazeDef[5][4] = HOLE(reward: -100)

    let mazeEnv = GridMazeEnvironment(maze: mazeDef)
    l01q_01(mazeEnv: mazeEnv, maxIterationCount: 10000, printIterationInterval: 10, convergedAtLessThanPercent: 0.01)
}

//----
func getPercentDiff(currentQValue: Float, nextQValue: Float) -> Float {
    var percentDiff: Float = 0.0
    let diff = abs(currentQValue-nextQValue)
    if diff == 0 {
        percentDiff = 0.0
    }
    let absOldValue = abs(currentQValue)
    if absOldValue == 0 {
        percentDiff = 1.0  // Any value which will be >theta actually
    }
    percentDiff = diff / absOldValue
    return percentDiff
}

//----
func l01q_01(mazeEnv: GridMazeEnvironment,
             maxIterationCount: Int,
             printIterationInterval: Int,
             convergedAtLessThanPercent: Float) {

    var qtableCurrent = mazeEnv.createQTableFromUniform(distributionTotal: 0.0)

    printMazeAndTable(header: "--- Before any iteration",
            mazeEnv: mazeEnv,
            qtable: qtableCurrent)

    var maxPercentDelta: Float = 0.0
    var iterationCurr: Int = 0
    for _ in 1...maxIterationCount {
        iterationCurr += 1

        let qtableNext = QTable(qtable: qtableCurrent)
        maxPercentDelta = 0.0

        // Process all states in the cells of the maze
        for (rowIndex, row) in mazeEnv._maze.enumerated() {
            for (colIndex, state) in row.enumerated() {

                if !state._isVisitable {
                    continue
                }

                var nextQValue: Float
                var percentDiff: Float

                (nextQValue, percentDiff) = l01q_01_updatedQValue(mazeEnv: mazeEnv, qtable: qtableCurrent, state: state, action: .LEFT)
                qtableNext[rowIndex, colIndex, .LEFT]._value = nextQValue
                maxPercentDelta = max(maxPercentDelta, percentDiff)

                (nextQValue, percentDiff) = l01q_01_updatedQValue(mazeEnv: mazeEnv, qtable: qtableCurrent, state: state, action: .UP)
                qtableNext[rowIndex, colIndex, .UP]._value = nextQValue
                maxPercentDelta = max(maxPercentDelta, percentDiff)

                (nextQValue, percentDiff) = l01q_01_updatedQValue(mazeEnv: mazeEnv, qtable: qtableCurrent, state: state, action: .DOWN)
                qtableNext[rowIndex, colIndex, .DOWN]._value = nextQValue
                maxPercentDelta = max(maxPercentDelta, percentDiff)

                (nextQValue, percentDiff) = l01q_01_updatedQValue(mazeEnv: mazeEnv, qtable: qtableCurrent, state: state, action: .RIGHT)
                qtableNext[rowIndex, colIndex, .RIGHT]._value = nextQValue
                maxPercentDelta = max(maxPercentDelta, percentDiff)
            }
        }

        // Break if delta percentage change was less than 10% for all calculations
        if maxPercentDelta < convergedAtLessThanPercent {
            break
        }

        if (iterationCurr % printIterationInterval) == 0 {
            printMazeAndTable(header: "--- After iteration: \(iterationCurr), maxPercentDelta at: \(maxPercentDelta)",
                    mazeEnv: mazeEnv,
                    qtable: qtableNext,
                    printPolicy: true)
        }
        qtableCurrent = qtableNext
    }

    print("All done, difference in qvalues: \(maxPercentDelta*100)% was less than threshold. Iteration count: \(iterationCurr)")
    printMazeAndTable(header: "Result",
            mazeEnv: mazeEnv,
            qtable: qtableCurrent,
            printPolicy: true)
}

//----
func l01q_01_updatedQValue(mazeEnv: GridMazeEnvironment, qtable: QTable, state: State, action: Action) -> (newQValue: Float, percentDiff: Float) {

    let currentQValue = qtable[state._row, state._col, action]._value

    // ALT 1: No probabilistic transitions
    // Target state has single jump-specification (having probability 1)
//    let (reward, nextState) = mazeEnv.probeAction(fromState: state, takingAction: action)
//    var nextStateActionValue = qtable.getMaxActionValue(state: nextState)
//    let nextQValue = reward + nextStateActionValue

    // ALT 2: Probabilistic transitions
    // Target state has >=1 jump-specifications
    var nextQValue: Float = 0
    let lts = mazeEnv.probeActionWithProbabilities(fromState: state, takingAction: action)
    for tsi in lts.indices {
        let prs = lts[tsi]
        nextQValue += prs.prob * (prs.reward + qtable.getMaxActionValues(state: prs.state)[0]._value)
    }

    let percentDiff = getPercentDiff(currentQValue: currentQValue, nextQValue: nextQValue)

    return (nextQValue, percentDiff)
}

/*
  QCurr = 0.0 for all [s,a]
  QNext = 0.0 for all [s,a]  // Could use just 1 Q-array but 2 allows us to see the deltas for each state-sweep
  delta = 0.0

  while (true) {

     // One full sweep
     for every (s,a) in non-terminal states s {
        (s',r) = lookupFrom(s,a)  // For simplicity, assume single deterministic target (s',r) for each (s,a)

        qnow = QCurr[s,a]
        QNext[s,a] = r + max_a'[ QCurr[s',a'] ]   // Gives = r + 0.0 if s' is terminal
        delta = max(delta, abs(qnow-QNext[s,a]))  // Or expected if we have probabilistic transitions
     }

     // Done if update is marginal
     if delta < theta {
        break
     }

     // Perform another sweep
     QCurr = QNext
  }

  // Derive pi from a greedy Q
*/


*/
