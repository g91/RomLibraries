#include "../../Scene/PathEngineVis/RuEntity_PathEngineVis.h"
#include "../../Scene/Gizmo/RuGizmo_Box.h"
#include "../../Scene/Widget/RuEntity_Widget_Flag.h"
#include "../../Scene/Widget/RuEntity_Widget_Line.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_PathEngineVis, IRuEntity_Container)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_PathEngineVis, "CRuEntity_PathEngineVis", "IRuEntity_Container")
ruCLASSGUID_DEFINE(CRuEntity_PathEngineVis, 0x44F28481, 0x23F947b7, 0x9564CEC1, 0x0221DE02)

// ************************************************************************************************************************************************************

CRuEntity_PathEngineVis::CRuEntity_PathEngineVis()
:	m_fontType(NULL),
	m_sourcePathHierarchy(NULL),
	m_selectedNodeKey(0xFFFFFFFF),
	m_pathDisplayEntity(NULL)
{
	// Initialize model queue
	m_modelQueue = ruNEW CRuLRUQueue<CRuGUID, CRuEntity *>(128, 1024);
}

CRuEntity_PathEngineVis::~CRuEntity_PathEngineVis()
{
	// Flush collision object queue
	FlushModelQueue();

	// Destroy collision object queue
	delete m_modelQueue;

	ruSAFE_RELEASE(m_sourcePathHierarchy);
	ruSAFE_RELEASE(m_pathDisplayEntity);
}

void CRuEntity_PathEngineVis::SetFontType(CRuFontType *fontType)
{
	m_fontType = fontType;
}

void CRuEntity_PathEngineVis::SetSourcePathHierarchy(IRuPathHierarchy_Base *sourcePathHierarchy)
{
	if(sourcePathHierarchy)
	{
		// Reset source path hierarchy
		sourcePathHierarchy->AddRef();
		ruSAFE_RELEASE(m_sourcePathHierarchy);
		m_sourcePathHierarchy = sourcePathHierarchy;
	}
	else
	{
		ruSAFE_RELEASE(m_sourcePathHierarchy);
	}

	// Clear all existing cache
	FlushModelQueue();
}

void CRuEntity_PathEngineVis::SetHighlightRay(const CRuRay3 &highlightRay)
{
	m_highlightRay = highlightRay;
}

INT32 CRuEntity_PathEngineVis::GetNodeKeyAtCursor(const CRuRay3 &ray)
{
	CRuStack<CRuEntity *> entityStack;

	for(INT32 i = 0; i < m_visibleObjects.Count(); ++i)
	{
		entityStack.Push(m_visibleObjects[i]);
	}

	CRuEntity *selectedEntity = NULL;

	while(!entityStack.IsEmpty())
	{
		CRuEntity *curEntity = entityStack.Pop();

		if(curEntity->GetType() == CRuEntity_Widget_Flag::Type())
		{
			if(Intersects(m_highlightRay, curEntity->GetWorldBounds()))
			{
				selectedEntity = curEntity;
			}
		}
		else if(curEntity->GetType() == CRuGizmo_Box::Type())
		{
			if(Intersects(m_highlightRay, curEntity->GetWorldBounds()))
			{
				selectedEntity = curEntity;
			}
		}
		else if(curEntity->GetType().IsSubClassOf(IRuEntity_Container::Type()))
		{
			IRuEntity_Container *container = static_cast<IRuEntity_Container *>(curEntity);

			// Push all entities held by the container onto the entity stack
			for(INT32 i = 0, numEntities = container->GetNumEntities(); i < numEntities; ++i)
			{
				entityStack.Push(container->GetEntity(i));
			}
		}
	}

	if(selectedEntity)
	{
		INT32 selectedNodeKey = reinterpret_cast<INT32>(selectedEntity->GetTag(ruENTITYTAG_USER_0));
		return selectedNodeKey;
	}

	return 0xFFFFFFFF;
}

IRuObject *CRuEntity_PathEngineVis::Clone(IRuObject *clonedObject)
{
	// This object cannot be cloned
	return NULL;
}

BOOL CRuEntity_PathEngineVis::ResetStates()
{
	return TRUE;
}

