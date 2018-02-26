/*
 ========================================================================
 Name        : GomokuPlayerList.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Container class for the view to choose the players.
 ========================================================================
 */
#ifndef GOMOKUPLAYERLIST_H
#define GOMOKUPLAYERLIST_H

#include <coecntrl.h>		
#include <barsread.h>
#include <stringloader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <aknviewappui.h>
#include <eikappui.h>
#include <gulicon.h>
#include <Gomoku.rsg>
#include <Gomoku.mbg>

#include "GomokuPlayerListView.h"
#include "Gomoku.hrh"

class MEikCommandObserver;
class CAknDoubleLargeStyleListBox;
class CEikTextListBox;
_LIT( KGomokuIconFile, "\\resource\\apps\\Gomoku.mif" );

/**
 * Container class for GomokuPlayerList
 * 
 * @class	CGomokuPlayerList GomokuPlayerList.h
 */
class CGomokuPlayerList: public CCoeControl
{
public:
	// constructors and destructor
	CGomokuPlayerList();
	static CGomokuPlayerList* NewL(const TRect& aRect,
			const CCoeControl* aParent, MEikCommandObserver* aCommandObserver,
			MEikListBoxObserver* aListBoxObserver);
	static CGomokuPlayerList* NewLC(const TRect& aRect,
			const CCoeControl* aParent, MEikCommandObserver* aCommandObserver,
			MEikListBoxObserver* aListBoxObserver);
	void ConstructL(const TRect& aRect, const CCoeControl* aParent,
			MEikCommandObserver* aCommandObserver,
			MEikListBoxObserver* aListBoxObserver);
	virtual ~CGomokuPlayerList();

public:
	// from base class CCoeControl
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandleResourceChange(TInt aType);

protected:
	// from base class CCoeControl
	void SizeChanged();

private:
	void InitializeControlsL(MEikListBoxObserver* aListBoxObserver);
	void LayoutControls();
	CCoeControl* iFocusControl;
	MEikCommandObserver* iCommandObserver;
public:
	void AddListBoxItemL(const TDesC& aString);
	static void
			AddListBoxItemL(CEikTextListBox* aListBox, const TDesC& aString);
	static RArray<TInt>* GetSelectedListBoxItemsLC(CEikTextListBox* aListBox);
	static void DeleteSelectedListBoxItemsL(CEikTextListBox* aListBox);
	CAknDoubleLargeStyleListBox* ListBox();
	static void CreateListBoxItemL(TDes& aBuffer, TInt aIconIndex,
			const TDesC& aMainText, const TDesC& aSecondaryText);
	static void CreateListBoxItemL(TDes& aBuffer, TInt aIconIndex,
			const TDesC& aMainAndSecondaryText);
	void AddListBoxResourceArrayItemL(TInt aResourceId, TInt aIconIndex);
	void SetupListBoxIconsL();
	static CGulIcon* LoadAndScaleIconL(const TDesC& aFileName, TInt aBitmapId,
			TInt aMaskId, TSize* aSize, TScaleMode aScaleMode);
	TBool HandleMarkableListCommandL(TInt aCommand);

	/**
	 * Get the name of the currently selected item in the list box.
	 *
	 * \param aAiName variable where to store the player name.
	 * It will be allocated (CreateL()) by this method.
	 */
	void GetSelectedNameL(RBuf& aAiName);
private:
	CAknDoubleLargeStyleListBox* iListBox;
public:
	enum TControls
	{
		EListBox,
		// add any user-defined entries here...
		ELastControl
	};
	enum TListBoxImages
	{
		EListBoxFirstUserImageIndex
	};
};

#endif // GOMOKUPLAYERLIST_H
