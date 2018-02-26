/*
 ============================================================================
 Name	  	 : CGomokuGameData from GomokuGameData.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuGameData implementation
 ============================================================================
 */

#include "GomokuGameData.h"

CGomokuGameData* CGomokuGameData::NewL()
{
	CGomokuGameData* self = new (ELeave) CGomokuGameData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self;
	return self;
}

void CGomokuGameData::ConstructL()
{
	// Connect to the file server and create the directory if needed
	User::LeaveIfError(iFileServerSession.Connect());
}

CGomokuGameData::CGomokuGameData() :
		// Initialize with default values
	iCurrentPlayer(1), iGridCols(10), iGridRows(10), iPairCheck(EFalse), iNumPairsWin(5)
{
}

CGomokuGameData::~CGomokuGameData()
{
	// Delete the player implementations and free the memory
	// for the player name RBuf's.
	delete iPlayer1;
	iPlayer1Name.Close();
	delete iPlayer2;
	iPlayer2Name.Close();
	iScores.Close();
	// Do NOT delete iGrid here - it is owned by the control
	// stack of the view and will be deleted by it on
	// view destruction.
	iFileServerSession.Close();
}

void CGomokuGameData::SetPlayer1(CGomokuPlayerInterface* aPlayer,
		const TDesC& aName)
{
	// Test if we're replacing the first player
	if (iPlayer1)
		delete iPlayer1;
	iPlayer1 = aPlayer;
	// Free the memory already allocated and recreate the RBuf
	// to copy the new player name.
	iPlayer1Name.Close();
	iPlayer1Name.Create(aName);
}

void CGomokuGameData::SetPlayer2(CGomokuPlayerInterface* aPlayer,
		const TDesC& aName)
{
	// Test if we're replacing the second player
	if (iPlayer2)
		delete iPlayer2;
	iPlayer2 = aPlayer;
	// Free the memory already allocated and recreate the RBuf
	// to copy the new player name.
	iPlayer2Name.Close();
	iPlayer2Name.Create(aName);
}

CGomokuPlayerInterface* CGomokuGameData::GetPlayer1()
{
	return iPlayer1;
}

CGomokuPlayerInterface* CGomokuGameData::GetPlayer2()
{
	return iPlayer2;
}

CGomokuPlayerInterface* CGomokuGameData::GetPlayer(TInt aPlayerNum)
{
	return (aPlayerNum == 1) ? iPlayer1 : iPlayer2;
}

CGomokuPlayerInterface* CGomokuGameData::GetCurrentPlayer()
{
	return (iCurrentPlayer == 1) ? iPlayer1 : iPlayer2;
}

TDesC& CGomokuGameData::GetPlayer1Name()
{
	return iPlayer1Name;
}

TDesC& CGomokuGameData::GetPlayer2Name()
{
	return iPlayer2Name;
}

TDesC& CGomokuGameData::GetCurrentPlayerName()
{
	return (iCurrentPlayer == 1) ? iPlayer1Name : iPlayer2Name;
}

TDesC& CGomokuGameData::GetOtherPlayerName()
{
	return (iCurrentPlayer == 1) ? iPlayer2Name : iPlayer1Name;
}

TPtrC CGomokuGameData::GetCurrentPlayerMainName()
{
	return GetPlayerMainName(GetCurrentPlayerName());
}

TPtrC CGomokuGameData::GetPlayerMainName(const TDesC& aPlayerName)
{
	TInt newLinePos = aPlayerName.Locate('\t');
	if (newLinePos < 0)
		newLinePos = aPlayerName.Length();
	return aPlayerName.Left(newLinePos);
}

eCellColor CGomokuGameData::GetCurrentPlayerColor()
{
	return GetCurrentPlayerNum() == 1 ? EColor1 : EColor2;
}

void CGomokuGameData::SetGridSize(TSize aGridSize)
{
	iGridCols = aGridSize.iWidth;
	iGridRows = aGridSize.iHeight;
}

