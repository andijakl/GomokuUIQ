/*
 ============================================================================
 Name		 : GomokuPlayerAiReference.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Reference implementation for an artificial intelligence
               that can handle both a game with and without pair check.
 ============================================================================
 */

#ifndef GOMOKUPLAYERAIREFERENCE_H_
#define GOMOKUPLAYERAIREFERENCE_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32debug.h>
#include <e32math.h>
#include "GomokuPlayerInterface.h"

/**
 * Extremely low rating for a space that is already taken and can no longer be used.
 */
#define TAKEN_SPACE		-20000

// CLASS DECLARATION


/**
 * Thread calculating the best move for an AI player.
 *
 * This routine will try out every possible move. For each move, the total rating
 * of the board is calculated. Then, the best move is chosen.
 * Instead of searching for patterns, this algorithm goes through each row of the
 * grid (in every direction: horizontal, vertical and both diagonal directions)
 * and takes a look at every possible 5-stone-segment. The stones in the segments
 * are counted and the segment a rating. This rating is summed up for the total board rating.
 * The total rating for each possible AI-move is stored in an array. After calculating the move,
 * the AI chooses the best move. If two moves are equally good, it calculates the
 * average of the surrounding cell ratings. If both moves have an equal surround rating,
 * the decision will be random.
 *
 * Explanation of the search method with examples:
 *
 * . . = empty field in the grid
 * .x. = enemy stone
 * .o. = own stone
 * .|. = analyze segment border
 *
 * The following situation should be considered as dangerous:
 *
 * -------------------------- BOARD SITUATION
 * . . . . . .x.x.x. . . . .
 * -------------------------- ANALYSIS
 * . | - - - -X|x.x. . . . .  - 1. segment - 1 stone found:  rating 1  (*2 -> enemy) = -2
 * . . | - - -X-X|x. . . . .  - 2. segment - 2 stones found: rating 4  (*2 -> enemy) = -8
 * . . . | - -X-X-X| . . . .  - 3. segment - 3 stones found: rating 16 (*2 -> enemy) = -32
 * . . . . | -X-X-X- | . . .  - 4. segment - 3 stones found: rating 16 (*2 -> enemy) = -32
 * . . . . . |X-X-X- - | . .  - 5. segment - 3 stones found: rating 16 (*2 -> enemy) = -32
 * . . . . . .x|X-X- - - | .  - 6. segment - 2 stones found: rating 4  (*2 -> enemy) = -8
 * . . . . . .x.x|X- - - - |  - 7. segment - 1 stone found:  rating 1  (*2 -> enemy) = -2
 * ------------------------------------------------------------------------------------
 *                                                                     Total Rating: -116
 *
 * This situation gets a quite high negative rating which is decreased from the total score of
 * the board rating. Therefore, a board with such a situation in it will get a very low
 * rating. The advantage is that one stone will also already be considered, even though it
 * only gets a very low rating. But in situations where no clear best move is available,
 * it pays if every single stone is considered. Of course this check is done in all directions.
 *
 * But what can the AI do to make this situation less dangerous? The following shows why
 * a move where the AI puts a stone directly next to the pattern above will get a higher rating:
 *
 * -------------------------- BOARD SITUATION
 * . . . . .o.x.x.x. . . . .
 * -------------------------- ANALYSIS
 * | - - - -O|x.x.x. . . . .  - 0. segment - 1 own stone:    rating 1                = 1
 * . | - - -O-X|x.x. . . . .  - 1. segment - own stone found:rating 0                = -0
 * . . | - -O-X-X|x. . . . .  - 2. segment - own stone found:rating 0                = -0
 * . . . | -O-X-X-X| . . . .  - 3. segment - own stone found:rating 0                = -0
 * . . . . |O-X-X-X- | . . .  - 4. segment - own stone found:rating 0                = -0
 * . . . . .o|X-X-X- - | . .  - 5. segment - 3 stones found: rating 16 (*2 -> enemy) = -32
 * . . . . .o.x|X-X- - - | .  - 6. segment - 2 stones found: rating 4  (*2 -> enemy) = -8
 * . . . . .o.x.x|X- - - - |  - 7. segment - 1 stone found:  rating 1  (*2 -> enemy) = -2
 * ------------------------------------------------------------------------------------
 *                                                                     Total Rating: -41
 *
 * Every time an own stone is in the analyzed segment, the enemy *can not* build 5 in a row
 * in this segment anymore. Therefore, it is not dangerous and not relevant anymore.
 * Using this logic, a move where the AI places its stone next to the pattern will get a
 * much better rating.
 *
 * The *2 for the enemy is because of the following situation: The AI rates the board after
 * it has made his own (temponary) move. Therefore, if the enemy has 3 in a row and the AI
 * also has 3 in a row somewhere else, the situation is not equal but much worse for the AI
 * because the enemy will make 4 out of this in the next move, basically winning the game.
 *
 * Of course the board doesn't only have to be rated for the enemy, the AI also has to find
 * good moves for itself. This is done by simply turning around the counting mechanism.
 * Each own stone (o) in a segment gets counted, if an enemy stone (x) is found in the segment,
 * it is no longer relevant. The total rating of each segment is the difference of the
 * enemy and the own ranking. Those two steps are combined to one logical step in the example above.
 *
 * If enabled, the AI also has a correction to find situations where the enemy player can
 * capture a pair of the AI (.x.o.o. .). Those situations get an additional penalty in the rating.
 */

