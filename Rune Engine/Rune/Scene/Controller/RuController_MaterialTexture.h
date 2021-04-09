#ifndef _RUCONTROLLER_MATERIALTEXTURE_H_
#define _RUCONTROLLER_MATERIALTEXTURE_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	@author John Tang
	@version 2004/12/10
*/
class CRuController_MaterialTexture : public IRuEntity_KeyableController
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuObject*								m_target;

	RuTextureChannel						m_targetChannel;
	CRuAnimTrack<char *>					m_track_TextureName;								//!< Texture name track

	// Run-time
	char*									m_curTextureName;
	IRuTexture*								m_curTexture;

	CRuTernaryStringTree<IRuTexture *>		m_textureCache;

public:
											CRuController_MaterialTexture();
	virtual									~CRuController_MaterialTexture();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);

	virtual IRuObject*						GetTarget();
	virtual BOOL							SetTarget(IRuObject *target);

	RuTextureChannel						GetTargetChannel();
	void									SetTargetChannel(RuTextureChannel targetChannel);

	// IRuEntity_KeyableController implementation
	virtual INT32							GetNumTracks() const;
	virtual BOOL							GetTrackByIndex(INT32 trackIndex, RuAnimTrackVariant &trackOut);
	virtual BOOL							GetTrackByName(const char *trackName, RuAnimTrackVariant &trackOut);

	// Event handlers
	BOOL									HandleEvent_Dispose(RuEventArgs *eventArgs);

protected:
	BOOL									ClearTextureCacheCallback(const void *key, void *data);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Controller_MaterialTexture, CRuController_MaterialTexture)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
