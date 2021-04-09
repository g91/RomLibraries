/*!
	@file RuCrypto_MD5.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUCRYPTO_MD5_
#define _RUCRYPTO_MD5_

// Base types
#include "../../Core/Type/RuType_Base.h"
#include "../../Core/Type/RuType_Object.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuCrypto_MD5
{
public:
	char*						CalculateMD5Digest(const char *text);
	char*						CalculateMD5Digest(const BYTE *text, INT32 textLen);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
