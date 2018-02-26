/*
 ============================================================================
 Name		 : GomokuGameView.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the ingame view class.
 ============================================================================
 */
#ifndef GOMOKUVIEWPLAY_H
#define GOMOKUVIEWPLAY_H

// INCLUDES
#include <aknview.h>
#include <aknviewappui.h>
#include <avkon.hrh>
#include <Gomoku.rsg>
#include <akncontext.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <aknnavi.h>

#include "Gomoku.hrh"
#include "GomokuGlobals.h"

#include "GomokuGameData.h"
#include "Grid.h"
#include "GridCell.h"
#include "GridObserver.h"
#include "GomokuPlayerObserver.h"

// CLASS DECLARATION

/**
 * Avkon view class for CGomokuGameView. It is registerd with the view server
 * by the AppUi. It owns the container control.
 */
class CGomokuGameView : public CAknView, MGridObserver,
		MGomokuPlayerObserver
	{
public:
	static CGomokuGameView* NewL(CGomokuGameData* aGameData);
	~CGomokuGameView();

	void SetGameData(CGomokuGameData* aGameData);

	TUid Id() const;

	/**
	 * Handles all commands in the view.
	 * Called by the UI framework when a command has been issued.
	 * The command Ids are defined in the .hrh file.
	 * 
	 * \param aCommand The command to be executed
	 */
	void HandleCommandL(TInt aCommand);

	// from MGridObserver
	void SelectedCellEventL(TPoint& aCellCoords);

	// from MGomokuPlayerObserver
	void HandleFinishedTurn(TInt aCol, TInt aRow);

    void HandleClientRectChange();
protected:
	void ViewConstructL();
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
private:
	CGomokuGameView(CGomokuGameData* aGameData);
	void ConstructL();

	/**
	 * Initialize the grid and all other settings for a new game.
	 * Also informs the player implementations about the current game and
	 * starts the timer to send an asynchronous request to the
	 * player implementation.
	 */
	void InitGameL();

	/**
	 * Update the view context text with the current game status.
	 */
	void ChangeViewContextTextL();

	/**
	 * Static timer callback function to make calls to the AI implementations
	 * asynchronous (with AOs) in order to simplify the structure of the
	 * plugins. This also gives time for the screen to redraw.
	 */
	static TInt TimerFunc(TAny* aObj);

	/**
	 * Non-static timer function that processes the move results.
	 * This will check if the game is over and if not, issue a request to
	 * the next player.
	 */
	void AsyncHandleFinishedTurnL();

	/**
	 * Change the text of the navigation pane to the specified
	 * resource ID.
	 *
	 * \param aResourceId id of the text resource to place in
	 * the navigation pane. Has to be defined as a RESOURCE TBUF in
	 * the resource files.
	 */
	void ChangeNaviPaneTextL (const TDesC& aText);
	
	/**
	 * Resets the contents stored in the navi pane.
	 */
	void CleanupStatusPaneL();

	/**
	 * Change the control button array (CBA) to the specified
	 * resource ID. This specifies the labels and commands
	 * assigned to both softkeys. The resource has to be
	 * defined in the resource file.
	 * 
	 * \param aResourceId id of the resource that contains the 
	 * cba definition.
	 */
	void ChangeCbaL(const TInt aResourceId);
private:
	/**
	 * ETrue if the view context text has already been set
	 * before. This is important to know because the text
	 * only has to be changed and not created if it has already
	 * been set before.
	 */
	TBool iSetViewContextText;

	/**
	 * Instance of the game data object.
	 * Not owned by this class!
	 */
	CGomokuGameData* iGameData;

	/**
	 * When the view is in the background, the game should be paused so that
	 * it does not eat up processor resources by AI calculations.
	 * ETrue if the game is paused and should not send new events to players.
	 * EFalse if the game is active.
	 */
	TBool iPaused;

	/**
	 * If a callback is received (e.g. from the AI or a network player) while
	 * the game is paused, it is not processed right away. Instead, this
	 * variable is set to ETrue in order to restart the asynchronous callback
	 * once the game is unpaused.
	 */
	TBool iPausedCallbackWaiting;

	/**
	 * Copy of the grid that is provided to the player implementations.
	 * This is seperate from the internal grid for safety and anti-
	 * cheating reasons.
	 */
	CGrid* iSendGrid;

	/**
	 * If a move is received by the player implementations, it is not processed
	 * instantly. Instead, a timer is started to make the processing asynchronous
	 * from the original call, in order to prevent recursive calls if
	 * two AI players compete against each other and the player implementations
	 * do not use active objects or threads.
	 * Therefore, the received move is saved for later processing.
	 * If set to -1, -1 the callback function will know that the move has
	 * already been processed.
	 */
	TPoint iSaveMoveForAsyncCallback;

	/**
	 * Timer object for handling asynchronous callbacks.
	 */
	CPeriodic* iTimer;

	/**
	 * Any current navi decorator
	 */
	CAknNavigationDecorator* iNaviDecorator;
	};

#endif // GOMOKUVIEWPLAY_H

