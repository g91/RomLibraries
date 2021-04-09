#ifndef _RUCOLLECTIONSTREE_H_
#define _RUCOLLECTIONSTREE_H_

#include "../../Core/Collection/RuCollections.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

enum RuTreeTraversalMethod
{
	ruTTM_PreOrder						= 0,
	ruTTM_InOrder						= 1,
	ruTTM_PostOrder						= 2,
	ruTTM_FORCE_DWORD					= 0x7FFFFFFF
};

// ************************************************************************************************************************************************************

typedef boost::function<BOOL (const void *, void *)> CRuBinaryTreeEnumerateCallbackFunctor;		// Parameters: (key, data)

/*!
	CRuBinaryTree
	@author Ceranos
	@version 2004-10-13
*/
template <class KeyType, class DataType>
class CRuBinaryTree
{
private:
	struct TreeNode
	{
		KeyType				m_key;
		DataType			m_data;
		TreeNode*			m_left;
		TreeNode*			m_right;
	};
	
	INT32					m_treeSize;
	TreeNode*				m_root;
	
public:
							CRuBinaryTree() : m_root(NULL),  m_treeSize(0) { };
							~CRuBinaryTree();
							
	void					Clear(TreeNode *curNode = NULL);					// release all memory used by tree

	// Tree maintenance
	BOOL					Add(const KeyType &key, const DataType &data);
	BOOL					Set(const KeyType &key, const DataType &data);
	BOOL					Remove(const KeyType &key);
	BOOL					ContainsKey(const KeyType &key) const;
	DataType&				Get(const KeyType &key);
	DataType&				GetAtIndex(INT32 index);
	KeyType&				GetKeyAtIndex(INT32 index);
	void					BalanceTree();

	BOOL					ForAllElements(CRuBinaryTreeEnumerateCallbackFunctor callback);

	// Tree enumeration
//	void					EnumerateKeys(CRuArrayList<KeyType *> *keyListOut, RuTreeTraversalMethod mode = ruTTM_InOrder);
//	void					EnumerateData(CRuArrayList<DataType *> *dataListOut, RuTreeTraversalMethod mode = ruTTM_InOrder);

	// Tree information and statistics
	INT32					Count() const;
//	INT32					GetTreeDepth();

private:
	INT32					FindInOrderPredecessor(TreeNode *node, TreeNode **nodeOut, TreeNode **nodeParentOut);
	INT32					FindInOrderSuccessor(TreeNode *node, TreeNode **nodeOut, TreeNode **nodeParentOut);
	void					DeleteNode(TreeNode *parentNode, TreeNode *node);
	void					RemoveParentRef(TreeNode *parentNode, TreeNode *childNode);
	DataType*				GetAtIndex(TreeNode *curNode, INT32 index, INT32 &curIndex);
	KeyType*				GetKeyAtIndex(TreeNode *curNode, INT32 index, INT32 &curIndex);

	BOOL					ForAllElements(TreeNode *root, CRuBinaryTreeEnumerateCallbackFunctor callback);


/*
	void					EnumerateKeysInOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode);
	void					EnumerateKeysPreOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode);
	void					EnumerateKeysPostOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode);
*/
	void					TreeToVine(TreeNode *root, int &size);
	void					Compression(TreeNode *root, int count);
	void					VineToTree(TreeNode *root, int size);
};

// ************************************************************************************************************************************************************

/*
--------------------------------------------------------------------
CRuBinaryTree

Beginning of function definition for templatized CRuBinaryTree

--------------------------------------------------------------------
*/

// *** Desc : Destructor. Performs cleanup.
//     Ret  : N/A
template <class KeyType, class DataType>
CRuBinaryTree<KeyType, DataType>::~CRuBinaryTree()
{
	Clear();
}

