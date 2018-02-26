/*
 ============================================================================
 Name		 : Grid.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGrid implementation
 ============================================================================
 */

#include "Grid.h"

CGrid::CGrid(MGridObserver* aGridObserver) :
	iLastMove(-1, -1), iCurCell(-1, -1), iGridObserver(aGridObserver),
			iBgColor(255, 255, 255), iInitialized(EFalse)
{
	// No implementation required
}

EXPORT_C CGrid::~CGrid()
{
	DeleteGrid();
	DeleteBackBuffer();
}

EXPORT_C CGrid* CGrid::NewLC(const TRect& aRect, const CCoeControl* aParent,
		MGridObserver* aGridObserver)
{
	CGrid* self = new (ELeave) CGrid(aGridObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent);
	return self;
}

EXPORT_C CGrid* CGrid::NewL(const TRect& aRect, const CCoeControl* aParent,
		MGridObserver* aGridObserver)
{
	CGrid* self = CGrid::NewLC(aRect, aParent, aGridObserver);
	CleanupStack::Pop(); // self;
	return self;
}

EXPORT_C CGrid* CGrid::NewL()
{
	CGrid* self = new (ELeave) CGrid(NULL);
	CleanupStack::PushL(self);
	self->ConstructL(TRect(-1, -1, -1, -1), NULL);
	CleanupStack::Pop(); // self;
	return self;
}

void CGrid::ConstructL(const TRect& aRect, const CCoeControl* aParent)
{
	if (aRect.iBr.iX != -1 && aRect.iTl.iX != -1)
	{
		if (aParent)
		{
			// Control has a parent ...
			// ... reuse parents window
			SetContainerWindowL(*aParent);
		}
		else
		{
			// Control should be window-owning ...
			// ... create an own window
			CreateWindowL();
		}
		EnableDragEvents();

		// Assign size to this control
		SetRect(aRect);

		// Create backbuffer-bitmap (fullscreen)
		CreateBackBufferL(aRect.Size());
		// Control-initialisation finished, ready to draw
		ActivateL();
	}
}

EXPORT_C void CGrid::ResizeGridL(TSize aGridSize)
{
	// First remove previous grid (if necessary).
	DeleteGrid();
	// Save the new size and create the new grid (will take care of deleting old itself).
	iGridCols = aGridSize.iWidth;
	iGridRows = aGridSize.iHeight;
	CreateGridL();
	iLastMove.SetXY(-1, -1);
	iCurCell.SetXY(iGridCols / 2, iGridRows / 2);
	iRemainingFreeCells = iGridCols * iGridRows;
	CalculateSize();
	iInitialized = ETrue;
}

EXPORT_C void CGrid::CopyFrom(CGrid const * const aCopyFromGrid)
{
	if (aCopyFromGrid->GetCols() != GetCols() || aCopyFromGrid->GetRows()
			!= GetRows())
	{
		// Error - grid sizes do not match
		User::Panic(_L("Error: Grid size does not match."), -101);
	}
	// Copy the original grid to the backup grid (used by the AI for processing)
	for (TInt x = 0; x < iGridCols; x++)
	{
		memcpy(iGrid[x], aCopyFromGrid->iGrid[x], sizeof(TGridCell) * iGridRows);
	}
}

EXPORT_C TInt CGrid::GetCols() const
{
	return iGridCols;
}

EXPORT_C TInt CGrid::GetRows() const
{
	return iGridRows;
}

EXPORT_C TSize CGrid::GetSize() const
{
	return TSize(iGridCols, iGridRows);
}

EXPORT_C TGridCell** CGrid::GetGrid() const
{
	return iGrid;
}

EXPORT_C TBool CGrid::IsInGrid(TInt aValue, TInt aGridSize)
{
	return (aValue >= 0 && aValue < aGridSize);
}

EXPORT_C void CGrid::SetBgColor(const TRgb &aBgColor)
{
	iBgColor = aBgColor;
}

void CGrid::CreateGridL()
{
	// Create new 2D dynamic arrays.
	iGrid = new (ELeave) TGridCell*[iGridCols];
	for (TInt i = 0; i < iGridCols; i++)
	{
		*(iGrid + i) = new (ELeave) TGridCell[iGridRows];
	}
}

void CGrid::DeleteGrid()
{
	// If the grid exists, delete it.
	if (iGrid)
	{
		for (TInt i = 0; i < iGridCols; i++)
		{
			delete[] *(iGrid + i);
		}
		delete[] iGrid;
		iGrid = NULL;
	}
}

