/*
 ============================================================================
 Name		 : CGomokuAppUi from GomokuAppUi.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuAppUi implementation
 ============================================================================
 */

#include "GomokuAppUi.h"

/**
 2nd stage construction of the App UI.
 Create view and add it to the framework.
 The framework will take over the ownership.
 */
void CGomokuAppUi::ConstructL()
{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// The game data is used by both views and therefore owned by the AppUi, which
	// is the parent of the views.
	iGameData = CGomokuGameData::NewL();

	if (iGameData->SettingsFileExistsL())
	{
		// Load the game settings if they have already
		// been saved to a file. Otherwise, the default settings
		// from the game data constructor will be used.
		iGameData->LoadSettingsFileL();
	}
	else
	{
		// First start -> automatically determine the optimal
		// Grid size for the current screen resolution.
		OptimizeGridSize();
	}

	// Create the player list view
	iGomokuPlayerListView = CGomokuPlayerListView::NewL();
	iGomokuPlayerListView->SetGameData(iGameData);
	AddViewL(iGomokuPlayerListView);

	// Create the game settings view
	iGomokuSettingsView = CGomokuSettingsView::NewL(iGameData);
	AddViewL(iGomokuSettingsView);

	// Create the game view
	iGomokuGameView = CGomokuGameView::NewL(iGameData);
	AddViewL(iGomokuGameView);

	SetDefaultViewL(*iGomokuPlayerListView);
	iActiveView = EGomokuPlayerListViewId;
}

TBool CGomokuAppUi::OptimizeGridSize()
{
	// Check if touch is enabled or not
	// Would be recommended to use PenEnabled directly -
	// but then, the application can't be compiled with the 3rd Ed.-SDK anymore :(
	// -> This approach is used from the FN Discussion boards
	TBool penEnabled = EFalse;

	RLibrary avkonDll;
	if ( avkonDll.Load( _L( "avkon.dll" ) ) == KErrNone )
	{
		#ifdef __WINS__
		TLibraryFunction PenEnabled = avkonDll.Lookup( 3184 );
		#else
		TLibraryFunction PenEnabled = avkonDll.Lookup( 4251 );
		#endif
	
		if ( PenEnabled != NULL )
		{
			penEnabled = PenEnabled();
		}
	
		avkonDll.Close();
	}
	// Make the cells at least 35x35 for touch devices and 25x25 for non-touch
	TInt cellSize = (penEnabled ? 35 : 25);
	
	// Set default values for grid size that fit to the screen
	TRect rect;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	// Each cell should be about 35 * 35 pixels.
	TInt gridCols = rect.Width() / cellSize;
	TInt gridRows = rect.Height() / cellSize;
	if (gridCols < 9)
		gridCols = 9;
	else if (gridCols > 25)
		gridCols = 25;
	if (gridRows < 9)
		gridRows = 9;
	else if (gridRows > 25)
		gridRows = 25;
	TSize prevGridSize(iGameData->GetGridSize());
	iGameData->SetGridSize(TSize(gridCols, gridRows));
	return (prevGridSize != iGameData->GetGridSize());
}

/**
 * Handle a command for this appui (override)
 * @param aCommand command id to be handled
 */