// *** Desc : Removes entire binary tree and release all associated memory
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::Clear(TreeNode *curNode)
{
	// If the parameter is NULL, release the entire tree from the root
	if(curNode == NULL)
	{
		// Abort if the tree is already empty
		if(m_root == NULL)
			return;

		curNode = m_root;
		m_root = NULL;
	}

	// Recursively release left and right children
	if(curNode->m_left != NULL)
		Clear(curNode->m_left);
	if(curNode->m_right != NULL)
		Clear(curNode->m_right);

	// Free memory used by the current node
	delete curNode;

	// eh...?
	m_treeSize = 0;
}

// *** Desc : Inserts new data into binary tree
//     Ret  : TRUE if successful, FALSE otherwise
template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::Add(const KeyType &key, const DataType &data)
{
	TreeNode *newNode;

	// Create new node
	newNode = ruNEW TreeNode;
	if(newNode == NULL)
		return FALSE;

	// Store key-data pair into node
	newNode->m_key = key;
	newNode->m_data = data;
	newNode->m_left = NULL;
	newNode->m_right = NULL;

	// Increment the tree size
	m_treeSize++;

	// Find proper place to insert the new node
	if(m_root == NULL)
	{
		m_root = newNode;
		return TRUE;
	}
	else
	{
		TreeNode *curNode = m_root;
		while(TRUE)
		{
			if(key < curNode->m_key)
			{
				if(curNode->m_left == NULL)
				{
					curNode->m_left = newNode;
					return TRUE;
				}
				else
				{
					curNode = curNode->m_left;
				}
			}
			else if(key > curNode->m_key)
			{
				if(curNode->m_right == NULL)
				{
					curNode->m_right = newNode;
					return TRUE;
				}
				else
				{
					curNode = curNode->m_right;
				}
			}
			// Data to be inserted has same key as an existing data in tree...
			else
			{
				// Undo insert and return FALSE
				m_treeSize--;
				delete newNode;
				return FALSE;
			}
		}
	}

	return FALSE;
}

template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::Set(const KeyType &key, const DataType &data)
{
	// Find the node matching the given data key
	TreeNode *curNode = m_root;
	while(TRUE)
	{
		if(curNode == NULL)
			return FALSE;

		// Key is found, return pointer to actual data
		if(key == curNode->m_key)
		{
			curNode->m_data = data;
			return TRUE;
		}
		else if(key < curNode->m_key)
		{
			curNode = curNode->m_left;
		}
		else
		{
			curNode = curNode->m_right;
		}
	}

	return FALSE;
}

// *** Desc : Removes data element matching the supplied data key from the binary tree
//     Ret  : TRUE if successful, FALSE if key is not found
template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::Remove(const KeyType &key)
{
	TreeNode *curNode, *parentNode;
	curNode = m_root;
	parentNode = NULL;

	// Find the node to delete
	while(TRUE)
	{
		// The key is not found, return
		if(curNode == NULL)
			return FALSE;

		if(key == curNode->m_key)
		{
			break;
		}
		else if(key < curNode->m_key)
		{
			parentNode = curNode;
			curNode = curNode->m_left;
		}
		else
		{
			parentNode = curNode;
			curNode = curNode->m_right;
		}
	}

	DeleteNode(parentNode, curNode);
/*
	TreeNode *curNode, *oldNode, *parentNode, *minNode, *minNodeParent, *minNodeRight;
	curNode = m_root;
	parentNode = NULL;

	// Remove the node
	// Case 1
	if(curNode->m_right == NULL)
	{
		// If the node does not have children, then remove references to it in its parent node and delete the node
		if(curNode->m_left == NULL)
		{
			RemoveParentRef(parentNode, curNode);
			delete curNode;
		}
		else
		{
			oldNode = curNode->m_left;
			*curNode = *oldNode;
			delete oldNode;
		}
	}
	// Case 2
	else if(curNode->m_right->m_left == NULL)
	{
		curNode->m_right->m_left = curNode->m_left;
		oldNode = curNode->m_right;
		*curNode = *oldNode;
		delete oldNode;
	}
	// Case 3
	else
	{
		// Find left-most child node of the right child and its parent node
		minNode = curNode->m_right->m_left;
		minNodeParent = curNode->m_right;
		while(minNode->m_left != NULL) 
		{
			minNodeParent = minNode;
			minNode = minNode->m_left;
		}

		// Overwrite data of left-most child node into the node to be deleted
		curNode->m_data = minNode->m_data;
		minNodeRight = minNode->m_right;

		// If left-most child node has a right child, copy the right child's data into
		// the existing left-most child node's structure and delete the extra node.
		// Otherwise, remove old link to the left-most child and delete the node
		if(minNodeRight != NULL)
		{
			*minNode = *minNodeRight;
			delete minNodeRight;
		}
		else
		{
			// Remove reference to left-most child node in its parent
			RemoveParentRef(minNodeParent, minNode);
			delete minNode;
		}
	}
*/
	m_treeSize--;
	return TRUE;
}

