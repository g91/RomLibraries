#pragma once

#include "../../Core/Collection/RuCollections.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

typedef boost::function<BOOL (const char *, void *)> CRuTernaryStringTreeEnumerateCallbackFunctor;		// Parameters: (const char *key, void *data)

// ************************************************************************************************************************************************************

/*!
	CRuTernaryStringTree
	@author John Tang
	@version 2007-01-20
*/
template <class DataType>
class CRuTernaryStringTree
{
private:
	struct TreeNode
	{
		char				m_splitChar;
		char				m_padding[3];

		TreeNode*			m_lo;
		TreeNode*			m_eq;
		TreeNode*			m_hi;
	};

	struct EndNode
	{
		char*				m_string;
		DataType			m_data;
	};

	struct TreeNodeBlock
	{
		TreeNode			m_nodes[32];
		TreeNodeBlock*		m_nextBlock;
	};
	
	INT32					m_treeSize;
	TreeNode*				m_root;

	TreeNodeBlock*			m_allocatedBlocks;

	// Temporaries
	CRuStack<TreeNode *>	m_unusedNodeStack;
	CRuStack<EndNode *>		m_unusedEndNodeStack;
	CRuStack<TreeNode *>	m_stack;
	
public:
							CRuTernaryStringTree();
							~CRuTernaryStringTree();
							
	void					Clear();

	// Tree maintenance
	BOOL					Add(const char *key, const DataType &data);
	BOOL					Set(const char *key, const DataType &data);
	BOOL					Remove(const char *key);
	BOOL					ContainsKey(const char *key) const;
	DataType&				Get(const char *key);
	BOOL					Get(const char *key, DataType &dataOut);
	void					BalanceTree();

	// Tree information and statistics
	INT32					Count() const;

	BOOL					ForAllElements(CRuTernaryStringTreeEnumerateCallbackFunctor callback);
	BOOL					EnumerateKeys(CRuArrayList<const char *> &keysOut);

protected:
	BOOL					Insert(const char *originalKey, const char *key, const DataType &data, BOOL replaceExisting)
							{
								assert(originalKey);

								if(m_root == NULL)
								{
									// Allocate root node
									m_root = AllocateTreeNode();
									m_root->m_splitChar = *key;
									m_root->m_lo = m_root->m_eq = m_root->m_hi = NULL;
								}

								TreeNode *node = m_root;

								while(TRUE)
								{
									if(*key < node->m_splitChar)
									{
										// If child node does not exist, create it
										if(node->m_lo == NULL)
										{
											TreeNode *newNode = AllocateTreeNode();
											newNode->m_splitChar = *key;
											newNode->m_lo = newNode->m_eq = newNode->m_hi = NULL;

											node->m_lo = newNode;
										}

										// Move on to child node
										node = node->m_lo;
									}
									else if(*key > node->m_splitChar)
									{
										// If child node does not exist, create it
										if(node->m_hi == NULL)
										{
											TreeNode *newNode = AllocateTreeNode();
											newNode->m_splitChar = *key;
											newNode->m_lo = newNode->m_eq = newNode->m_hi = NULL;

											node->m_hi = newNode;
										}

										// Move on to child node
										node = node->m_hi;
									}
									else
									{
										// Increment key pointer
										++key;

										// If child node does not exist, create it
										if(node->m_eq == NULL)
										{
											TreeNode *newNode = AllocateTreeNode();
											newNode->m_splitChar = *key;
											newNode->m_lo = newNode->m_eq = newNode->m_hi = NULL;

											node->m_eq = newNode;
										}

										// Move on to child node
										node = node->m_eq;
									}

									if(*key == NULL && *key == node->m_splitChar)
									{
										if(node->m_eq)
										{
											// we found an existing key
											if(replaceExisting)
											{
												reinterpret_cast<EndNode *>(node->m_eq)->m_data = data;

												return TRUE;
											}
										}
										else
										{
											EndNode *endNode = m_unusedEndNodeStack.IsEmpty()? ruNEW EndNode : m_unusedEndNodeStack.Pop();

											INT32 strLength = static_cast<INT32>(strlen(originalKey) + 1);
											endNode->m_string = ruNEW char [strLength];
											memcpy(endNode->m_string, originalKey, strLength);
											endNode->m_data = data;

											node->m_eq = reinterpret_cast<TreeNode *>(endNode);

											++m_treeSize;

											return TRUE;
										}

										break;
									}
								}

								return FALSE;
							}

