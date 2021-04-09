#pragma once

// Base types
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Type/RuType_Event.h"

// Memory manager
#include "../../Core/Memory/RuMemoryManager.h"

// Base exceptions
#include "../../Core/Exception/RuExceptions.h"

// Base collections
#include "../../Core/Collection/RuCollections.h"
#include "../../Core/Collection/RuCollectionsHash.h"
#include "../../Core/Collection/RuCollectionsLRU.h"
#include "../../Core/Collection/RuCollection_TernaryStringTree.h"

// Archive and stream IO
#include "../../Core/Stream/RuArchiveIO.h"
#include "../../Core/Stream/RuStreamIO.h"

#ifdef KALYDO
#include <vector>
#endif //KALYDO

#pragma managed(push, off)

// ************************************************************************************************************************************************************

#define ruCLASSGUID_DECLARE																										\
	public:																														\
		virtual const CRuGUID&					GetClassGUID() const;															\
		static const CRuGUID&					ClassGUID();													

#define ruCLASSGUID_DEFINE(name, long0, long1, long2, long3)																	\
	const CRuGUID &name::GetClassGUID() const { return ClassGUID(); }															\
	const CRuGUID &name::ClassGUID() { static CRuGUID classGUID(long0, long1, long2, long3); return classGUID; }

// ************************************************************************************************************************************************************

#define ruOBJECTMODULE_DEFINE(moduleName, className)														\
	class moduleName : public IRuObjectModule																\
	{																										\
	public:																									\
		virtual									~moduleName() { }											\
																											\
		virtual const CRuGUID&					GetClassGUID() const { return className::ClassGUID(); }		\
		virtual IRuObject*						CreateObject() const { return ruNEW className(); }			\
	};

// ************************************************************************************************************************************************************

#define ruSAFE_ADDREF(x) if(x) { x->AddRef(); }
#define ruSAFE_RELEASE(x) if(x) { x->Release(); x = NULL; }
#define ruSAFE_DELETE(x) if(x) { delete x; x = NULL; }
#define ruSAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = NULL; }

// ************************************************************************************************************************************************************

extern BOOL					RuReadGUID(IRuStream *stream, CRuGUID &guidOut);
extern BOOL					RuReadAndValidateGUID(IRuStream *stream, const CRuGUID &expectedGUID);
extern BOOL					RuReadAndValidateGUID(IRuStream *stream, const CRuGUID &expectedGUID0, const CRuGUID &expectedGUID1);

// ************************************************************************************************************************************************************

class IRuObject;

// ************************************************************************************************************************************************************

typedef		boost::variant<	INT8,
							INT16,
							INT32,
							INT64,
							UINT8,
							UINT16,
							UINT32,
							UINT64,
							REAL,
							POINTER,
							CRuColor4,
							IRuObject *,
							const char * > RuPropertyType;

// ************************************************************************************************************************************************************

class RuPropertyTypeSerializer : public boost::static_visitor<>
{
protected:
	IRuStream*								m_targetStream;

public:
											RuPropertyTypeSerializer(IRuStream *targetStream);

	RuPropertyType							SerializeFrom();

	// Output visitor
	void									operator() (INT8 &value) const;
	void									operator() (INT16 &value) const;
	void									operator() (INT32 &value) const;
	void									operator() (INT64 &value) const;
	void									operator() (UINT8 &value) const;
	void									operator() (UINT16 &value) const;
	void									operator() (UINT32 &value) const;
	void									operator() (UINT64 &value) const;
	void									operator() (REAL &value) const;
	void									operator() (POINTER &value) const;
	void									operator() (CRuColor4 &value) const;
	void									operator() (IRuObject *value) const;
	void									operator() (const char *value) const;
};

// ************************************************************************************************************************************************************

class RuObjectEvent_Cloned_Args : public RuEventArgs
{
public:
	IRuObject*							m_object;						//!< Object being cloned

	RuObjectEvent_Cloned_Args(IRuObject *obj)
	:	m_object(obj)
	{
	}
};

// ************************************************************************************************************************************************************

class RuObjectEvent_Dispose_Args : public RuEventArgs
{
public:
	IRuObject*							m_object;						//!< Object being disposed

	RuObjectEvent_Dispose_Args(IRuObject *obj)
	:	m_object(obj)
	{
	}
};

// ************************************************************************************************************************************************************

/*!
	Base object specification for the system object model.

	@version 2004/08/01
*/
class IRuObject : public IRuReferenceCounted
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	char*									m_resourceName;									//!< Resource name of the object
	char*									m_objectName;									//!< Name of the object
	CRuGUID									m_guid;											//!< The object's run-time GUID
	DWORD									m_hashCode;										//!< Hash code of the object's memory address

	CRuEvent								m_event_Cloned;									//!< Entity has been cloned event
	CRuEvent								m_event_Dispose;								//!< Entity destroy event

	CRuTernaryStringTree<RuPropertyType>	m_objectProperties;								//!< Object properties