template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::ContainsKey(const KeyType &key) const
{
	// Find the node matching the given data key
	TreeNode *curNode = m_root;
	while(TRUE)
	{
		if(curNode == NULL)
			return FALSE;

		// Key is found, return pointer to actual data
		if(key == curNode->m_key)
		{
			return TRUE;
		}
		else if(key < curNode->m_key)
		{
			curNode = curNode->m_left;
		}
		else
		{
			curNode = curNode->m_right;
		}
	}

	return FALSE;
}

template <class KeyType, class DataType>
DataType &CRuBinaryTree<KeyType, DataType>::Get(const KeyType &key)
{
	// Find the node matching the given data key
	TreeNode *curNode = m_root;
	while(TRUE)
	{
		if(curNode == NULL)
			throw CRuArgumentOutOfRangeException();

		// Key is found, return pointer to actual data
		if(key == curNode->m_key)
		{
			return curNode->m_data;
		}
		else if(key < curNode->m_key)
		{
			curNode = curNode->m_left;
		}
		else
		{
			curNode = curNode->m_right;
		}
	}

	throw CRuArgumentOutOfRangeException();
}

template <class KeyType, class DataType>
DataType &CRuBinaryTree<KeyType, DataType>::GetAtIndex(INT32 index)
{
	INT32 curIndex = 0;

	return *GetAtIndex(m_root, index, curIndex);
}

template <class KeyType, class DataType>
KeyType &CRuBinaryTree<KeyType, DataType>::GetKeyAtIndex(INT32 index)
{
	INT32 curIndex = 0;

	return *GetKeyAtIndex(m_root, index, curIndex);
}

template <class KeyType, class DataType>
DataType *CRuBinaryTree<KeyType, DataType>::GetAtIndex(TreeNode *curNode, INT32 index, INT32 &curIndex)
{
	DataType *returnData = NULL;

	if(curNode->m_left)
	{
		returnData = GetAtIndex(curNode->m_left, index, curIndex);
	}

	if(returnData == NULL && index == curIndex)
	{
		returnData = &curNode->m_data;
	}

	++curIndex;

	if(returnData == NULL && curNode->m_right)
	{
		returnData = GetAtIndex(curNode->m_right, index, curIndex);
	}

	return returnData;
}

template <class KeyType, class DataType>
KeyType *CRuBinaryTree<KeyType, DataType>::GetKeyAtIndex(TreeNode *curNode, INT32 index, INT32 &curIndex)
{
	KeyType *returnKey = NULL;

	if(curNode->m_left)
	{
		returnKey = GetKeyAtIndex(curNode->m_left, index, curIndex);
	}

	if(returnKey == NULL && index == curIndex)
	{
		returnKey = &curNode->m_key;
	}

	++curIndex;

	if(returnKey == NULL && curNode->m_right)
	{
		returnKey = GetKeyAtIndex(curNode->m_right, index, curIndex);
	}

	return returnKey;
}

// *** Desc : Balances the tree structure using the Day/Stout/Warren(DSW)Algorithm.
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::BalanceTree()
{
	int size;

	if(m_root == NULL)
		return;

	// Create a pseudo root
	TreeNode *pseudoRoot = ruNEW TreeNode();
	pseudoRoot->m_left = NULL;
	pseudoRoot->m_right = m_root;

	// Deconstruct tree into a degenerate list
	TreeToVine(pseudoRoot, size);

	// Reconstruct tree into a balanced tree
	VineToTree(pseudoRoot, size);

	// New root is the right child of the pseudo root
	m_root = pseudoRoot->m_right;

	// Delete the pseudo root
	delete pseudoRoot;
}

