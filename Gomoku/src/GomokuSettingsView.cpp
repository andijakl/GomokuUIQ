/*
 ============================================================================
 Name		 : GomokuSettingsView.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the view for setting up the game parameters.
 ============================================================================
 */

#include "GomokuSettingsView.h"

CGomokuSettingsView::CGomokuSettingsView(CGomokuGameData* aGameData) :
	iGameData(aGameData)
{
	// No implementation required
}

CGomokuSettingsView* CGomokuSettingsView::NewL(CGomokuGameData* aGameData)
{
	CGomokuSettingsView* self = new (ELeave) CGomokuSettingsView(aGameData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

void CGomokuSettingsView::ConstructL()
{
	BaseConstructL(R_GOMOKU_SETTINGS_VIEW);
}

CGomokuSettingsView::~CGomokuSettingsView()
{
	if (iNaviDecorator)
	{
		delete iNaviDecorator;
		iNaviDecorator = NULL;
	}
	if (iGomokuSettings)
	{
		AppUi()->RemoveFromStack(iGomokuSettings);
	}
	delete iGomokuSettings;
}

/**
 * @return The UID for this view
 */
TUid CGomokuSettingsView::Id() const
{
	return TUid::Uid(EGomokuSettingsViewId);
}


void CGomokuSettingsView::ChangeCbaL(const TInt aResourceId)
{
	CEikButtonGroupContainer* Cba = CEikButtonGroupContainer::Current();
	if (Cba)
	{
		Cba->SetCommandSetL(aResourceId);
		Cba->DrawNow();
	}
}

void CGomokuSettingsView::ChangeNaviPaneTextL(const TInt aResourceId)
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

		HBufC* labelText = StringLoader::LoadLC(aResourceId);
		iNaviDecorator = naviPane->CreateNavigationLabelL(*labelText);
		CleanupStack::PopAndDestroy(labelText);

		naviPane->PushL(*iNaviDecorator);
	}
}

void CGomokuSettingsView::CleanupStatusPaneL()
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

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CGomokuSettingsView::HandleCommandL(TInt aCommand)
{
	switch (aCommand)
	{
	default:
	{
		AppUi()->HandleCommandL(aCommand);
		break;
	}
	}
}

/**
 *	Handles user actions during activation of the view, 
 *	such as initializing the content.
 */
void CGomokuSettingsView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
		TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	ChangeNaviPaneTextL(R_GAME_SETTINGS);
	ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_BACK);
	
	if (!iGomokuSettings)
	{
		iGomokuSettings = new (ELeave) CGomokuSettings(iGameData);

		iGomokuSettings->SetMopParent(this);
		iGomokuSettings->ConstructFromResourceL(R_GOMOKU_SETTINGS_LIST);
		AppUi()->AddToStackL(*this, iGomokuSettings);
	}
	iGomokuSettings->UpdateItemVisibilityL();
	iGomokuSettings->MakeVisible(ETrue);
	iGomokuSettings->SetRect(ClientRect());
	iGomokuSettings->ActivateL();
	iGomokuSettings->DrawNow();
}

/**
 */
void CGomokuSettingsView::DoDeactivate()
{
	TRAPD(err, CleanupStatusPaneL());
	if (iGomokuSettings)
	{
		iGomokuSettings->MakeVisible(EFalse);
	}
}

void CGomokuSettingsView::UpdateItemsFromGameDataL()
{
	if (iGomokuSettings)
	{
		iGomokuSettings->UpdateItemsFromGameDataL();
	}
}
