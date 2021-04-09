#pragma once

// Rune system core
#include "../../Engine/Instantiator/RuMeshInstantiator_Base.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Static : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Static(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Static();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_StaticColored : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_StaticColored(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_StaticColored();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_StaticColoredOld : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_StaticColoredOld(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_StaticColoredOld();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Prelit : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Prelit(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Prelit();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_PrelitNoTex : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_PrelitNoTex(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_PrelitNoTex();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Skinned : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Skinned(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Skinned();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_CompressedTerrain : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_CompressedTerrain(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_CompressedTerrain();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Position : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Position(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Position();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Normal : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Normal(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Normal();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Normal_UBYTE4 : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Normal_UBYTE4(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Normal_UBYTE4();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_Diffuse : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_Diffuse(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_Diffuse();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

class CRuMeshInstantiator_UV0 : public IRuMeshInstantiator
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	IRuEngine*								m_engine;
	IRuVertexStreamLayout*					m_vertexStreamLayout;

public:
											CRuMeshInstantiator_UV0(IRuEngine *engine);
	virtual									~CRuMeshInstantiator_UV0();

	virtual const IRuVertexStreamLayout*	GetVertexStreamLayout() const;
	virtual void							Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const;
};

// ************************************************************************************************************************************************************

#pragma managed(pop)
