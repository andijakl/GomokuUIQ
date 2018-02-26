/*
 ============================================================================
 Name		 : GridObserver.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Defines the interface for a class that observes the grid
 control, in order to receive callbacks about selected cells.
 ============================================================================
 */

#ifndef GRIDOBSERVER_H_
#define GRIDOBSERVER_H_

/**
 * This interface should be implemented by the observer of the grid
 * (e.g. the owning view or a dedicated game engine). Through it, the
 * control can report which cell was selected by the user, after
 * the control got the pointer/keyboard event through the control
 * stack of the view.
 */
class MGridObserver
	{
public:
	/**
	 * This method is used by the grid to report which cell was
	 * selected by a pointer / keyboard event. The cell coordinates
	 * specify the column and the row. If the pointer event was received
	 * outside of the grid, this will not be reported.
	 * The grid will also report pointer events if the cell is not
	 * empty (neutral) anymore.
	 */
	virtual void SelectedCellEventL(TPoint& aCellCoords) = 0;
	};

#endif /*GRIDOBSERVER_H_*/
