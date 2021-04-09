#include "../../Core/Type/RuType_Object.h"

// Stream IO helper
#include "../../Helper/RuHelper_StreamIO.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_ROOTCLASS(IRuObject)
ruHEAP_DEFINE_ROOTCLASS(IRuObject, "IRuObject")
ruCLASSGUID_DEFINE(IRuObject, 0x905982B3, 0xFF48498a, 0x93334D0A, 0x3CB24A39)

// ************************************************************************************************************************************************************

BOOL RuReadGUID(IRuStream *stream, CRuGUID &guidOut)
{
	// Read four UINT32 values from the stream to determine class GUID
	UINT32 guidValues[4];

	for(INT32 i = 0; i < 4; ++i)
	{
		if(stream->Read(&guidValues[i], sizeof(UINT32)) != sizeof(UINT32))
		{
			return FALSE;
		}
	}

	guidOut.Set(guidValues[0], guidValues[1], guidValues[2], guidValues[3]);

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuReadAndValidateGUID(IRuStream *stream, const CRuGUID &expectedGUID)
{
	// Read four UINT32 values from the stream to determine GUID
	UINT32 guidValues[4];

	for(INT32 i = 0; i < 4; ++i)
	{
		if(stream->Read(&guidValues[i], sizeof(UINT32)) != sizeof(UINT32))
		{
			return FALSE;
		}
	}

	CRuGUID readGUID(guidValues[0], guidValues[1], guidValues[2], guidValues[3]);

	if(readGUID != expectedGUID)
	{
		// Restore the stream position
		stream->Seek(-((INT32) readGUID.Size()), ruSSM_Current);

		return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

BOOL RuReadAndValidateGUID(IRuStream *stream, const CRuGUID &expectedGUID0, const CRuGUID &expectedGUID1)
{
	// Read four UINT32 values from the stream to determine GUID
	UINT32 guidValues[4];

	for(INT32 i = 0; i < 4; ++i)
	{
		if(stream->Read(&guidValues[i], sizeof(UINT32)) != sizeof(UINT32))
		{
			return FALSE;
		}
	}

	CRuGUID readGUID(guidValues[0], guidValues[1], guidValues[2], guidValues[3]);

	if(readGUID != expectedGUID0 && readGUID != expectedGUID1)
	{
		// Restore the stream position
		stream->Seek(-((INT32) readGUID.Size()), ruSSM_Current);

		return FALSE;
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

RuPropertyTypeSerializer::RuPropertyTypeSerializer(IRuStream *targetStream)
{
	m_targetStream = targetStream;
}

RuPropertyType RuPropertyTypeSerializer::SerializeFrom()
{
	// Read target ID
	INT32 typeID = 0;
	m_targetStream->Read(&typeID, sizeof(INT32));

	switch(typeID)
	{
		case 0:
			{
				INT8 value;
				m_targetStream->Read(&value, sizeof(INT8));
				return value;
			}

		case 1:
			{
				INT16 value;
				m_targetStream->Read(&value, sizeof(INT16));
				return value;
			}

		case 2:
			{
				INT32 value;
				m_targetStream->Read(&value, sizeof(INT32));
				return value;
			}

		case 3:
			{
				INT64 value;
				m_targetStream->Read(&value, sizeof(INT64));
				return value;
			}

		case 4:
			{
				UINT8 value;
				m_targetStream->Read(&value, sizeof(UINT8));
				return value;
			}

		case 5:
			{
				UINT16 value;
				m_targetStream->Read(&value, sizeof(UINT16));
				return value;
			}

		case 6:
			{
				UINT32 value;
				m_targetStream->Read(&value, sizeof(UINT32));
				return value;
			}

		case 7:
			{
				UINT64 value;
				m_targetStream->Read(&value, sizeof(UINT64));
				return value;
			}

		case 8:
			{
				REAL value;
				m_targetStream->Read(&value, sizeof(REAL));
				return value;
			}

		case 9:
			{
				return NULL;
			}

		case 10:
			{
				CRuColor4 value;
				m_targetStream->Read(&value.m_a, sizeof(REAL));
				m_targetStream->Read(&value.m_r, sizeof(REAL));
				m_targetStream->Read(&value.m_g, sizeof(REAL));
				m_targetStream->Read(&value.m_b, sizeof(REAL));
				return value;
			}

		case 11:
			{
				IRuObject *value = RuObjectFactory().CreateObject(m_targetStream);
				return value;
			}

		case 12:
			{
				char *value = NULL;
				::SerializeFrom(m_targetStream, &value);
				return value;
			}
	}

	return 0;
}

void RuPropertyTypeSerializer::operator() (INT8 &value) const
{
	// Write type ID
	INT32 typeID = 0;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(INT8));
}

void RuPropertyTypeSerializer::operator() (INT16 &value) const
{
	// Write type ID
	INT32 typeID = 1;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(INT16));
}

void RuPropertyTypeSerializer::operator() (INT32 &value) const
{
	// Write type ID
	INT32 typeID = 2;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(INT32));
}

void RuPropertyTypeSerializer::operator() (INT64 &value) const
{
	// Write type ID
	INT32 typeID = 3;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(INT64));
}

void RuPropertyTypeSerializer::operator() (UINT8 &value) const
{
	// Write type ID
	INT32 typeID = 4;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(UINT8));
}