public:
											IRuObject();
	virtual									~IRuObject();

	virtual void							Dispose();

	// Resource and object name access
	const char*								GetResourceName() const;
	void									SetResourceName(const char *resourceName);

	const char*								GetObjectName() const;
	void									SetObjectName(const char *objectName);

	DWORD									HashCode();

	// Events
	CRuEvent&								Event_Cloned();
	CRuEvent&								Event_Dispose();

	// Property access
	virtual BOOL							GetProperty(const char *propertyName, RuPropertyType &propertyValueOut);
	virtual BOOL							SetProperty(const char *propertyName, const RuPropertyType &propertyValue);
	virtual BOOL							CopyPropertiesFrom(IRuObject *obj);

	// Overridable interface
	virtual const CRuGUID&					GetGUID() const;								//!< Returns the unique identifier for this object
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);			//!< Clones the object

	// Serialization interface
	virtual BOOL							SerializeFrom(IRuStream *stream);				//!< Serializes the object from the stream
	virtual BOOL							SerializeTo(IRuStream *stream);					//!< Serializes the object to the stream

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

#ifdef KALYDO
	// Get all dependencies so they can be downloaded too. Returns whether they need to be loaded.
	virtual BOOL							GetDependencies(std::vector<CRuString>& dependencies) { return TRUE; }
#endif //KALYDO
protected:
	BOOL									ClearPropertiesCallback(const char *key, void *data);
};

// ************************************************************************************************************************************************************

class IRuObjectModule
{
public:
	RUAPI virtual									~IRuObjectModule() { }

	RUAPI virtual const CRuGUID&					GetClassGUID() const = 0;						// Gets the class GUID of the object this module creates
	RUAPI virtual IRuObject*						CreateObject() const = 0;						// Creates an object
};

// ************************************************************************************************************************************************************

class CRuObjectFactory
{
protected:
	CRuHashMap<CRuGUID, IRuObjectModule *>*			m_moduleHash;									//!< Object construction modules

	// Pooled object
	CRuHashMap<CRuGUID, CRuStack<IRuObject *> *>*	m_objectPools;									//!< Object pools

public:
	RUAPI 											CRuObjectFactory();
	RUAPI 											~CRuObjectFactory();

	RUAPI void										AttachModule(IRuObjectModule *module);			// Attaches specified module
	RUAPI IRuObject*								CreateObject(const CRuGUID &classID);			// Creates an object from the class ID
	RUAPI IRuObject*								CreateObject(IRuStream *stream);				// Creates an object from the stream

	RUAPI IRuObject*								CreatePooledObject(const CRuGUID &classID);
	RUAPI void										DestroyPooledObject(IRuObject *obj);

	BOOL											ClearModuleDictionary();
	BOOL											FreeObjectPools();

protected:
	CRuStack<IRuObject *>&							GetObjectPool(const CRuGUID &classID);