TSize CGomokuGameData::GetGridSize()
{
	return TSize(iGridCols, iGridRows);
}

TInt& CGomokuGameData::GetGridCols()
{
	return iGridCols;
}

TInt& CGomokuGameData::GetGridRows()
{
	return iGridRows;
}

void CGomokuGameData::SetGrid(CGrid* aGrid)
{
	iGrid = aGrid;
}

CGrid* CGomokuGameData::GetGrid()
{
	return iGrid;
}

TInt CGomokuGameData::GetCurrentPlayerNum()
{
	return iCurrentPlayer;
}

void CGomokuGameData::SwitchCurrentPlayer()
{
	iCurrentPlayer = (iCurrentPlayer == 1) ? 2 : 1;
}

void CGomokuGameData::SetCurrentPlayerNum(TInt aPlayer)
{
	iCurrentPlayer = aPlayer;
}

TBool& CGomokuGameData::GetPairCheckEnabled()
{
	return iPairCheck;
}

void CGomokuGameData::SetPairCheckEnabled(TBool aPairCheck)
{
	iPairCheck = aPairCheck;
}

void CGomokuGameData::SetNumPairsWin(TInt aNumPairsWin)
{
	iNumPairsWin = aNumPairsWin;
}

TInt& CGomokuGameData::GetNumPairsWin()
{
	return iNumPairsWin;
}

void CGomokuGameData::IncreasePairsCaptured(TInt aPlayer,
		TInt aAdditionalPairsCaptured)
{
	iPairsCaptured[aPlayer - 1] += aAdditionalPairsCaptured;
}

TInt CGomokuGameData::GetPairsCaptured(TInt aPlayer)
{
	return iPairsCaptured[aPlayer - 1];
}

void CGomokuGameData::ResetPairsCaptured()
{
	iPairsCaptured[0] = 0;
	iPairsCaptured[1] = 0;
}

void CGomokuGameData::SetGamePhase(eGamePhase aGamePhase)
{
	iGamePhase = aGamePhase;
}

eGamePhase CGomokuGameData::GetGamePhase()
{
	return iGamePhase;
}


//------------------------------------------------------------------------------
void CGomokuGameData::SaveSettingsFileL()
{	
	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NGomokuFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Create a direct file store that will contain the game progress
	CFileStore* store = CDirectFileStore::ReplaceLC(iFileServerSession, fullName, EFileWrite);
	store->SetTypeL(KDirectFileStoreLayoutUid);

	// Create the stream
	RStoreWriteStream stream;
	TStreamId id = stream.CreateLC(*store);

	// Write game progress
	stream.WriteInt32L(INI_FILE_VERSION_NUMBER);
	WriteSettingsL(stream);
	WriteStatsL(stream);

	// Commit the changes to the stream
	stream.CommitL();
	CleanupStack::PopAndDestroy();	// stream

	// Set the stream in the store and commit the store
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy(2);	// store, fullName
}

//------------------------------------------------------------------------------
void CGomokuGameData::LoadSettingsFileL()
{
	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NGomokuFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Open the file store
	CFileStore* store = CDirectFileStore::OpenLC(iFileServerSession, fullName, EFileRead);

	// Open the data stream inside the store
	RStoreReadStream stream;
	stream.OpenLC(*store, store->Root());

	// Read all the data
	TInt fileVersion = stream.ReadInt32L();
	if (fileVersion == INI_FILE_VERSION_NUMBER)
	{
		ReadSettingsL(stream);
		ReadStatsL(stream);
	}

	// Delete all the remaining stuff on the cleanup stack
	// (fullName, store, stream)
	CleanupStack::PopAndDestroy(3);
}

