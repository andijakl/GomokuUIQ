/*
============================================================================
 Name		 : CGomokuDocument from GomokuDocument.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Declares main application document class.
============================================================================
*/

#ifndef GOMOKUDOCUMENT_H
#define GOMOKUDOCUMENT_H

#include <akndoc.h>

/**
This class represents the document in Gomoku application,
For a Symbian application to work properly the document class need to be
derived from CAknDocument.
*/
class CGomokuDocument : public CAknDocument
	{
public:
	static CGomokuDocument* NewL(CEikApplication& aApp);

private:
	// from CQikDocument
	CEikAppUi* CreateAppUiL();

	CGomokuDocument(CEikApplication& aApp);
	void ConstructL();
	};
#endif // GOMOKUDOCUMENT_H
