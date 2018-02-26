/*
============================================================================
 Name		 : CGomokuApplication from GomokuApplication.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares main application class.
============================================================================
*/
#ifndef GOMOKUAPPAPPLICATION_H
#define GOMOKUAPPAPPLICATION_H

#include <aknapp.h>

/**
This class is the entry point to the application.
*/
class CGomokuApplication : public CAknApplication
	{
private:
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	CApaApplication* NewApplication();
	};

#endif // GOMOKUAPPAPPLICATION_H