class CGomokuPlayerAiReference : public CGomokuPlayerInterface
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CGomokuPlayerAiReference();

	/**
	 * Two-phased constructor.
	 */
	static CGomokuPlayerAiReference* NewL();

public:
	/**
	 * Initialize the player implementation. This method is called by
	 * the game engine before the game is started and provides
	 * the necessary parameters to the implementation.
	 *
	 * \param aGrid a reference of the grid that will be valid for the
	 * whole game. The implementation should save a pointer to this
	 * grid to be able to read the current grid state during the game.
	 * Player implementations should not modify the grid. The engine
	 * has its own private copy of the reference grid and synchronizes
	 * the grid that the implementations use with the reference grid after
	 * every move to be on the safe side, though.
	 * \param aYourColor the color this implementation is assigned with for
	 * this game.
	 * \param aPairCheck ETrue if pair check is activated for this game.
	 * \param aNumPairsWin number of captured pairs required for winning a game.
	 */
	void InitL(CGrid* aGrid, eCellColor aYourColor, TBool aPairCheck, TInt aNumPairsWin);

	/**
	 * Called when the turn of this player is started. The implementation
	 * should analyze the grid through the pointer it gets through the
	 * InitL() method. Once the implementation is finished with
	 * calculating its move, it should call the callback function
	 * defined in the MGomokuPlayerObserver-interface.
	 *
	 * It is an advantage if the player starts its own thread for
	 * calculating the next move, or if it uses a background active object
	 * to do so. Otherwise, the game will hang.
	 *
	 * If the calculation is fast, this does not have to be done, however.
	 * The game engine handles the callbacks of the implementation
	 * asynchronously using an own timer, in order to ease the implementation
	 * of players.
	 */
	void StartTurn();

	/**
	 * If a player is active, the game engine forwards selected cell events
	 * (through the pointer or key events) to the implementation. This
	 * is necessary for human players, but could also be used to implement
	 * half-automatic AIs or other things.
	 *
	 * \param aCol column of the grid cell that was selected.
	 * \param aRow row of the grid cell that was selected.
	 */
	void HandleSelectedCellL(TInt aCol, TInt aRow);

	/**
	 * This method is called by the game engine when the game is over,
	 * the parameter provides the color of the winning player.
	 *
	 * This is important for network players, which might have to send
	 * this result over the air to the other game instance.
	 *
	 * \param aWinnerColor color of the winning player.
	 */
	void GameOver(eCellColor aWinnerColor);
