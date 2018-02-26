/*
 ============================================================================
 Name		 : GomokuPlayerListView.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares the view for choosing player implementations.
 ============================================================================
 */

#include "GomokuPlayerListView.h"

/**
 * First phase of Symbian two-phase construction. Should not contain any
 * code that could leave.
 */
CGomokuPlayerListView::CGomokuPlayerListView() :
	iCurrentPlayer(1)
{
}

/** 
 * The view's destructor removes the container from the control
 * stack and destroys it.
 */
CGomokuPlayerListView::~CGomokuPlayerListView()
{
	if (iNaviDecorator)
	{
		delete iNaviDecorator;
		iNaviDecorator = NULL;
	}
	delete iGomokuPlayerList;
	iGomokuPlayerList = NULL;
}

/**
 * Symbian two-phase constructor.
 * This creates an instance then calls the second-phase constructor
 * without leaving the instance on the cleanup stack.
 * @return new instance of CGomokuPlayerListView
 */
CGomokuPlayerListView* CGomokuPlayerListView::NewL()
{
	CGomokuPlayerListView* self = CGomokuPlayerListView::NewLC();
	CleanupStack::Pop(self);
	return self;
}

/**
 * Symbian two-phase constructor.
 * This creates an instance, pushes it on the cleanup stack,
 * then calls the second-phase constructor.
 * @return new instance of CGomokuPlayerListView
 */
CGomokuPlayerListView* CGomokuPlayerListView::NewLC()
{
	CGomokuPlayerListView* self = new (ELeave) CGomokuPlayerListView();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

/**
 * Second-phase constructor for view.  
 * Initialize contents from resource.
 */
void CGomokuPlayerListView::ConstructL()
{
	BaseConstructL(R_GOMOKU_PLAYER_LIST_GOMOKU_PLAYER_LIST_VIEW);
}

/**
 * @return The UID for this view
 */
TUid CGomokuPlayerListView::Id() const
{
	return TUid::Uid(EGomokuPlayerListViewId);
}

/**
 * Handle a command for this view (override)
 * @param aCommand command id to be handled
 */
void CGomokuPlayerListView::HandleCommandL(TInt aCommand)
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
void CGomokuPlayerListView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
		TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	SetupStatusPaneL();

	if (iGomokuPlayerList == NULL)
	{
		iGomokuPlayerList = CreateContainerL();
		iGomokuPlayerList->SetMopParent(this);
		AppUi()->AddToStackL(*this, iGomokuPlayerList);

		GetPlayerTypesByDiscoveryL();
		SetCurrentPlayerL(iCurrentPlayer);
	}
	iGomokuPlayerList->MakeVisible(ETrue);
}

/**
 */
void CGomokuPlayerListView::DoDeactivate()
{
	TRAPD(err, CleanupStatusPaneL());

	if (iGomokuPlayerList)
	{
		iGomokuPlayerList->MakeVisible(EFalse);
		AppUi()->RemoveFromViewStack(*this, iGomokuPlayerList);
		delete iGomokuPlayerList;
		iGomokuPlayerList = NULL;
	}

}

/** 
 * Handle status pane size change for this view (override)
 */
void CGomokuPlayerListView::HandleStatusPaneSizeChange()
{
	CAknView::HandleStatusPaneSizeChange();

	// this may fail, but we're not able to propagate exceptions here
	TVwsViewId view;
	AppUi()->GetActiveViewId(view);
	if (view.iViewUid == Id())
	{
		TInt result;
		TRAP( result, SetupStatusPaneL() );
	}

}

void CGomokuPlayerListView::SetupStatusPaneL()
{
	ChangeNaviPaneTextL((iCurrentPlayer == 1) ? R_PLAYER1_TEXT : R_PLAYER2_TEXT);
}

void CGomokuPlayerListView::ChangeNaviPaneTextL(const TInt aResourceId)
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

void CGomokuPlayerListView::CleanupStatusPaneL()
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
 *	Creates the top-level container for the view.  You may modify this method's
 *	contents and the CGomokuPlayerList::NewL() signature as needed to initialize the
 *	container, but the signature for this method is fixed.
 *	@return new initialized instance of CGomokuPlayerList
 */
CGomokuPlayerList* CGomokuPlayerListView::CreateContainerL()
{
	return CGomokuPlayerList::NewL(ClientRect(), NULL, this, this);
}

