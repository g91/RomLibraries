/*!
	@project Rune
	@file RuPathFinder_AStar.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#ifndef _RUPATHFINDER_ASTAR_H_
#define _RUPATHFINDER_ASTAR_H_

#include "../PathEngine/RuPathFinder_Base.h"
#include "../Core/Type/RuType_BitField.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuPathFinder_AStar : public IRuPathFinder_Base
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ASNodeDesc
	{
		ASNodeDesc*							m_previousNode;				//!< The node by which we got to the current node

		CRuPathNode*						m_pathNode;					//!< Path node

		REAL								m_g;						//!< A* g cost
		REAL								m_h;						//!< A* heuristic cost
		REAL								m_f;						//!< A* g + h
	};

	// Path finder settings
	INT32									m_maxExpansions;

	// A* runtime data
	CRuKeyHash<PTRVALUE>*					m_openHash;
	CRuKeyHash<PTRVALUE>*					m_closedHash;


	CRuStack<ASNodeDesc *>					m_freeNodeList;				//!< List of cached, unused path nodes
	CRuBinaryHeap<REAL, ASNodeDesc *>		m_openList;					//!< List of nodes currently on open list
	CRuArrayList<ASNodeDesc *>				m_closedList;				//!< List of nodes currently on closed list
	CRuQueue<ASNodeDesc *>					m_pathQueue;				//!< Reconstructed path queue

public:
											CRuPathFinder_AStar();
	virtual									~CRuPathFinder_AStar();

	virtual BOOL							SetMaximumExpansions(INT32 maxExpansions);
	virtual BOOL							FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut);

protected:
	void									ResetPathFinder();
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
