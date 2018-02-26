/*
* ============================================================================
*  Name     : CDynamicSettingListSlider from CDynamicSettingListSlider.cpp
*  Part of  : Dynamic Setting List
*  Created  : 09/21/2005 by Forum Nokia
*  Version  : 1.0
*
*  Copyright (c) 2005 - 2007 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ============================================================================
*/

// INCLUDE FILES
#include "DynamicSettingListSlider.h"
#include <aknslidersettingpage.h>

// ================= MEMBER FUNCTIONS =======================

// Constructor
CDynamicSettingListSlider::CDynamicSettingListSlider( TInt aIdentifier, TInt& aSliderValue )
	: CAknSliderSettingItem(aIdentifier, aSliderValue)
	{

	}

// ----------------------------------------------------
// CDynamicSettingListAppView::CreateAndExecuteSettingPageL()
// Creates and executes a slider setting page.
// ----------------------------------------------------
//
void CDynamicSettingListSlider::CreateAndExecuteSettingPageL()
	{
	CAknSettingPage* dlg = CreateSettingPageL();

	SetSettingPage( dlg );
	SettingPage()->SetSettingPageObserver(this);

	// This has to be called so that slider setting page will work correctly in
	// all environments (WINS UDEB, GCCE UREL, etc):
    // CAknSliderSettingItem has a  fault that occurs at least in S60 2nd
    // Edition emulator debug builds and in 3rd Edition gcce builds. This fault
    // prevents creating CAknSliderSettingItem dynamically. A workaround is
    // to create a proprietary slider setting item and inherit it from
    // CAknSliderSettingItem (see CDynamicSettingListSlider.h). Then override
    // CAknSliderSettingItem::CreateAndExecuteSettingPageL
    // and use the solution pointed out in
    // CDynamicSettingListSlider::CreateAndExecuteSettingPageL.

	SettingPage()->SetSettingTextL(SettingName());

  	SettingPage()->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
	SetSettingPage(0);
	}

// End of file
