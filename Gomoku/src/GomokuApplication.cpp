/*
============================================================================
 Name		 : CGomokuApplication from GomokuApplication.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuApplication implementation
============================================================================
*/

#include <eikstart.h>

#include "GomokuApplication.h"
#include "GomokuDocument.h"
#include "GomokuGlobals.h" // contains the applications UID

/**
The function is called by the UI framework to ask for the
application's UID. The returned value is defined by the
constant KUidGomokuApp and must match the second value
defined in the project definition file.
*/
TUid CGomokuApplication::AppDllUid() const
	{
	return KUidGomokuApp;
	}

/**
This function is called by the UI framework at application start-up.
It creates an instance of the document class.
*/
CApaDocument* CGomokuApplication::CreateDocumentL()
	{
	return CGomokuDocument::NewL(*this);
	}

/**
The function is called by the framework immediately after it has started the
application's EXE. It is called by the framework and is expected to have
exactly this prototype.

@return Instance of the application class.
*/
CApaApplication* NewApplication()
	{
	return new CGomokuApplication;
	}

/**
E32Main() contains the program's start up code, the entry point for an EXE.
*/
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
