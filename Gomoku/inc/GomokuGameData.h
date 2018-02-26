/*
 ============================================================================
 Name		 : GomokuGameData.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Central storage class for game settings and data.
 ============================================================================
 */

#ifndef GOMOKUGAMEDATA_H_
#define GOMOKUGAMEDATA_H_

#include <f32file.h>
#include <s32file.h>
#include <e32cmn.h>
#include "GomokuPlayerInterface.h"
#include "Grid.h"
#include "GomokuFunctions.h"
#include "GomokuScore.h"

/**
 * Name of the settings file for storing game parameters.
 */
_LIT(KFileStore, "Settings.dat");

/**
 * Version of the ini file number. Prevents the game from reading
 * an old and potentially incompatible settings file after upgrading
 * Gomoku to a new version on the phone.
 */
#define INI_FILE_VERSION_NUMBER					3

/**
 * The current game phase.
 * The paused state is handled in an own variable, as it is
 * independent of the game phase.
 */
typedef enum
	{
	ESetup,
	EIngame,
	EGameOver,
	ETie
	} eGamePhase;

/**
 * The game data class manages the relevant data and settings for an
 * active game. This includes ownership of the two player implementations
 * as well as information about the board size and activated rules.
 */
class CGomokuGameData : public CBase
	{
public:
	/**
	 * Two-phased constructor.
	 */
	static CGomokuGameData* NewL();
	~CGomokuGameData();

private:
	CGomokuGameData();
	void ConstructL();
public:
	// ------------------------------------------------------------------------------
	// Player-related methods
	// ------------------------------------------------------------------------------
	/**
	 * Assign an already instantiated player implementation to the first player.
	 * If a player has already been assigned before, it will be
	 * automatically deleted.
	 *
	 * \param aPlayer object instance of the new first player
	 * \param aName name of the first player. The descriptor will be copied
	 * from the provided instance.
	 */
	void SetPlayer1(CGomokuPlayerInterface* aPlayer, const TDesC& aName);

	/**
	 * Assign an already instantiated player implementation to the second player.
	 * If a player has already been assigned before, it will be
	 * automatically deleted.
	 *
	 * \param aPlayer object instance of the new second player
	 * \param aName name of the second player. The descriptor will be copied
	 * from the provided instance.
	 */
	void SetPlayer2(CGomokuPlayerInterface* aPlayer, const TDesC& aName);

	/**
	 * Retrieve the first player.
	 */

	CGomokuPlayerInterface* GetPlayer1();
	/**
	 * Retrieve the name of the first player.
	 */
	TDesC& GetPlayer1Name();

	/**
	 * Retrieve the second player.
	 */
	CGomokuPlayerInterface* GetPlayer2();

	/**
	 * Retrieve the name of the second player.
	 */
	TDesC& GetPlayer2Name();

	/**
	 * Retrieve the player with the specified number.
	 *
	 * \param aPlayerNum number of the player to retrieve.
	 * Has to be 1 or 2, does not start with 0.
	 */
	CGomokuPlayerInterface* GetPlayer(TInt aPlayerNum);

	/**
	 * Get the currently active player.
	 */
	CGomokuPlayerInterface* GetCurrentPlayer();

	/**
	 * Get the name of the currently active player.
	 */
	TDesC& GetCurrentPlayerName();

	/**
	 * Get the name of the currently NOT active player.
	 */
	TDesC& GetOtherPlayerName();
	
	/**
	 * Get the main part of the currently active player name.
	 * (Name returnd by implementation, until first \t).
	 */
	TPtrC GetCurrentPlayerMainName();
	
	/**
	 * Retrieve the main part of the name passed as a parameter.
	 * (Name returnd by implementation, until first \t).
	 */
	TPtrC GetPlayerMainName(const TDesC& aPlayerName);

	/**
	 * Get the color of the currently active player.
	 */
	eCellColor GetCurrentPlayerColor();

	/**
	 * Get the number of the currently active player.
	 *
	 * \return 1 if player 1 is active, 2 if player 2 is active.
	 */
	TInt GetCurrentPlayerNum();

	/**
	 * Switch the currently active player (1->2->1...)
	 */
	void SwitchCurrentPlayer();

	/**
	 * Set the currently active player to the specified number.
	 *
	 * \param aPlayer 1 to active player 1, 2 for player 2.
	 */
	void SetCurrentPlayerNum(TInt aPlayer);

	// ------------------------------------------------------------------------------
	// Grid- and settings-related methods
	// ------------------------------------------------------------------------------
	/**
	 * Set the size of the grid. This does not
	 * resize the grid, it is just for keeping the settings.
	 * Resizing the grid has to be done directly through
	 * the grid instance.
	 *
	 * \param aGridSize the new grid size, specifying the number of
	 * columns and rows.
	 */
	void SetGridSize(TSize aGridSize);

	/**
	 * Retrieve the current size of the grid.
	 * This returns the value of the settings and does
	 * not query the grid for its size.
	 *
	 * \return currently stored grid size.
	 */
	TSize GetGridSize();
	
	/**
	 * Retrieve the number of colums used in the grid.
	 */
	TInt& GetGridCols();

	/**
	 * Retrieve the number of rows used in the grid.
	 */
	TInt& GetGridRows();

	/**
	 * Store a pointer to the provided grid instance.
	 * This class does not take ownership of the grid instance!
	 *
	 * \param aGrid pointer to the grid.
	 */
	void SetGrid(CGrid* aGrid);

	/**
	 * Retrieve the grid instance.
	 */
	CGrid* GetGrid();

	/**
	 * Retrieve if the pair check has been enabled.
	 *
	 * \return ETrue if the pair check is active,
	 * EFalse otherwise.
	 */
	TBool& GetPairCheckEnabled();
	
	/**
	 * Sets whether the pair check should be enabled.
	 *
	 * \param aPairCheck ETrue if it pair check is enabled,
	 * EFalse otherwise.
	 */
	void SetPairCheckEnabled(TBool aPairCheck);

	/**
	 * Sets how many pairs have to be captured in order
	 * to win the game. The setting is only relevant if
	 * pair check is enabled.
	 * 
	 * \param aNumPairsWin number of pairs required to win
	 * the game.
	 */
	void SetNumPairsWin(TInt aNumPairsWin);
	
	/**
	 * Retrieve how many captured pairs are required to win the
	 * game by this method.
	 */
	TInt& GetNumPairsWin();
	
	/**
	 * Reset the counter that stores how many pairs
	 * have already been captured by both players.
	 */
	void ResetPairsCaptured();

	/**
	 * Increase the number of captured pairs by one
	 * for the specified player.
	 *
	 * \param aPlayer 1 for the first player, 2 for the second.
	 */
	void IncreasePairsCaptured(TInt aPlayer, TInt aAdditionalPairsCaptured);

	/**
	 * Get the current counter of captured pairs for
	 * the specified player.
	 *
	 * \param aPlayer 1 for the first player, 2 for the second.
	 */
	TInt GetPairsCaptured(TInt aPlayer);

	/**
	 * Set the game phase to the specified value.
	 *
	 * \param aGamePhase new game phase. See the definition of
	 * eGamePhase for more details.
	 */
	void SetGamePhase(eGamePhase aGamePhase);

	/**
	 * Get the current game phase.
	 */
	eGamePhase GetGamePhase();

    /**
    * Save the current settings to a file.
    */
    void SaveSettingsFileL();

    /**
    * Deletes the settings file.
    */
    void DeleteSettingsFileL();

    /**
    * Returns whether the settings file exists.
    * @return ETrue if a settings file was found.
    */
	TBool SettingsFileExistsL();
	
    /**
    * Load the settings from a file.
    */
    void LoadSettingsFileL();
    
    /**
     * Get the saved score for the player as specified
     * by the full player name.
     * 
     * \param aPlayerName full name (type) of the player to query.
     * \return statistics for this player.
     */
    TGomokuScore GetScoreForPlayer(const TDesC& aPlayerName);
    
    /**
     * Update the statistics for the specified player.
     * 
     * \param aPlayerName full name (type) of the player to query.
     * \param aTotalGames number of total games the player has played
     * (not counting games against the same player or ties).
     * \param aWins how many games out of the total games this player
     * implementation won.
     */
    void SetScoreForPlayerL(const TDesC& aPlayerName, const TInt aTotalGames, const TInt aWins);
    
private:
    /**
     * Read the game settings from the stream sent as a parameter.
     * Expects the stream to be at the position where the settings start.
     * This method does not check if the position is correct.
     */
    void ReadSettingsL(RStoreReadStream& aStream);
    
    /**
     * Read the player statistics from the stream sent as a parameter.
     * Expects the stream to be at the position where the settings start.
     * This method does not check if the position is correct.
     */
    void ReadStatsL(RStoreReadStream& aStream);
    
    /**
     * Write the current game settings to the write stream at its
     * current position.
     */
    void WriteSettingsL(RStoreWriteStream& aStream);

    /**
     * Write all the currently known player statistics to the write stream at its
     * current position.
     */
    void WriteStatsL(RStoreWriteStream& aStream);

    
private:
	/**
	 * Instance of the first player implementation.
	 * This object is owned by this class, even though
	 * it is instantiated (through the ECom framework) outside
	 * of this class.
	 */
	CGomokuPlayerInterface* iPlayer1;

	/**
	 * Name of the first player.
	 */
	RBuf iPlayer1Name;
	
	/**
	 * Store how many games this player implementation of player 1
	 * has already won.
	 */
	TInt iPlayer1Wins;
	
	/**
	 * How many games the first player has played in total
     * (not counting games against the same player or ties).
	 */
	TInt iPlayer1TotalGames;

	/**
	 * Instance of the first player implementation.
	 * This object is owned by this class, even though
	 * it is instantiated (through the ECom framework) outside
	 * of this class.
	 */
	CGomokuPlayerInterface* iPlayer2;

	/**
	 * Name of the second player.
	 */
	RBuf iPlayer2Name;

	/**
	 * Store how many games this player implementation of player 2
	 * has already won.
	 */
	TInt iPlayer2Wins;
	
	/**
	 * How many games the second player has played in total
     * (not counting games against the same player or ties).
	 */
	TInt iPlayer2TotalGames;
	
	/**
	 * Instance of the grid. This object is not owned by the GameData-class,
	 * but instead by the control stack of the view!
	 */
	CGrid* iGrid;

	/**
	 * Currently active player.
	 * 1 for player 1, 2 for the second one.
	 */
	TInt iCurrentPlayer;

	/**
	 * Columns of the grid.
	 * Not automatically synchronized with
	 * the actual iGrid object instance, but instead only used
	 * for storing and keeping game settings!
	 */
	TInt iGridCols;

	/**
	 * Rows of the grid.
	 * Not automatically synchronized with
	 * the actual iGrid object instance, but instead only used
	 * for storing and keeping game settings!
	 */
	TInt iGridRows;

	/**
	 * Whether the pair check is enabled.
	 */
	TBool iPairCheck;
	
	/**
	 * Number of pairs that have to be captured to win the game.
	 */
	TInt iNumPairsWin;

	/**
	 * Store how many pairs each player has already captured.
	 */
	TInt iPairsCaptured[2];

	/**
	 * Stores the current game phase.
	 */
	eGamePhase iGamePhase;

    /**
    * The file server for saving the game progress.
    */
    RFs iFileServerSession;
    
    /**
     * Dynamic array that stores the scores for the players.
     * Only contains players that have actually been involved
     * in a game.
     */
    RArray<TGomokuScore> iScores;
	};

#endif /*GOMOKUGAMEDATA_H_*/
