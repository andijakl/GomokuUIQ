/*
 ============================================================================
 Name		 : GomokuGameView.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the ingame view class.
 ============================================================================
 */

#include "GomokuGameView.h"

/**
 Creates and constructs the view.

 @param aAppUi Reference to the AppUi
 @param aParentViewId Back behavior
 @return Pointer to a CLayoutView object
 */
CGomokuGameView* CGomokuGameView::NewL(CGomokuGameData* aGameData)
{
	CGomokuGameView* self = new (ELeave) CGomokuGameView(aGameData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

/**
 Constructor for the view.
 Passes the application UI reference to the construction of the super class.

 @param aAppUi Reference to the application UI
 @param aParentViewId Back behavior
 */
CGomokuGameView::CGomokuGameView(CGomokuGameData* aGameData) :
	iSetViewContextText(EFalse), iGameData(aGameData), iPaused(EFalse),
			iPausedCallbackWaiting(EFalse), iSaveMoveForAsyncCallback(-1, -1)
{
}

/**
 Destructor for the view.
 */
CGomokuGameView::~CGomokuGameView()
{
	if (iNaviDecorator)
	{
		delete iNaviDecorator;
		iNaviDecorator = NULL;
	}
	if (iGameData->GetGrid())
	{
		AppUi()->RemoveFromViewStack(*this, iGameData->GetGrid());
	}
	// iGameData->GetGrid() doesn't have to be deleted by us, it is owned by the
	// control stack of the view and is automatically deleted.
	delete iSendGrid;
	iTimer->Cancel();
	delete iTimer;
}

/**
 2nd stage construction of the view.
 */
void CGomokuGameView::ConstructL()
{
	// Calls ConstructL that initiate the standard values.
	// This should always be called in the concrete view implementations.
	BaseConstructL(R_GOMOKU_GAME_VIEW);
	ViewConstructL();

	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
}

/**
 Inherited from base class and called upon by the UI Framework.
 It creates the view with commands and controls from resource.
 */
void CGomokuGameView::ViewConstructL()
{
	CGrid* grid = CGrid::NewL(ClientRect(), NULL, this);
	iGameData->SetGrid(grid);

	AppUi()->AddToStackL(*this, grid);
	grid->SetMopParent(this);
	grid->SetFocus(ETrue);
}

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CGomokuGameView::DoActivateL(const TVwsViewId& aPrevViewId,
		TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
	if (aPrevViewId.iViewUid == TUid::Uid(EGomokuSettingsViewId))
	{
		// The user just came from the settings screen - start a new game
		InitGameL();
	}
	else
	{
		// The view has been in the background (multitasking) and just resumed
		// foreground state - unpause the game
		iPaused = EFalse;
		if (iPausedCallbackWaiting)
		{
			// If a callback is waiting, restart the timer to
			// process the event now
			iPausedCallbackWaiting = EFalse;
			iTimer->Cancel();
			iTimer->Start(10, 10, TCallBack(TimerFunc, this));
		}
	}
	iGameData->GetGrid()->MakeVisible(ETrue);
}

/**
 */
void CGomokuGameView::DoDeactivate()
{
	iGameData->GetGrid()->MakeVisible(EFalse);
	// The view lost foreground status - pause the game
	// This does not cancel a currently active player, as this could
	// be problematic for AI or network players. Instead, the game
	// stops after the move of the currently active player has been
	// received.
	iPaused = ETrue;
	// If a move is received while the game is in paused state,
	// this variable will be set to true so that the game knows
	// it has to process the event when the game is unpaused.
	iPausedCallbackWaiting = EFalse;
}

void CGomokuGameView::InitGameL()
{
	// Cancel the timer in case it is still active (it shouldn't be, but anyway).
	iTimer->Cancel();
	// Unpause the game
	iPaused = EFalse;
	iPausedCallbackWaiting = EFalse;

	// Reset the game data
	iGameData->SetGamePhase(EIngame);
	iGameData->ResetPairsCaptured();

	// Make sure the grid has the correct size and background color
	iGameData->GetGrid()->ResizeGridL(iGameData->GetGridSize());
	iGameData->GetGrid()->SetBgColor(TRgb(255, 255, 255));
	iGameData->GetGrid()->ExtendToMainPane();

	// Recreate the grid instance that the players will get
	delete iSendGrid;
	iSendGrid = NULL;
	iSendGrid = CGrid::NewL();
	iSendGrid->ResizeGridL(iGameData->GetGridSize());
	iSendGrid->CopyFrom(iGameData->GetGrid());

	// Initialize the player implementations with the settings
	// of this game.
	iGameData->GetPlayer1()->SetObserver(this);
	iGameData->GetPlayer1()->InitL(iSendGrid, EColor1,
			iGameData->GetPairCheckEnabled(), iGameData->GetNumPairsWin());
	iGameData->GetPlayer2()->SetObserver(this);
	iGameData->GetPlayer2()->InitL(iSendGrid, EColor2,
			iGameData->GetPairCheckEnabled(), iGameData->GetNumPairsWin());

	// Update the view with the current player
	iGameData->SetCurrentPlayerNum(1);
	ChangeViewContextTextL();
	ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);

	// Initiate a callback, set the last move cache to -1 so that
	// the callback function will know that it should issue a request to the next
	// (the first) player and not process a move first.
	iSaveMoveForAsyncCallback.SetXY(-1, -1);
	iTimer->Start(10, 10, TCallBack(TimerFunc, this));
}

/**
 * @return The UID for this view
 */
TUid CGomokuGameView::Id() const
{
	return TUid::Uid(EGomokuGameViewId);
}

void CGomokuGameView::SetGameData(CGomokuGameData* aGameData)
{
	iGameData = aGameData;
}


void CGomokuGameView::ChangeCbaL(const TInt aResourceId)
{
	CEikButtonGroupContainer* Cba = CEikButtonGroupContainer::Current();
	if (Cba)
	{
		Cba->SetCommandSetL(aResourceId);
		Cba->DrawNow();
	}
}

void CGomokuGameView::ChangeNaviPaneTextL(const TDesC& aText)
{
	// set the navi pane content
	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi =
			StatusPane()->PaneCapabilities(naviPaneUid);
	if (subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned())
	{
		CAknNavigationControlContainer
				* naviPane =
						static_cast<CAknNavigationControlContainer*> (StatusPane()->ControlL(
								naviPaneUid));
		if (iNaviDecorator)
		{
			delete iNaviDecorator;
			iNaviDecorator = NULL;
		}
		iNaviDecorator = naviPane->CreateNavigationLabelL(aText);

		naviPane->PushL(*iNaviDecorator);
	}
}

void CGomokuGameView::CleanupStatusPaneL()
{
	// reset the navi pane 
	TUid naviPaneUid = TUid::Uid(EEikStatusPaneUidNavi);
	CEikStatusPaneBase::TPaneCapabilities subPaneNavi =
			StatusPane()->PaneCapabilities(naviPaneUid);
	if (subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned())
	{
		CAknNavigationControlContainer
				* naviPane =
						static_cast<CAknNavigationControlContainer*> (StatusPane()->ControlL(
								naviPaneUid));
		if (iNaviDecorator)
		{
			naviPane->Pop(iNaviDecorator);
			delete iNaviDecorator;
			iNaviDecorator = NULL;
		}
	}
}

void CGomokuGameView::HandleClientRectChange()
{
	if ( iGameData->GetGrid() )
	{
		iGameData->GetGrid()->SetRect( ClientRect() );
	}
}


void CGomokuGameView::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
	{
	case EGomokuRestartGameCmd:
	{
		// Initialize a new game
		InitGameL();
		break;
	}
	case EGomokuNewGameCmd:
	{
		iGameData->SetGamePhase(EGameOver);
		// Go back to the settings view.
		AppUi()->HandleCommandL(EGomokuBackCmd);
		break;
	}
	default:
	{
		AppUi()->HandleCommandL(aCommand);
		break;
	}
	}
}