private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CGomokuPlayerAiReference();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();

private:
	/**
	 * Static callback function for the CIdle active object.
	 * This method can call the non-static processing function
	 * of the AI implementation, which will do the actual processing
	 * of the computer move (-> DoBackgroundCalcMove()).
	 *
	 * \param aAi pointer to an instance of the implementation, which
	 * allows to call the non-static method.
	 * \return ETrue if the processing isn't finished yet and
	 * the active object should call this method again as soon as possible.
	 * EFalse if processing has finished and this method doesn't need
	 * to be called anymore (for this turn).
	 */
	static TInt BackgroundCalcMove(TAny* aAi);

	/**
	 * The non-static processing function which will be called numerous
	 * times by the CIdle active object. It will process one row of
	 * the game grid for each call, in order to retain responsivness
	 * of the total application.
	 *
	 * \return ETrue if the processing isn't finished yet and
	 * the active object should call this method again as soon as possible.
	 * EFalse if processing has finished and this method doesn't need
	 * to be called anymore (for this turn).
	 */
	TInt DoBackgroundCalcMove();

	/**
	 * Go through the whole board in all directions and calculate its total rating.
	 * \return the total rating of this board.
	 */
	TInt AnalyzeBoard();

	/**
	 * Calculate the surround rating for the specified cell.
	 * Surround rating is the average rating of all (free) neighbor cells.
	 *
	 * \param aX x coordinate to calculate surround rating for
	 * \param aY y coordinate to calculate surround rating for.
	 * \return average surround rating.
	 */
	TReal CalcSurroundRating(TInt aX, TInt aY);

	/**
	 * Add the rating of all rows specified by the parameters ot the total rating.
	 * Example: starting at the top, going to the bottom, and doing that for all rows.
	 *
	 * \param aTotalRating current total rating of the board. The value calculated by
	 * this process will be added.
	 * \param aRowStart coordinate where the row should start
	 * \param aRowEnd coordinate where the row should end
	 * \param is x or y value specified by the two parameters above.
	 * \param aOtherStartValue one coordinate will always start at the position specified
	 * above (looping through aRowStart to aRowEnd), the other one will always get the start
	 * value assigned by this parameter.
	 * \param aDx change of the x coordinate for each step
	 * \param aDy change of the y coordinate for each step
	 */
	void CalcRow(TInt& aTotalRating, TInt aRowStart, TInt aRowEnd,
			TBool aAssignRowToX, TInt aOtherStartValue, TInt aDx, TInt aDy);

	/**
	 * Set the analyze segment array back to neutral.
	 */
	void ResetAnalyzeSeg();

	/**
	 * Add the specified color to the analyze segment (round array) and increase count.
	 * \param aCellColor the color of the stone to add.
	 */
	void AddToSeg(eCellColor aCellColor);

	/**
	 * Count how many stones with the specified color are currently in the analyze segment.
	 * If one stone with the opposite color is found, the rating is 0.
	 * \param aCheckForColor count number of stones for this color
	 * \return number of stones of the specified color. 0 if no stone of this color is in the
	 * segment, or if at least one stone of the opposite color is in the segment.
	 */
	TInt CountHitsInSeg(eCellColor aCheckForColor);

	/**
	 * Calculate a rating for the number of stones. If 3 stones with the same color are
	 * in a segment, it gets a higher rating than if only 1. If the number of stones was
	 * counted for the enemy, he will get a higher rating for the same stone situation.
	 * This is because the AI move has already been done and the enemy will be the next to move.
	 * \param aMyColor color of the own player
	 * \param aCountedFor color of the stones that have been counted
	 * \param aScore number of stones in the segment
	 * \return the rating of the segment.
	 */
	TInt RateHitsForPlayer(eCellColor aMyColor, eCellColor aCountedFor,
			TInt aScore);

	/**
	 * Calculate the rating of the segment, considering both the own situation and the enemy
	 * situation. This is done by counting the number of stones in the segment and rating this.
	 * \return the rating for this segment. The higher the better.
	 */
	TInt CalcTotalRatingForSeg();

	/**
	 * Make sure that the value isn't below 0 or bigger than the specified value.
	 * \param aValue value to trim
	 * \param aGridSize maximum value
	 * \return trimmed value
	 */
	TInt TrimToGridSize(TInt aValue, TInt aGridSize);

	/**
	 * Create a two-dimensional array of the same size as the gaming grid.
	 * During the calculation, this is used to store the rating for each
	 * grid cell.
	 */
	void CreateRatingGridL();

	/**
	 * Delete the rating grid.
	 */
	void DeleteRatingGrid();

	/**
	 * To analyze the result of a possible move, the AI needs to actually execute
	 * the move to be able to analyze the new situation. As this shouldn't be
	 * done on the grid provided by the game engine, the AI keeps its own copy
	 * of the grid, which it is free to modify.
	 * Of course, this is also important to undo a move, in order to try the next
	 * possible move.
	 */
	void CopyToWorkingGrid();
