/*
 ============================================================================
 Name		 : GomokuPlayerHuman.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuPlayerHuman implementation
 ============================================================================
 */

#include "GomokuPlayerHuman.h"

CGomokuPlayerHuman::CGomokuPlayerHuman()
:
iIsActive(EFalse)
	{
	// No implementation required
	}

CGomokuPlayerHuman::~CGomokuPlayerHuman()
	{
	}

CGomokuPlayerHuman* CGomokuPlayerHuman::NewL()
	{
	CGomokuPlayerHuman* self = new (ELeave)CGomokuPlayerHuman();
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); // self;
	return self;
	}

void CGomokuPlayerHuman::ConstructL()
	{

	}

void CGomokuPlayerHuman::HandleSelectedCellL(TInt aCol, TInt aRow)
	{
	if (!iIsActive)
		{
		RDebug::Print(_L("Human player: Got cell event but is not active"));
		return;
		}

	// Check if the requested cell is still free so that the player
	// can place is move there.
	if (iGrid && (iGrid->GetGridCell(aCol, aRow))->IsFree())
		{
		// This move is ok, send it back to the listener interface
		if (iObserver)
			{
			iIsActive = EFalse;
			iObserver->HandleFinishedTurn(aCol, aRow);
			}
		}
	}

void CGomokuPlayerHuman::StartTurn()
	{
	iIsActive = ETrue;
	}

void CGomokuPlayerHuman::GameOver(eCellColor /*aWinnerColor*/)
	{

	}

void CGomokuPlayerHuman::InitL(CGrid* aGrid, eCellColor /*aYourColor*/, TBool /*aPairCheck*/, TInt /*aNumPairsWin*/)
	{
	iGrid = aGrid;
	}