EXPORT_C TGridCell* CGrid::GetGridCell(const TInt aCol, const TInt aRow) const
{
	return &iGrid[aCol][aRow];
}

EXPORT_C TInt CGrid::SetGridCell(TInt aX, TInt aY, eCellColor aCellColor,
		TBool aPairCheck)
{
	TInt numPairsRemoved = 0;
	// First, simply set this cell to the new color.
	iGrid[aX][aY].SetColor(aCellColor);
	// Decrease the number of available cells
	iRemainingFreeCells--;
	// Save the last addition position for highlighting it when drawing
	iLastMove.SetXY(aX, aY);

	// Do pair check processing...
	if (aPairCheck && aCellColor != EColorNeutral)
	{
		eCellColor enemyCellColor = (aCellColor == EColor1) ? EColor2 : EColor1;
		TInt checkDx, checkDy;
		// Go in all directions.
		for (checkDx = -1; checkDx <= 1; checkDx++)
		{
			for (checkDy = -1; checkDy <= 1; checkDy++)
			{
				// Search in the direction specified by the dx and dy variables for pairs.
				if (FindPairs(aX, aY, checkDx, checkDy, aCellColor,
						enemyCellColor))
				{
					// Found a pair
					numPairsRemoved++;
					iRemainingFreeCells += 2;
					// Delete both enemy cells
					iGrid[aX + checkDx][aY + checkDy].SetColor(EColorNeutral);
					iGrid[aX + checkDx * 2][aY + checkDy * 2].SetColor(
							EColorNeutral);
				}
			}
		}
	}

	return numPairsRemoved;
}

TBool CGrid::FindPairs(TInt aX, TInt aY, TInt aDx, TInt aDy,
		eCellColor aMyCellColor, eCellColor aEnemyCellColor)
{
	TBool foundPair = false;
	// Define how far to go in each direction (max.)
	TInt maxX = aX + aDx * 3;
	TInt maxY = aY + aDy * 3;
	// Only search if the maximum value is still in the grid. If we're near the border,
	// don't do the pair check.
	if (IsInGrid(maxX, iGridCols) && IsInGrid(maxY, iGridRows))
	{
		// If we get through with the check without finding a cell that is not of the
		// checkForCellColor, we found a pair.
		foundPair = ETrue;

		// First, search for 2 enemy cells.
		eCellColor checkForCellColor = aEnemyCellColor;
		for (TInt i = 0; i < 3; i++)
		{
			aX += aDx;
			aY += aDy;

			// After two cells, the next one has to be of the own color.
			if (i == 2)
				checkForCellColor = aMyCellColor;

			// This stone isn't the expected color - no pair. Bad luck.
			if (iGrid[aX][aY].GetColor() != checkForCellColor)
			{
				foundPair = EFalse;
				break;
			}
		}
	}
	return foundPair;
}

EXPORT_C TInt CGrid::GetRemainingFreeCells()
{
	return iRemainingFreeCells;
}

EXPORT_C TBool CGrid::WinnerCheck(eCellColor aCheckForColor, TInt aCheckAtX,
		TInt aCheckAtY)
{
	TBool gameWon = EFalse;
	// Horizontal
	gameWon = WinnerCheckCells(aCheckForColor, aCheckAtX - 4, aCheckAtY, 1, 0);
	if (gameWon)
		return ETrue;
	// Vertical
	gameWon = WinnerCheckCells(aCheckForColor, aCheckAtX, aCheckAtY - 4, 0, 1);
	if (gameWon)
		return ETrue;
	// Diagonal 1
	gameWon = WinnerCheckCells(aCheckForColor, aCheckAtX - 4, aCheckAtY - 4, 1,
			1);
	if (gameWon)
		return ETrue;
	// Diagonal 2
	gameWon = WinnerCheckCells(aCheckForColor, aCheckAtX - 4, aCheckAtY + 4, 1,
			-1);
	if (gameWon)
		return ETrue;

	return EFalse;
}

TBool CGrid::WinnerCheckCells(eCellColor aCheckForColor, TInt aStartX,
		TInt aStartY, TInt aDx, TInt aDy)
{
	TBool gameWon = EFalse;
	TInt foundInRow = 0;
	for (TInt step = 0; step < 9; step++)
	{
		if (IsInGrid(aStartX, iGridCols) && IsInGrid(aStartY, iGridRows))
		{
			if (iGrid[aStartX][aStartY].GetColor() == aCheckForColor)
			{
				foundInRow++;
				if (foundInRow == 5)
				{
					gameWon = ETrue;
					break;
				}
			}
			else
			{
				foundInRow = 0;
			}
		}
		aStartX += aDx;
		aStartY += aDy;
	}
	return gameWon;
}

