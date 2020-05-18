
/*
//----
func l01_calculateValue(mazeEnv: GridMazeEnvironment, fromState: State, vtable: VTable) -> Float {
    let (rewardUp,    stateUp)    = mazeEnv.probeAction(fromState: fromState, takingAction: Action.UP)
    let (rewardDown,  stateDown)  = mazeEnv.probeAction(fromState: fromState, takingAction: Action.DOWN)
    let (rewardLeft,  stateLeft)  = mazeEnv.probeAction(fromState: fromState, takingAction: Action.LEFT)
    let (rewardRight, stateRight) = mazeEnv.probeAction(fromState: fromState, takingAction: Action.RIGHT)

    let newStateValue  // Calculate new values based on current mazeMap
            = 0.25 * (rewardUp    + vtable[stateUp._row, stateUp._col])
            + 0.25 * (rewardDown  + vtable[stateDown._row, stateDown._col])
            + 0.25 * (rewardLeft  + vtable[stateLeft._row, stateLeft._col])
            + 0.25 * (rewardRight + vtable[stateRight._row, stateRight._col])

    return newStateValue
}

//----
func l01() {
    l01_02()
}

//----
func l01_01() {

    // Sutton is 4x4 with -1 at all
    let (row,col) = (5,5)
    let mazeEnv = create_TLAndBRCornerGoals(
            rowCount: row,
            colCount: col,
            rewardTopLeft: 1,
            rewardBottomRight: 1,
            rewardSpaces: -1,
            rewardBounceBack: -1)
    var vtableCurr = VTable(rowCount: row, colCount: col)

    printMazeAndTable(header: "--- Before any iteration",
            mazeEnv: mazeEnv,
            vtable: vtableCurr)

    for k in 1...10 {
        let vtableNext = VTable(vtable: vtableCurr)

        // Process all states in the cells of the maze
        for (rowIndex, row) in mazeEnv._maze.enumerated() {
            for (colIndex, state) in row.enumerated() {

                if state._isEnd {
                    continue
                }

                let newStateValue = l01_calculateValue(mazeEnv: mazeEnv, fromState: state, vtable: vtableCurr)
                vtableNext[rowIndex, colIndex] = newStateValue  // But write the updates to the update mazeMap
            }
        }

        vtableCurr = vtableNext
        printMazeAndTable(header: "--- After iteration: \(k)", mazeEnv: mazeEnv, vtable: vtableCurr)
    }
}

//----
func l01_02() {
    let (row, col) = (4, 4)
    let mazeEnv = create_TLAndBRCornerGoals(
            rowCount: row,
            colCount: col,
            rewardTopLeft: -1,
            rewardBottomRight: -1,
            rewardSpaces: -1,
            rewardBounceBack: -1)
    var vtableCurr = VTable(rowCount: row, colCount: col)

    printMazeAndTable(header: "--- Before any iteration",
            mazeEnv: mazeEnv,
            vtable: vtableCurr,
            printPolicy: true)

    let stopCondition = StopTrainingCondition(
            mazeEnv: mazeEnv,
            iterationsMax: 1000,
            stopOnConvergence: false,
            stopOnPercentDiff: 0.01,
            debug: false)

    vtableCurr = vtableIterationSequence(
            mazeEnv: mazeEnv,
            vtable: vtableCurr,
            stopCondition: stopCondition,
            printEachIteration: true)

    printDoneReport(header: "StopCondition is reached", mazeEnv: mazeEnv, stopCondition: stopCondition, vtable: vtableCurr)
}

//----
func vtableIterationSequence(mazeEnv: GridMazeEnvironment,
                             vtable: VTable,
                             stopCondition: StopTrainingCondition,
                             printEachIteration: Bool) -> VTable {
    var isDone = false
    var vtableCurr = vtable
    repeat {
        // We're not doing in-place (chapter 4.1), book says we should normally do in-place
        // Observe, in Swift this will be a value-copy since Array type is struct
        let vtableNext = VTable(vtable: vtableCurr)

        // Process all states in the row and column cells of the maze
        for (rowIndex, row) in mazeEnv._maze.enumerated() {
            for (colIndex, state) in row.enumerated() {

                if !state._isVisitable {
                    continue
                }

                let newStateValue = l01_calculateValue(mazeEnv: mazeEnv, fromState: state, vtable: vtable)
                vtableNext[rowIndex, colIndex] = newStateValue  // But write the updates to the update mazeMap
            }
        }

        vtableCurr = vtableNext
        isDone = stopCondition.reportIteration(vtable: vtable)

        if printEachIteration {
            printMazeAndTable(header: "--- Evaluation, after state sweep: \(stopCondition.getIterationCount()), done: \(isDone)",
                    mazeEnv: mazeEnv,
                    vtable: vtableCurr,
                    printPolicy: true)
        }
    } while (!isDone)

    return vtableCurr
}

//----
func printDoneReport(header: String,
                     mazeEnv: GridMazeEnvironment,
                     stopCondition: StopTrainingCondition,
                     vtable: VTable) {
    print("We're done, termination condition:\n...\(stopCondition.getTerminationStr())")
    print()
    printMazeAndTable(header: "\(header)", mazeEnv: mazeEnv, vtable: vtable, printPolicy: true)
}


*/
