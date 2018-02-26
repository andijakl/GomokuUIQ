/*
============================================================================
 Name		 : GomokuFunctions.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuDocument implementation
============================================================================
*/

#include "GomokuFunctions.h"

const void NGomokuFunctions::AppendFullPath(const TDesC& aFilename, TFileName& aFullName)
{
    aFullName.Copy(aFilename);
    CompleteWithAppPath(aFullName);
}

const TInt NGomokuFunctions::GetStoreDriveL(RFs& aFs)
{
    TInt driveNum;
    TChar driveChar = NGomokuFunctions::GetStoreDriveCharL(aFs);
    aFs.CharToDrive(driveChar, driveNum);
    return driveNum;
}

const TChar NGomokuFunctions::GetStoreDriveCharL(RFs& aFs)
{
    // Symbian OS 9.2: return aFs.GetSystemDrive();
    // Get the full name and path of the application
    TFileName appNamePath = CEikonEnv::Static()->EikAppUi()->Application()->AppFullName();
#if defined(__WINS__) // if the resources are in emulators c (not z)
    appNamePath.Replace(0,1,_L("c"));
#endif
	return appNamePath[0];
}

const void NGomokuFunctions::AddPrivatePathL(RFs& aFs, const TDesC& aFilename, RBuf& aCompleteName)
{
	// Make sure the private path exists on the drive we need
    aFs.CreatePrivatePath(NGomokuFunctions::GetStoreDriveL(aFs));
    // Reset the RBuf-variable and create it with the required size
    aCompleteName.Close();
    aCompleteName.CreateL(255);
    // Get the private path
    aFs.PrivatePath(aCompleteName);
    // Get the drive where the application is installed again, this time
    // as char and not as a number
    TBuf<1> drive;
    drive.Append(NGomokuFunctions::GetStoreDriveCharL(aFs));
    // Insert the drive to the private path, otherwise it'd be missig
    aCompleteName.Insert(0, drive);
    _LIT(KCharColon, ":");
    aCompleteName.Insert(1, KCharColon);
    // Add the filename to the path, so that we now have a full URI, containing
    // the drive, path and filename
    aCompleteName.Append(aFilename);
}