void CGrid::SizeChanged()
{
	const TSize newScreenSize = this->Size();
	// Resize the back buffer bitmap
	TRAPD(err, CreateBackBufferL(newScreenSize));
	if (iGridCols > 0 && iGridRows > 0)
		CalculateSize();
}

/** 
 * Handle global resource changes, such as scalable UI or skin events (override)
 */
void CGrid::HandleResourceChange(TInt aType)
{
	CCoeControl::HandleResourceChange(aType);
	if (aType == KEikDynamicLayoutVariantSwitch)
	{
		// User switched the layout configuration or the screen resolution :)
		// -> we have to recreate the layout
		ExtendToMainPane();
	}
}

EXPORT_C void CGrid::ExtendToMainPane()
{
	TRect rect;
	// ask where container's rectangle should be
	// EMainPane equals to area returned by
	//CEikAppUi::ClientRect()
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
	SetRect(rect);
}

void CGrid::CalculateSize()
{
	// The total available space of this control
	TRect controlRect(Rect());
	// Calculate the individual grid size, considering the total
	// available space and the number of columns / rows.
	// Due to the integer division this is clipped to the closest
	// lower value - this is intentionally.
	iGridCellSize.SetSize(controlRect.Width() / iGridCols, controlRect.Height()
			/ iGridRows);
	// Calculate the total size taken by the actual grid using the (rounded)
	// cell size.
	TSize totalGridSize(iGridCellSize.iWidth * iGridCols, iGridCellSize.iHeight
			* iGridRows);
	// Calculate the offset on the sides if the grid size is
	// smaller than the space available for the control
	TSize offset((controlRect.Width() - totalGridSize.iWidth) / 2,
			(controlRect.Height() - totalGridSize.iHeight) / 2);
	// Position the grid rectangle in absolute coordinates.
	iTotalGridRect.SetRect(controlRect.iTl.iX + offset.iWidth,
			controlRect.iTl.iY + offset.iHeight, controlRect.iTl.iX
					+ offset.iWidth + totalGridSize.iWidth, controlRect.iTl.iY
					+ offset.iHeight + totalGridSize.iHeight);
}

void CGrid::DeleteBackBuffer()
{
	if (iBackBufferBmpDrawingDevice)
	{
		delete iBackBufferBmpDrawingDevice;
		iBackBufferBmpDrawingDevice = NULL;
	}
	if (iBackBufferBmpGc)
	{
		delete iBackBufferBmpGc;
		iBackBufferBmpGc = NULL;
	}
	if (iBackBufferBmp)
	{
		delete iBackBufferBmp;
		iBackBufferBmp = NULL;
	}
}

void CGrid::CreateBackBufferL(const TSize aSize)
{
	DeleteBackBuffer();
	iBackBufferBmp = new (ELeave) CFbsBitmap;
	User::LeaveIfError(iBackBufferBmp->Create(aSize,
			iEikonEnv->ScreenDevice()->DisplayMode()));
	iBackBufferBmpDrawingDevice = CFbsBitmapDevice::NewL(iBackBufferBmp);
	//create graphics context for the bitmap
	User::LeaveIfError(iBackBufferBmpDrawingDevice->CreateContext(
			iBackBufferBmpGc));
}

