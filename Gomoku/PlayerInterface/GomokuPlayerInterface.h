/*
 ============================================================================
 Name		 : GomokuPlayerInterface.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Defines the interface that has to be implemented by
 player plugins.
 ============================================================================
 */
#ifndef PLAYERINTERFACE_H_
#define PLAYERINTERFACE_H_

#include <e32base.h>
#include <ECom.h>
#include "Grid.h"
#include "GomokuPlayerObserver.h"

// UID of this interface
const TUid KCGomokuPlayerInterfaceUid =
	{
			0xE0000E00
	};

class CGomokuPlayerInterface : public CBase
	{
public:
	/**
	 * Instantiate the default implementation of the player interface
	 * through the ecom framework.
	 */
	IMPORT_C static CGomokuPlayerInterface* NewL();

	/**
	 * Instantiate the specified implementation of the player interface.
	 * \param aCue name of the player implementation that should
	 * be instantiated by the ecom framework.
	 */
	IMPORT_C static CGomokuPlayerInterface* NewL(const TDesC8& aCue);

	/**
	 * Cleanup and deregister the instance from the ecom framework.
	 */
	IMPORT_C virtual ~CGomokuPlayerInterface();

	/**
	 * Retrieves a list of all the implementations which satisfy the
	 * specified interface. The aImplInfoArray on exit contains
	 * the plug-in implementations who's plug-in DLLs have
	 * sufficient capabilities to be loaded by the calling client process.
	 *
	 * \param aImplInfoArray A reference to a client owned array
	 * which will be filled with interface implementation data.
	 * The array will first be cleared and all items destroyed
	 * before adding new data.
	 */
	IMPORT_C static void ListImplementationsL(RImplInfoPtrArray& aImplInfoArray);

public:
	/**
	 * Set the observer that will handle the callbacks when the
	 * move is calculated by the player implementation.
	 *
	 * \param aObserver pointer to a class implementing the observer
	 * interface.
	 */
	IMPORT_C void SetObserver(MGomokuPlayerObserver* aObserver);

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
	virtual void
			InitL(CGrid* aGrid, eCellColor aYourColor, TBool aPairCheck, TInt aNumPairsWin) = 0;

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
	virtual void StartTurn() = 0;

	/**
	 * If a player is active, the game engine forwards selected cell events
	 * (through the pointer or key events) to the implementation. This
	 * is necessary for human players, but could also be used to implement
	 * half-automatic AIs or other things.
	 *
	 * \param aCol column of the grid cell that was selected.
	 * \param aRow row of the grid cell that was selected.
	 */
	virtual void HandleSelectedCellL(TInt aCol, TInt aRow) = 0;

	/**
	 * This method is called by the game engine when the game is over,
	 * the parameter provides the color of the winning player.
	 *
	 * This is important for network players, which might have to send
	 * this result over the air to the other game instance.
	 *
	 * \param aWinnerColor color of the winning player.
	 */
	virtual void GameOver(eCellColor aWinnerColor) = 0;

protected:
	/**
	 * Observer of this player, which the player implementation can use
	 * to issue callbacks when it has successfully calculated its move.
	 */
	MGomokuPlayerObserver* iObserver;

private:
	// Unique instance identifier key
	TUid iDtor_ID_Key;
	};

#endif /*PLAYERINTERFACE_H_*/