	void					Enumerate(TreeNode *node, CRuStack<TreeNode *> &nodeStack)
							{
								if(node == NULL)
								{
									return;
								}

								// Invoke on lo node
								Enumerate(node->m_lo, nodeStack);

								if(node->m_splitChar)
								{
									// Invoke on eq node
									Enumerate(node->m_eq, nodeStack);
								}
								else
								{
									if(node->m_eq)
									{
										nodeStack.Push(node);
									}
								}

								// Invoke on hi node
								Enumerate(node->m_hi, nodeStack);
							}

	BOOL					ForAllElements(TreeNode *node, CRuTernaryStringTreeEnumerateCallbackFunctor callback)
							{
								if(node == NULL)
								{
									return TRUE;
								}

								// Invoke on lo node
								if(ForAllElements(node->m_lo, callback) == FALSE)
								{
									return FALSE;
								}

								if(node->m_splitChar)
								{
									// Invoke on eq node
									if(ForAllElements(node->m_eq, callback) == FALSE)
									{
										return FALSE;
									}
								}
								else
								{
									if(node->m_eq)
									{
										if(callback(reinterpret_cast<EndNode *>(node->m_eq)->m_string, &reinterpret_cast<EndNode *>(node->m_eq)->m_data) == FALSE)
										{
											return FALSE;
										}
									}
								}

								// Invoke on hi node
								if(ForAllElements(node->m_hi, callback) == FALSE)
								{
									return FALSE;
								}

								return TRUE;
							}

	void					AllocateTreeNodeBlock()
							{
								// Allocate new tree node block
								TreeNodeBlock *block = ruNEW TreeNodeBlock();
								block->m_nextBlock = m_allocatedBlocks;
								m_allocatedBlocks = block;

								// Push everything onto the unused node stack
								for(INT32 i = 0; i < 32; ++i)
								{
									m_unusedNodeStack.Push(&block->m_nodes[i]);
								}
							}

	TreeNode*				AllocateTreeNode()
							{
								// If there are no unused nodes, allocate a new node block
								if(m_unusedNodeStack.IsEmpty())
								{
									AllocateTreeNodeBlock();
								}

								// Pop a node off the stack
								return m_unusedNodeStack.Pop();
							}
};

// ************************************************************************************************************************************************************

template <class DataType>
CRuTernaryStringTree<DataType>::CRuTernaryStringTree()
:	m_root(NULL),
	m_treeSize(0),
	m_allocatedBlocks(NULL)
{
}

template <class DataType>
CRuTernaryStringTree<DataType>::~CRuTernaryStringTree()
{
	Clear();

	TreeNodeBlock *nextTreeNodeBlock = NULL;
	while(m_allocatedBlocks)
	{
		nextTreeNodeBlock = m_allocatedBlocks->m_nextBlock;
		delete m_allocatedBlocks;
		m_allocatedBlocks = nextTreeNodeBlock;
	}

	while(m_unusedEndNodeStack.IsEmpty() == FALSE)
	{
		delete m_unusedEndNodeStack.Pop();
	}
}