void CGomokuGameData::ReadSettingsL(RStoreReadStream& aStream)
{
	// Only load the data if the file version of the saved file
	// matches with the compatible version of the current application
	iGridCols = aStream.ReadInt32L();
	iGridRows = aStream.ReadInt32L();
	iPairCheck = aStream.ReadInt32L();	
	iNumPairsWin = aStream.ReadInt32L();
}

void CGomokuGameData::WriteSettingsL(RStoreWriteStream& aStream)
{
	aStream.WriteInt32L(iGridCols);
	aStream.WriteInt32L(iGridRows);
	aStream.WriteInt32L(iPairCheck);
	aStream.WriteInt32L(iNumPairsWin);
}


void CGomokuGameData::ReadStatsL(RStoreReadStream& aStream)
{
	TInt numPlayers = aStream.ReadInt32L();
	for (int i = 0; i < numPlayers; i++)
	{
		TInt playerNameLength;
		TBuf<100> playerName;
		TInt curTotalGames;
		TInt curWins;
		playerNameLength = aStream.ReadInt32L();
		aStream.ReadL(playerName, playerNameLength);
		curTotalGames = aStream.ReadInt32L();
		curWins = aStream.ReadInt32L();
		iScores.AppendL(TGomokuScore(playerName, curTotalGames, curWins));
	}
}

void CGomokuGameData::WriteStatsL(RStoreWriteStream& aStream)
{
	const TInt numScores = iScores.Count();
	aStream.WriteInt32L(numScores);
	for (int i = 0; i < numScores; i++)
	{
		aStream.WriteInt32L(iScores[i].iPlayerName.Length());
		aStream.WriteL(iScores[i].iPlayerName);
		aStream.WriteInt32L(iScores[i].iTotalGames);
		aStream.WriteInt32L(iScores[i].iWins);
	}
}

TGomokuScore CGomokuGameData::GetScoreForPlayer(const TDesC& aPlayerName)
{
	TIdentityRelation<TGomokuScore> matcher(TGomokuScore::MatchName);
	TInt idx = iScores.Find(TGomokuScore(aPlayerName, 0, 0), matcher);
	if (idx == KErrNotFound)
	{
		return TGomokuScore(aPlayerName, 0, 0);
	}
	else
	{
		return iScores[idx];
	}
}

void CGomokuGameData::SetScoreForPlayerL(const TDesC& aPlayerName, const TInt aTotalGames, const TInt aWins)
{
	TIdentityRelation<TGomokuScore> matcher(TGomokuScore::MatchName);
	TInt idx = iScores.Find(TGomokuScore(aPlayerName, 0, 0), matcher);
	if (idx == KErrNotFound)
	{
		// Player not yet in the array - add the player
		TGomokuScore newPlayer(aPlayerName, aTotalGames, aWins);
		iScores.AppendL(newPlayer);
	}
	else
	{
		iScores[idx].iTotalGames = aTotalGames;
		iScores[idx].iWins = aWins;
	}
	SaveSettingsFileL();
}

//------------------------------------------------------------------------------
void CGomokuGameData::DeleteSettingsFileL()
{
	RBuf fullName;
	fullName.CleanupClosePushL();
	NGomokuFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Delete the file through the file server session
	iFileServerSession.Delete(fullName);
	CleanupStack::PopAndDestroy();
}


//------------------------------------------------------------------------------
TBool CGomokuGameData::SettingsFileExistsL()
{
	TBool returnVar = ETrue;

	// Create full filename
	RBuf fullName;
	fullName.CleanupClosePushL();
	NGomokuFunctions::AddPrivatePathL(iFileServerSession, KFileStore, fullName);

	// Try to open the file
	RFile checkFile;
	TInt errorCode = checkFile.Open(iFileServerSession, fullName, EFileRead);
	// If the file is not found, return false.
	if (errorCode == KErrNotFound)
		returnVar = EFalse;

	// Close the handle to the file
	checkFile.Close();
	CleanupStack::PopAndDestroy();

	return returnVar;
}