void CGrid::Draw(const TRect &aRect) const
{
	TInt x, y;
	CWindowGc& gc = SystemGc();

	// Let S60 draw the background of the current theme
	//DrawBackground(aRect);

	// If the grid has not been created yet, don't continue!
	if (!iInitialized)
		return;

	// Draw the empty background of the grid itself (might be
	// smaller than the total background of the control)
	iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	iBackBufferBmpGc->SetBrushColor(KRgbBlack);
	iBackBufferBmpGc->Clear(aRect);

	iBackBufferBmpGc->SetBrushColor(iBgColor);
	iBackBufferBmpGc->Clear(iTotalGridRect);

	// Draw the black grid
	iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ENullBrush);
	iBackBufferBmpGc->SetPenColor(TRgb(0, 0, 0));
	for (x = iTotalGridRect.iTl.iX; x <= iTotalGridRect.iBr.iX; x
			+= iGridCellSize.iWidth)
	{
		iBackBufferBmpGc->DrawLine(TPoint(x, iTotalGridRect.iTl.iY), TPoint(x,
				iTotalGridRect.iBr.iY));
	}
	for (y = iTotalGridRect.iTl.iY; y <= iTotalGridRect.iBr.iY; y
			+= iGridCellSize.iHeight)
	{
		iBackBufferBmpGc->DrawLine(TPoint(iTotalGridRect.iTl.iX, y), TPoint(
				iTotalGridRect.iBr.iX, y));
	}

	// Highlight cell of last move
	iBackBufferBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBackBufferBmpGc->SetPenStyle(CGraphicsContext::ENullPen);
	if (iLastMove.iX > -1 && iLastMove.iY > -1)
	{
		iBackBufferBmpGc->SetBrushColor(TRgb(150, 255, 150));
		iBackBufferBmpGc->DrawRect(GetRectOfCell(iLastMove));
	}

	// Highlight currently selected cell
	if (iCurCell.iX > -1 && iCurCell.iY > -1)
	{
		iBackBufferBmpGc->SetBrushColor(TRgb(255, 255, 0));
		iBackBufferBmpGc->DrawRect(GetRectOfCell(iCurCell));
	}

	// Draw contents of the cells
	for (y = 0; y < iGridRows; y++)
	{
		for (x = 0; x < iGridCols; x++)
		{
			if (!iGrid[x][y].IsFree())
			{
				if (iGrid[x][y].GetColor() == EColor1)
				{
					DrawPlayer1Symbol(*iBackBufferBmpGc, GetRectOfCell(TPoint(
							x, y)));
				}
				else
				{
					DrawPlayer2Symbol(*iBackBufferBmpGc, GetRectOfCell(TPoint(
							x, y)));
				}
			}
		}
	}
	gc.BitBlt(TPoint(0, 0), iBackBufferBmp, aRect);
}

TKeyResponse CGrid::OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType)
{
	CCoeControl::OfferKeyEventL(aKeyEvent, aType);
	if (aType != EEventKey)
		return EKeyWasNotConsumed;

	// Enable control using the number keys
	// and translate them to joystick scan codes.
	TInt combinedKeyCode = aKeyEvent.iScanCode;
	switch (aKeyEvent.iCode)
	{
	case '1':
		combinedKeyCode = EStdKeyLeftUpArrow;
		break;
	case '2':
		combinedKeyCode = EStdKeyUpArrow;
		break;
	case '3':
		combinedKeyCode = EStdKeyRightUpArrow;
		break;
	case '4':
		combinedKeyCode = EStdKeyLeftArrow;
		break;
	case '5':
		combinedKeyCode = EStdKeyDevice3;
		break;
	case '6':
		combinedKeyCode = EStdKeyRightArrow;
		break;
	case '7':
		combinedKeyCode = EStdKeyLeftDownArrow;
		break;
	case '8':
		combinedKeyCode = EStdKeyDownArrow;
		break;
	case '9':
		combinedKeyCode = EStdKeyRightDownArrow;
		break;
	}

	switch (combinedKeyCode)
	{
	case EStdKeyLeftUpArrow:
	{
		iCurCell.iX--;
		iCurCell.iY--;
		break;
	}
	case EStdKeyUpArrow:
	{
		iCurCell.iY--;
		break;
	}
	case EStdKeyRightUpArrow:
	{
		iCurCell.iX++;
		iCurCell.iY--;
		break;
	}
	case EStdKeyLeftArrow:
	{
		iCurCell.iX--;
		break;
	}
	case EStdKeyRightArrow:
	{
		iCurCell.iX++;
		break;
	}
	case EStdKeyLeftDownArrow:
	{
		iCurCell.iX--;
		iCurCell.iY++;
		break;
	}
	case EStdKeyDownArrow:
	{
		iCurCell.iY++;
		break;
	}
	case EStdKeyRightDownArrow:
	{
		iCurCell.iX++;
		iCurCell.iY++;
		break;
	}
	case EStdKeyDevice3: // OK (Softkey center)
	case EStdKeyEnter:
	case EStdKeyNkpEnter:
	{
		SelectedCellEventL(iCurCell);
	}
		break;
	default:
		return EKeyWasNotConsumed;
	}

	if (iCurCell.iY < 0)
		iCurCell.iY = iGridRows - 1;
	if (iCurCell.iY >= iGridRows)
		iCurCell.iY = 0;
	if (iCurCell.iX < 0)
		iCurCell.iX = iGridCols - 1;
	if (iCurCell.iX >= iGridCols)
		iCurCell.iX = 0;

	DrawDeferred();
	return EKeyWasConsumed;
}

