/*
 ============================================================================
 Name		 : GomokuPlayerAiRandom.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuPlayerAiRandom implementation
 ============================================================================
 */

#include "GomokuPlayerAiRandom.h"

CGomokuPlayerAiRandom::CGomokuPlayerAiRandom()
	{
	// No implementation required
	}

CGomokuPlayerAiRandom::~CGomokuPlayerAiRandom()
	{
	}

CGomokuPlayerAiRandom* CGomokuPlayerAiRandom::NewL()
	{
	CGomokuPlayerAiRandom* self = new (ELeave)CGomokuPlayerAiRandom();
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); // self;
	return self;
	}

void CGomokuPlayerAiRandom::ConstructL()
	{
	// Initialize the random number generator with the current time
	TTime time;
	time.HomeTime ();
	iSeed = time.Int64 ();
	}


void CGomokuPlayerAiRandom::HandleSelectedCellL(TInt /*aCol*/, TInt /*aRow*/)
	{
	// The AI does not need to consider user inputs
	}

void CGomokuPlayerAiRandom::StartTurn()
	{
	// Search for a free field
	TInt computerMoveX = 0;
	TInt computerMoveY = 0;
	TBool foundFreeCell = EFalse;

	// This algorithm just thinks of a random cell and checks if it
	// is free. Obviously, this isn't very smart and will take an
	// increasingly long time the more cells are already taken...
	do
		{
		computerMoveX = Math::Rand(iSeed) % iGridSize.iWidth;
		computerMoveY = Math::Rand(iSeed) % iGridSize.iHeight;
		foundFreeCell = (iGrid->GetGridCell(computerMoveX, computerMoveY))->IsFree();
		} while (!foundFreeCell);

	// If a free cell is found, send it back as our move to the observer.
	iObserver->HandleFinishedTurn (computerMoveX, computerMoveY);
	}

void CGomokuPlayerAiRandom::GameOver(eCellColor /*aWinnerColor*/)
	{
	// The AI does not have to consider the game over event.
	// This would only be necessary for remote (network) players
	// to inform the other game client.
	}

void CGomokuPlayerAiRandom::InitL(CGrid* aGrid, eCellColor aYourColor, TBool aPairCheck, TInt aNumPairsWin)
	{
	// Store the information provided in instance variables of this class
	iGrid = aGrid;
	iGridSize.SetSize(iGrid->GetCols(), iGrid->GetRows());
	iCurrentPlayer = aYourColor;
	iEnemyPlayer = (aYourColor == EColor1) ? EColor2 : EColor1;
	iPairCheck = aPairCheck;
	iNumPairsWin = aNumPairsWin;
	}
