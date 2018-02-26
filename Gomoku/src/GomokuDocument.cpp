/*
============================================================================
 Name		 : CGomokuDocument from GomokuDocument.h
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuDocument implementation
============================================================================
*/

#include <s32strm.h>

#include "GomokuDocument.h"
#include "GomokuAppUi.h"
#include "GomokuGlobals.h"

/**
Creates and constructs the document. This is called by
CGomokuApplication::CreateDocumentL() which in turn is called by the
UI framework.
*/
CGomokuDocument* CGomokuDocument::NewL(CEikApplication& aApp)
	{
	CGomokuDocument* self = new (ELeave) CGomokuDocument(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
The constructor of the document class just passes the
supplied reference to the constructor initialization list.
*/
CGomokuDocument::CGomokuDocument(CEikApplication& aApp)
	: CAknDocument(aApp)
	{
	}

/**
2nd stage construction of the model.
All code that shall be called in initializing phase and might leave shall be
added here.
*/
void CGomokuDocument::ConstructL()
	{
	}

/**
This is called by the UI framework as soon as the document has been created.
It creates an instance of the ApplicationUI. The Application UI class is an
instance of a CEikAppUi derived class.
*/
CEikAppUi* CGomokuDocument::CreateAppUiL()
	{
	return new (ELeave) CGomokuAppUi;
	}