void CGrid::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	if (!iInitialized)
		return;
	TPoint newCell = GetCellForCoords(aPointerEvent.iPosition);
	switch (aPointerEvent.iType)
	{
	case TPointerEvent::EButton1Down:
	{
		// Button 1 down
		TPoint newCell = GetCellForCoords(aPointerEvent.iPosition);
		// If a cell inside the reach of the grid was selected...
		if (newCell.iX != -1 && newCell.iY != -1)
		{
			iCurCell = newCell;
		}
		break;
	}
	case TPointerEvent::EButton1Up:
	{
		// button 1 up
		if (newCell.iX != -1 && newCell.iY != -1)
		{
			iCurCell = newCell;
			SelectedCellEventL(iCurCell);
		}
		break;
	}
	case TPointerEvent::EDrag:
	{
		// drag event
		if (newCell.iX != -1 && newCell.iY != -1)
		{
			iCurCell = newCell;
		}
		break;
	}
	case TPointerEvent::EMove:
	{
		// move event
		break;
	}
	default:
		break;

	}
	DrawDeferred();
}

TPoint CGrid::GetCellForCoords(const TPoint& aPointerCoords) const
{
	// Convert the coordinates to the cell position, considering
	// the offset of the total grid
	TPoint cell((aPointerCoords.iX - iTotalGridRect.iTl.iX)
			/ iGridCellSize.iWidth, (aPointerCoords.iY - iTotalGridRect.iTl.iY)
			/ iGridCellSize.iHeight);
	// Check if the coordinates are within the grid size
	if (cell.iX < 0 || cell.iX >= iGridCols || cell.iY < 0 || cell.iY
			>= iGridRows)
	{
		cell.iX = -1;
		cell.iY = -1;
	}
	return cell;
}

TRect CGrid::GetRectOfCell(const TPoint& aCellCoords) const
{
	const TPoint upperLeft(iTotalGridRect.iTl.iX + aCellCoords.iX
			* iGridCellSize.iWidth + 1, iTotalGridRect.iTl.iY + aCellCoords.iY
			* iGridCellSize.iHeight + 1);
	return TRect(upperLeft.iX, upperLeft.iY, upperLeft.iX
			+ iGridCellSize.iWidth - 1, upperLeft.iY + iGridCellSize.iHeight
			- 1);
}

void CGrid::DrawPlayer1Symbol(CFbsBitGc& aGc, const TRect& aRect) const
{
	// Draw a red X
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenColor(TRgb(223, 60, 0));

	aGc.DrawLine(TPoint(aRect.iTl.iX + 1, aRect.iTl.iY + 1), TPoint(
			aRect.iBr.iX - 2, aRect.iBr.iY - 1));
	aGc.DrawLine(TPoint(aRect.iTl.iX + 2, aRect.iTl.iY + 1), TPoint(
			aRect.iBr.iX - 1, aRect.iBr.iY - 1));
	// Draw it two times with a slight offset, as it wouldn't
	// look good otherwise
	aGc.DrawLine(TPoint(aRect.iTl.iX + 1, aRect.iBr.iY - 2), TPoint(
			aRect.iBr.iX - 1, aRect.iTl.iY + 1));
	aGc.DrawLine(TPoint(aRect.iTl.iX + 1, aRect.iBr.iY - 3), TPoint(
			aRect.iBr.iX - 1, aRect.iTl.iY));
}

void CGrid::DrawPlayer2Symbol(CFbsBitGc& aGc, const TRect& aRect) const
{
	// Draw a blue O
	aGc.SetPenStyle(CGraphicsContext::ESolidPen);
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.SetPenColor(TRgb(31, 58, 255));
	// The ellipse has to be drawn 4 times in order to look good enough
	// and regular. Just drawing it with a rectangle 1px smaller on each
	// side would not result in a double-line, but break up in the corners.
	TRect tmpRect(aRect);
	tmpRect.iTl += TPoint(1, 1);
	tmpRect.iBr -= TPoint(1, 1);
	aGc.DrawEllipse(tmpRect);
	tmpRect.iTl += TPoint(1, 0);
	tmpRect.iBr -= TPoint(1, 0);
	aGc.DrawEllipse(tmpRect);
	tmpRect.iTl += TPoint(-1, 1);
	tmpRect.iBr -= TPoint(-1, 1);
	aGc.DrawEllipse(tmpRect);

}

void CGrid::SelectedCellEventL(TPoint& aCellCoords)
{
	// Check if the selected cell is valid (empty)
	if (iInitialized && iGridObserver && iGrid
			&& iGrid[aCellCoords.iX][aCellCoords.iY].IsFree())
	{
		// Send a callback to the grid observer
		iGridObserver->SelectedCellEventL(aCellCoords);
	}
}
