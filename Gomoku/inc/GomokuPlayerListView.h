/*
 ============================================================================
 Name		 : GomokuPlayerListView.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the view for choosing player implementations.
 ============================================================================
 */

#ifndef GOMOKUPLAYERLISTVIEW_H
#define GOMOKUPLAYERLISTVIEW_H

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
#include "GomokuPlayerList.h"

#include <e32debug.h>
#include "GomokuPlayerInterface.h"
#include "GomokuGameData.h"

class CGomokuPlayerList;	

void CleanupEComArray(TAny* aArray);

/**
 * Avkon view class for GomokuPlayerListView. It is registerd with the view server
 * by the AppUi. It owns the container control.
 * @class	CGomokuPlayerListView GomokuPlayerListView.h
 */					
class CGomokuPlayerListView : public CAknView, MEikListBoxObserver
	{
public:
	// constructors and destructor
	CGomokuPlayerListView();
	static CGomokuPlayerListView* NewL();
	static CGomokuPlayerListView* NewLC();        
	void ConstructL();
	virtual ~CGomokuPlayerListView();

	/**
	 * Store a pointer to the game data instance.
	 * This is owned by the AppUi and used by both views to
	 * keep track of the game settings.
	 *
	 * \param aGameData pointer to a game data object.
	 */
	void SetGameData(CGomokuGameData* aGameData);

	/**
	 * Should be called after a selection event from the listbox has
	 * been received.
	 * This method queries which player implementation is currently selected
	 * and starts the next necessary steps. These include instantiating
	 * the player implementation and switching to the selection for the
	 * second player or to the settings page of the view.
	 */
	void HandleSelectedPlayerL();
	
	/**
	 * Returns the number of the player that can currently be selected.
	 * 
	 * \return 1 for player 1, 2 for player 2
	 */
	TInt GetCurrentPlayer();
	
	/**
	 * Set the current player that can be selected by the user.
	 * Also updates the navi pane text and the softkeys.
	 * 
	 * \param aPlayer which player is currently active (1 or 2).
	 */
	void SetCurrentPlayerL(TInt aPlayer);

	/**
	 * Get the name of the currently selected item in the list box.
	 *
	 * \param aAiName variable where to store the player name.
	 * It will be allocated (CreateL()) by this method.
	 */
	void GetSelectedNameL(RBuf& aAiName);

	/**
	 * Create and return an implementation of the player with the specified
	 * name through the ECom framework.
	 *
	 * \param name of the implementation to instantiate.
	 * \param return the requested implementation of the interface.
	 */
	CGomokuPlayerInterface* CreatePlayerImplementationL(const TDesC& aName);

						
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
	
	CGomokuPlayerList* CreateContainerL();
	

	/**
	* From MEikListBoxObserve.
	* @param aEventType - type of event
	*/ 
	void HandleListBoxEventL(CEikListBox*, TListBoxEvent aEventType);

protected:
	// from base class CAknView
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	void DoDeactivate();
	void HandleStatusPaneSizeChange();
	
private:
	void SetupStatusPaneL();
	
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
	 * Resets the contents stored in the navi pane.
	 */
	void CleanupStatusPaneL();

	/**
	 * Query the ECom framework for available implementations
	 * of the player interface. The name of all implementations that
	 * are found are added to the list box owned by this view.
	 */
	void GetPlayerTypesByDiscoveryL();
	
private:
	
	/**
	 * The container that contains the control for this view.
	 */
	CGomokuPlayerList* iGomokuPlayerList;

	/**
	 * Any current navi decorator
	 */
	CAknNavigationDecorator* iNaviDecorator;
	
	/**
	 * Instance of the game data object.
	 * Not owned by this class!
	 */
	CGomokuGameData* iGameData;

	/**
	 * The player for which the user is currently selecting
	 * the implementation.
	 */
	TInt iCurrentPlayer;
	};

#endif // GOMOKUPLAYERLISTVIEW_H
