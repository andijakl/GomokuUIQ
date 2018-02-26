/*
============================================================================
 Name		 : CGomokuAppUi from GomokuAppUi.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares main application UI class.
============================================================================
*/

#ifndef GOMOKUAPPUI_H
#define GOMOKUAPPUI_H

#include <aknviewappui.h>
#include <avkon.hrh>
#include <ECom.h>
#include <aknnotewrappers.h>
#include <AknQueryDialog.h>
#include <Gomoku.rsg>
#include "GomokuGlobals.h"
#include "GomokuGameData.h"
#include "GomokuPlayerListView.h"
#include "GomokuSettingsView.h"
#include "GomokuGameView.h"
#include "GomokuScore.h"
#include "Gomoku.hrh"

/**
This class represents the application UI in Gomoku application,
It has responsibility to create the view.
*/
class CGomokuAppUi : public CAknViewAppUi
	{
public:
	void ConstructL();

	~CGomokuAppUi();

private:  // Functions from base classes

	/**
	* From CEikAppUi, HandleCommandL.
	* Takes care of command handling.
	* @param aCommand Command to be handled.
	*/
	void HandleCommandL( TInt aCommand );
	
private:
	/*
	 * Adapt the grid size to the current display size,
	 * so that the cells are (almost) square and have
	 * a reasonable pixel size.
	 * 
	 * @return ETrue if the size was changed from the previous
	 * settings. EFalse if no change was necessary.
	 */
	TBool OptimizeGridSize();

private:
	/**
	 * Pointer to the central game data class (owned by this class).
	 * Contains all relevant settings and parameters for the game. 
	 */
	CGomokuGameData* iGameData;
	
	/** The view that allows choosing both player implementations. */
	CGomokuPlayerListView* iGomokuPlayerListView;
	
	/** The settings view for choosing the game parameters. */
	CGomokuSettingsView* iGomokuSettingsView;

	/** The view that contains the actual game contents (grid etc.). */ 
	CGomokuGameView* iGomokuGameView;
	
	/** ID of the view that is currently visible on the screen. */
	TGomokuViewUids iActiveView;
	};
#endif // GOMOKUAPPUI_H
