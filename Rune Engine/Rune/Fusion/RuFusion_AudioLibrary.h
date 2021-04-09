#ifndef _RUFUSION_AUDIOLIBRARY_H_
#define _RUFUSION_AUDIOLIBRARY_H_

// Base types
#include "../Symphony/RuSymphony_Base.h"
#include "../Core/Collection/RuCollectionsTree.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuFusion_AudioRegistry : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct TagDescriptor
	{
		CRuArrayList<CRuString *>							m_associatedAudio;			//!< Associated audio resource paths
	};

	CRuTernaryStringTree<TagDescriptor *>					m_tagDescriptors;

	CRuArrayList<const char *>*			m_enumeration;

public:
										CRuFusion_AudioRegistry();
	virtual								~CRuFusion_AudioRegistry();

	BOOL								AssociateAudio(const char *tag, const char *audioResource);
	const char*							GetAssociatedAudio(const char *tag);

	// Highly non-performant enumeration routines
	void								GetAudioTags(CRuArrayList<const char *> *tagList);

protected:
	BOOL								EnumerateAudioTagCallback(const char *key, void *data);
	BOOL								FreeAudioTagCallback(const char *key, void *data);
};

// ************************************************************************************************************************************************************

class CRuFusion_AudioLibrary : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	CRuTernaryStringTree<CRuFusion_AudioRegistry *>			m_audioRegistries;

	CRuArrayList<const char *>			m_enumeration;

public:
										CRuFusion_AudioLibrary();
	virtual								~CRuFusion_AudioLibrary();

	BOOL								AssociateAudio(const char *audioClass, const char *tag, const char *audioResource);
	const char*							GetAssociatedAudio(const char *audioClass, const char *audioTag);

	// Highly non-performant enumeration routines
	CRuArrayList<const char *>*			GetAudioClasses();
	CRuArrayList<const char *>*			GetAudioTags(const char *audioClass);

protected:
	BOOL								EnumerateAudioClassCallback(const char *key, void *data);
	BOOL								FreeAudioClassCallback(const char *key, void *data);
};

// ************************************************************************************************************************************************************

extern CRuFusion_AudioLibrary*			g_ruAudioLibrary;
extern CRuFusion_AudioLibrary*			RuFusion_GetAudioLibrary();

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
