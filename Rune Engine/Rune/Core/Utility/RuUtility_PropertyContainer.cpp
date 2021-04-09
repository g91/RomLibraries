/*!
	@file RuUtility_PropertyContainer.cpp

	Copyright (c) 2004-2006 All rights reserved
*/

#include "../../Core/Utility/RuUtility_PropertyContainer.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuPropertyContainer, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuPropertyContainer, "CRuPropertyContainer", "IRuObject")
ruCLASSGUID_DEFINE(CRuPropertyContainer, 0xCD3D97E4, 0x78E24ec9, 0xA98A397, 0x984D1BEB)

// ************************************************************************************************************************************************************

CRuPropertyContainer::CRuPropertyContainer()
:	m_propertyHash(NULL)
{
	m_propertyHash = ruNEW CRuStringHash<RuPropertyType>(256);
}

CRuPropertyContainer::~CRuPropertyContainer()
{
	delete m_propertyHash;
}

BOOL CRuPropertyContainer::GetProperty(const char *propertyName, RuPropertyType &propertyValueOut)
{
	return m_propertyHash->Get(propertyName, propertyValueOut);
}

BOOL CRuPropertyContainer::SetProperty(const char *propertyName, const RuPropertyType &propertyValue)
{
	return m_propertyHash->Set(propertyName, propertyValue);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
