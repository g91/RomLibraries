#include "../Collision/RuCollisionHierarchy_Simple.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuCollisionHierarchy_Simple, IRuCollisionHierarchy)
ruHEAP_DEFINE_SUBCLASS(CRuCollisionHierarchy_Simple, "CRuCollisionHierarchy_Simple", "IRuCollisionHierarchy")
ruCLASSGUID_DEFINE(CRuCollisionHierarchy_Simple, 0x026A3C69, 0xB1144f8d, 0xBF597609, 0xD29FD2F9)

// ************************************************************************************************************************************************************

CRuCollisionHierarchy_Simple::CRuCollisionHierarchy_Simple()
{
}

CRuCollisionHierarchy_Simple::~CRuCollisionHierarchy_Simple()
{
	for(INT32 i = 0; i < m_colObjects.Count(); ++i)
	{
		ruSAFE_RELEASE(m_colObjects[i]);
	}

	m_colObjects.Clear();
}

IRuObject *CRuCollisionHierarchy_Simple::Clone(IRuObject *clonedObject)
{
	// Not clone-able
	return NULL;
}

BOOL CRuCollisionHierarchy_Simple::SerializeFrom(IRuStream *stream)
{
	// Validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuCollisionHierarchy::SerializeFrom(stream);

	// Read bounds
	stream->Read(&m_bounds.Minima().x, sizeof(float));
	stream->Read(&m_bounds.Minima().y, sizeof(float));
	stream->Read(&m_bounds.Minima().z, sizeof(float));
	stream->Read(&m_bounds.Maxima().x, sizeof(float));
	stream->Read(&m_bounds.Maxima().y, sizeof(float));
	stream->Read(&m_bounds.Maxima().z, sizeof(float));

	// Read number of collision objects
	INT32 colObjCount = 0;
	stream->Read(&colObjCount, sizeof(INT32));

	for(INT32 i = 0; i < colObjCount; ++i)
	{
		IRuObject *object = RuObjectFactory().CreateObject(stream);

		if(object && object->GetType().IsSubClassOf(IRuCollisionObject::Type()))
		{
			m_colObjects.Add(static_cast<IRuCollisionObject *>(object));
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuCollisionHierarchy_Simple::SerializeTo(IRuStream *stream)
{
	const INT32 version = 1;

	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	stream->Write(&version, sizeof(INT32));

	// Chain serialization to base class
	IRuCollisionHierarchy::SerializeTo(stream);

	// Write bounds
	stream->Write(&m_bounds.Minima().x, sizeof(float));
	stream->Write(&m_bounds.Minima().y, sizeof(float));
	stream->Write(&m_bounds.Minima().z, sizeof(float));
	stream->Write(&m_bounds.Maxima().x, sizeof(float));
	stream->Write(&m_bounds.Maxima().y, sizeof(float));
	stream->Write(&m_bounds.Maxima().z, sizeof(float));

	// Write number of collision objects
	INT32 colObjCount = m_colObjects.Count();
	stream->Write(&colObjCount, sizeof(INT32));

	// Write each collision object
	for(INT32 i = 0; i < colObjCount; ++i)
	{
		m_colObjects[i]->SerializeTo(stream);
	}

	return TRUE;
}

const CRuAABB &CRuCollisionHierarchy_Simple::GetBounds() const
{
	return m_bounds;
}

BOOL CRuCollisionHierarchy_Simple::QueryCollision(CRuCollisionQuery *colQuery)
{
	for(INT32 i = 0; i < m_colObjects.Count(); ++i)
	{
		BOOL result = m_colObjects[i]->QueryCollision(colQuery);
		if(!result)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CRuCollisionHierarchy_Simple::AddCollisionObject(IRuCollisionObject *colObject)
{
	// Make sure no collision object with the same key exists
	for(INT32 i = 0; i < m_colObjects.Count(); ++i)
	{
		if(colObject->GetCollisionObjectKey() == m_colObjects[i]->GetCollisionObjectKey())
		{
			return FALSE;
		}
	}

	// Add collision object to list
	ruSAFE_ADDREF(colObject);
	m_colObjects.Add(colObject);

	// Calculate bounds
	CalculateBounds();

	return TRUE;
}

IRuCollisionObject *CRuCollisionHierarchy_Simple::GetCollisionObject(INT32 objKey)
{
	for(INT32 i = 0; i < m_colObjects.Count(); ++i)
	{
		if(objKey == m_colObjects[i]->GetCollisionObjectKey())
		{
			return m_colObjects[i];
		}
	}

	return NULL;
}

BOOL CRuCollisionHierarchy_Simple::RemoveCollisionObject(INT32 objKey)
{
	for(INT32 i = 0; i < m_colObjects.Count(); ++i)
	{
		if(objKey == m_colObjects[i]->GetCollisionObjectKey())
		{
			// Remove collision object
			ruSAFE_RELEASE(m_colObjects[i]);
			m_colObjects.RemoveAt(i);

			// Calculate bounds
			CalculateBounds();

			return TRUE;
		}
	}

	return FALSE;
}

void CRuCollisionHierarchy_Simple::CalculateBounds()
{
	if(m_colObjects.Count() > 0)
	{
		m_bounds = m_colObjects[0]->GetBounds();

		for(INT32 i = 1; i < m_colObjects.Count(); ++i)
		{
			m_bounds.Merge(m_colObjects[i]->GetBounds());
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
