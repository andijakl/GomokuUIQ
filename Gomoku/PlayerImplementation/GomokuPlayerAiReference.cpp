/*
 ============================================================================
 Name		 : GomokuPlayerAiReference.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuPlayerAiReference implementation
 ============================================================================
 */

#include "GomokuPlayerAiReference.h"

CGomokuPlayerAiReference::CGomokuPlayerAiReference() :
	iCurrentPlayer(EColor1), iEnemyPlayer(EColor2), iCurPos(0), iNthAnalysis(0)
	{
	// No implementation required
	}

CGomokuPlayerAiReference::~CGomokuPlayerAiReference()
	{
	if ( iIdleAO)
		{
		iIdleAO->Cancel ();
		delete iIdleAO;
		}
	delete iWorkingGrid;
	DeleteRatingGrid ();
	}

CGomokuPlayerAiReference* CGomokuPlayerAiReference::NewL()
	{
	CGomokuPlayerAiReference* self = new (ELeave)CGomokuPlayerAiReference();
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); // self;
	return self;
	}

void CGomokuPlayerAiReference::ConstructL()
	{
	iWorkingGrid = CGrid::NewL ();
	TTime time;
	time.HomeTime ();
	iSeed = time.Int64 ();
	}

void CGomokuPlayerAiReference::HandleSelectedCellL(TInt /*aCol*/, TInt /*aRow*/)
	{
	// The AI does not need to consider user inputs
	}

void CGomokuPlayerAiReference::InitL(CGrid* aGrid, eCellColor aYourColor,
		TBool aPairCheck, TInt aNumPairsWin)
	{
	iGrid = aGrid;
	iWorkingGrid->ResizeGridL (iGrid->GetSize ());
	iGridSize.SetSize (iGrid->GetCols (), iGrid->GetRows ());
	CreateRatingGridL ();
	iCurrentPlayer = aYourColor;
	iEnemyPlayer = (aYourColor == EColor1) ? EColor2 : EColor1;
	iPairCheck = aPairCheck;
	iNumPairsWin = aNumPairsWin;
	}

void CGomokuPlayerAiReference::StartTurn()
	{
	iIsActive = ETrue;

	// Calculate the AI move
	if ( !(iIdleAO))
		{
		TRAPD(err, iIdleAO = CIdle::NewL (CActive::EPriorityIdle))
		;
		}
	// Indicate that we have to process a new turn
	iProcessingActive = EFalse;
	if ( iIdleAO)
		{
		iIdleAO->Start (TCallBack (BackgroundCalcMove, this));
		}
	}

void CGomokuPlayerAiReference::GameOver(eCellColor /*aWinnerColor*/)
	{
	// The AI does not have to consider the game over event.
	// This would only be necessary for remote (network) players
	// to inform the other game client.
	}

TInt CGomokuPlayerAiReference::BackgroundCalcMove(TAny* aAi)
	{
	return ((CGomokuPlayerAiReference*)aAi)->DoBackgroundCalcMove ();
	}

