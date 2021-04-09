#ifndef _RUENTITYCOMPOSITEMODEL_H_
#define _RUENTITYCOMPOSITEMODEL_H_

#include "../../Scene/Base/RuEntityBase.h"
#include "../../Scene/Base/RuEntityModel.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuCompositeModel;

// ************************************************************************************************************************************************************

class CRuCompositeModelTemplate : public IRuObject
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct PartDescriptor
	{
		INT32								m_partID;						//!< Part ID
		CRuString							m_partName;						//!< Part name
		INT32								m_partTexureX;					//!< Composite texture origin x
		INT32								m_partTexureY;					//!< Composite texture origin y
		INT32								m_partTexureWidth;				//!< Source texture width
		INT32								m_partTexureHeight;				//!< Source texture height
	};

	CRuString								m_descriptorString;				//!< String descriptor of the composite model template
	CRuArrayList<PartDescriptor *>			m_descriptors;					//!< Parsed descriptor structure  of the composite model template
	char*									m_assemblyRule;					//!< Optional model assembly rule

public:
											CRuCompositeModelTemplate();
	virtual									~CRuCompositeModelTemplate();

	// IRuObject serialization
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Descriptor string setting
	void									SetDescriptorString(const char *descriptorString);
	const char*								GetDescriptorString() const;

	// Descriptor access
	INT32									GetNumParts() const;
	INT32									GetPartIndex(INT32 partID) const;
	INT32									GetPartID(INT32 partIndex) const;
	const char*								GetPartName(INT32 partIndex) const;
	BOOL									GetPartTextureQuad(INT32 partIndex, INT32 &xOut, INT32 &yOut, INT32 &widthOut, INT32 &heightOut) const;

	// Assembly rule setting
	const char*								GetAssemblyRule();
	BOOL									SetAssemblyRule(const char *assemblyRule);

	// Construction
	CRuCompositeModel*						Construct();
};

// ************************************************************************************************************************************************************

class CRuCompositeModel : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct ModelPart
	{
		CRuEntity*							m_model;
//		IRuMesh*							m_mesh;
//		IRuMaterial*						m_material;
		BOOL								m_layerActivated[4];
		RUCOLOR								m_layerColor[4];

											ModelPart()
											{
												m_model = NULL;

												for(INT32 i = 0; i < 4; ++i)
												{
													m_layerActivated[i] = FALSE;
													m_layerColor[i] = 0xFFFFFFFF;
												}
											}
	};

	INT32									m_maxMipLevel;
	INT32									m_fullCompositionSize;		//!< The size of the original intended, completely composited texture size
	INT32									m_compositeTextureSize;		//!< The actual requested size of the composited diffuse texture size
	INT32									m_maskTextureSize;			//!< The actual requested size of the composited mask texture size
	INT32									m_colorMapTextureSize;
	INT32									m_glossTextureSize;			//!< The actual requested size of the composited gloss texture size
	BOOL									m_enablePartCompositing;	//!< Actually composite parts into one model?

	CRuCompositeModelTemplate*				m_modelTemplate;
	CRuArrayList<ModelPart>					m_modelParts;

	BOOL									m_modelIsBuilt;
	BOOL									m_textureRebuildRequired;
	CRuModel*								m_compositeModel;
	CRuMesh*								m_compositeMesh;
	CRuMaterial*							m_compositeMaterial;

	IRuTexture*								m_sys_BlackTexture;

public:
											CRuCompositeModel();
	virtual									~CRuCompositeModel();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	virtual BOOL							Update(REAL elapsedTime);

	// Collision
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 subModelIndex, IRuMesh **meshOut, IRuMaterial **materialOut);

	// Model composition
	BOOL									SetMaxMipLevel(INT32 maxMipLevel);
	BOOL									EnablePartCompositing(BOOL enable);
	BOOL									SetPart(INT32 partID, CRuEntity *partModel);
	BOOL									SetPartColors(INT32 partID, const BOOL *layerActivated, const RUCOLOR *layerColor);

	BOOL									BuildModel();
	BOOL									ResetModel();

	// Model conversion
	CRuModel*								ConvertToStandardModel();

	friend									CRuCompositeModelTemplate;

protected:
	BOOL									InitializeTextures();
	void									BuildColorMap();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_CompositeModelTemplate, CRuCompositeModelTemplate)

// ************************************************************************************************************************************************************

#pragma managed

#endif
