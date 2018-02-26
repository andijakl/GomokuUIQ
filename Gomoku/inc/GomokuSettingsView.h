/*
 ============================================================================
 Name		 : GomokuSettingsView.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the view for setting up the game parameters.
 ============================================================================
 */

#ifndef GOMOKUSETTINGSVIEW_H
#define GOMOKUSETTINGSVIEW_H

// INCLUDES

#include <aknview.h>
#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <barsread.h>
#include <stringloader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <akncontext.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <aknnavi.h>
#include <eikbtgpc.h>
#include <Gomoku.rsg>

#include "Gomoku.hrh"
#include "GomokuSettings.h"

#include <e32debug.h>
#include "GomokuGameData.h"


// CLASS DECLARATION

/**
 * Avkon view class for CGomokuSettingsView. It is registerd with the view server
 * by the AppUi. It owns the container control.
 */
class CGomokuSettingsView: public CAknView
{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CGomokuSettingsView();

	/**
	 * Two-phased constructor.
	 */
	static CGomokuSettingsView* NewL(CGomokuGameData* aGameData);

	// from base class CAknView
	TUid Id() const;

	/**
	 * Handles all commands in the view.
	 * Called by the UI framework when a command has been issued.
	 * The command Ids are defined in the .hrh file.
	 * 
	 * \param aCommand The command to be executed
	 */
	void HandleCommandL( TInt aCommand );

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CGomokuSettingsView(CGomokuGameData* aGameData);

	/**
	 * Default constructor for performing 2nd stage construction
	 */
	void ConstructL();

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
	
	/**
	 * Change the text of the navigation pane to the specified
	 * resource ID.
	 *
	 * \param aResourceId id of the text resource to place in
	 * the navigation pane. Has to be defined as a RESOURCE TBUF in
	 * the resource files.
	 */
	void ChangeNaviPaneTextL (const TInt aResourceId);

	/**
	 * Resets the contents stored in the navi pane.
	 */
	void CleanupStatusPaneL();
	
protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	
public:
	/**
	 * Update the items in the settings list with the current
	 * values stored in the game data class. Also refreshes the
	 * view so that the changes get visible.
	 */
	void UpdateItemsFromGameDataL();
private:

	/**
	 * The container that contains the control for this view.
	 */
	CGomokuSettings* iGomokuSettings;
	
	/**
	 * Instance of the game data object.
	 * Not owned by this class!
	 */
	CGomokuGameData* iGameData;

	/**
	 * Any current navi decorator
	 */
	CAknNavigationDecorator* iNaviDecorator;
};

#endif // GOMOKUSETTINGSVIEW_H