TInt CGomokuPlayerAiReference::DoBackgroundCalcMove()
	{
	if ( !iProcessingActive)
		{
		iProcessingActive = ETrue;
		// Start a new processing task
		iProcessingCol = 0;
		iProcessingRow = 0;
		// Make a backup of the grid. The AI will work on the backup and not the live data
		CopyToWorkingGrid ();
		}

	if ( iProcessingRow == iWorkingGrid->GetRows ())
		{
		// Processed all fields - search the best turn and send it back
		// to the game
		// Search for best move in the rating field
		TInt bestMoveX = 0;
		TInt bestMoveY = 0;
		TInt bestRating=  TAKEN_SPACE;
		// Surround rating - if two fields with equal rating are found, the AI takes a look
		// at the rating of their surrounding fields. The field with the better surround rating
		// will be chosen. If there are more than one fields with the same surround rating, a random
		// one is chosen.
		TReal bestSurroundRating=  TAKEN_SPACE;
		TReal tempSurroundRating=  TAKEN_SPACE;
		TInt x, y;
		for (y=0; y < iWorkingGrid->GetRows (); y++)
			{
			for (x=0; x < iWorkingGrid->GetCols (); x++)
				{
				if ( iRatingGrid[x][y] > bestRating)
					{
					// We found a better move than the previous best -> Save the coordinates
					// and calculate the surround rating (which is automatically the new best
					// as the move is the best).
					bestMoveX = x;
					bestMoveY = y;
					bestRating = iRatingGrid[x][y];
					bestSurroundRating = CalcSurroundRating (x, y);
					}
				else
					if ( iRatingGrid[x][y] == bestRating)
						{
						// We already found another cell with the same rating as this one. Instead of
						// simply using the first move that we find, we choose the cell where the surrounding
						// cells are better.
						tempSurroundRating = CalcSurroundRating (x, y);
						TBool useNewCell = EFalse;
						if ( tempSurroundRating > bestSurroundRating)
							{
							useNewCell = ETrue;
							}
						else
							{
							if ( tempSurroundRating == bestSurroundRating)
								{
								// If two cells are equally good, use some randomness so
								// that not all games are the same.
								// Accept new cell with 1/5 propability
								if ( TInt (Math::Rand (iSeed)% 5)== 1)
									{
									useNewCell = ETrue;
									}
								}
							}
						// Decided to use the new cell instead of the previous best - save the coordinates
						// and its surround rating
						if ( useNewCell)
							{
							bestMoveX = x;
							bestMoveY = y;
							bestSurroundRating = tempSurroundRating;
							}
						}
				}
			}

		iIsActive = EFalse;
		iProcessingActive = EFalse;
		iObserver->HandleFinishedTurn (bestMoveX, bestMoveY);

		// The AI has finished processing, the CIdle-object should
		// be deactivated
		return EFalse;
		}
	else
		{
		// We need to know number of removed pairs so that we can restore the field when
		// pairs where deleted from the grid while the AI was trying all possible alternatives
		TInt removedPairs = 0;
		for (iProcessingCol = 0; iProcessingCol < iWorkingGrid->GetCols (); iProcessingCol ++)
			{
			// Process the next grid cell
			// Only consider this field for a move if it's not already taken.
			if ( iWorkingGrid->GetGridCell(iProcessingCol, iProcessingRow)->GetColor ()== EColorNeutral)
				{
				// Set the cell of the backup board. This routine will also do the paircheck (if enabled)
				// and remove stones.
				removedPairs = iWorkingGrid->SetGridCell (iProcessingCol,
						iProcessingRow, iCurrentPlayer, iPairCheck);
				// Analyze what the board is like after this move
				iRatingGrid[iProcessingCol][iProcessingRow] = AnalyzeBoard ();
				// Undo temp move
				if ( removedPairs > 0)
					{
					// Restore backup board (by overwriting it with a new backup of the original board)
					// because pairs where removed and more of the board was changed.
					CopyToWorkingGrid ();
					}
				else
					{
					// No pair was deleted, only undo move directly to save time.
					iWorkingGrid->SetGridCell (iProcessingCol, iProcessingRow,
							EColorNeutral, EFalse);
					}
				}
			else
				{
				// Field is taken - assign an ultra low rating so that it will not be chosen.
				iRatingGrid[iProcessingCol][iProcessingRow] = TAKEN_SPACE;
				}
			}

		// Schedule the next row - if processing the whole row at once
		// To process each cell individually for each callback, use
		// the code below instead.
		iProcessingRow ++;

		/*
		 // Use the following code instead if each turn should only process one
		 // cell instead of a whole row. Keep in mind that this will
		 // increase the time required to calculate the computer move because
		 // of the additional overhead required, but will lead to a better
		 // responsiveness of the application.
		 // Note: Remove the iProcessingCol for-loop if using the code below!

		 // Schedule the next cell
		 iProcessingCol ++;
		 if (iProcessingCol == iWorkingGrid->GetCols())
		 {
		 // Reached the end of the current row
		 iProcessingCol = 0;
		 iProcessingRow ++;
		 }
		 */

		// The AI has further work to do!
		return ETrue;
		}

	}

TReal CGomokuPlayerAiReference::CalcSurroundRating(TInt aX, TInt aY)
	{
	TInt x, y;
	// How many surrounding cells are considered (-> corners, taken cells)
	TInt consideringNumCells = 0;
	// Variable to sum up the rating of the surrounding cells
	TInt sumSurrounding = 0;
	// Take a look at the 9 surrounding cells of the coordinate
	for (x = aX - 1; x < aX + 2; x++)
		{
		if ( iWorkingGrid->IsInGrid (x, iWorkingGrid->GetCols ()))
			{
			for (y = aY - 1; y < aY + 2; y++)
				{
				if ( iWorkingGrid->IsInGrid (y, iWorkingGrid->GetRows ())&& !(x == aX && y == aY) && iRatingGrid[x][y] != TAKEN_SPACE)
					{
					// add the rating of this cell to the surround sum.
					consideringNumCells ++;
					sumSurrounding += iRatingGrid[x][y];
					}
				}
			}
		}
	// return the average surround rating.
	return ((TReal)sumSurrounding / (TReal)consideringNumCells);
	}

