/*
============================================================================
 Name		 : GridCell.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Simple class that can store the color of a single grid cell.
============================================================================
*/

#ifndef GRIDCELL_H
#define GRIDCELL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>

/**
 * Available colors of the cell.
 * Neutral is used for an emtpy cell,
 * EColor 1 indicates that this cell is occupied by player 1.
 * EColor 2 is for the 2nd player, respectively
 */
typedef enum {
    EColorNeutral = 0,
    EColor1,
    EColor2
} eCellColor;

// CLASS DECLARATION

/**
*  TGridCell
*
*/
class TGridCell
{
public:
TGridCell();


    /**
     * Set this cell back to neutral.
     */
    inline void Reset();

    /**
     * Set the color of this cell to the specified color.
     * \param aColor color to set this cell to.
     */
    inline void SetColor(eCellColor aColor);

    /**
     * Return the color of the current cell.
     * \return color of this cell.
     */
    inline eCellColor GetColor();

    /**
     * Checks if this cell is free -> if it has neutral color.
     * \return if this cell is free.
     */
    inline bool IsFree();
private:
    eCellColor iCellColor;
};

inline void TGridCell::Reset()
{
    iCellColor = EColorNeutral;
};

inline void TGridCell::SetColor(eCellColor aColor)
{
    iCellColor = aColor;
};

inline eCellColor TGridCell::GetColor()
{
    return iCellColor;
};

inline bool TGridCell::IsFree()
{
    return (iCellColor == EColorNeutral);
};

#endif // GRIDCELL_H