template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::ForAllElements(CRuBinaryTreeEnumerateCallbackFunctor callback)
{
	if(m_root)
	{
		return ForAllElements(m_root, callback);
	}

	return TRUE;
}

/*
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::EnumerateKeys(CRuArrayList<KeyType *> *keyListOut, RuTreeTraversalMethod mode)
{
	if(m_root != NULL)
	{
		switch(mode)
		{
			case ruTTM_InOrder:
				EnumerateKeysInOrder(keyListOut, m_root);
				break;
			case ruTTM_PreOrder:
				EnumerateKeysPreOrder(keyListOut, m_root);
				break;
			case ruTTM_PostOrder:
				EnumerateKeysPostOrder(keyListOut, m_root);
				break;
		}
	}
}
*/

template <class KeyType, class DataType>
INT32 CRuBinaryTree<KeyType, DataType>::Count() const
{
	return m_treeSize;
}

template <class KeyType, class DataType>
INT32 CRuBinaryTree<KeyType, DataType>::FindInOrderPredecessor(TreeNode *node, TreeNode **nodeOut, TreeNode **nodeParentOut)
{
	// Clear depth
	INT32 depth = 0;

	// Find the right-most child of the left subtree
	if(node && node->m_left)
	{
		TreeNode *parentNode = node;
		TreeNode *returnNode = node->m_left;

		while(returnNode->m_right)
		{
			parentNode = returnNode;
			returnNode = returnNode->m_right;
			++depth;
		}

		*nodeOut = returnNode;
		*nodeParentOut = parentNode;

		return depth;
	}

	return -1;
}

template <class KeyType, class DataType>
INT32 CRuBinaryTree<KeyType, DataType>::FindInOrderSuccessor(TreeNode *node, TreeNode **nodeOut, TreeNode **nodeParentOut)
{
	// Clear depth
	INT32 depth = 0;

	// Find the left-most child of the right subtree
	if(node && node->m_right)
	{
		TreeNode *parentNode = node;
		TreeNode *returnNode = node->m_right;

		while(returnNode->m_left)
		{
			parentNode = returnNode;
			returnNode = returnNode->m_left;
			++depth;
		}

		*nodeOut = returnNode;
		*nodeParentOut = parentNode;

		return depth;
	}

	return -1;
}

template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::DeleteNode(TreeNode *parentNode, TreeNode *node)
{
	if(node->m_left == NULL && node->m_right == NULL)
	{
		// Remove reference at parent node
		RemoveParentRef(parentNode, node);

		// Delete the node
		delete node;
	}
	else if(node->m_left == NULL)
	{
		// Store pointer to right child
		TreeNode *oldNode = node->m_right;

		// Copy contents of right child to node
		*node = *(node->m_right);

		// Delete the now unused right child
		delete oldNode;
	}
	else if(node->m_right == NULL)
	{
		// Store pointer to left child
		TreeNode *oldNode = node->m_left;

		// Copy contents of left child to node
		*node = *(node->m_left);

		// Delete the now unused left child
		delete oldNode;
	}
	else
	{
		// Find predecessor and successor nodes
		TreeNode *predecessor, *predecessorParent, *successor, *successorParent;
		INT32 predecessorDepth = FindInOrderPredecessor(node, &predecessor, &predecessorParent);
		INT32 successorDepth = FindInOrderSuccessor(node, &successor, &successorParent);

		// Use the node that is deepest in the tree to avoid un-balancing the tree
		TreeNode *replacementNode = predecessorDepth > successorDepth ? predecessor : successor;
		TreeNode *replacementNodeParent = predecessorDepth > successorDepth ? predecessorParent : successorParent;

		// Copy contents of replacement node into temporary
		TreeNode replacementNodeContents = *replacementNode;

		// Delete replacement node
		DeleteNode(replacementNodeParent, replacementNode);

		// Copy contents of replacement node into node
		node->m_key = replacementNodeContents.m_key;
		node->m_data = replacementNodeContents.m_data;
	}
}

