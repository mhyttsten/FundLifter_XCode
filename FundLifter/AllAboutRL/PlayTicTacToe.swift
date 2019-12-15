/*
import OpenSpiel

//----
public func playTicTacToe() {
  let qtable = playTicTacToeLearn()
  playTicTacToeWithLearnedAgent(qtable: qtable)
}

// *************************************

//----
func getPlayerSymbol(player: Player) -> String {
  switch player {
  case .player(0):
    return "x"
  case .player(1):
    return "o"
  default:
    fatalError("Unexpected: TicTacToe only has player 0 and 1")
  }
}

//----
public func playTicTacToeBasic() {
  let game = TicTacToe()
  var gameState = game.initialState
  
  print("About to play a TicTacToe game!")
  
  // Obs, describe how the players switch between iterations of the while statement
  while !gameState.isTerminal {
    print("\n*** Now starting step: \(gameState.history.count)")
    print("Board entering this step \(gameState.description)", terminator: "")
    print("Player: \(gameState.currentPlayer)), will now place symbol: \(getPlayerSymbol(player: gameState.currentPlayer))")
    
    let legalActions = gameState.legalActions
    print("Possible actions (\(legalActions.count): \(legalActions)")
    let action = legalActions.randomElement()!
    print("--> \(gameState.currentPlayer) taking action \(action)")
    gameState.apply(action)
  }
  
  print("\nGame over!\n\(gameState)", terminator: "")
  print("""
    --> Player 0 score: \(gameState.utility(for: .player(0)))
    --> Player 1 score: \(gameState.utility(for: .player(1)))
    """)
}

// ******************************************************

//----
public struct ActionValue {
  let action: TicTacToe.Action
  var value: Double
  
  init(action: TicTacToe.Action, value: Double) {
    self.action = action
    self.value = value
  }
}

//----
/*
 V(St) = V(St) + lr[Gt - V(St)]
 V(St) = V(St) + lr*Gt - lr*V(St)
 V(St) = V(St) - lr*V(St) + lr*Gt
 V(St) = (1-lr)*V(St) + lr*Gt
 
 Gt = R_t+1 + g*R_t+2 + g^2*R_t+3...
 Explain the if we go from end, then state before terminal is t so Gt = R_t+1
 */

// ********************

// Note here: Because the players switch, our QTable is learning how to play both player0 and player1