TInt CGomokuPlayerAiReference::AnalyzeBoard()
	{
	TInt totalRating = 0;

	// --- Horizontal
	// Go from left to right starting at the left border in direction: right
	CalcRow (totalRating, 0, iWorkingGrid->GetRows (), EFalse, 0, 1, 0);

	// --- Vertical
	// Go from top to bottom starting at the upper border in direction: down
	CalcRow (totalRating, 0, iWorkingGrid->GetCols (), ETrue, 0, 0, 1);

	// --- Diagonal 1
	// Go from top to bottom starting at the left border in direction: down right
	CalcRow (totalRating, 0, iWorkingGrid->GetRows ()- 4, EFalse, 0, 1, 1);
	// Go from left to right starting at the upper border in direction: down right
	CalcRow (totalRating, 1, iWorkingGrid->GetCols ()- 4, ETrue, 0, 1, 1);

	// --- Diagonal 2
	// Go from top to bottom starting at the left border in direction: up right
	CalcRow (totalRating, 4, iWorkingGrid->GetRows (), EFalse, 0, 1, -1);
	// Go from left to right starting at the lower border in direction: up right
	CalcRow (totalRating, 1, iWorkingGrid->GetCols ()- 4, ETrue,
			iWorkingGrid->GetRows ()-1, 1, -1);

	return totalRating;
	}

void CGomokuPlayerAiReference::CalcRow(TInt& aTotalRating, TInt aRowStart,
		TInt aRowEnd, TBool aAssignRowToX, TInt aOtherStartValue, TInt aDx,
		TInt aDy)
	{
	TInt x, y;
	eCellColor curCellColor = EColorNeutral;
	// Variable when potentially dangerous pair situation should be considered "XOO." then gets a penalty.
	TInt countMyPairs = 0;
	// Go through all rows as specified by the parameters
	for (TInt curRowPos = aRowStart; curRowPos < aRowEnd; curRowPos++)
		{
		// Starting analysis of a new row - reset the analyze segment
		ResetAnalyzeSeg ();
		countMyPairs = 0;
		// Set start value so that x and y coordinates can be increased in each iteration of the current row-loop.
		if ( aAssignRowToX)
			{
			x = curRowPos;
			y = aOtherStartValue;
			}
		else
			{
			y = curRowPos;
			x = aOtherStartValue;
			}
		// Go through the whole row.
		do
			{
			// Add the color of the current cell to the 5 cells wide analyzing segment
			// (round array structure).
			curCellColor = iWorkingGrid->GetGridCell(x, y)->GetColor ();
			AddToSeg (curCellColor);
			// calculate the rating of the current segment and add it to the total rating
			aTotalRating += CalcTotalRatingForSeg ();

			// The pair check prevention mechanism is a bit more complicated, as it needs
			// to search for a special pattern, whereas the normal AI calculations only
			// have to count the number of stones in a segment.
			if ( iPairCheck)
				{
				if ( curCellColor == iCurrentPlayer)
					{
					// Count how many cells of the own color are found in a row.
					countMyPairs ++;
					}
				else
					{
					// If a stone in another color than the own was found, check if the previous
					// two stones where of the own color (and we're not directly at the border of the grid).
					// If less than two stones preceded this event, it would have been reset previously.
					// If more stones are in a row, countMyPairs would be > 2.
					if ( countMyPairs == 2 && iNthAnalysis > 3)
						{
						// Get history color stored three cells ago.
						TInt tempPos = iCurPos - 3;
						// Do correction for round array structure
						if ( tempPos < 0)
							tempPos = 5 + tempPos;
						// If the stone 3 fields ago has the opposite color of the current one
						// and the two stones in the middle are the own color,
						// we have a potentially dangerous pair capture situation - therefore, decrease
						// the total rating.
						if ( iAnalyzeSeg[tempPos] != curCellColor)
							{
							aTotalRating -= 16;
							}
						}
					// A stone not in the own color was found - reset counter.
					countMyPairs = 0;
					}
				}

			// Go through the row in the specified direction until we have reached the end
			x += aDx;
			y += aDy;
			}
		while (x < iWorkingGrid->GetCols ()&& y < iWorkingGrid->GetRows ()&& y >= 0);
		}
	}

void CGomokuPlayerAiReference::ResetAnalyzeSeg()
	{
	// Reset analyze array to neutral + all counting variables to the beginning.
	iCurPos = 0;
	iNthAnalysis = 0;
	for (TInt i = 0; i < 5; i ++)
		{
		iAnalyzeSeg[i] = EColorNeutral;
		}
	}

