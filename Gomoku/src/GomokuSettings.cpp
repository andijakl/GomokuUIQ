/*
 ========================================================================
 Name		 : GomokuSettings.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Container class for the view to set the game settings.
 ========================================================================
 */

#include "GomokuSettings.h"

CGomokuSettings::CGomokuSettings(CGomokuGameData* aGameData) :
	iGameData(aGameData)
{
}

CAknSettingItem* CGomokuSettings::CreateSettingItemL(TInt aIdentifier)
{
	switch (aIdentifier)
	{
	case ESettingsPairCheckbox:
		return new (ELeave) CAknBinaryPopupSettingItem(aIdentifier,
				iGameData->GetPairCheckEnabled());

	case ESettingsNumPairsWinSlider:
		return new (ELeave) CAknSliderSettingItem(aIdentifier,
				iGameData->GetNumPairsWin());

	case ESettingsColsSlider:
		return new (ELeave) CAknSliderSettingItem(aIdentifier,
				iGameData->GetGridCols());

	case ESettingsRowsSlider:
		return new (ELeave) CAknSliderSettingItem(aIdentifier,
				iGameData->GetGridRows());
	default:
		return NULL;
	}
}

void CGomokuSettings::UpdateItemsFromGameDataL()
{
	// Load the values for all items from the game data class.
	(*SettingItemArray())[ESettingsPairCheckbox]->LoadL();
	(*SettingItemArray())[ESettingsNumPairsWinSlider]->LoadL();
	(*SettingItemArray())[ESettingsColsSlider]->LoadL();
	(*SettingItemArray())[ESettingsRowsSlider]->LoadL();
	// Make sure the shown items correspond to the current settings.
	UpdateItemVisibilityL();
}

void CGomokuSettings::UpdateItemVisibilityL()
{
	// Only show the item that allows setting the number of pairs
	// if pair check is enabled.
	if (iGameData->GetPairCheckEnabled())
	{
		(*SettingItemArray())[ESettingsNumPairsWinSlider]->SetHidden(EFalse);
	}
	else
	{
		(*SettingItemArray())[ESettingsNumPairsWinSlider]->SetHidden(ETrue);
	}
	// Update the screen with the changes in the control.
	HandleChangeInItemArrayOrVisibilityL();
}

void CGomokuSettings::HandleResourceChange(TInt aType)
{
	CCoeControl::HandleResourceChange(aType);

	// Scalable UI support
	if (aType == KEikDynamicLayoutVariantSwitch)
	{
		TRect rect;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
		SetRect(rect);
	}
}

void CGomokuSettings::EditItemL(TInt aIndex, TBool aCalledFromMenu)
{
	// Let the selected item go into edit mode.
	CAknSettingItemList::EditItemL(aIndex, aCalledFromMenu);
	// After editing is finished, make sure the new value
	// is written back to the game data.
	(*SettingItemArray())[aIndex]->StoreL();
	// Make sure the shown items correspond to the current settings.
	UpdateItemVisibilityL();
}

void CGomokuSettings::SizeChanged()
{
	if (ListBox())
	{
		ListBox()->SetRect(Rect());
	}
}