	BOOL											ClearModuleDictionaryCallback(const void *key, void *data);
	BOOL											FreeObjectPoolsCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

inline IRuObject::IRuObject()
:	m_resourceName(NULL),
	m_objectName(NULL)
{
	m_hashCode = RuGlobalHashGen().GetHashCode(reinterpret_cast<PTRVALUE>(this));
}

inline IRuObject::~IRuObject()
{
	// Free the resource name
	ruSAFE_DELETE_ARRAY(m_resourceName);

	// Free the object name
	ruSAFE_DELETE_ARRAY(m_objectName);

	// Iterate through property values and perform clean-up
	m_objectProperties.ForAllElements(boost::bind(&IRuObject::ClearPropertiesCallback, this, _1, _2));
}

/*!
	Gets the object's system name. This is usually the resource path from which the object
	is loaded.

	@return The object's system name, or an empty string if the object is unnamed.
*/
inline const char *IRuObject::GetResourceName() const
{
	if(m_resourceName)
	{
		return m_resourceName;
	}

	return "";
}

/*!
	Sets the object's system name. Defined as the resource path from which the object
	is loaded. If the object is not directly loaded as a resource, this method should not
	be called.

	@param resourceName The name that this entity will assume.
*/
inline void IRuObject::SetResourceName(const char *resourceName)
{
	ruSAFE_DELETE_ARRAY(m_resourceName);

	if(resourceName)
	{
		m_resourceName = ruNEW char [strlen(resourceName) + 1];
		strcpy(m_resourceName, resourceName);
	}
	else
	{
		m_resourceName = NULL;
	}
}

inline const char *IRuObject::GetObjectName() const
{
	if(m_objectName)
	{
		return m_objectName;
	}

	return "";
}

inline void IRuObject::SetObjectName(const char *objectName)
{
	ruSAFE_DELETE_ARRAY(m_objectName);

	if(objectName)
	{
		m_objectName = ruNEW char [strlen(objectName) + 1];
		strcpy(m_objectName, objectName);
	}
	else
	{
		m_objectName = NULL;
	}
}

inline DWORD IRuObject::HashCode()
{
	return m_hashCode;
}

inline CRuEvent &IRuObject::Event_Cloned()
{
	return m_event_Cloned;
}

inline CRuEvent &IRuObject::Event_Dispose()
{
	return m_event_Dispose;
}

inline BOOL IRuObject::GetProperty(const char *propertyName, RuPropertyType &propertyValueOut)
{
	return m_objectProperties.Get(propertyName, propertyValueOut);
}

inline BOOL IRuObject::SetProperty(const char *propertyName, const RuPropertyType &propertyValue)
{
	// Retrieve old value in property
	RuPropertyType oldPropertyValue;
	if(m_objectProperties.Get(propertyName, oldPropertyValue))
	{
		const char * const *oldValueAsString = boost::get<const char *>(&oldPropertyValue);
		if(oldValueAsString)
		{
			// Delete old value
			const char *derefString = *oldValueAsString;
			ruSAFE_DELETE_ARRAY(derefString);
		}
		else
		{
			IRuObject * const *oldValueAsObject = boost::get<IRuObject *>(&oldPropertyValue);
			if(oldValueAsObject)
			{
				// Decrement object reference count
				IRuObject *derefObject = *oldValueAsObject;
				ruSAFE_RELEASE(derefObject);
			}
		}
	}

	// Special handling for char* and IRuObject* types
	const char * const *valueAsString = boost::get<const char *>(&propertyValue);
	if(valueAsString)
	{
		// Make a copy of the string
		const char *derefString = *valueAsString;
		INT32 derefStringLength = static_cast<INT32>(strlen(derefString) + 1);
		char *stringCopy = ruNEW char [derefStringLength];
		memcpy(stringCopy, derefString, derefStringLength);

		// Store copy of string in property
		return m_objectProperties.Set(propertyName, stringCopy);
	}
	else
	{
		IRuObject * const *valueAsObject = boost::get<IRuObject *>(&propertyValue);
		if(valueAsObject)
		{
			// Increment object reference count
			IRuObject *derefObject = *valueAsObject;
			derefObject->AddRef();

			// Store object in property
			return m_objectProperties.Set(propertyName, derefObject);
		}
	}

	return m_objectProperties.Set(propertyName, propertyValue);
}

inline BOOL IRuObject::CopyPropertiesFrom(IRuObject *obj)
{
	CRuArrayList<const char *> propertyKeys;
	if(obj->m_objectProperties.EnumerateKeys(propertyKeys))
	{
		for(INT32 i = 0; i < propertyKeys.Count(); ++i)
		{
			RuPropertyType propertyValue;
			obj->m_objectProperties.Get(propertyKeys[i], propertyValue);

			IRuObject **propObjPtr = boost::get<IRuObject *>(&propertyValue);
			IRuObject *propObj = NULL;
			if(propObjPtr && (propObj = *propObjPtr))
			{
				IRuObject *propObjClone = propObj->Clone();

				if(propObjClone)
				{
//					assert(propObjClone);
					this->SetProperty(propertyKeys[i], propObjClone);
					ruSAFE_RELEASE(propObjClone);
				}
			}
			else
			{
				this->SetProperty(propertyKeys[i], propertyValue);
			}
		}

		return TRUE;
	}

	return FALSE;
}

inline const CRuGUID &IRuObject::GetGUID() const
{
	return m_guid;
}


/*!
	Clones the object.

	The clone function of any given subclass must call its parent class' Clone
	function to properly complete the cloning chain. In general, the following
	steps should be followed for cloning:

	1. If clonedObject is NULL, create a new instance of the class being cloned.
	   Otherwise, use the supplied clonedObject.
	2. Call parent class' Clone function using clonedObject as the parameter.
	3. Perform cloning operations on the class.
	4. Return pointer to clonedObject if successful, or NULL if the object cannot
	   be cloned or there is a failure.

	@param clonedObject Pointer to a cloned object previously created by a subclass.

	@return A pointer to the newly cloned object.
*/
inline IRuObject *IRuObject::Clone(IRuObject *clonedObject)
{
	// Create a new cloned object if necessary
	clonedObject = clonedObject ? clonedObject : ruNEW IRuObject();

	// Copy the resource name if it exists
	if(m_resourceName)
	{
		clonedObject->m_resourceName = ruNEW char [strlen(m_resourceName) + 1];
		strcpy(clonedObject->m_resourceName, m_resourceName);
	}

	// Copy the object name if it exists
	if(m_objectName)
	{
		clonedObject->m_objectName = ruNEW char [strlen(m_objectName) + 1];
		strcpy(clonedObject->m_objectName, m_objectName);
	}

	// Copy object properties
	clonedObject->CopyPropertiesFrom(this);

	// Trigger clone event
	RuObjectEvent_Cloned_Args clonedArgs(this);
	m_event_Cloned.Trigger(&clonedArgs);

	return clonedObject;
}

// ************************************************************************************************************************************************************

inline CRuObjectFactory &RuObjectFactory()
{
	static CRuObjectFactory objFactory;

	return objFactory;
};

extern UINT32								g_ruCounter_0;
extern UINT32								g_ruCounter_1;
extern UINT32								g_ruCounter_2;
extern UINT32								g_ruCounter_3;

// ************************************************************************************************************************************************************

#pragma managed(pop)