void RuPropertyTypeSerializer::operator() (UINT16 &value) const
{
	// Write type ID
	INT32 typeID = 5;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(UINT16));
}

void RuPropertyTypeSerializer::operator() (UINT32 &value) const
{
	// Write type ID
	INT32 typeID = 6;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(UINT32));
}

void RuPropertyTypeSerializer::operator() (UINT64 &value) const
{
	// Write type ID
	INT32 typeID = 7;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(UINT64));
}

void RuPropertyTypeSerializer::operator() (REAL &value) const
{
	// Write type ID
	INT32 typeID = 8;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value, sizeof(REAL));
}

void RuPropertyTypeSerializer::operator() (POINTER &value) const
{
	// Write type ID
	INT32 typeID = 9;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// pointer values are meaningless...
}

void RuPropertyTypeSerializer::operator() (CRuColor4 &value) const
{
	// Write type ID
	INT32 typeID = 10;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	m_targetStream->Write(&value.m_a, sizeof(REAL));
	m_targetStream->Write(&value.m_r, sizeof(REAL));
	m_targetStream->Write(&value.m_g, sizeof(REAL));
	m_targetStream->Write(&value.m_b, sizeof(REAL));
}

void RuPropertyTypeSerializer::operator() (IRuObject *value) const
{
	// Write type ID
	INT32 typeID = 11;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	value->SerializeTo(m_targetStream);
}

void RuPropertyTypeSerializer::operator() (const char *value) const
{
	// Write type ID
	INT32 typeID = 12;
	m_targetStream->Write(&typeID, sizeof(INT32));

	// Write value
	::SerializeTo(m_targetStream, &value);
}

// ************************************************************************************************************************************************************

CRuObjectFactory::CRuObjectFactory()
{
	// Create the hash map for object modules
	m_moduleHash = ruNEW CRuHashMap<CRuGUID, IRuObjectModule *>(8192);

	// Create the hash map for object pools
	m_objectPools = ruNEW CRuHashMap<CRuGUID, CRuStack<IRuObject *> *>(512);
}

CRuObjectFactory::~CRuObjectFactory()
{
	// Destroy all object modules held by the factory
	ClearModuleDictionary();

	// Destroy hash map
	ruSAFE_DELETE(m_moduleHash);

	// Destroy all unused pooled objects
	FreeObjectPools();

	// Destroy object pool hash map
	ruSAFE_DELETE(m_objectPools);
}

void CRuObjectFactory::AttachModule(IRuObjectModule *module)
{
	CRuGUID classGUID = module->GetClassGUID();

	// If the factory already has the specified object module, ignore and return
	if(m_moduleHash->ContainsKey(module->GetClassGUID(), module->GetClassGUID().HashCode()))
	{
		return;
	}

	// Add object module to the hash map
	m_moduleHash->Add(module->GetClassGUID(), module->GetClassGUID().HashCode(), module);
}

