/*

///----
func l02q() {

    var mazeDef = createMatrix_RxC_WithSpaces(rowCount:6, colCount: 6, rewardSpaces: -1, rewardBounceBack: -1)

    // var mazeDef = GridMazeEnvironment(rowCount:6, colCount: 6, border: WALL(), rest: SPACE(r:-1)
    mazeDef[5][0] = START()
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
    l02q_01(mazeEnv: mazeEnv, maxIterationCount: 10000, printIterationInterval: 10, convergedAtLessThanPercent: 0.01)
}

//----
func l02q_selectAction(fromState: State, qtable: QTable, epsilon: Float) -> Action {

    let isGreedy = randomNumber(probabilities: [(1-epsilon), (epsilon)]) == 0
    let maxActionValues = qtable.getMaxActionValues(state: fromState)

    // If all values are max, or we are greedy, then select a random action from this list
    if maxActionValues.count == 4 || isGreedy {
        return maxActionValues[Int.random(in: 0..<maxActionValues.count)]._action
    }

    // Select a random non-max value
    let nonMaxValues = qtable[fromState._row, fromState._col]._actionValues.filter { $0._value != maxActionValues[0]._value }
    return nonMaxValues[Int.random(in: 0..<nonMaxValues.count)]._action
}


//----
func l02q_01(mazeEnv: GridMazeEnvironment,
             maxIterationCount: Int,
             printIterationInterval: Int,
             convergedAtLessThanPercent: Float) {

    // The environment gives us a QTable to work with, so it gives us information how many states there are
    let qtable = mazeEnv.createQTableFromUniform(distributionTotal: 0.0)

    printMazeAndTable(header: "--- Before any iteration",
            mazeEnv: mazeEnv,
            qtable: qtable)

    var episodeCount = 0
    var stepCount = 0
    var endState = [State]()
    var state = mazeEnv.reset()
    repeat {
        episodeCount += 1
        state = mazeEnv.reset()

//        let qtableOld = QTable(qtable: qtable)  // Make a copy so we can compare the diffs
        while !state._isEnd {

            // With 0.9 probability, select action with max Q value in state
            //      0.1 probability, select a random action (ensures exploration)
            stepCount += 1
            let action = l02q_selectAction(fromState: state, qtable: qtable, epsilon: 0.1)
            let (reward, statePrime) = mazeEnv.step(state: state, action: action)

            // Why is this equation stabilizing on certain values, i.e. why doesn't it grow indefinately (also a Q for DP case)
            qtable[state._row, state._col, action]._value =
                    (1-0.1) * qtable[state._row, state._col, action]._value
                  + 0.1     * (reward + 0.9*qtable.getMaxValue(state: statePrime))

            state = statePrime
        }


//        if maxPercentDelta < convergedAtLessThanPercent {
//            break
//        }
//        if (iterationCurr % printIterationInterval) == 0 {
//            printMazeAndTable(header: "--- After episode: \(iterationCurr), maxPercentDelta at: \(maxPercentDelta)",
//                    mazeEnv: mazeEnv,
//                    qtable: qtable,
//                    printPolicy: true)
//        }


        // We could add convergence if they policy across x number of episodes has not changed
        // Talk about convergence strategies

        endState.append(state)
        if episodeCount % 100 == 0 {
            print("At episode: \(episodeCount), average steps per episode: \(stepCount/10)")
//            for s in endState {
//                print("...end state: " + s._oneLetterDescription)
//            }
            endState = [State]()
            stepCount = 0
        }


    } while episodeCount < 5000

    print("Trained for: \(episodeCount), average steps per episode: \(stepCount/episodeCount)")

    printMazeAndTable(header: "Result",
            mazeEnv: mazeEnv,
            qtable: qtable,
            printPolicy: true)
}

*/
