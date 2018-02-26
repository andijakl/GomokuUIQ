/*
* ============================================================================
*  Name     : CDynamicSettingListAppView from DynamicSettingListAppView.cpp
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
#include <aknsettingitemlist.h> 

// CLASS DECLARATION

class CDynamicSettingListSlider : public CAknSliderSettingItem
	{
public: // Constructor

	CDynamicSettingListSlider( TInt aIdentifier, TInt& aSliderValue );

protected: // Functions from base classes

	/**
	* Creates and executes a slider setting page.
	*/
	void CreateAndExecuteSettingPageL();
	};
	
// End of file
