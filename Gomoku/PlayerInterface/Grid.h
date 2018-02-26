/*
 ============================================================================
 Name		 : Grid.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Defines the game grid control that manages the state of the
 individual cells.
 ============================================================================
 */

#ifndef GRID_H
#define GRID_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <akndef.h>
#include <COECNTRL.H>
#include <AknUtils.h>
#include <fbs.h>  
#include <bitstd.h>
#include <bitdev.h>
#include <eikon.hrh>

#include "GridCell.h"
#include "GridObserver.h"

#ifndef EStdKeyLeftUpArrow
// TStdScanCode aliases - taken from S60 5th Edition
// These keycodes are not defined for S60 3rd Edition, MR
// and have therefore be defined manually when compiling using this SDK.
#define EStdKeyLeftUpArrow      EStdKeyDevice10  // Diagonal arrow event
#define EStdKeyRightUpArrow     EStdKeyDevice11  // Diagonal arrow event
#define EStdKeyRightDownArrow   EStdKeyDevice12  // Diagonal arrow event
#define EStdKeyLeftDownArrow    EStdKeyDevice13  // Diagonal arrow event
#endif

// CLASS DECLARATION

/**
 *  Defines the UI control that can display and manage the game grid.
 */
class CGrid : public CCoeControl
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	IMPORT_C ~CGrid();

	/**
	 * Two-phased constructor.
	 */
	IMPORT_C static CGrid* NewL(const TRect& aRect, const CCoeControl* aParent, MGridObserver* aGridObserver);

	/**
	 * Two-phased constructor.
	 */
	IMPORT_C static CGrid* NewLC(const TRect& aRect, const CCoeControl* aParent, MGridObserver* aGridObserver);

	/**
	 * Two-phased constructor.
	 */
	IMPORT_C static CGrid* NewL();
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CGrid(MGridObserver* aGridObserver);

	/**
	 * Default constructor for performing 2nd stage construction
	 */
	void ConstructL(const TRect& aRect, const CCoeControl* aParent);

public:

	/**
	 * Delete the old grid and create a new one with the specified size.
	 */
	IMPORT_C void ResizeGridL(TSize aGridSize);

	/**
	 * Check if this value is still in the grid (essentially, this checks
	 * if the number is >= 0 and < aGridSize).
	 */
	IMPORT_C TBool IsInGrid(TInt aValue, TInt aGridSize);

	/**
	 * Get a reference to an individual cell of the grid.
	 */
	IMPORT_C TGridCell* GetGridCell(const TInt aCol, const TInt aRow) const;

	/**
	 * Set the cell of the specified grid to the specified color.
	 * Also takes care of pair checks.
	 * \return the number of pairs that were removed.
	 */
	IMPORT_C TInt SetGridCell(TInt aX, TInt aY, eCellColor aCellColor, TBool aPairCheck);

	/**
	 * Get the number of columns of this grid.
	 * \return the number of columns of this grid.
	 */
	IMPORT_C TInt GetCols() const;

	/**
	 * Get the number of rows of this grid.
	 * \return the number of rows of this grid.
	 */
	IMPORT_C TInt GetRows() const;

	/**
	 * Get the size (columns, rows) of this grid
	 * \return the size (columns, rows) of this grid
	 */
	IMPORT_C TSize GetSize() const;

	/**
	 * Get the array containing the individual cells.
	 * Note that it is recommended to use the access functions
	 * instead of directly manipulating the grid.
	 * \return the array containing the grid cells
	 */
	IMPORT_C TGridCell** GetGrid() const;

	/**
	 * Initialize this grid instance with the data from the
	 * provided grid.
	 * This method does not resize the grid - the grid sizes
	 * have to match, otherwise a panic with the code
	 * -101 will be raised.
	 *
	 * \param aCopyFromGrid grid to copy from.
	 */
	IMPORT_C void CopyFrom(CGrid const * const aCopyFromGrid);

	/**
	 * Get the number of remaining free (neutral color) cells
	 * of this grid.
	 */
	IMPORT_C TInt GetRemainingFreeCells();

	/**
	 * Check around the specified coordinates if the specified
	 * color owns 5 cells in a row.
	 *
	 * \param aCheckForColor which color to search for in the
	 * surroundings of the specified point
	 * \param aCheckAtX column in the center of the area to search
	 * \param aCheckAtY row in the center of the area to search
	 */
	IMPORT_C TBool WinnerCheck(eCellColor aCheckForColor, TInt aCheckAtX, TInt aCheckAtY);

	/**
	 * Set the background color to use for displaying the grid.
	 * Usually white, but can be set to the winners color to make
	 * it more visible who has won.
	 *
	 * \param aBgColor new background color.
	 */
	IMPORT_C void SetBgColor (const TRgb &aBgColor);

	/**
	 * Make the control take all the available space of the main pane.
	 */
	IMPORT_C void ExtendToMainPane();
public:
	//	from CCoeControl
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);
protected:
	//  From CCoeControl
	void SizeChanged();
	void HandleResourceChange( TInt aType );

private:
	/**
	 * Calculate the size of the whole grid and individual grid cells
	 * based on the size of the control.
	 */
	void CalculateSize();

	/**
	 * Delete the back buffer bitmap and its other cached objects (context, device).
	 */
	void DeleteBackBuffer();

	/**
	 * Create a back buffer bitmap (and context + device) with the specified size.
	 */
	void CreateBackBufferL(TSize aSize);