private:
/**
 * The active object that takes care of calling the processing method
 * until the calculation is finished. The calculation itself is done in
 * small increments, as this approach uses active objects and no thread - and
 * only one active object can be active at the same time, therefore blocking
 * the rest of the application.
 */
	CIdle* iIdleAO;

	/**
	 * Pointer to the grid, owned by the game engine.
	 * This grid always contains the current game state and should not
	 * be modified by this implementation.
	 */
	CGrid* iGrid;

	/**
	 * True if this implementation is currently active.
	 * This is the case in the timeframe between StartTurn() is called
	 * and the implementation sends back its move to the observer.
	 */
	TBool iIsActive;

	/**
	 * Local copy of the grid, which can be used by the AI to try possible
	 * moves, in order to evaluate their impact on the game.
	 */
	CGrid* iWorkingGrid;

	/**
	 * Rating grid that is used to store the ratings of all analyzed cells
	 * during the calculation of the next move. The grid cell with the highest
	 * rating will be the best move that this implementation can find.
	 */
	TInt** iRatingGrid;

	/**
	 * Stores if pair check is activated in the current game.
	 */
	TBool iPairCheck;

	/**
	 * Number of captured pairs required for winning a game.
	 */
	TInt iNumPairsWin;
	
	/**
	 * If a background calculation is already active, this is set to ETrue.
	 * In case it is set to EFalse, the first call to the calculation method
	 * will know that a new calculation just started and allows it to initialize
	 * everything. After this, iProcessingActive will be set to ETrue - until
	 * calculating the computer move has finished.
	 */
	TBool iProcessingActive;

	/**
	 * Currently processed column of the grid for the background calculation.
	 * Needs to be stored so that the AI remembers its progress until the
	 * next call of the DoBackgroundCalcMove() method.
	 */
	TInt iProcessingCol;

	/**
	 * Currently processed row of the background calculation. Needs to be stored
	 * so that the AI remembers its progress until the next call of the
	 * DoBackgroundCalcMove() method.
	 */
	TInt iProcessingRow;

	/**
	 * When starting to analyze, this variable stores for which player to calculate a move.
	 */
	eCellColor iCurrentPlayer;

	/**
	 * The opposite color of the own player.
	 */
	eCellColor iEnemyPlayer;

	/**
	 * Round array to store 5 stones, which can then be analyzed.
	 */
	eCellColor iAnalyzeSeg[5];

	/**
	 * Current position in the analyze segment array.
	 */
	TInt iCurPos;

	/**
	 * Count how many cells have been added to the segment in the current row,
	 * to be able to handle the border or the grid correctly.
	 */
	TInt iNthAnalysis;

	/**
	 * Size of the grid that is used in the current game.
	 * Stored here for easier and faster access.
	 */
	TSize iGridSize;

	/**
	 * Seed for the random number generator, initialized when this
	 * object is instantiated.
	 */
	TInt64 iSeed;
	};

#endif /*GOMOKUPLAYERAIREFERENCE_H_*/