void CGomokuAppUi::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
	{
	// Go to the next page in the settings process
	// (player 1 -> player 2 -> game settings)
	case EGomokuNextCmd:
	{
		iGomokuPlayerListView->HandleSelectedPlayerL();
		if (iGomokuPlayerListView->GetCurrentPlayer() == 1)
		{
			// Proceed to player 2
			iGomokuPlayerListView->SetCurrentPlayerL(2);
		}
		else if (iGomokuPlayerListView->GetCurrentPlayer() == 2)
		{
			// Switch to game settings view
			iActiveView = EGomokuSettingsViewId;
			ActivateLocalViewL(TUid::Uid(EGomokuSettingsViewId));
		}
		break;
	}
		// Return to the previous view of the settings process.
	case EAknSoftkeyBack:
	case EGomokuBackCmd:
	{
		if (iActiveView == EGomokuPlayerListViewId)
		{
			// Return to player 1 selection
			// If player 1 is selected, the back command isn't displayed -
			// instead, the exit command is active
			iGomokuPlayerListView->SetCurrentPlayerL(1);
		}
		else if (iActiveView == EGomokuSettingsViewId)
		{
			// Return to player 2 selection
			iActiveView = EGomokuPlayerListViewId;
			ActivateLocalViewL(TUid::Uid(EGomokuPlayerListViewId));
			iGomokuPlayerListView->SetCurrentPlayerL(2);
		}
		else if (iActiveView == EGomokuGameViewId)
		{
			// Back to player 1 selection
			iActiveView = EGomokuPlayerListViewId;
			ActivateLocalViewL(TUid::Uid(EGomokuPlayerListViewId));
			iGomokuPlayerListView->SetCurrentPlayerL(1);
		}
		break;
	}
		// Show statistics for the currently selected player
	case EGomokuShowStatsCmd:
	{
		// Get name of currently selected player
		RBuf playerType;
		playerType.CleanupClosePushL();
		iGomokuPlayerListView->GetSelectedNameL(playerType);
		// Get stats for the player
		TGomokuScore curPlayerScore = iGameData->GetScoreForPlayer(playerType);
		// Load format string from resource file
		HBufC* statsText = CEikonEnv::Static()->AllocReadResourceLC(R_STATS_DLG_TEXT);
		// Format string
		TBuf<255> dlgText;
		TBuf<60> playerName(iGameData->GetPlayerMainName(playerType));
		dlgText.Format(*statsText, &playerName, curPlayerScore.iTotalGames,
				curPlayerScore.iWins);
		CleanupStack::PopAndDestroy(statsText);
		CleanupStack::PopAndDestroy(); // playerType
		// Show the stats to the user
		CAknInformationNote* note = new (ELeave) CAknInformationNote();
		note->ExecuteLD(dlgText);
		break;
	}
	// Reset the statistics for the currently selected player
	case EGomokuResetStatsCmd:
	{
		// Ask the user if he really wants to reset
		// the stats for this player
		CAknQueryDialog* dlg;
		dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
		TInt resetStats = dlg->ExecuteLD(R_GOMOKU_STATS_RESET_QUERY);
		// If yes: Reset stats
		if (resetStats)
		{
			// Get name of currently selected player
			RBuf playerType;
			playerType.CleanupClosePushL();
			iGomokuPlayerListView->GetSelectedNameL(playerType);
			// Set stats for player to 0
			iGameData->SetScoreForPlayerL(playerType, 0, 0);
			CleanupStack::PopAndDestroy(); // playerType
			// Display success note
			CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote();
			HBufC* success_text = CEikonEnv::Static()->AllocReadResourceLC(
					R_RESET_STATS_SUCCESS);
			note->ExecuteLD(*success_text);
			CleanupStack::PopAndDestroy(success_text);
		}
		break;
	}
	// Start a new game
	case EGomokuStartGameCmd:
	{
		// Save the settings from the settings dialog to the settings file.
		iGameData->SaveSettingsFileL();
		iActiveView = EGomokuGameViewId;
		ActivateLocalViewL(TUid::Uid(EGomokuGameViewId));
		break;
	}
	// Optimize the grid size for the display
	case EGomokuOptimizeGridSize:
	{
		TBool gridSizeChanged = OptimizeGridSize();
		iGomokuSettingsView->UpdateItemsFromGameDataL();
		// Display success note
		CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote();
		HBufC* status_text;
		if (gridSizeChanged)
		{
			// Grid size was changed
			status_text = CEikonEnv::Static()->AllocReadResourceLC(
					R_GRID_OPTIMAL_SIZE_OK);
		}
		else
		{
			// Grid size was already optimal before
			status_text = CEikonEnv::Static()->AllocReadResourceLC(
					R_GRID_OPTIMAL_SIZE_ALREADY_OK);
		}
		note->ExecuteLD(*status_text);
		CleanupStack::PopAndDestroy(status_text);
		break;
	}
	// About the game creator
	case EGomokuAboutCmd:
	{
		// Display a simple system about box.
		CAknInformationNote* note = new (ELeave) CAknInformationNote();
		HBufC* aboutText = CEikonEnv::Static()->AllocReadResourceLC(
				R_ABOUT_TEXT);
		note->ExecuteLD(*aboutText);
		CleanupStack::PopAndDestroy(aboutText);
		break;
	}
	// Esit the game
	case EAknSoftkeyExit:
	case EEikCmdExit:
	{
		TBool reallyQuit = ETrue;
		// Check if ingame -> if yes, display dialog box.
		if (iActiveView == EGomokuGameViewId)
		{
			// Ask the user if he really wants to reset
			// the stats for this player
			CAknQueryDialog* dlg;
			dlg = CAknQueryDialog::NewL(CAknQueryDialog::ENoTone);
			TInt reallyQuitAnswer = dlg->ExecuteLD(R_GOMOKU_EXIT_QUERY);
			// If yes: Reset stats
			if (!reallyQuitAnswer)
			{
				reallyQuit = EFalse;
			}
		}
		if (reallyQuit)
		{
			Exit();
		}
		break;
	}
	}
}

CGomokuAppUi::~CGomokuAppUi()
{
	// Delete the game data instance variable
	delete iGameData;
	// Close the session to the ECom framework.
	REComSession::FinalClose();
}