// *** Desc : Given a parent node and its child node, removes the link to the child node from the parent
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::RemoveParentRef(TreeNode *parentNode, TreeNode *childNode)
{
	if(parentNode == NULL)
	{
		m_root = NULL;
		return;
	}

	if(parentNode->m_left == childNode)
		parentNode->m_left = NULL;
	else if(parentNode->m_right == childNode)
		parentNode->m_right = NULL;
}

template <class KeyType, class DataType>
BOOL CRuBinaryTree<KeyType, DataType>::ForAllElements(TreeNode *root, CRuBinaryTreeEnumerateCallbackFunctor callback)
{
	callback(&root->m_key, &root->m_data);

	if(root->m_left)
	{
		ForAllElements(root->m_left, callback);
	}

	if(root->m_right)
	{
		ForAllElements(root->m_right, callback);
	}

	return TRUE;
}

/*
// *** Desc
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::EnumerateKeysInOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode)
{
	if(curNode->m_left != NULL)
		EnumerateKeysInOrder(keyListOut, curNode->m_left);

	keyListOut->Add(curNode->m_key);

	if(curNode->m_right != NULL)
		EnumerateKeysInOrder(keyListOut, curNode->m_right);
}

// *** Desc
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::EnumerateKeysPreOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode)
{
	keyListOut->Add(curNode->m_key);

	if(curNode->m_left != NULL)
		EnumerateKeysPreOrder(keyListOut, curNode->m_left);

	if(curNode->m_right != NULL)
		EnumerateKeysPreOrder(keyListOut, curNode->m_right);
}

// *** Desc
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::EnumerateKeysPostOrder(CRuArrayList<KeyType *> *keyListOut, TreeNode *curNode)
{
	if(curNode->m_left != NULL)
		EnumerateKeysPostOrder(keyListOut, curNode->m_left);

	if(curNode->m_right != NULL)
		EnumerateKeysPostOrder(keyListOut, curNode->m_right);

	keyListOut->Add(curNode->m_key);
}
*/
// *** Desc : Collapses tree into a linked list.
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::TreeToVine(TreeNode *root, int &size)
{
	TreeNode *vineTail, *remainder, *tempPtr;

	vineTail = root;
	remainder = vineTail->m_right;
	size = 0;
	while(remainder != NULL)
	{
		// If no left subtree, move right
		if(remainder->m_left == NULL)
		{
			vineTail = remainder;
			remainder = remainder->m_right;
			size++;
		}
		// Else eliminate the left subtree by rotations
		else
		{
			tempPtr = remainder->m_left;
			remainder->m_left = tempPtr->m_right;
			tempPtr->m_right = remainder;
			remainder = tempPtr;
			vineTail->m_right = tempPtr;
		}
	}
}

// *** Desc : Left-rotates tree [count] number of times
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::Compression(TreeNode *root, int count)
{
	TreeNode *scanner = root;

	for(int j = 0; j < count; j++)
	{
		// Leftward rotation
		TreeNode *child = scanner->m_right;
		scanner->m_right = child->m_right;
		scanner = scanner->m_right;
		child->m_right = scanner->m_left;
		scanner->m_left = child;
	}
}

// *** Desc : Reconstructs a degenerate linked list tree into a balanced tree with
//            pseudo-root.
//     Ret  : None
template <class KeyType, class DataType>
void CRuBinaryTree<KeyType, DataType>::VineToTree(TreeNode *root, int size)
{
	// Calculate size of full binary tree
	int fullCount = 1;
	while(fullCount <= size)						// Iterate one step past full tree
		fullCount = fullCount + fullCount + 1;		// pow(2, k) - 1
	fullCount = fullCount / 2;

	// Compress the tree(left-rotate)
	Compression(root, size - fullCount);
	for(size = fullCount; size > 1; size /= 2)
		Compression(root, size / 2);
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
