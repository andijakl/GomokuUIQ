/*
 ============================================================================
 Name		 : GomokuPlayerObserver.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Defines the interface for callbacks from the player
 implementations to the game engine.
 ============================================================================
 */

#ifndef GOMOKUPLAYEROBSERVER_H_
#define GOMOKUPLAYEROBSERVER_H_

/**
 * This interface should be implemented by the observer
 * of the player implementations. It is used to report
 * that the player has finished its move.
 */
class MGomokuPlayerObserver
	{
public:
	/**
	 * Report to the observer of the player implementation
	 * that the move has been finished.
	 * The parameters have to provide valid coordinates of
	 * the selected (still empty) cell.
	 *
	 * \param aCol column (x-coordinate) of the selected cell.
	 * \param aRow row (y-coordinate) of the selected cell.
	 */
	virtual void HandleFinishedTurn(TInt aCol, TInt aRow) = 0;
	};

#endif /*GOMOKUPLAYEROBSERVER_H_*/
