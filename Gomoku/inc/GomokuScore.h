/*
 ============================================================================
 Name		 : GomokuScore.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Simple class that stores player names and their score.
 ============================================================================
 */

#ifndef GOMOKUSCORE_H_
#define GOMOKUSCORE_H_

#include <e32des16.h>  

class TGomokuScore
{
public:
	/**
	 * Constructor for creating a player. Creates an internal copy
	 * of the name provided as a parameter. Max length = 100 chars.
	 */
	TGomokuScore(const TDesC& aPlayerName, TInt aTotalGames, TInt aWins)
	: iTotalGames(aTotalGames),
	  iWins(aWins)
	{
		iPlayerName.Copy(aPlayerName);
	};
	
	/**
	 * Match two elements. Required for finding an element in the array.
	 */
	static TBool MatchName(const TGomokuScore& aElement1, const TGomokuScore& aElement2)
	{
		// Use built-in comparison function of descriptor base class
		return (aElement1.iPlayerName.CompareF(aElement2.iPlayerName) == 0);
	};
public:
	/** Player name. Max length = 100 chars. */
	TBuf<100> iPlayerName;
	/** 
	 * Total games played by this player implementation. 
	 * Does not count ties or games where two identical
	 * player implementations played against each other.
	 */
	TInt iTotalGames;
	/** How many of the total games did this player win. */
	TInt iWins;
};

#endif /* GOMOKUSCORE_H_ */
