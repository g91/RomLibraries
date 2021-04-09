#ifndef _RUENTITYMODEL_H_
#define _RUENTITYMODEL_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

class CRuModel : public IRuEntity_Renderable
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	struct SubModel
	{
		INT32								m_meshIterationNumber;
		IRuMesh*							m_mesh;
		IRuMaterial*						m_material;
	};

	CRuArrayList<SubModel>					m_subModels;

public:
											CRuModel();
	virtual									~CRuModel();

	// IRuObject interface
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Memory reporting
	virtual INT32							GetMemoryFootprint(CRuKeyHash<PTRVALUE> *enumHash = NULL);

	virtual BOOL							Update(REAL meshIndex);

	// Collision interface
	virtual BOOL							QueryCollision(CRuCollisionQuery *colQuery);

	virtual INT32							GetNumMeshes() const;
	virtual BOOL							GetMesh(INT32 meshIndex, IRuMesh **meshOut, IRuMaterial **materialOut);
	BOOL									SetMesh(INT32 meshIndex, IRuMesh *mesh);
	INT32									GetMeshIndex(IRuMesh *mesh);

	BOOL									AddSubModel(IRuMesh *mesh, IRuMaterial *material);
	BOOL									RemoveSubModel(INT32 meshIndex);
	BOOL									RemoveAllSubModels();

	virtual void							CalculateEntityBounds();
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Model, CRuModel)

// ************************************************************************************************************************************************************

inline INT32 CRuModel::GetNumMeshes() const
{
	return m_subModels.Count();
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