BOOL CRuEntity_PathEngineVis::Update(REAL elapsedTime)
{
	CRuStack<CRuEntity *> entityStack;

	// Update visible graphs and push onto working stack
	for(INT32 i = 0; i < m_visibleObjects.Count(); ++i)
	{
		if(m_visibleObjects[i])
		{
			m_visibleObjects[i]->Update(elapsedTime);
			entityStack.Push(m_visibleObjects[i]);
		}
	}
/*
	// Find the currently highlighted widget
	CRuEntity_Widget_Flag *selectedFlagWidget = NULL;

	while(!entityStack.IsEmpty())
	{
		CRuEntity *curEntity = entityStack.Pop();

		if(curEntity->GetType() == CRuEntity_Widget_Flag::Type())
		{
			if(Intersects(m_highlightRay, curEntity->GetWorldBounds()))
			{
				selectedFlagWidget = static_cast<CRuEntity_Widget_Flag *>(curEntity);
				static_cast<CRuEntity_Widget_Flag *>(curEntity)->SetHighlight(TRUE);
			}
			else
			{
				static_cast<CRuEntity_Widget_Flag *>(curEntity)->SetHighlight(FALSE);
			}
		}
		else if(curEntity->GetType().IsSubClassOf(IRuEntity_Container::Type()))
		{
			IRuEntity_Container *container = static_cast<IRuEntity_Container *>(curEntity);

			// Push all entities held by the container onto the entity stack
			for(INT32 i = 0, numEntities = container->GetNumEntities(); i < numEntities; ++i)
			{
				entityStack.Push(container->GetEntity(i));
			}
		}
	}

	CRuVector3 offset(0.0f, 10.0f, 0.0f);

	if(selectedFlagWidget)
	{
		INT32 selectedNodeKey = reinterpret_cast<INT32>(selectedFlagWidget->GetTag(ruENTITYTAG_USER_0));

		if(selectedNodeKey != m_selectedNodeKey)
		{
			m_selectedNodeKey = selectedNodeKey;

			INT32 graphKey = RuPathEngine_GraphKeyFromNodeKey(m_selectedNodeKey);
			INT32 nodeID = RuPathEngine_NodeIDFromNodeKey(m_selectedNodeKey);

			IRuPathGraph_Base *pathGraph = m_sourcePathHierarchy->GetPathGraph(graphKey);
			const CRuPathNode *pathNode = pathGraph->GetPathNode(nodeID);

			// Create new container
			ruSAFE_DETACH(m_pathDisplayEntity);
			ruSAFE_RELEASE(m_pathDisplayEntity);
			m_pathDisplayEntity = ruNEW CRuFrame();

			for(INT32 j = 0; j < pathNode->Neighbors().Count(); ++j)
			{
				CRuEntity_Widget_Line *lineWidget = ruNEW CRuEntity_Widget_Line();

				lineWidget->SetOption(ruENTITYOPTION_BILLBOARD, ruBILLBOARDTYPE_AXIAL);

				CRuVector3 v0 = pathNode->Position() + offset;
				CRuVector3 v1 = pathGraph->GetPathNode(pathNode->Neighbors()[j])->Position() + offset;

				lineWidget->SetLine(v0, v1, FALSE, TRUE, 0xFF00FFFF);

				static_cast<CRuFrame *>(m_pathDisplayEntity)->AddEntity(lineWidget);

				ruSAFE_RELEASE(lineWidget);
			}

			INT32 linkID = pathGraph->FindPathNodeLink(nodeID);

			if(linkID >= 0)
			{
				RuPathNodeLinkType linkType;
				REAL linkDistance;
				INT32 linkData;
				pathGraph->GetPathNodeLink(linkID, nodeID, linkType, linkDistance, linkData);

				if(linkType == ruPATHNODELINKTYPE_FIXED)
				{
					INT32 linkGraphKey = RuPathEngine_GraphKeyFromNodeKey(linkData);
					INT32 linkNodeID = RuPathEngine_NodeIDFromNodeKey(linkData);

					IRuPathGraph_Base *linkPathGraph = m_sourcePathHierarchy->GetPathGraph(linkGraphKey);

					if(linkPathGraph)
					{
						const CRuPathNode *linkPathNode = linkPathGraph->GetPathNode(linkNodeID);

						CRuEntity_Widget_Line *lineWidget = ruNEW CRuEntity_Widget_Line();

						lineWidget->SetOption(ruENTITYOPTION_BILLBOARD, ruBILLBOARDTYPE_AXIAL);

						CRuVector3 v0 = pathNode->Position() + offset;
						CRuVector3 v1 = linkPathNode->Position() + offset;

						lineWidget->SetLine(v0, v1, FALSE, TRUE, 0xFFFF0000);

						static_cast<CRuFrame *>(m_pathDisplayEntity)->AddEntity(lineWidget);

						ruSAFE_RELEASE(lineWidget);
					}
				}
			}
		}
	}

	if(m_pathDisplayEntity)
	{
		this->AddChild(m_pathDisplayEntity);
	}
*/
	return IRuEntity_Container::Update(elapsedTime);
}