void CGomokuGameView::SelectedCellEventL(TPoint& aCellCoords)
{
	if (iGameData->GetGamePhase() == EIngame)
	{
		// The user selected a cell in the grid
		// -> forward this event to the currently active player.
		// If it's an AI player, it will most likely discard it.
		// If it's a human player, this will be the move of
		// the player, which will be sent back.
		iGameData->GetCurrentPlayer()->HandleSelectedCellL(aCellCoords.iX,
				aCellCoords.iY);
	}
}

void CGomokuGameView::ChangeViewContextTextL()
{
	// Read the formatting text, which is different if the pair check is enabled
	TInt resourceId =
			(iGameData->GetPairCheckEnabled()) ? R_CURRENT_TURN_PAIRS_TEXT
					: R_CURRENT_TURN_TEXT;
	RBuf unformattedText(iEikonEnv->AllocReadResourceL(resourceId));
	unformattedText.CleanupClosePushL();
	// Compile the formatted text that contains the information
	TBuf<80> viewContextText;
	TBuf<60> playerMainName(iGameData->GetCurrentPlayerMainName());
	if (iGameData->GetPairCheckEnabled())
	{
		// With pair check - also include the info how many pairs
		// have already been captured.
		viewContextText.Format(unformattedText,
				iGameData->GetCurrentPlayerNum(),
				&playerMainName,
				iGameData->GetPairsCaptured(1), iGameData->GetPairsCaptured(2));
	}
	else
	{
		// Without pair check - simply print the current player information
		viewContextText.Format(unformattedText,
				iGameData->GetCurrentPlayerNum(),
				&playerMainName);
	}

	ChangeNaviPaneTextL(viewContextText);
	CleanupStack::PopAndDestroy(1);	// unformattedText
}

