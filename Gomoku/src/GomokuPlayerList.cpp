/*
 ========================================================================
 Name        : GomokuPlayerList.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Container class for the view to choose the players.
 ========================================================================
 */
#include "GomokuPlayerList.h"

/**
 * First phase of Symbian two-phase construction. Should not 
 * contain any code that could leave.
 */
CGomokuPlayerList::CGomokuPlayerList()
{
	iListBox = NULL;
}
/** 
 * Destroy child controls.
 */
CGomokuPlayerList::~CGomokuPlayerList()
{
	delete iListBox;
	iListBox = NULL;
}

/**
 * Construct the control (first phase).
 *  Creates an instance and initializes it.
 *  Instance is not left on cleanup stack.
 * @param aRect bounding rectangle
 * @param aParent owning parent, or NULL
 * @param aCommandObserver command observer
 * @return initialized instance of CGomokuPlayerList
 */
CGomokuPlayerList* CGomokuPlayerList::NewL(const TRect& aRect,
		const CCoeControl* aParent, MEikCommandObserver* aCommandObserver,
		MEikListBoxObserver* aListBoxObserver)
{
	CGomokuPlayerList* self = CGomokuPlayerList::NewLC(aRect, aParent,
			aCommandObserver, aListBoxObserver);
	CleanupStack::Pop(self);
	return self;
}

/**
 * Construct the control (first phase).
 *  Creates an instance and initializes it.
 *  Instance is left on cleanup stack.
 * @param aRect The rectangle for this window
 * @param aParent owning parent, or NULL
 * @param aCommandObserver command observer
 * @return new instance of CGomokuPlayerList
 */
CGomokuPlayerList* CGomokuPlayerList::NewLC(const TRect& aRect,
		const CCoeControl* aParent, MEikCommandObserver* aCommandObserver,
		MEikListBoxObserver* aListBoxObserver)
{
	CGomokuPlayerList* self = new (ELeave) CGomokuPlayerList();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent, aCommandObserver, aListBoxObserver);
	return self;
}

/**
 * Construct the control (second phase).
 *  Creates a window to contain the controls and activates it.
 * @param aRect bounding rectangle
 * @param aCommandObserver command observer
 * @param aParent owning parent, or NULL
 */
void CGomokuPlayerList::ConstructL(const TRect& aRect,
		const CCoeControl* aParent, MEikCommandObserver* aCommandObserver,
		MEikListBoxObserver* aListBoxObserver)
{
	if (aParent == NULL)
	{
		CreateWindowL();
	}
	else
	{
		SetContainerWindowL(*aParent);
	}
	iFocusControl = NULL;
	iCommandObserver = aCommandObserver;
	InitializeControlsL(aListBoxObserver);
	SetRect(aRect);
	ActivateL();
}

/**
 * Return the number of controls in the container (override)
 * @return count
 */
TInt CGomokuPlayerList::CountComponentControls() const
{
	return (int) ELastControl;
}

/**
 * Get the control with the given index (override)
 * @param aIndex Control index [0...n) (limited by #CountComponentControls)
 * @return Pointer to control
 */
CCoeControl* CGomokuPlayerList::ComponentControl(TInt aIndex) const
{
	switch (aIndex)
	{
	case EListBox:
		return iListBox;
	}
	// handle any user controls here...

	return NULL;
}

/**
 *	Handle resizing of the container. This implementation will lay out
 *  full-sized controls like list boxes for any screen size, and will layout
 *  labels, editors, etc. to the size they were given in the UI designer.
 *  This code will need to be modified to adjust arbitrary controls to
 *  any screen size.
 */
void CGomokuPlayerList::SizeChanged()
{
	CCoeControl::SizeChanged();
	LayoutControls();

}

/**
 * Layout components as specified in the UI Designer
 */
void CGomokuPlayerList::LayoutControls()
{
	iListBox->SetExtent(TPoint(0, 0), iListBox->MinimumSize());
}

/**
 *	Handle key events.
 */
TKeyResponse CGomokuPlayerList::OfferKeyEventL(const TKeyEvent& aKeyEvent,
		TEventCode aType)
{
	if (aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow)
	{
		// Listbox takes all events even if it doesn't use them
		return EKeyWasNotConsumed;
	}

	if (iFocusControl
			&& iFocusControl->OfferKeyEventL(aKeyEvent, aType)
					== EKeyWasConsumed)
	{
		return EKeyWasConsumed;
	}
	return CCoeControl::OfferKeyEventL(aKeyEvent, aType);
}

/**
 *	Initialize each control upon creation.
 */
void CGomokuPlayerList::InitializeControlsL(
		MEikListBoxObserver* aListBoxObserver)
{
	iListBox = new (ELeave) CAknDoubleLargeStyleListBox;
	iListBox->SetContainerWindowL(*this);
	iListBox->ConstructL(this, 0);
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
			CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto);
	// the listbox owns the items in the list and will free them
	iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
	iListBox->SetListBoxObserver(aListBoxObserver);
	iListBox->ActivateL();

	// setup the icon array so graphics-style boxes work
	SetupListBoxIconsL();

	// add list items

	iListBox->SetFocus(ETrue);
	iFocusControl = iListBox;

}

