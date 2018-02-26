/*
 ============================================================================
 Name		 : GomokuPlayerAiRandom.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Sample implementation of a very simple random artificial
 "intelligence"
 ============================================================================
 */

#ifndef GOMOKUPLAYERAIRANDOM_H
#define GOMOKUPLAYERAIRANDOM_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32debug.h>
#include <e32math.h>
#include "GomokuPlayerInterface.h"

// CLASS DECLARATION

/**
 *  CGomokuPlayerAiRandom
 *
 */
class CGomokuPlayerAiRandom : public CGomokuPlayerInterface
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CGomokuPlayerAiRandom();

	/**
	 * Two-phased constructor.
	 */
	static CGomokuPlayerAiRandom* NewL();

public:
	/**
	 * Initialize the player implementation. This method is called by
	 * the game engine before the game is started and provides
	 * the necessary parameters to the implementation.
	 *
	 * \param aGrid a reference of the grid that will be valid for the
	 * whole game. The implementation should save a pointer to this
	 * grid to be able to read the current grid state during the game.
	 * Player implementations should not modify the grid. The engine
	 * has its own private copy of the reference grid and synchronizes
	 * the grid that the implementations use with the reference grid after
	 * every move to be on the safe side, though.
	 * \param aYourColor the color this implementation is assigned with for
	 * this game.
	 * \param aPairCheck ETrue if pair check is activated for this game.
	 * \param aNumPairsWin number of captured pairs required for winning a game.
	 */
	void InitL(CGrid* aGrid, eCellColor aYourColor, TBool aPairCheck, TInt aNumPairsWin);

	/**
	 * Called when the turn of this player is started. The implementation
	 * should analyze the grid through the pointer it gets through the
	 * InitL() method. Once the implementation is finished with
	 * calculating its move, it should call the callback function
	 * defined in the MGomokuPlayerObserver-interface.
	 *
	 * It is an advantage if the player starts its own thread for
	 * calculating the next move, or if it uses a background active object
	 * to do so. Otherwise, the game will hang.
	 *
	 * If the calculation is fast, this does not have to be done, however.
	 * The game engine handles the callbacks of the implementation
	 * asynchronously using an own timer, in order to ease the implementation
	 * of players.
	 */
	void StartTurn();

	/**
	 * If a player is active, the game engine forwards selected cell events
	 * (through the pointer or key events) to the implementation. This
	 * is necessary for human players, but could also be used to implement
	 * half-automatic AIs or other things.
	 *
	 * \param aCol column of the grid cell that was selected.
	 * \param aRow row of the grid cell that was selected.
	 */
	void HandleSelectedCellL(TInt aCol, TInt aRow);

	/**
	 * This method is called by the game engine when the game is over,
	 * the parameter provides the color of the winning player.
	 *
	 * This is important for network players, which might have to send
	 * this result over the air to the other game instance.
	 *
	 * \param aWinnerColor color of the winning player.
	 */
	void GameOver(eCellColor aWinnerColor);

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CGomokuPlayerAiRandom();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	/**
	 * Pointer to the grid, owned by the game engine.
	 * This grid always contains the current game state and should not
	 * be modified by this implementation.
	 */
	CGrid* iGrid;

	/**
	 * Stores if pair check is activated in the current game.
	 */
	TBool iPairCheck;

	/**
	 * Number of captured pairs required for winning a game.
	 */
	TInt iNumPairsWin;
	
	/**
	 * When starting to analyze, this variable stores for which player to calculate a move.
	 */
	eCellColor iCurrentPlayer;

	/**
	 * The opposite color of the own player.
	 */
	eCellColor iEnemyPlayer;

	/**
	 * Size of the grid that is used in the current game.
	 * Stored here for easier and faster access.
	 */
	TSize iGridSize;

	/**
	 * Seed for the random number generator, initialized when this
	 * object is instantiated.
	 */
	TInt64 iSeed;
	};

#endif // GOMOKUPLAYERAIRANDOM_H