IRuObject *CRuObjectFactory::CreateObject(const CRuGUID &classID)
{
	// Attempt to locate a proper module to create the object
	IRuObjectModule *module = NULL;
	if(m_moduleHash->Get(classID, classID.HashCode(), module))
	{
		// Return the created object
		return module->CreateObject();
	}

	return NULL;
}

IRuObject *CRuObjectFactory::CreateObject(IRuStream *stream)
{
	// Read class GUID
	CRuGUID classGUID;
	if(RuReadGUID(stream, classGUID) == FALSE)
	{
		return NULL;
	}

	// Rewind
	stream->Seek(-16, ruSSM_Current);

	// Attempt to locate a proper module to create the object
	IRuObjectModule *module = NULL;
	if(m_moduleHash->Get(classGUID, classGUID.HashCode(), module))
	{
		// Create the object
		IRuObject *object = module->CreateObject();
		
		if(object)
		{
			// Serialize object from stream
			object->SerializeFrom(stream);

			return object;
		}
	}

	return NULL;
}

IRuObject *CRuObjectFactory::CreatePooledObject(const CRuGUID &classID)
{
	CRuStack<IRuObject *> &objPool = GetObjectPool(classID);

	if(objPool.IsEmpty())
	{
		// Attempt to locate a proper module to create the object
		IRuObjectModule *module = NULL;
		if(m_moduleHash->Get(classID, classID.HashCode(), module))
		{
			// Return the created object
			return module->CreateObject();
		}

		return NULL;
	}

	return objPool.Pop();
}

void CRuObjectFactory::DestroyPooledObject(IRuObject *obj)
{
	CRuStack<IRuObject *> &objPool = GetObjectPool(obj->ClassGUID());
	objPool.Push(obj);
}

BOOL CRuObjectFactory::ClearModuleDictionary()
{
	// Release all modules held in the dictionary
	m_moduleHash->ForAllElements(boost::bind(&CRuObjectFactory::ClearModuleDictionaryCallback, this, _1, _2));

	// Clear all module entries
	m_moduleHash->Clear();

	return TRUE;
}

BOOL CRuObjectFactory::FreeObjectPools()
{
	// Release all object pools held in the dictionary
	m_objectPools->ForAllElements(boost::bind(&CRuObjectFactory::FreeObjectPoolsCallback, this, _1, _2));

	// Clear all object pool entries
	m_objectPools->Clear();

	return TRUE;
}

CRuStack<IRuObject *> &CRuObjectFactory::GetObjectPool(const CRuGUID &classID)
{
	CRuStack<IRuObject *> *objPool = NULL;

	if(m_objectPools->Get(classID, classID.HashCode(), objPool) == NULL)
	{
		// If the requested pool does not exist, create one
		objPool = ruNEW CRuStack<IRuObject *>();
		m_objectPools->Add(classID, classID.HashCode(), objPool);
	}

	return *objPool;
}

BOOL CRuObjectFactory::ClearModuleDictionaryCallback(const void *key, void *data)
{
	// Release module
	delete (*((IRuObjectModule **) data));

	return TRUE;
}

BOOL CRuObjectFactory::FreeObjectPoolsCallback(const void *key, void *data)
{
	CRuStack<IRuObject *> *stack = *reinterpret_cast<CRuStack<IRuObject *> **>(data);

	while(stack->IsEmpty() == FALSE)
	{
		IRuObject *obj = stack->Pop();
		ruSAFE_RELEASE(obj);
	}

	ruSAFE_DELETE(stack);

	return TRUE;
}

// ************************************************************************************************************************************************************

void IRuObject::Dispose()
{
	// Trigger the dispose event
	RuObjectEvent_Dispose_Args disposeArgs(this);
	m_event_Dispose.Trigger(&disposeArgs);
}