/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CGomokuPlayerList::HandleResourceChange(TInt aType)
{
	CCoeControl::HandleResourceChange(aType);
	SetRect(
			iAvkonViewAppUi->View(TUid::Uid(EGomokuPlayerListViewId))->ClientRect());
}

/**
 *	Add a list box item to a list.
 */
void CGomokuPlayerList::AddListBoxItemL(const TDesC& aString)
{
	CTextListBoxModel* model = iListBox->Model();
	CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
	itemArray->AppendL(aString);
	iListBox->HandleItemAdditionL();
}

/**
 *	Add a list box item to a list.
 */
void CGomokuPlayerList::AddListBoxItemL(CEikTextListBox* aListBox,
		const TDesC& aString)
{
	CTextListBoxModel* model = aListBox->Model();
	CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
	itemArray->AppendL(aString);
	aListBox->HandleItemAdditionL();
}

void CGomokuPlayerList::GetSelectedNameL(RBuf& aPlayerType)
{
	CTextListBoxModel* model = iListBox->Model();

	// Gets the index of the highlighted or the selected item from the
	// selection array
	const TInt currentIndex = iListBox->CurrentItemIndex();

	// Retrieve list box data, the data returned is Open().
	TPtrC listBoxData = model->ItemText(currentIndex);

	// Delete trailing icon info (up to and including the first \t)
	const TInt firstTabPos = listBoxData.Locate('\t') + 1;

	// Pushes the data onto the cleanup stack.
	// When CleanupStack::PopAndDestroy() is called, the data will be closed.
	//CleanupClosePushL (*listBoxData);
	// Get the name of the currently selected item
	aPlayerType.Close();
	aPlayerType.CreateL(listBoxData.Right(listBoxData.Length() - firstTabPos));
	// When done using the list box data it must call Close().
	//CleanupStack::PopAndDestroy (listBoxData);

}
/**
 * Get the array of selected item indices, with respect to the list model.
 * The array is sorted in ascending order.
 * The array should be destroyed with two calls to CleanupStack::PopAndDestroy(),
 * the first with no argument (referring to the internal resource) and the
 * second with the array pointer.
 * @return newly allocated array, which is left on the cleanup stack;
 *	or NULL for empty list. 
 */
RArray<TInt>* CGomokuPlayerList::GetSelectedListBoxItemsLC(
		CEikTextListBox* aListBox)
{
	CAknFilteredTextListBoxModel* model =
			static_cast<CAknFilteredTextListBoxModel *> (aListBox->Model());
	if (model->NumberOfItems() == 0)
		return NULL;

	// get currently selected indices
	const CListBoxView::CSelectionIndexArray* selectionIndexes =
			aListBox->SelectionIndexes();
	TInt selectedIndexesCount = selectionIndexes->Count();
	if (selectedIndexesCount == 0)
		return NULL;

	// copy the indices and sort numerically
	RArray<TInt>* orderedSelectedIndices = new (ELeave) RArray<TInt> (
			selectedIndexesCount);

	// push the allocated array
	CleanupStack::PushL(orderedSelectedIndices);

	// dispose the array resource
	CleanupClosePushL(*orderedSelectedIndices);

	// see if the search field is enabled
	CAknListBoxFilterItems* filter = model->Filter();
	if (filter)
	{
		// when filtering enabled, translate indices back to underlying model
		for (TInt idx = 0; idx < selectedIndexesCount; idx++)
		{
			TInt filteredItem = (*selectionIndexes)[idx];
			TInt actualItem = filter->FilteredItemIndex(filteredItem);
			orderedSelectedIndices->InsertInOrder(actualItem);
		}
	}
	else
	{
		// the selection indices refer directly to the model
		for (TInt idx = 0; idx < selectedIndexesCount; idx++)
			orderedSelectedIndices->InsertInOrder((*selectionIndexes)[idx]);
	}

	return orderedSelectedIndices;
}

/**
 * Delete the selected item or items from the list box.
 */
void CGomokuPlayerList::DeleteSelectedListBoxItemsL(CEikTextListBox* aListBox)
{
	CAknFilteredTextListBoxModel* model =
			static_cast<CAknFilteredTextListBoxModel *> (aListBox->Model());
	if (model->NumberOfItems() == 0)
		return;

	RArray<TInt>* orderedSelectedIndices = GetSelectedListBoxItemsLC(aListBox);
	if (!orderedSelectedIndices)
		return;

	// Delete selected items from bottom up so indices don't change on us
	CDesCArray* itemArray = static_cast<CDesCArray*> (model->ItemTextArray());
	TInt currentItem = 0;

	for (TInt idx = orderedSelectedIndices->Count(); idx-- > 0;)
	{
		currentItem = (*orderedSelectedIndices)[idx];
		itemArray->Delete(currentItem);
	}

	// dispose the array resources
	CleanupStack::PopAndDestroy();

	// dispose the array pointer
	CleanupStack::PopAndDestroy(orderedSelectedIndices);

	// refresh listbox's cursor now that items are deleted
	AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(aListBox,
			currentItem, ETrue);
}

