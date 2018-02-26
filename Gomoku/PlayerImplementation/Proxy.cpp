/*
 ============================================================================
 Name		 : Proxy.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : Implementation proxy for the implemented players.
 ============================================================================
 */

#include <e32std.h>
#include <ImplementationProxy.h>

// Header files of the AI implementations included in this collection
#include "GomokuPlayerHuman.h"
#include "GomokuPlayerAiReference.h"
#include "GomokuPlayerAiDefensive.h"
#include "GomokuPlayerAiAggressive.h"
#include "GomokuPlayerAiRandom.h"


// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(0xE0000E01,	CGomokuPlayerHuman::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0xE0000E03,	CGomokuPlayerAiReference::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0xE0000E04,	CGomokuPlayerAiDefensive::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0xE0000E05,	CGomokuPlayerAiAggressive::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0xE0000E02,	CGomokuPlayerAiRandom::NewL)
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