BOOL IRuObject::SerializeFrom(IRuStream *stream)
{
	// Read and validate class GUID
	if(RuReadAndValidateGUID(stream, ClassGUID(), this->GetClassGUID()) == FALSE)
	{
		return FALSE;
	}

	// Read version
	INT32 version = 1;
	stream->Read(&version, sizeof(INT32));

	switch(version)
	{
		case 1:
			{
				// Read object name
				INT32 objectNameLength;
				stream->Read(&objectNameLength, sizeof(INT32));

				m_objectName = ruNEW char [objectNameLength];
				stream->Read(m_objectName, objectNameLength);
			}

			break;

		case 2:
			{
				// Read object name
				INT32 objectNameLength;
				stream->Read(&objectNameLength, sizeof(INT32));

				m_objectName = ruNEW char [objectNameLength];
				stream->Read(m_objectName, objectNameLength);

				// Read number of properties
				INT32 numProperties = 0;
				stream->Read(&numProperties, sizeof(INT32));

				for(INT32 i = 0; i < numProperties; ++i)
				{
					// Read property name
					char *propertyName = NULL;
					::SerializeFrom(stream, &propertyName);

					// Read property value
					RuPropertyType propertyValue;
					::SerializeFrom(stream, &propertyValue);

					// Set property value
					m_objectProperties.Set(propertyName, propertyValue);

					ruSAFE_DELETE_ARRAY(propertyName);
				}
			}

			break;
	}

	return TRUE;
}

BOOL IRuObject::SerializeTo(IRuStream *stream)
{
	// Write class GUID
	stream->Write(ClassGUID().Data(), ClassGUID().Size());

	// Write version
	INT32 version = 2;
	stream->Write(&version, sizeof(INT32));

	// Write object name
	INT32 objectNameLength = (INT32) strlen(this->GetObjectName()) + 1;
	stream->Write(&objectNameLength, sizeof(INT32));
	stream->Write(this->GetObjectName(), objectNameLength);

	// Write object properties
	CRuArrayList<const char *> propertyKeys;
	if(m_objectProperties.EnumerateKeys(propertyKeys))
	{
		// Write number of properties
		INT32 numProperties = propertyKeys.Count();
		stream->Write(&numProperties, sizeof(INT32));

		for(INT32 i = 0; i < propertyKeys.Count(); ++i)
		{
			// Get property value
			RuPropertyType propertyValue;
			m_objectProperties.Get(propertyKeys[i], propertyValue);

			// Write property name
			::SerializeTo(stream, &(propertyKeys[i]));

			// Write property value
			::SerializeTo(stream, &propertyValue);
		}
	}

	return TRUE;
}

INT32 IRuObject::GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash)
{
	// Perform check against enumeration hash
	if(enumHash)
	{
		if(enumHash->ContainsKey(reinterpret_cast<PTRVALUE>(this)))
		{
			return -1;
		}
		else
		{
			enumHash->Add(reinterpret_cast<PTRVALUE>(this));
		}
	}

	// Calculate object footprint
	INT32 footprint = sizeof(*this);

	footprint += (m_resourceName ? (INT32)strlen(m_resourceName) + 1 : 0);
	footprint += (m_objectName ? (INT32)strlen(m_objectName) + 1 : 0);

	return footprint;
}

BOOL IRuObject::ClearPropertiesCallback(const char *key, void *data)
{
	RuPropertyType propertyValue = *((RuPropertyType *) data);

	const char * const *valueAsString = boost::get<const char *>(&propertyValue);
	if(valueAsString)
	{
		// Delete old value
		const char *derefString = *valueAsString;
		ruSAFE_DELETE_ARRAY(derefString);
	}
	else
	{
		IRuObject * const *valueAsObject = boost::get<IRuObject *>(&propertyValue);
		if(valueAsObject)
		{
			// Decrement object reference count
			IRuObject *derefObject = *valueAsObject;
			ruSAFE_RELEASE(derefObject);
		}
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

UINT32									g_ruCounter_0 = 0;
UINT32									g_ruCounter_1 = 0;
UINT32									g_ruCounter_2 = 0;
UINT32									g_ruCounter_3 = 0;

// ************************************************************************************************************************************************************

#pragma managed(pop)