void CGomokuGameView::HandleFinishedTurn(TInt aCol, TInt aRow)
{
	if (iGameData->GetGamePhase() != EIngame)
	{
		// Only process a turn if the game is currently active.
		return;
	}
	if (iTimer->IsActive())
	{
		User::Panic(
				_L("Didn't handle previous turn but got the next turn - impossible, implementation error of the client."),
				-100);
	}

	// Start new callback to process the move asynchronously
	iSaveMoveForAsyncCallback.SetXY(aCol, aRow);
	iTimer->Start(10, 10, TCallBack(TimerFunc, this));

}

TInt CGomokuGameView::TimerFunc(TAny* aObj)
{
	// Start the non-static processing method
	CGomokuGameView* view = (CGomokuGameView*) aObj;
	TRAPD(err, view->AsyncHandleFinishedTurnL ());
	return 0;
}

void CGomokuGameView::AsyncHandleFinishedTurnL()
{
	// If the game is paused, do not process the callback now.
	// Instead, remember that the callback has to be processed
	// once the game is unpaused
	if (iPaused)
	{
		iPausedCallbackWaiting = ETrue;
		iTimer->Cancel();
		return;
	}

	// Asynchronous (2nd) callback - executed if the game
	// is already finished, so that the UI had time to draw the
	// end result and we can display the dialog box on top of it.
	if (iGameData->GetGamePhase() != EIngame)
	{
		// Game is over, no further callbacks are required
		iTimer->Cancel();
		// Save the score

		// Only count win if two different players were playing against each other
		if (iGameData->GetGamePhase() != ETie && iGameData->GetPlayer1Name().CompareF(iGameData->GetPlayer2Name()) != 0)
		{
			// Increment games and wins for winner
			TGomokuScore curPlayerScore = iGameData->GetScoreForPlayer(iGameData->GetCurrentPlayerName());
			iGameData->SetScoreForPlayerL(iGameData->GetCurrentPlayerName(), curPlayerScore.iTotalGames + 1, curPlayerScore.iWins + 1);
			// Increment games for looser
			curPlayerScore = iGameData->GetScoreForPlayer(iGameData->GetOtherPlayerName());
			iGameData->SetScoreForPlayerL(iGameData->GetOtherPlayerName(), curPlayerScore.iTotalGames + 1, curPlayerScore.iWins);
		}
		
		// Prepare message asking if the user wants to play another game
		HBufC* winTitle = iEikonEnv->AllocReadResourceLC(R_RESTART_LINE1_TEXT);
		RBuf winMsg;
		winMsg.CleanupClosePushL();
		if (iGameData->GetGamePhase() == ETie)
		{
			// The game ended in a tie
			winMsg.Assign(iEikonEnv->AllocReadResourceL(R_TIE_TEXT));
		}
		else
		{
			// One of the players won - print which one it was.
			HBufC* winMsgOrig = iEikonEnv->AllocReadResourceLC(
					R_PLAYER_WON_TEXT);
			winMsg.CreateL(winMsgOrig->Length());
			winMsg.Format(*winMsgOrig, iGameData->GetCurrentPlayerNum());
			CleanupStack::PopAndDestroy(winMsgOrig);
		}
		// Display the dialog to ask the user if he wants to restart the same game
		// or choose the settings for a new one.
		TBool restartGame = iEikonEnv->QueryWinL(*winTitle, winMsg);
		CleanupStack::PopAndDestroy(2); // winMsg, winTitle
		if (restartGame)
		{
			// Restart the game with the same configuration
			InitGameL();
		}
		else
		{
			// Go back to the settings view
			// would be: AppUi()->HandleCommandL(EGomokuBackCmd);
			// Better alternative: don't do anything, so that the user can take a look
			// at the finished game board. A new game can be started through
			// the options menu.
			ChangeCbaL(R_GOMOKU_GAME_OPTIONS_NEWGAME_CONTROL_PANE);
		}
	}

	// The game is not over yet - process the callback:
	// Either process the move or - on the second callback, after it has already
	// been processed by the first callback, activate the next player.
	else
	{
		// A move is waiting to be processed!
		if (iSaveMoveForAsyncCallback.iX != -1 && iSaveMoveForAsyncCallback.iY
				!= -1)
		{
			// Set the grid cell to the color of the current player and
			// save how many new pairs were captured
			TInt numPairsCaptured = iGameData->GetGrid()->SetGridCell(
					iSaveMoveForAsyncCallback.iX, iSaveMoveForAsyncCallback.iY,
					iGameData->GetCurrentPlayerColor(),
					iGameData->GetPairCheckEnabled());
			// If at least one pair was captured, increase the number of captured pairs.
			if (numPairsCaptured > 0)
			{
				iGameData->IncreasePairsCaptured(
						iGameData->GetCurrentPlayerNum(), numPairsCaptured);
			}

			// Check in the area of the last move if the player won the game
			TBool wonGame = iGameData->GetGrid()->WinnerCheck(
					iGameData->GetCurrentPlayerColor(),
					iSaveMoveForAsyncCallback.iX, iSaveMoveForAsyncCallback.iY);

			// If a player has captured enough pairs, he won as well
			if (iGameData->GetPairCheckEnabled()
					&& iGameData->GetPairsCaptured(
							iGameData->GetCurrentPlayerNum()) >= iGameData->GetNumPairsWin())
			{
				wonGame = ETrue;
			}

			// If the game is over because the player got 5 in a row or captured 5+ pairs...
			if (wonGame)
			{
				// The game is over
				iGameData->SetGamePhase(EGameOver);
				// Set the background color of the game field to the winning player's color
				if (iGameData->GetCurrentPlayerColor() == EColor1)
				{
					// Red background color
					iGameData->GetGrid()->SetBgColor(TRgb(255, 150, 150));
				}
				else
				{
					// Blue background color
					iGameData->GetGrid()->SetBgColor(TRgb(150, 150, 255));
				}
				// Update the status text in the view
				ChangeViewContextTextL();
				// Inform the player implementations about the game result
				// (sends the winning color to the players)
				iGameData->GetPlayer1()->GameOver(
						iGameData->GetCurrentPlayerColor());
				iGameData->GetPlayer2()->GameOver(
						iGameData->GetCurrentPlayerColor());
			}
			else
			{
				// Check for a tie
				if (iGameData->GetGrid()->GetRemainingFreeCells() == 0)
				{
					iGameData->SetGamePhase(ETie);
					// Gray background color
					iGameData->GetGrid()->SetBgColor(TRgb(150, 150, 150));
					// Inform the player implementations about the game result
					// (sends the neutral color to the players -> no winning color!)
					iGameData->GetPlayer1()->GameOver(EColorNeutral);
					iGameData->GetPlayer2()->GameOver(EColorNeutral);
				}
			}
			// The move has been processed, delete the coordinates
			// so that the next callback of the still active timer will
			// activate the next player.
			// If the game is already over, this will be displayed to the
			// user as well in the callback, in order to give the UI time
			// to update the display and the grid.
			iSaveMoveForAsyncCallback.SetXY(-1, -1);
			// Copy the internal (reference) grid to the public grid that is
			// accessed by the player implementations in order to prevent
			// cheating by the player implementations.
			iSendGrid->CopyFrom(iGameData->GetGrid());

			if (iGameData->GetGamePhase() == EIngame)
			{
				// Still ingame - switch to the other player!
				iGameData->SwitchCurrentPlayer();
				// Update the status text in the view
				ChangeViewContextTextL();
			}
		}
		else
		{
			// The second callback after a move has been received and when
			// the game is not over yet -> don't request any more callbacks
			// and start the turn of the next player.
			iTimer->Cancel();
			iGameData->GetCurrentPlayer()->StartTurn();
		}
	}

	// Issue a screen update
	iGameData->GetGrid()->DrawDeferred();
}
