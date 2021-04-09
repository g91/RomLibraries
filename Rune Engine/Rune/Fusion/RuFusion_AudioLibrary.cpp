#include "../Fusion/RuFusion_AudioLibrary.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuFusion_AudioRegistry, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_AudioRegistry, "CRuFusion_AudioRegistry", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_AudioRegistry, 0xD0AE6B60, 0x3A4840bc, 0xB7E63E07, 0x8A5BD167)

ruRTTI_DEFINE_SUBCLASS(CRuFusion_AudioLibrary, IRuObject)
ruHEAP_DEFINE_SUBCLASS(CRuFusion_AudioLibrary, "CRuFusion_AudioLibrary", "IRuObject")
ruCLASSGUID_DEFINE(CRuFusion_AudioLibrary, 0x7EB54FEC, 0xE74947d1, 0x9796F019, 0xB6803AAE)

// ************************************************************************************************************************************************************

CRuFusion_AudioRegistry::CRuFusion_AudioRegistry()
{
}

CRuFusion_AudioRegistry::~CRuFusion_AudioRegistry()
{
	// Clear off the tree...
	m_tagDescriptors.ForAllElements(boost::bind(&CRuFusion_AudioRegistry::FreeAudioTagCallback, this, _1, _2));
}

BOOL CRuFusion_AudioRegistry::AssociateAudio(const char *tag, const char *audioResource)
{
	TagDescriptor *tagDesc = NULL;

	// If the specified tag does not exist, create it
	if(m_tagDescriptors.ContainsKey(tag) == FALSE)
	{
		tagDesc = ruNEW TagDescriptor();
		m_tagDescriptors.Add(tag, tagDesc);
	}
	else
	{
		tagDesc = m_tagDescriptors.Get(tag);
	}

	tagDesc->m_associatedAudio.Add(ruNEW CRuString(audioResource));

	return TRUE;
}

const char *CRuFusion_AudioRegistry::GetAssociatedAudio(const char *tag)
{
	if(m_tagDescriptors.ContainsKey(tag))
	{
		TagDescriptor *tagDesc = m_tagDescriptors.Get(tag);

		if(tagDesc->m_associatedAudio.Count() > 0)
		{
			INT32 audioIndex = RuGlobalMTRand().GenerateUINT32() % tagDesc->m_associatedAudio.Count();

			return *(tagDesc->m_associatedAudio[audioIndex]);
		}
	}

	return NULL;
}

void CRuFusion_AudioRegistry::GetAudioTags(CRuArrayList<const char *> *tagList)
{
	// Store pointer to tag list
	m_enumeration = tagList;

	// Perform enumeration into tag list
	m_tagDescriptors.ForAllElements(boost::bind(&CRuFusion_AudioRegistry::EnumerateAudioTagCallback, this, _1, _2));
}

BOOL CRuFusion_AudioRegistry::EnumerateAudioTagCallback(const char *key, void *data)
{
//	CRuString *str = (CRuString *) key;

	m_enumeration->Add(key);

	return TRUE;
}

BOOL CRuFusion_AudioRegistry::FreeAudioTagCallback(const char *key, void *data)
{
	TagDescriptor *tagDescriptor = *reinterpret_cast<TagDescriptor **>(data);

	for(INT32 i = 0; i < tagDescriptor->m_associatedAudio.Count(); ++i)
	{
		delete tagDescriptor->m_associatedAudio[i];
	}

	tagDescriptor->m_associatedAudio.Clear();

	delete tagDescriptor;

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuFusion_AudioLibrary::CRuFusion_AudioLibrary()
{
}

CRuFusion_AudioLibrary::~CRuFusion_AudioLibrary()
{
	// Clear off registries
	m_audioRegistries.ForAllElements(boost::bind(&CRuFusion_AudioLibrary::FreeAudioClassCallback, this, _1, _2));
}

BOOL CRuFusion_AudioLibrary::AssociateAudio(const char *audioClass, const char *tag, const char *audioResource)
{
	CRuFusion_AudioRegistry *audioRegistry = NULL;

	if(m_audioRegistries.ContainsKey(audioClass))
	{
		audioRegistry = m_audioRegistries.Get(audioClass);
	}
	else
	{
		audioRegistry = ruNEW CRuFusion_AudioRegistry();
		m_audioRegistries.Add(audioClass, audioRegistry);
	}

	audioRegistry->AssociateAudio(tag, audioResource);

	return TRUE;
}

const char *CRuFusion_AudioLibrary::GetAssociatedAudio(const char *audioClass, const char *audioTag)
{
	if(m_audioRegistries.ContainsKey(audioClass))
	{
		CRuFusion_AudioRegistry *audioRegistry = m_audioRegistries.Get(audioClass);
		return audioRegistry->GetAssociatedAudio(audioTag);
	}

	return NULL;
}

CRuArrayList<const char *> *CRuFusion_AudioLibrary::GetAudioClasses()
{
	// Clear old enumeration
	m_enumeration.Clear();

	// Perform enumeration
	m_audioRegistries.ForAllElements(boost::bind(&CRuFusion_AudioLibrary::EnumerateAudioClassCallback, this, _1, _2));

	return &m_enumeration;
}

CRuArrayList<const char *> *CRuFusion_AudioLibrary::GetAudioTags(const char *audioClass)
{
	// Clear old enumeration
	m_enumeration.Clear();

	if(m_audioRegistries.ContainsKey(audioClass))
	{
		CRuFusion_AudioRegistry *audioRegistry = m_audioRegistries.Get(audioClass);
		audioRegistry->GetAudioTags(&m_enumeration);
	}

	return &m_enumeration;
}

BOOL CRuFusion_AudioLibrary::EnumerateAudioClassCallback(const char *key, void *data)
{
	m_enumeration.Add(key);

	return TRUE;
}

BOOL CRuFusion_AudioLibrary::FreeAudioClassCallback(const char *key, void *data)
{
	CRuFusion_AudioRegistry *audioRegistry = *reinterpret_cast<CRuFusion_AudioRegistry **>(data);

	ruSAFE_RELEASE(audioRegistry);

	return TRUE;
}

// ************************************************************************************************************************************************************

CRuFusion_AudioLibrary *g_ruAudioLibrary = NULL;

CRuFusion_AudioLibrary *RuFusion_GetAudioLibrary()
{
	return g_ruAudioLibrary;
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