//----
public typealias QTable = [String: [ActionValue]]
public func playTicTacToeLearn() -> QTable {
    
  var qtable = QTable()
  var gamePlayCount = 0
  var gameResultsPlayer1Win = 0
  var gameResultsPlayer0Win = 0
  var gameResultsDraw = 0
  while gamePlayCount < 2000 {
    
    var playerEpisode = [Player: [(state: TicTacToe.State, action: TicTacToe.Action)]]()  // Player is hashable
    
    gamePlayCount += 1
    
    let game = TicTacToe()
    var gameState = game.initialState
    while !gameState.isTerminal {
      
      if !qtable.keys.contains(gameState.description) {
        //qtable[gameState.description] = gameState.legalActions.map { ActionValue(action: $0, value: 0.0) }
        qtable[gameState.description] = gameState.legalActions.map { ActionValue(action: $0, value: 0.0) }
      }
      
      // Select action with highest value (greedy)
      let max = qtable[gameState.description]!.max { $0.value < $1.value }
      let maxList = qtable[gameState.description]!.filter { $0.value == max!.value }
      var action = maxList[Int.random(in: 0..<maxList.count)].action
      // With 10% probability, don't be greedy, instead use random action to promote exploration == e-greedy
      // But only do this while we are training
      if Double.random(in: 0..<1) < 0.1 {
        action = qtable[gameState.description]![Int.random(in: 0..<qtable[gameState.description]!.count)].action
      }
      
      // For each player, store sequence of (state,action) pairs, i.e when in state which action was taken
      if playerEpisode[gameState.currentPlayer] == nil {
        playerEpisode[gameState.currentPlayer] = [(TicTacToe.State, TicTacToe.Action)]()
      }
      // Append this (state,action) for the player, except for the terminal state
      playerEpisode[gameState.currentPlayer]!.append((state: gameState, action: action))
      
      // Take action from gameState
      gameState.apply(action)
    }  // End of game
    
    // Keep track of who won the game
    let rewardPlayer0 = gameState.utility(for: .player(0))
    let rewardPlayer1 = gameState.utility(for: .player(1))
    if rewardPlayer0 > rewardPlayer1 {
      gameResultsPlayer0Win += 1
    } else if rewardPlayer0 < rewardPlayer1 {
      gameResultsPlayer1Win += 1
    } else {
      gameResultsDraw += 1
    }
    if gamePlayCount % 1000 == 0 {
      print("Played: \(gamePlayCount) games"
        + "\n...States recorded in QTable: \(qtable.count)"
        + "\n...player0 won: \(gameResultsPlayer0Win)"
        + "\n...player1 won: \(gameResultsPlayer1Win)"
        + "\n...draw: \(gameResultsDraw)")
      gameResultsPlayer0Win = 0
      gameResultsPlayer1Win = 0
      gameResultsDraw = 0
    }
    
    // We now have a complete game play, let's learn from it!
    // Update qtable based on (state,action) sequence from last->first, since reward signal should be strongest at end
    for (player, playSequence) in playerEpisode {  // For each play, we have the [(state,action)] taken
      var reward = gameState.utility(for: player)  // Reward for player at end of game, only reward we get

      // Go through events from end to beginning
      for timeStep in (0..<playSequence.count).reversed() {
        let state = playSequence[timeStep].state
        let actionIndex = qtable[state.description]!.firstIndex { $0.action == playSequence[timeStep].action }!
        
        let lr = 0.1
        let discount = 0.9
        
        let oldQValue = qtable[state.description]![actionIndex].value
        qtable[state.description]![actionIndex].value = (1-lr)*oldQValue + lr*reward
        
        reward *= discount  // Reduce reward signal by 10% by each step while traversing from end to start
        // Only G_t+1 (or R_t+1 for last state) has a value since only terminal state gives a reward
      }
    }
  }
  
  print("qtable:    \(qtable.count)")
  print("We're all done with the training")
  return qtable
}

//----
func playTicTacToeWithLearnedAgent(qtable: QTable) {
  
  var gamePlayCount = 0
  while gamePlayCount < 100 {
    gamePlayCount += 1
    
    let game = TicTacToe()
    var gameState = game.initialState

    while !gameState.isTerminal {
      var action = gameState.legalActions[Int.random(in: 0..<gameState.legalActions.count)]
      if let max = qtable[gameState.description]?.max(by: { $0.value < $1.value }) {
        let maxList = qtable[gameState.description]!.filter { $0.value == max.value }
        action = maxList[Int.random(in: 0..<maxList.count)].action
      }
      gameState.apply(action)
    }

    let rewardPlayer0 = gameState.utility(for: .player(0))
    let rewardPlayer1 = gameState.utility(for: .player(1))
    if rewardPlayer0 > rewardPlayer1 {
      print("Game \(gamePlayCount): Player0 won!")
    } else if rewardPlayer0 < rewardPlayer1 {
      print("Game \(gamePlayCount): Player1 won!")
    } else {
      print("Game \(gamePlayCount): Draw!")
    }
  }
}

// *********************************************************************
// *********************************************************************
// *********************************************************************

// Cleaning up the code into functions a bit a making it reusable

//typealias QTable = [String: [ActionValue]]
//class QTablePlayer {
//    let qtable: QTable
//    var eprobability: Double = 0.0
//
//    init(qtableP: QTable) {
//        self.qtable = qtableP
//    }
//
//    func setRandomActionProbability(eprobabilityP: Double)  {
//        self.eprobability = eprobabilityP
//    }
//}

*/
