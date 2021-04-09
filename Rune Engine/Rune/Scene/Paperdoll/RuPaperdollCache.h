/*!
	@file RuPaperdollCache.h

	Copyright (c) 2004-2007 All rights reserved
*/

#ifndef _RUPAPERDOLLCACHE_H_
#define _RUPAPERDOLLCACHE_H_

#include "../../Core/Crypto/RuCrypto_MD5.h"
#include "../../Scene/Paperdoll/RuEntityPaperdoll.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@version 2007.09.26
*/
class CRuPaperdollCache : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ModelDesc
	{
		REAL								m_timeSinceLastUse;
		CRuModel*							m_model;
	};

	CRuCrypto_MD5							m_md5Gen;							//!< MD5 digest generator
	char*									m_cachePath;						//!< Cache root path (relative to resource path)

	CRuTernaryStringTree<ModelDesc *>		m_paperdollTree;					//!< String tree of cached & loaded paperdolls

	REAL									m_elapsedTimeSinceLastUpdate;

public:
											CRuPaperdollCache();
	virtual									~CRuPaperdollCache();

	void									SetCachePath(const char *cachePath);

	BOOL									CachePaperdoll(const char *rootName, CRuPaperdoll *paperdoll);
	CRuModel*								LoadCachedPaperdoll(const char *rootName, CRuPaperdoll *paperdoll);
	BOOL									LoadCachedPaperdoll(const char *rootName, CRuPaperdoll *paperdoll, CRuModel **modelOut);

	void									Update(REAL elapsedTime);

protected:
	BOOL									ClearPaperdollTreeCallback(const void *key, void *data);
	BOOL									GCPaperdollTreeCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