void CGomokuPlayerAiReference::AddToSeg(eCellColor aCellColor)
	{
	// Increase position
	iCurPos ++;
	// Round array - if 5+, start again from 0.
	iCurPos %= 5;
	// Store the color
	iAnalyzeSeg[iCurPos] = aCellColor;
	// Store the number of stones added so that the segment isn't considered after
	// only for example 3 stones have been added when starting on the border of the grid.
	// If no correction would be done, the grid would look like it would be empty all around,
	// leading to a EFalse rating.
	iNthAnalysis ++;
	}

TInt CGomokuPlayerAiReference::CountHitsInSeg(eCellColor aCheckForColor)
	{
	TInt score = 0;
	// Only analyze the segment when more than 5 stones have already been added  so that the segment isn't considered after
	// only for example 3 stones have been added when starting on the border of the grid.
	// If no correction would be done, the grid would look like it would be empty all around,
	// leading to a EFalse rating.
	if ( iNthAnalysis >= 5)
		{
		for (TInt i=0; i<5; i++)
			{
			if ( iAnalyzeSeg[i] == aCheckForColor)
				{
				// Increase the score if a stone of the specified color is found.
				score ++;
				}
			else
				if ( iAnalyzeSeg[i] != EColorNeutral)
					{
					// If opposite color was found, the score is automatically 0.
					score = 0;
					break;
					}
			}
		}
	return score;
	}

TInt CGomokuPlayerAiReference::RateHitsForPlayer(eCellColor aMyColor,
		eCellColor aCountedFor, TInt aScore)
	{
	TInt rating = 0;
	// Assign a rating depending on how many stones were found.
	// The more stones, the higher the rating. It is always doubled, leaving space
	// for the enemy getting double points (see below).
	switch (aScore)
		{
		case 1:
			rating = 1;
			break;
		case 2:
			rating = 4;
			break;
		case 3:
			rating = 16;
			break;
		case 4:
			rating = 64;
			break;
		case 5:
			rating = 512; // Win situation - consider this higher than by normal rules
			break;
		}

	// If hits are counted for the enemy, rate it twice as much. This compensates
	// that the AI has already done a (temponary) move. Example:
	// Both the enemy and the AI have 3 in a row. As the enemy is the next, he will make
	// 4 out of it. Therefore, all constellations of the enemy receive a higher rating
	// than the own constellations.
	if ( aMyColor != aCountedFor)
		rating *= 2;

	return rating;
	}

TInt CGomokuPlayerAiReference::CalcTotalRatingForSeg()
	{
	// Analyze the own rating, counting how many own stones are in this segment.
	TInt myScore = CountHitsInSeg (iCurrentPlayer);
	// Rate it from the perspective of the current player
	TInt myRating = RateHitsForPlayer (iCurrentPlayer, iCurrentPlayer, myScore);

	// Analyze the rating of the enemy. Again, first count the number of stones in the segment.
	TInt enemyScore = CountHitsInSeg (iEnemyPlayer);
	// Rate it from the perspective that we counted stones of the enemy player.
	TInt enemyRating = RateHitsForPlayer (iCurrentPlayer, iEnemyPlayer,
			enemyScore);

	// Return the difference of those two ratings.
	return myRating - enemyRating;
	}

TInt CGomokuPlayerAiReference::TrimToGridSize(TInt aValue, TInt aGridSize)
	{
	if ( aValue < 0)
		aValue = 0;
	if ( aValue >= aGridSize)
		aValue = aGridSize - 1;
	return aValue;
	}

void CGomokuPlayerAiReference::CopyToWorkingGrid()
	{
	iWorkingGrid->CopyFrom (iGrid);
	}

void CGomokuPlayerAiReference::CreateRatingGridL()
	{
	// First remove previous grid (if necessary).
	DeleteRatingGrid ();

	// Create new 2D dynamic arrays.
	if ( iGridSize.iWidth > 0 && iGridSize.iHeight > 0)
		{
		iRatingGrid = new (ELeave) TInt* [iGridSize.iWidth];
		for (TInt i=0; i<iGridSize.iWidth; i++)
			{
			*(iRatingGrid+i) = new (ELeave) TInt[iGridSize.iHeight];
			}
		}
	}

void CGomokuPlayerAiReference::DeleteRatingGrid()
	{
	// If the grid exists, delete it.
	if ( iRatingGrid)
		{
		for (TInt i=0; i<iGridSize.iWidth; i++)
			{
			delete[] *(iRatingGrid+i);
			}
		delete[] iRatingGrid;
		iRatingGrid = NULL;
		}
	}