INT32 CRuEntity_PathEngineVis::GetNumEntities()
{
	return m_visibleObjects.Count();
}

CRuEntity *CRuEntity_PathEngineVis::GetEntity(INT32 index)
{
	return m_visibleObjects[index];
}

BOOL CRuEntity_PathEngineVis::QueryCollision(CRuCollisionQuery *colQuery)
{
	return FALSE;
}

void CRuEntity_PathEngineVis::SetCamera(CRuCamera *viewCamera)
{
	// Clear existing visible list
	this->RemoveAllChildren();
	m_visibleObjects.Clear();

	// Abort of there is no path hierarchy selected
	if(m_sourcePathHierarchy == NULL)
		return;

	// Enumerate graphs within the display region
	CRuSphere displayBounds(viewCamera->GetCameraPosition(), 960.0f);
	CRuArrayList<INT32> pathGraphKeys;

	m_sourcePathHierarchy->EnumeratePathGraphKeys(displayBounds, pathGraphKeys);

	// Instance displayed graphs
	const CRuPlane4 *frustumPlanes = viewCamera->GetFrustumPlanes();

	for(INT32 i = 0; i < pathGraphKeys.Count(); ++i)
	{
		IRuPathGraph_Base *pathGraph = m_sourcePathHierarchy->GetPathGraph(pathGraphKeys[i]);

		// Test visibility against camera
		if(IntersectsFrustum(frustumPlanes, pathGraph->Bounds()))
		{
			// Instance the path graph model
			CRuEntity *model = InstanceModel(pathGraph);

			// Append to visible list
			if(model)
			{
				this->AddChild(model);
				m_visibleObjects.Add(model);
			}
		}
	}
}

CRuEntity *CRuEntity_PathEngineVis::InstanceModel(IRuPathGraph_Base *pathGraph)
{
	CRuGUID objKey = pathGraph->GetGUID();
	DWORD objKeyHash = objKey.HashCode();

	CRuEntity *graphEntity = NULL;

	if(m_modelQueue->Get(objKey, objKeyHash, graphEntity) == FALSE)
	{
		// Reclaim space
		CRuEntity *reclaimedGraphEntity;
		while(m_modelQueue->Reclaim(objKey, objKeyHash, 1, reclaimedGraphEntity))
		{
			ruSAFE_RELEASE(reclaimedGraphEntity);
		}

		// Create path graph visualizer
		graphEntity = ruNEW CRuEntity_PathGraphVis();
		static_cast<CRuEntity_PathGraphVis *>(graphEntity)->SetPathGraph(pathGraph);

		// Add the model to the queue
		m_modelQueue->Add(objKey, objKeyHash, graphEntity, 1);
	}

	return graphEntity;
}

BOOL CRuEntity_PathEngineVis::FlushModelQueue()
{
	if(m_modelQueue)
	{
		// Iterate through all instanced entities
		m_modelQueue->ForAllElements(FlushModelQueueCallback, this);

		// Clear instanced entity queue
		m_modelQueue->Clear();
	}

	return TRUE;
}

BOOL CRuEntity_PathEngineVis::FlushModelQueueCallback(const void *key, const void *data, void *userData)
{
	CRuModel *model = *((CRuModel **) data);

	model->DetachFromParent();
	ruSAFE_RELEASE(model);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
