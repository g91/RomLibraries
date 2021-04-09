/*!
	@project Rune
	@file RuPathFinder_AStar.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/12/21
*/

#include "../PathEngine/RuPathFinder_AStar.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPathFinder_AStar, IRuPathFinder_Base)
ruHEAP_DEFINE_SUBCLASS(CRuPathFinder_AStar, "CRuPathFinder_AStar", "IRuPathFinder_Base")
ruCLASSGUID_DEFINE(CRuPathFinder_AStar, 0x03A5C52A, 0xEAE743f8, 0xAE2C5A62, 0x280509CA)

// ************************************************************************************************************************************************************

CRuPathFinder_AStar::CRuPathFinder_AStar()
:	m_maxExpansions(1024)
{
	m_openHash = ruNEW CRuKeyHash<PTRVALUE>(m_maxExpansions * 2);
	m_closedHash = ruNEW CRuKeyHash<PTRVALUE>(m_maxExpansions * 2);
}

CRuPathFinder_AStar::~CRuPathFinder_AStar()
{
	// Reset path finder
	ResetPathFinder();

	// Delete pre-allocated path nodes
	while(!m_freeNodeList.IsEmpty())
		delete m_freeNodeList.Pop();

	delete m_openHash;
	delete m_closedHash;
}

BOOL CRuPathFinder_AStar::SetMaximumExpansions(INT32 maxExpansions)
{
	m_maxExpansions = maxExpansions;

	return TRUE;
}

BOOL CRuPathFinder_AStar::FindPath(CRuPathNode *startNode, CRuPathNode *endNode, CRuArrayList<CRuPathNode *> &pathOut)
{
//	DWORD t0 = timeGetTime();



	REAL heuristicScale = 1.25f;

	ASNodeDesc *finalNode = NULL;
	CRuVector3 endCenter, startCenter, tmpVector;
	REAL newG, newH, newF;

	// Reset pathfinder state
	ResetPathFinder();

	// Create starting node descriptor
	ASNodeDesc *node = m_freeNodeList.IsEmpty() ? ruNEW ASNodeDesc() : m_freeNodeList.Pop();

	tmpVector.SetToDifference(endNode->Centroid(), startNode->Centroid());

	node->m_previousNode = NULL;
	node->m_pathNode = startNode;
	node->m_g = 0.0f;
	node->m_h = tmpVector.SquaredMagnitude() * heuristicScale;
	node->m_f = node->m_g + node->m_h;
	
	// Put starting node on the open list
	m_openList.Push(node->m_f, node);
	m_openHash->Add(reinterpret_cast<PTRVALUE>(node->m_pathNode));

	// --- Actual algorithm
	INT32 openedNodes = 0;

	while(!m_openList.IsEmpty() && openedNodes < m_maxExpansions)
	{
		++openedNodes;

		// Remove the lowest cost node from open list and put it onto closed list
		ASNodeDesc *curNode = m_openList.Pop();
		CRuPathNode *curPathNode = curNode->m_pathNode;

		m_closedList.Add(curNode);
		m_closedHash->Add(reinterpret_cast<PTRVALUE>(curNode->m_pathNode));

		// If the node is goal, we are done!
		if(curNode->m_pathNode == endNode)
		{
			finalNode = curNode;
			break;
		}

		// Process each of the node's neighbors
		for(INT32 i = 0; i < curPathNode->GetNumNeighbors(); ++i)
		{
			CRuPathNode *neighborNode = curPathNode->GetNeighborNode(i);
			REAL neighborCost = curPathNode->GetNeighborCost(i);

			tmpVector.SetToDifference(endNode->Centroid(), neighborNode->Centroid());

			// Weigh more heavily against moving along the Y axis
			tmpVector.m_y *= 4.0f;

			newG = neighborCost;
			newH = tmpVector.SquaredMagnitude() * heuristicScale;
			newF = newG + newH;

			// Make sure the neighbor node is not on the closed list
			if(!m_closedHash->ContainsKey(reinterpret_cast<PTRVALUE>(neighborNode)))
			{
				// Is the node already on the open list?
				if(m_openHash->ContainsKey(reinterpret_cast<PTRVALUE>(neighborNode)))
				{
					// --- supposed to reconsider node here...
				}
				else
				{
					ASNodeDesc *newNode = m_freeNodeList.IsEmpty() ? ruNEW ASNodeDesc() : m_freeNodeList.Pop();

					// If the node is not on the open list, add it
					newNode->m_previousNode = curNode;
					newNode->m_pathNode = neighborNode;
					newNode->m_g = newG;
					newNode->m_h = newH;
					newNode->m_f = newF;
					m_openList.Push(newNode->m_f, newNode);
					m_openHash->Add(reinterpret_cast<PTRVALUE>(neighborNode));
				}
			}
		}
	}

/*
	DWORD t1 = timeGetTime();

	DWORD d = t1 - t0;

	char moo[200];
	sprintf(moo, "%u", d);
	MessageBox(NULL, moo, moo, MB_OK);
*/

	// Path reconstruction
	if(finalNode)
	{
		while(finalNode != NULL)
		{
			ASNodeDesc *newASNodeDesc = m_freeNodeList.IsEmpty() ? ruNEW ASNodeDesc() : m_freeNodeList.Pop();

			newASNodeDesc->m_previousNode = finalNode->m_previousNode;
			newASNodeDesc->m_pathNode = finalNode->m_pathNode;

			m_pathQueue.Queue(newASNodeDesc);
			finalNode = finalNode->m_previousNode;
		}

		// Construct vector path list
		while(!m_pathQueue.IsEmpty())
		{
			// Dequeue node in reverse order and push onto free list
			ASNodeDesc *asNodeDesc = m_pathQueue.DequeueTail();
			m_freeNodeList.Push(asNodeDesc);

			// Add path position to output list
			pathOut.Add(asNodeDesc->m_pathNode);
		}

		return TRUE;
	}

	return FALSE;
}

void CRuPathFinder_AStar::ResetPathFinder()
{
	// Clear open list
	while(!m_openList.IsEmpty())
	{
		m_freeNodeList.Push(m_openList.Pop());
	}

	// Clear path queue
	while(!m_pathQueue.IsEmpty())
	{
		m_freeNodeList.Push(m_pathQueue.Dequeue());
	}

	// Clear closed list
	for(INT32 i = 0; i < m_closedList.Count(); i++)
	{
		m_freeNodeList.Push(m_closedList[i]);
	}
	m_closedList.Clear();

	// Reset flags
	m_openHash->Clear();
	m_closedHash->Clear();
}

// ************************************************************************************************************************************************************

#pragma managed
