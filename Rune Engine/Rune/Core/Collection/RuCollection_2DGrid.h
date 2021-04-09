#pragma once

#include "../../Core/Collection/RuCollections.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

template <class DataType>
class CRuCollection_2DGrid
{
private:
	struct GridNode
	{
		DataType					m_data;						//!< Data at the node

		GridNode*					m_prev;						//!<
		GridNode*					m_next;
	};

	INT32							m_width;
	INT32							m_height;

	GridNode***						m_grid;

public:
									CRuCollection_2DGrid();
									~CRuCollection_2DGrid();

	BOOL							Initialize(INT32 width, INT32 height);

	BOOL							Add(INT32 x, INT32 y, DataType &data);
	BOOL							Remove(INT32 x, INT32 y, DataType &data);

	INT32							Count();
	INT32							Count(INT32 x, INT32 y);
	DataType&						Get(INT32 x, INT32 y, INT32 idx);

	INT32							GetWidth();
	INT32							GetHeight();
};

// ************************************************************************************************************************************************************

template <class DataType>
CRuCollection_2DGrid<DataType>::CRuCollection_2DGrid()
:	m_width(0),
	m_height(0),
	m_grid(NULL)
{
}

template <class DataType>
CRuCollection_2DGrid<DataType>::~CRuCollection_2DGrid()
{
}

template <class DataType>
BOOL CRuCollection_2DGrid<DataType>::Initialize(INT32 width, INT32 height)
{
	// Release old grid

	// Store grid dimensions
	m_width = width;
	m_height = height;

	// Allocate grid storage
	m_grid = ruNEW GridNode ** [m_width];

	for(INT32 i = 0; i < m_width; ++i)
	{
		m_grid[i] = ruNEW GridNode * [m_height];
		memset(m_grid[i], 0, sizeof(GridNode *) * m_height);
	}

	return TRUE;
}

template <class DataType>
BOOL CRuCollection_2DGrid<DataType>::Add(INT32 x, INT32 y, DataType &data)
{
	// Abort if out of bounds
	if(x < 0 || x >= m_width || y < 0 || y >= m_height)
		return FALSE;

	// Make sure the data is unique
	GridNode *node = m_grid[x][y];
	while(node)
	{
		if(node->m_data == data)
		{
			return FALSE;
		}

		node = node->m_next;
	}

	// Create new node and attach it to the front of the list
	GridNode *newNode = ruNEW GridNode();
	newNode->m_data = data;
	newNode->m_prev = NULL;
	newNode->m_next = m_grid[x][y];

	if(newNode->m_next)
	{
		newNode->m_next->m_prev = newNode;
	}

	m_grid[x][y] = newNode;

	return TRUE;
}

template <class DataType>
BOOL CRuCollection_2DGrid<DataType>::Remove(INT32 x, INT32 y, DataType &data)
{
	// Abort if out of bounds
	if(x < 0 || x >= m_width || y < 0 || y >= m_height)
		return FALSE;

	// Find data in the list
	GridNode *node = m_grid[x][y];
	while(node)
	{
		if(node->m_data == data)
		{
			// Detach node from list
			if(node->m_next)
			{
				if(node->m_prev)
				{
					// Node is in the middle of the list
					node->m_next->m_prev = node->m_prev;
					node->m_prev->m_next = node->m_next;
				}
				else
				{
					// Node is the first in the list
					node->m_next->m_prev = NULL;
					m_grid[x][y] = node->m_next;
				}
			}
			else
			{
				if(node->m_prev)
				{
					// Node is the last in the list
					node->m_prev->m_next = NULL;
				}
				else
				{
					// Node is the only one in the list
					m_grid[x][y] = NULL;
				}
			}

			// Delete node
			ruSAFE_DELETE(node);

			return TRUE;
		}

		node = node->m_next;
	}

	return FALSE;
}

template <class DataType>
INT32 CRuCollection_2DGrid<DataType>::Count()
{
	INT32 nodeCount = 0;

	for(INT32 x = 0; x < m_width; ++x)
	{
		for(INT32 z = 0; z < m_height; ++z)
		{
			nodeCount += this->Count(x, z);
		}
	}

	return nodeCount;
}

template <class DataType>
INT32 CRuCollection_2DGrid<DataType>::Count(INT32 x, INT32 y)
{
	// Abort if out of bounds
	if(x < 0 || x >= m_width || y < 0 || y >= m_height)
		return 0;

	INT32 nodeCount = 0;

	GridNode *node = m_grid[x][y];
	while(node)
	{
		++nodeCount;
		node = node->m_next;
	}

	return nodeCount;
}

template <class DataType>
DataType &CRuCollection_2DGrid<DataType>::Get(INT32 x, INT32 y, INT32 idx)
{
	// Abort if out of bounds
	if(x < 0 || x >= m_width || y < 0 || y >= m_height || idx < 0)
		throw CRuArgumentOutOfRangeException();

	INT32 nodeIdx = -1;

	GridNode *node = m_grid[x][y];
	while(node)
	{
		++nodeIdx;

		if(nodeIdx == idx)
			break;

		node = node->m_next;
	}

	if(nodeIdx != idx)
		throw CRuArgumentOutOfRangeException();

	return node->m_data;
}

template <class DataType>
INT32 CRuCollection_2DGrid<DataType>::GetWidth()
{
	return m_width;
}

template <class DataType>
INT32 CRuCollection_2DGrid<DataType>::GetHeight()
{
	return m_height;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