void CGomokuPlayerListView::SetGameData(CGomokuGameData* aGameData)
{
	iGameData = aGameData;
}

void CGomokuPlayerListView::GetPlayerTypesByDiscoveryL()
{
	if (!iGomokuPlayerList)
		return;

	// Read info about all implementations into infoArray
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	CGomokuPlayerInterface::ListImplementationsL(infoArray);

	// Loop through each info for each implementation
	// and create and use each in turn
	TBuf<255> buf;
	TBuf<512> listString;
	HBufC* aiName = CEikonEnv::Static()->AllocReadResourceLC(R_AI);
	int iconIdx = 0;
	for (TInt i = 0; i < infoArray.Count(); i++)
	{
		TPtrC8 type = infoArray[i]->DataType();
		// Need to convert narrow descriptor to be wide in order to print it
		buf.Copy(type);
		if (buf.Find(*aiName) != KErrNotFound)
			iconIdx = 1;
		else
			iconIdx = 0;
		iGomokuPlayerList->CreateListBoxItemL(listString, iconIdx, buf);
		iGomokuPlayerList->AddListBoxItemL(listString);

		buf.Zero();
	}

	// Clean up
	CleanupStack::PopAndDestroy(aiName);
	CleanupStack::PopAndDestroy(); //infoArray, results in a call to CleanupEComArray
}

void CGomokuPlayerListView::HandleListBoxEventL(CEikListBox*,
		TListBoxEvent aEventType)
{
	if (aEventType == EEventEnterKeyPressed || aEventType == EEventItemDoubleClicked)
	{
		AppUi()->HandleCommandL(EGomokuNextCmd);
	}
}

void CGomokuPlayerListView::GetSelectedNameL(RBuf& aPlayerType)
{
	if (iGomokuPlayerList)
	{
		iGomokuPlayerList->GetSelectedNameL(aPlayerType);
	}
}

void CGomokuPlayerListView::HandleSelectedPlayerL()
{
	// First retrieve the name of the currently selected player in the listbox
	RBuf playerType;
	playerType.CleanupClosePushL();
	iGomokuPlayerList->GetSelectedNameL(playerType);
	// Create an implementation of the requested player
	CGomokuPlayerInterface* player = CreatePlayerImplementationL(playerType);

	if (GetCurrentPlayer() == 1)
	{
		// Assigned first player: switch to 2nd player
		iGameData->SetPlayer1(player, playerType); // Game data takes ownership of the player
	}
	else
	{
		// Assigned second player
		iGameData->SetPlayer2(player, playerType);
	}
	CleanupStack::PopAndDestroy(); // playerType
}

TInt CGomokuPlayerListView::GetCurrentPlayer()
{
	return iCurrentPlayer;
}

void CGomokuPlayerListView::SetCurrentPlayerL(TInt aPlayer)
{
	if (aPlayer < 1 || aPlayer > 2)
	{
		return;
	}
	iCurrentPlayer = aPlayer;
	switch (iCurrentPlayer)
	{
	case 1:
		ChangeNaviPaneTextL(R_PLAYER1_TEXT);
		ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
		break;
	case 2:
		ChangeNaviPaneTextL(R_PLAYER2_TEXT);
		ChangeCbaL(R_AVKON_SOFTKEYS_OPTIONS_BACK);
		break;
	}
}

void CGomokuPlayerListView::ChangeCbaL(const TInt aResourceId)
{
	CEikButtonGroupContainer* Cba = CEikButtonGroupContainer::Current();
	if (Cba)
	{
		Cba->SetCommandSetL(aResourceId);
		Cba->DrawNow();
	}
}

CGomokuPlayerInterface* CGomokuPlayerListView::CreatePlayerImplementationL(
		const TDesC& aPlayerType)
{
	// Convert the name from the 16 bit variant of the listbox back to
	// an 8 bit descriptor
	RBuf8 playerType8;
	playerType8.CleanupClosePushL();
	playerType8.CreateL(aPlayerType.Length());
	playerType8.Copy(aPlayerType);

	// Create an instance of the player
	CGomokuPlayerInterface * aiInterface = CGomokuPlayerInterface::NewL(
			playerType8);
	CleanupStack::PopAndDestroy(); // name8
	return aiInterface;
}

// CleanupEComArray function is used for cleanup support of locally declared arrays
void CleanupEComArray(TAny* aArray)
{
	(static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArray))->Close();
}