private:
	//  From CCoeControl
	TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

	//  From CCoeControl
	void Draw(const TRect &aRect) const;

	/**
	 * Get the rectangle that is occupied on the screen
	 * by the cell at the specified coordinates.
	 *
	 * \param aCellCoords column and row of the cell to query.
	 * \return rectangle in screen coordinates occupied by
	 * the specified cell.
	 */
	TRect GetRectOfCell(const TPoint& aCellCoords) const;

	/**
	 * Get the coordinates of the cell that occupies the specified
	 * screen coordinates. Used to map the coordinates of a pointer
	 * event to a selected cell.
	 *
	 * \param aPointerCoords screen coordinates at which position to
	 * return the cell.
	 * \return column and row of the cell at the specified screen coordinates.
	 */
	TPoint GetCellForCoords(const TPoint& aPointerCoords) const;

	/**
	 * Called when a cell has been selected by the keyboard or the
	 * pointer. Informs the observer of the grid about the event.
	 *
	 * \param aCellCoords column and row of the selected cell.
	 */
	void SelectedCellEventL(TPoint& aCellCoords);

	/**
	 * Check if 5 adjoining cells of the specified color exist in
	 * the specified row. The search will start at the specified
	 * coordinates and go eight times in the direction specified
	 * by aDx and aDy (increment for column / row for each step,
	 * can also be negative).
	 * Returns ETrue if yes, EFalse otherwise.
	 *
	 * \param aCheckForColor search for this player color
	 * \param aStartX start searching at this column
	 * \param aStartY start searching at this row
	 * \param aDx in/decrease the column by this value each step
	 * \param aDy in/decrease the row by this value each step
	 */
	TBool WinnerCheckCells(eCellColor aCheckForColor, TInt aStartX,
			TInt aStartY, TInt aDx, TInt aDy);

	/**
	 * Draw the symbol for the first player into the specified
	 * rectangle.
	 * \param aGc graphics context to draw to.
	 * \param aRect rectangle to fill with the symbol of the player.
	 */
	void DrawPlayer1Symbol(CFbsBitGc& aGc, const TRect& aRect) const;

	/**
	 * Draw the symbol for the second player into the specified
	 * rectangle.
	 * \param aGc graphics context to draw to.
	 * \param aRect rectangle to fill with the symbol of the player.
	 */
	void DrawPlayer2Symbol(CFbsBitGc& aGc, const TRect& aRect) const;

	/**
	 * Create the grid with the current size.
	 * Note: This function will not delete any existing grid beforehand!
	 */
	void CreateGridL();

	/**
	 * Delete the grid.
	 */
	void DeleteGrid();

	/**
	 * For activated pair check - search for a pair of stones that is
	 * enclosed by two stones of the own color.
	 * Searches in the specified direction, starting at column aX and
	 * row aY and in/decreases the column and row by aDx and aDy for each
	 * step.
	 * \param aX column where to start searching
	 * \param aY row where to start searching
	 * \param aDx increment (positive) or decrement (negative) for the column
	 * of each steop.
	 * \param aDy increment (positive) or decrement (negative) for the row
	 * of each steop.
	 * \return whether a pair was found in this direction.
	 */
	TBool FindPairs(TInt aX, TInt aY, TInt aDx, TInt aDy,
			eCellColor aMyCellColor, eCellColor aEnemyCellColor);

private:
	/**
	 * Columns (width, x) of the grid.
	 */
	TInt iGridCols;

	/**
	 * Rows (Height, y) of the grid.
	 */
	TInt iGridRows;

	/**
	 * 2D Array containing the grid.
	 */
	TGridCell** iGrid;

	/**
	 * X Coordinate of the last move for highlighting the cell.
	 */
	TPoint iLastMove;

	/**
	 * Screen size (in pixels) of an individual grid cell.
	 */
	TSize iGridCellSize;

	/**
	 * Rectangle of the whole grid - this might be smaller than the
	 * size of the control, as all grid cells have the equal size.
	 */
	TRect iTotalGridRect;

	/**
	 * Cell coordinates of the currently selected cell.
	 * Used for highlighting it when drawing the grid.
	 */
	TPoint iCurCell;

	/**
	 * Pointer to the observer which should receive selected cell
	 * events.
	 */
	MGridObserver* iGridObserver;

	/**
	 * How many cells are still empty. When 0 -> tie.
	 */
	TInt iRemainingFreeCells;

	/**
	 * Background color of the grid. Usually white, but when a player
	 * wins the game, the background can be colored in his player
	 * color.
	 */
	TRgb iBgColor;

	/**
	 * Set to ETrue if the grid array has already been created.
	 * This control can't draw the grid before ResizeGridL() has
	 * been called once.
	 */
	TBool iInitialized;

	/** Back buffer bitmap */
	CFbsBitmap* iBackBufferBmp;

	/** Graphics context of back buffer bitmap  */
	CFbsBitGc* iBackBufferBmpGc;

	/** Drawing Device of back buffer bitmap. */
	CFbsBitmapDevice* iBackBufferBmpDrawingDevice;

	};

#endif // GRID_H

