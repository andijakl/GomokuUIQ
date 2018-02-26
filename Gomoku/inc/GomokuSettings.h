/*
 ============================================================================
 Name		 : GomokuSettings.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Container class for the view to change the game settings.
 ============================================================================
 */

#ifndef GOMOKUSETTINGS_H
#define GOMOKUSETTINGS_H

// INCLUDES
#include <AknSettingItemList.h>
#include <e32std.h>
#include <e32base.h>
#include <coemain.h>
#include <coecntrl.h>
#include <aknsettingitemlist.h> 
#include <eikfrlbd.h>
#include "DynamicSettingListSlider.h"
#include "GomokuGameData.h"
#include <Gomoku.rsg>
#include "Gomoku.hrh"

// CLASS DECLARATION

/**
 *  CGomokuSettings
 * 
 */
class CGomokuSettings: public CAknSettingItemList
{
public:
	// Constructors and destructor

	/**
	 * Constructor for performing 1st stage construction
	 */
	CGomokuSettings(CGomokuGameData* aGameData);

	/**
	 * Default constructor for performing 2nd stage construction
	 */
	void ConstructL(const TRect& aRect);

public:
	// Functions from base classes

	/**
	 * Framework method to create a setting item based upon the user id aSettingId.
	 * From CAknSettingItemList, CreateSettingItemL.
	 */
	virtual CAknSettingItem* CreateSettingItemL(TInt identifier);

	/**
	 * Edit one of the items in the settings list.
	 */
	void EditItemL(TInt aIndex, TBool aCalledFromMenu);

	/**
	 * Update the items in the settings list with the current
	 * values stored in the game data class. Also refreshes the
	 * view so that the changes get visible.
	 */
	void UpdateItemsFromGameDataL();

	/**
	 * Check which items of the settings list should be visible
	 * based on the current settings.
	 */
	void UpdateItemVisibilityL();

private:
	// Functions from base classes

	/**
	 * Called by framework when layout is changed.
	 * From CoeControl, HandleResourceChange.
	 */
	virtual void HandleResourceChange(TInt aType);

	/**
	 * Called by framework when the view size is changed.
	 * From CoeControl, SizeChanged.
	 */
	virtual void SizeChanged();

private:
	// Data members

	/**
	 * Instance of the game data object.
	 * Not owned by this class!
	 */
	CGomokuGameData* iGameData;

};

#endif // GOMOKUSETTINGS_H