template <class DataType>
void CRuTernaryStringTree<DataType>::Clear()
{
	m_stack.Clear();
	m_stack.Push(m_root);

	while(m_stack.IsEmpty() == FALSE)
	{
		TreeNode *node = m_stack.Pop();

		if(node)
		{
			m_stack.Push(node->m_lo);
			m_stack.Push(node->m_hi);

			if(node->m_splitChar == NULL)
			{
				if(node->m_eq)
				{
					delete[] reinterpret_cast<EndNode *>(node->m_eq)->m_string;
					m_unusedEndNodeStack.Push(reinterpret_cast<EndNode *>(node->m_eq));
				}
			}
			else
			{
				m_stack.Push(node->m_eq);
			}

			m_unusedNodeStack.Push(node);
		}
	}

	m_root = NULL;
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::Add(const char *key, const DataType &data)
{
	return Insert(key, key, data, FALSE);
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::Set(const char *key, const DataType &data)
{
	return Insert(key, key, data, TRUE);
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::Remove(const char *key)
{
	BOOL retVal = FALSE;

	TreeNode *node = m_root;

	m_stack.Clear();

	while(node)
	{
		// Push node onto the trace stack
		m_stack.Push(node);

		if(*key < node->m_splitChar)
		{
			node = node->m_lo;
		}
		else if(*key > node->m_splitChar)
		{
			node = node->m_hi;
		}
		else
		{
			if(*key++ == NULL)
			{
				if(node->m_eq)
				{
					// Push end node onto the unused end node stack
					EndNode *endNode = reinterpret_cast<EndNode *>(node->m_eq);
					delete[] endNode->m_string;
					m_unusedEndNodeStack.Push(endNode);

					// Clear end node pointer
					node->m_eq = NULL;

					--m_treeSize;

					retVal = TRUE;
				}

				break;
			}

			node = node->m_eq;
		}
	}

	// Examine the trace stack and remove any unused nodes
	while(m_stack.IsEmpty() == FALSE)
	{
		TreeNode *traceNode = m_stack.Pop();

		// If the node is a leaf, it is no longer used
		if(traceNode->m_lo == NULL && traceNode->m_eq == NULL && traceNode->m_hi == NULL)
		{
			if(m_stack.IsEmpty() == FALSE)
			{
				// Eliminate link from parent to trace node
				TreeNode *traceNodeParent = m_stack.Pop();

				if(traceNodeParent->m_lo == traceNode)
				{
					traceNodeParent->m_lo = NULL;
				}
				else if(traceNodeParent->m_eq == traceNode)
				{
					traceNodeParent->m_eq = NULL;
				}
				else
				{
					traceNodeParent->m_hi = NULL;
				}

				m_stack.Push(traceNodeParent);

				// Push tree node onto the unused node stack
				m_unusedNodeStack.Push(traceNode);
			}
		}
	}

	return retVal;
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::ContainsKey(const char *key) const
{
	TreeNode *node;

	node = m_root;

	while(node)
	{
		if(*key < node->m_splitChar)
		{
			node = node->m_lo;
		}
		else if(*key > node->m_splitChar)
		{
			node = node->m_hi;
		}
		else
		{
			if(*key++ == NULL)
			{
				if(node->m_eq)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}

			node = node->m_eq;
		}
	}

	return FALSE;
}

template <class DataType>
DataType &CRuTernaryStringTree<DataType>::Get(const char *key)
{
	TreeNode *node;

	node = m_root;

	while(node)
	{
		if(*key < node->m_splitChar)
		{
			node = node->m_lo;
		}
		else if(*key > node->m_splitChar)
		{
			node = node->m_hi;
		}
		else
		{
			if(*key++ == NULL)
			{
				if(node->m_eq)
				{
					return reinterpret_cast<EndNode *>(node->m_eq)->m_data;
				}
				else
				{
					throw CRuArgumentOutOfRangeException();
				}
			}

			node = node->m_eq;
		}
	}

	throw CRuArgumentOutOfRangeException();
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::Get(const char *key, DataType &dataOut)
{
	TreeNode *node;

	node = m_root;

	while(node)
	{
		if(*key < node->m_splitChar)
		{
			node = node->m_lo;
		}
		else if(*key > node->m_splitChar)
		{
			node = node->m_hi;
		}
		else
		{
			if(*key++ == NULL)
			{
				if(node->m_eq)
				{
					dataOut = reinterpret_cast<EndNode *>(node->m_eq)->m_data;
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}

			node = node->m_eq;
		}
	}

	return FALSE;
}

template <class DataType>
void CRuTernaryStringTree<DataType>::BalanceTree()
{
}

template <class DataType>
INT32 CRuTernaryStringTree<DataType>::Count() const
{
	return m_treeSize;
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::ForAllElements(CRuTernaryStringTreeEnumerateCallbackFunctor callback)
{
	if(m_root)
	{
		CRuStack<TreeNode *> nodeStack(m_treeSize);
		Enumerate(m_root, nodeStack);

		while(nodeStack.IsEmpty() == FALSE)
		{
			TreeNode *node = nodeStack.Pop();

			if(callback(reinterpret_cast<EndNode *>(node->m_eq)->m_string, &reinterpret_cast<EndNode *>(node->m_eq)->m_data) == FALSE)
			{
				return FALSE;
			}
		}

		// Passing a function object around is REALLY slow
//		return ForAllElements(m_root, callback);
	}

	return TRUE;
}

template <class DataType>
BOOL CRuTernaryStringTree<DataType>::EnumerateKeys(CRuArrayList<const char *> &keysOut)
{
	CRuStack<TreeNode *> nodeStack(m_treeSize);
	Enumerate(m_root, nodeStack);

	while(nodeStack.IsEmpty() == FALSE)
	{
		TreeNode *node = nodeStack.Pop();
		keysOut.Add(reinterpret_cast<EndNode *>(node->m_eq)->m_string);
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
