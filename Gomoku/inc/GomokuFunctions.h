/*
 ============================================================================
 Name		 : GomokuFunctions.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Common functions for working with drives.
 ============================================================================
 */

#ifndef __GOMOKUFUNCTIONS_H__
#define __GOMOKUFUNCTIONS_H__

#include <e32std.h>

#include <eikenv.h>
#include <bautils.h>
#include <eikappui.h>
#include <eikapp.h>
#include <aknutils.h>

/*! 
  @namespace NGomokuFunctions
  
  @discussion Useful functions that are necessary for many parts of the game.
  */
namespace NGomokuFunctions
{
    /**
    * Adds the full path and drive letter of the application to a filename.
    * @param aFilename the filename the path should be added to.
    * @param aFullName the filename with its full path is written to this
    * variable (pass by reference).
    */
    const void AppendFullPath(const TDesC& aFilename, TFileName& aFullName);
    
    /**
     * Get the drive number of the drive where this application is installed
     * (and therefore where it should put its data files)
     * @param aFs file server session, required to get the drive name
     * @return drive number of the drive where the app. is installed
     */
    const TInt GetStoreDriveL(RFs& aFs);
    
    /**
     * Get the drive letter of the drive where this application is installed
     * (and therefore where it should put its data files)
     * @param aFs file server session, required to get the drive name
     * @return drive letter (1 char) where the app. is installed
     */
    const TChar GetStoreDriveCharL(RFs& aFs);
    
    /**
     * Add the drive and full private path to the specified filename.
     * The drive will be the drive where the application was installed.
     * @param aFs file server session, required to get the drive name.
     * @param aFilename name of the file that should be appended to the full
     * private path.
     * @param aCompleteName the contents of this RBuf will be reset, it is
     * filled with the full URI of the specified filename - including
     * the drive and private path of the application.
     */
    const void AddPrivatePathL(RFs& aFs, const TDesC& aFilename, RBuf& aCompleteName);
}

#endif
