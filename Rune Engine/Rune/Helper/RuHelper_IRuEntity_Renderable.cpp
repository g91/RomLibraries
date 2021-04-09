/*!
	@project Rune
	@file RuHelper_IRuEntity_Renderable.cpp

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2005/04/13
*/

#include "../Helper/RuHelper_IRuEntity_Renderable.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

static CRuThread_CriticalSection local_EntityQueueCS;
static CRuQueue<CRuEntity *> local_EntityQueue;

static CRuQueue<CRuEntity *> &RuHelperInternal_LockEntityQueue()
{
	local_EntityQueueCS.Enter();

	local_EntityQueue.Clear();

	return local_EntityQueue;
}

static BOOL RuHelperInternal_UnlockEntityQueue()
{
	local_EntityQueueCS.Leave();

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Enumerates all meshes exposed by the entity and its descendents using pre-order traversal.

	@param entity Entity to enumerate
	@param meshListOut Output mesh list.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuEntity_EnumerateMeshes(CRuEntity *entity, CRuArrayList<IRuMesh *> &meshListOut, CRuArrayList<CRuMatrix4x4> *worldTransformListOut)
{
	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(entity);
		CRuMatrix4x4 worldTransform = renderable->GetWorldTransform();

		for(INT32 i = 0; i < renderable->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;

			if(renderable->GetMesh(i, &mesh, &material))
			{
				meshListOut.Add(mesh);

				if(worldTransformListOut)
				{
					worldTransformListOut->Add(worldTransform);
				}
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateMeshes(curChild, meshListOut, worldTransformListOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Enumerates all skinnable meshes exposed by the entity and its descendents using pre-order traversal.

	@param entity Entity to enumerate
	@param meshListOut Output mesh list.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuEntity_EnumerateSkinnableMeshes(CRuEntity *entity, CRuArrayList<IRuMesh *> &meshListOut)
{
	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()) && entity->GetOption(ruENTITYOPTION_SKINNED) != false)
	{
		IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(entity);

		for(INT32 i = 0; i < renderable->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;

			if(renderable->GetMesh(i, &mesh, &material))
			{
				meshListOut.Add(mesh);
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateSkinnableMeshes(curChild, meshListOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Enumerates all materials used by the entity and its descendents using pre-order traversal.

	@param entity Entity to enumerate
	@param materialListOut Output material list.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuEntity_EnumerateMaterials(CRuEntity *entity, CRuArrayList<IRuMaterial *> &materialListOut)
{
	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(entity);

		for(INT32 i = 0; i < renderable->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;
			
			if(renderable->GetMesh(i, &mesh, &material))
			{
				materialListOut.Add(material);
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateMaterials(curChild, materialListOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

/*!
	Enumerates all materials used by the entity and its descendents using pre-order traversal.

	@param entity Entity to enumerate
	@param honorNoEnumerateFlag When TRUE, any entities with the property "act:no enumerate" set to true will not be enumerated
	@param materialListOut Output material list.

	@return TRUE if successful, FALSE otherwise.
*/
BOOL RuEntity_EnumerateMaterials(CRuEntity *entity, BOOL honorNoEnumerateFlag, CRuArrayList<IRuMaterial *> &materialListOut)
{
	// Skip enumeration if the entity is flagged as do not enumerate
	RuPropertyType propertyValue;
	BOOL doNotEnumerate = entity->GetProperty("act:no enumerate", propertyValue) ? boost::get<BOOL>(propertyValue) : FALSE;

	if(doNotEnumerate)
	{
		return TRUE;
	}

	// Enumerate materials for renderable entities
	if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
	{
		IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(entity);

		for(INT32 i = 0; i < renderable->GetNumMeshes(); ++i)
		{
			IRuMesh *mesh;
			IRuMaterial *material;

			if(renderable->GetMesh(i, &mesh, &material))
			{
				materialListOut.Add(material);
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_EnumerateMaterials(curChild, honorNoEnumerateFlag, materialListOut);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

IRuEntity_Renderable *RuEntity_FindFirstRenderableEntity(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a CRuModel
		if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
		{
			RuHelperInternal_UnlockEntityQueue();
			return static_cast<IRuEntity_Renderable *>(curEntity);
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

IRuMesh *RuEntity_FindFirstMesh(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a renderable
		if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
		{
			IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(curEntity);

			if(renderable->GetNumMeshes() > 0)
			{
				IRuMesh *mesh;
				IRuMaterial *material;

				if(renderable->GetMesh(0, &mesh, &material))
				{
					RuHelperInternal_UnlockEntityQueue();
					return mesh;
				}
			}
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

IRuMesh *RuEntity_FindFirstSkinnableMesh(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	// Initialize entity queue
	CRuQueue<CRuEntity *> &entityQueue = RuHelperInternal_LockEntityQueue();
	entityQueue.Queue(entity);

	// Perform breadth-first traversal of entity tree
	while(entityQueue.IsEmpty() == FALSE)
	{
		CRuEntity *curEntity = entityQueue.Dequeue();

		// Check if entity is a renderable
		if(curEntity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
		{
			IRuEntity_Renderable *renderable = static_cast<IRuEntity_Renderable *>(curEntity);

			for(INT32 i = 0; i < renderable->GetNumMeshes(); ++i)
			{
				IRuMesh *mesh;
				IRuMaterial *material;

				if(renderable->GetMesh(i, &mesh, &material))
				{
					IRuMorphTarget *morphTarget = mesh->GetMorphTarget(0);

					if(morphTarget)
					{
						if(morphTarget->GetBlendIndex() && morphTarget->GetBlendWeight())
						{
							RuHelperInternal_UnlockEntityQueue();
							return mesh;
						}
					}
				}
			}
		}

		// Iterate through child entities
		CRuEntity *curChild = curEntity->GetFirstChild();
		while(curChild)
		{
			// Push child onto stack
			entityQueue.Queue(curChild);

			// Advance to next sibling
			curChild = curChild->GetNextSibling();
		}
	}

	RuHelperInternal_UnlockEntityQueue();
	return NULL;
}

// ************************************************************************************************************************************************************

BOOL RuEntity_UpdateSkinShaders(CRuEntity *entity)
{
	// Debug assertion
	assert(entity);

	if(entity->GetOption(ruENTITYOPTION_SKINNED))
	{
		if(entity->GetType().IsSubClassOf(IRuEntity_Renderable::Type()))
		{
			IRuEntity_Renderable *renderableEntity = static_cast<IRuEntity_Renderable *>(entity);

			for(INT32 i = 0; i < renderableEntity->GetNumMeshes(); ++i)
			{
				IRuMesh *mesh = NULL;
				IRuMaterial *material = NULL;

				if(renderableEntity->GetMesh(i, &mesh, &material) && mesh && material)
				{
					material->SetSkinned(TRUE);
				}
			}
		}
	}

	CRuEntity *curChild = entity->GetFirstChild();
	while(curChild)
	{
		// Recurse to child entities
		RuEntity_UpdateSkinShaders(curChild);

		// Advance to next sibling
		curChild = curChild->GetNextSibling();
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
