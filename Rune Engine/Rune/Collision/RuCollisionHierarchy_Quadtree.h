#ifndef _RUCOLLISIONHIERARCHY_QUADTREE_H_
#define _RUCOLLISIONHIERARCHY_QUADTREE_H_

// Base types
#include "../Collision/RuCollisionHierarchy_Base.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

/*!
	A streamed AABB tree. 
*/
class CRuCollisionHierarchy_Quadtree : public IRuCollisionHierarchy
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ColObjectDesc
	{
		INT32									m_key;													//!< Collision object key
		INT32									m_iteration;											//!< TODO: Iteration number of the collision object
		INT32									m_offset;												//!< Collision object offset
		CRuAABB									m_bounds;												//!< Collision object bounds
	};

	struct Node
	{
		CRuVector3								m_center;												//!< True center of the node
		CRuAABB									m_bounds;												//!< Bounding box of the node
		CRuArrayList<ColObjectDesc *>*			m_colObjects;											//!< List of collison object descriptors
		Node*									m_children[4];											//!< Pointers to children

												Node();
												Node(const CRuAABB &bounds, float maxExtents);
												~Node();
	
		BOOL									IsLeaf()
		{
			return m_children[0] == NULL;
		}

		BOOL									AddCollisionObject(IRuCollisionObject *colObject, INT32 objOffset);
		BOOL									RemoveCollisionObject(INT32 objKey);

		BOOL									SerializeFrom(IRuStream *stream);
		BOOL									SerializeTo(IRuStream *stream);

		void									Enumerate(CRuCollisionQuery *colQuery, CRuArrayList<ColObjectDesc *> &objListOut);
	};

	// Source stream
	IRuStream*									m_stream;												//!< Data stream

	// Header stuff
	INT32										m_tableAddressOffset;									//!< Offset, from beginning of data stream, to the 32-bit quantity that stores the collision tree table offset in the stream
	INT32										m_zoneTableAddressOffset;								//!< Offset, from beginning of data stream, to the 32-bit quantity that stores the zone table offset in the stream

	// House-keeping
	CRuAABB										m_bounds;
	float										m_gridSpacing;
	char*										m_srcResourceName;

	// Tree
	Node*										m_root;													//!< Root node

	// Collision object instancing data
	CRuLRUQueue<INT32, IRuCollisionObject *>*	m_colObjQueue;											//!< LRU queue of instanced collision objects

	// Workspace
	CRuArrayList<ColObjectDesc *>				m_objList;												//!< Scratch pad

public:
												CRuCollisionHierarchy_Quadtree();
	virtual										~CRuCollisionHierarchy_Quadtree();

	BOOL										Create(IRuStream *stream, const CRuAABB &bounds, float maxExtents, float gridSpacing, const char *srcResourceName);
	BOOL										Open(IRuStream *stream);
	BOOL										Save();

	const CRuAABB&								GetBounds() const;
	float										GetGridSpacing() const;
	const char*									GetSourceResourceName() const;
	
	// Hierarchy management
	virtual BOOL								AddCollisionObject(IRuCollisionObject *colObject);
	virtual BOOL								RemoveCollisionObject(INT32 objKey);

	// Collision query interface
	virtual BOOL								QueryCollision(CRuCollisionQuery *colQuery);

protected:
	IRuCollisionObject*							InstanceCollisionObject(ColObjectDesc *objDesc);
	BOOL										FlushColObjQueue();
	static BOOL									FlushColObjQueueCallback(const void *key, const void *data, void *userData);
};

// ************************************************************************************************************************************************************

#pragma managed

#endif
