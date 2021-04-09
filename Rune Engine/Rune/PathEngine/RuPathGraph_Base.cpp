/*!
	@project Rune
	@file RuPathGraph_Base.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#include "../PathEngine/RuPathGraph_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(IRuPathGraph_Base, IRuObject)
ruHEAP_DEFINE_SUBCLASS(IRuPathGraph_Base, "IRuPathGraph_Base", "IRuObject")
ruCLASSGUID_DEFINE(IRuPathGraph_Base, 0xAE828545, 0x82A54c5a, 0xA86C0184, 0x2C2160DB)

// ************************************************************************************************************************************************************

INT32 IRuPathGraph_Base::InsertNeighbor(IRuPathGraph_Base *neighbor)
{
	for(INT32 i = 0; i < m_neighbors.Count(); ++i)
	{
		if(m_neighbors[i] == neighbor)
		{
			return -1;
		}
	}

	m_neighbors.Add(neighbor);

	return m_neighbors.Count() - 1;
}

BOOL IRuPathGraph_Base::ClearAllNeighbors()
{
	m_neighbors.Clear();

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