/**
 * Get the listbox.
 */
CAknDoubleLargeStyleListBox* CGomokuPlayerList::ListBox()
{
	return iListBox;
}

/**
 * Create a list box item with the given column values.
 */
void CGomokuPlayerList::CreateListBoxItemL(TDes& aBuffer, TInt aIconIndex,
		const TDesC& aMainText, const TDesC& aSecondaryText)
{
	_LIT ( KStringHeader, "%d\t%S\t%S" );

	aBuffer.Format(KStringHeader(), aIconIndex, &aMainText, &aSecondaryText);
}

/**
 * Create a list box item with the given column values.
 */
void CGomokuPlayerList::CreateListBoxItemL(TDes& aBuffer, TInt aIconIndex,
		const TDesC& aMainAndSecondaryText)
{
	_LIT ( KStringHeader, "%d\t%S" );

	aBuffer.Format(KStringHeader(), aIconIndex, &aMainAndSecondaryText);
}

/**
 *	Add an item to the list by reading the text items from the array resource
 *	and setting a single image property (if available) from an index
 *	in the list box's icon array.
 *	@param aResourceId id of an ARRAY resource containing the textual
 *	items in the columns
 *	@param aIconIndex the index in the icon array, or -1
 */
void CGomokuPlayerList::AddListBoxResourceArrayItemL(TInt aResourceId,
		TInt aIconIndex)
{
	CDesCArray* array = iCoeEnv->ReadDesCArrayResourceL(aResourceId);
	CleanupStack::PushL(array);
	// This is intended to be large enough, but if you get 
	// a USER 11 panic, consider reducing string sizes.
	TBuf<512> listString;
	CreateListBoxItemL(listString, aIconIndex, (*array)[0], (*array)[1]);
	AddListBoxItemL(iListBox, listString);
	CleanupStack::PopAndDestroy(array);
}

/**
 *	Set up the list's icon array.
 */
void CGomokuPlayerList::SetupListBoxIconsL()
{
	CArrayPtr<CGulIcon>* icons = NULL;
	icons = new (ELeave) CAknIconArray(1);
	CleanupStack::PushL(icons);
	CGulIcon* icon;
	// for EListBoxGomtstList_iconIndex
	icon = LoadAndScaleIconL(KGomokuIconFile, EMbmGomokuIconhuman,
			EMbmGomokuIconhuman_mask, NULL, EAspectRatioPreserved);
	CleanupStack::PushL(icon);
	icons->AppendL(icon);
	CleanupStack::Pop(icon);
	icon = LoadAndScaleIconL(KGomokuIconFile, EMbmGomokuIconai,
			EMbmGomokuIconai_mask, NULL, EAspectRatioPreserved);
	CleanupStack::PushL(icon);
	icons->AppendL(icon);
	CleanupStack::Pop(icon);
	CleanupStack::Pop(icons);

	if (icons)
	{
		iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);
	}
}

/**
 * This routine loads and scales a bitmap or icon.
 *
 * @param aFileName the MBM or MIF filename
 * @param aBitmapId the bitmap id
 * @param aMaskId the mask id or -1 for none
 * @param aSize the TSize for the icon, or NULL to use real size
 * @param aScaleMode one of the EAspectRatio* enums when scaling
 */
CGulIcon* CGomokuPlayerList::LoadAndScaleIconL(const TDesC& aFileName,
		TInt aBitmapId, TInt aMaskId, TSize* aSize, TScaleMode aScaleMode)
{
	CFbsBitmap* bitmap;
	CFbsBitmap* mask;
	AknIconUtils::CreateIconL(bitmap, mask, aFileName, aBitmapId, aMaskId);

	TSize size;
	if (aSize == NULL)
	{
		// Use size from the image header.  In case of SVG,
		// we preserve the image data for a while longer, since ordinarily
		// it is disposed at the first GetContentDimensions() or SetSize() call.
		AknIconUtils::PreserveIconData(bitmap);
		AknIconUtils::GetContentDimensions(bitmap, size);
	}
	else
	{
		size = *aSize;
	}

	AknIconUtils::SetSize(bitmap, size, aScaleMode);
	AknIconUtils::SetSize(mask, size, aScaleMode);

	if (aSize == NULL)
	{
		AknIconUtils::DestroyIconData(bitmap);
	}

	return CGulIcon::NewL(bitmap, mask);
}
/** 
 * Handle commands relating to markable lists.
 */
TBool CGomokuPlayerList::HandleMarkableListCommandL(TInt aCommand)
{
	return EFalse;
}

