/*
 ============================================================================
 Name		 : GomokuPlayerInterface.cpp
 Copyright   : Andreas Jakl, 2007-2009
 
 All rights reserved. This program and the accompanying materials
 are made available under the terms of the Eclipse Public License v2.0
 which accompanies this distribution, and is available at
 https://www.eclipse.org/legal/epl-2.0/
 
 Description : CGomokuPlayerInterface implementation
 ============================================================================
 */

#include "GomokuPlayerInterface.h"

EXPORT_C CGomokuPlayerInterface* CGomokuPlayerInterface::NewL()
	{
	// If no special implementation is chosen, this will instantiate
	// the default implementation - in this case, a human player.
	const TUid KDefaultPlayer =
		{0xE0000E01};

	// Create an implementation of the default through the ECom session.
	TAny* playerInterface = REComSession::CreateImplementationL(KDefaultPlayer, _FOFF(CGomokuPlayerInterface, iDtor_ID_Key));

	return (reinterpret_cast<CGomokuPlayerInterface*>(playerInterface));
	}

EXPORT_C CGomokuPlayerInterface* CGomokuPlayerInterface::NewL(const TDesC8& aCue)
	{
	// Resolution using the default ECOM resolver
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aCue);
	// Allows wildcards in the search string
	resolverParams.SetWildcardMatch(ETrue);

	// Find implementation for our interface.
	// - KCGomokuPlayerInterfaceUid is the UID of our custom ECOM
	//   interface. It is defined in GomokuPlayerInterface.h
	// - This call will leave if the plugin architecture cannot find an
	//   implementation.
	// - The returned pointer points to one of our interface implementation
	//   instances.
	TAny* playerInterface = REComSession::CreateImplementationL(KCGomokuPlayerInterfaceUid,
			// _FOFF specifies offset of iDtor_ID_Key so that
			// ECOM framework can update its value.
			_FOFF(CGomokuPlayerInterface, iDtor_ID_Key), NULL, resolverParams);

	return (reinterpret_cast<CGomokuPlayerInterface*>(playerInterface));

	}

EXPORT_C void CGomokuPlayerInterface::ListImplementationsL(
		RImplInfoPtrArray& aImplInfoArray)
	{
	// Call the ecom framework to list the available implementations
	// of the interface with the id: KCGomokuPlayerInterfaceUid
	REComSession::ListImplementationsL (KCGomokuPlayerInterfaceUid, aImplInfoArray);
	}

EXPORT_C CGomokuPlayerInterface::~CGomokuPlayerInterface()
	{
	// Notify ECOM that this object is being deleted
	REComSession::DestroyedImplementation (iDtor_ID_Key);
	}

EXPORT_C void CGomokuPlayerInterface::SetObserver(MGomokuPlayerObserver* aObserver)
	{
	// Keep a reference to the observer
	iObserver = aObserver;
	}
