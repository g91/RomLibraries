/*!
	@file RuEntity_CollectorFX.h

	Copyright (c) 2004-2006 All rights reserved
*/

#ifndef _RUENTITY_COLLECTORFX_H_
#define _RUENTITY_COLLECTORFX_H_

#include "../../Scene/Base/RuEntityBase.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

/*!
	Collector fx entity.

	@version 2007.07.20
*/
class CRuEntity_CollectorFX : public CRuEntity
{
	ruRTTI_DECLARE
	ruHEAP_DECLARE
	ruCLASSGUID_DECLARE

protected:
	enum RuCollectorState
	{
		ruCOLLECTORSTATE_READY,
		ruCOLLECTORSTATE_ACTIVE,
		ruCOLLECTORSTATE_FINISHED
	};

	struct EffectEntityDesc
	{
		CRuEntity*							m_entity;									//!< FX entity

		// Stage 1(separation) variables
		CRuVector3							m_velocity;

		// Stage 2(collection) variables
		CRuVector3							m_origin;									//!< Origin of the fx entity path
		REAL								m_deviationVectorAngle;
		REAL								m_deviationVectorLength;

		BOOL								m_finished;
	};

	CRuEntity*								m_effectEntity;
	char*									m_effectStartMotion;
	char*									m_effectFlightMotion;
	char*									m_effectEndMotion;
	INT32									m_effectEntityGenerateCount;
	CRuVector3								m_generatorPosition;
	REAL									m_generatorStrength;
	REAL									m_generatorVelocityDamping;					//!< Damping speed in units per second
	REAL									m_pathDeviation;
	REAL									m_collectionSpeed;

	// Runtime
	RuCollectorState						m_collectorState;
	CRuArrayList<EffectEntityDesc>			m_effectEntities;

public:
											CRuEntity_CollectorFX();
	virtual									~CRuEntity_CollectorFX();

	CRuEntity*								GetEffectEntity();
	void									SetEffectEntity(CRuEntity *entity);

	const char*								GetEffectStartMotion();
	void									SetEffectStartMotion(const char *motion);

	const char*								GetEffectFlightMotion();
	void									SetEffectFlightMotion(const char *motion);

	const char*								GetEffectEndMotion();
	void									SetEffectEndMotion(const char *motion);

	INT32									GetEffectEntityGenerateCount();
	void									SetEffectEntityGenerateCount(INT32 numEntities);

	const CRuVector3&						GetGeneratorPosition();
	void									SetGeneratorPosition(const CRuVector3 &position);

	REAL									GetGeneratorStrength();
	void									SetGeneratorStrength(REAL strength);

	REAL									GetGeneratorVelocityDamping();
	void									SetGeneratorVelocityDamping(REAL damping);

	REAL									GetPathDeviation();
	void									SetPathDeviation(REAL deviation);

	REAL									GetCollectionSpeed();
	void									SetCollectionSpeed(REAL collectionSpeed);

	BOOL									IsCollectorFXFinished();

	// Base class implementation
	virtual IRuObject*						Clone(IRuObject *clonedObject = NULL);
	virtual BOOL							SerializeFrom(IRuStream *stream);
	virtual BOOL							SerializeTo(IRuStream *stream);

	// Base interface
	virtual BOOL							Update(REAL elapsedTime);
};

// ************************************************************************************************************************************************************

ruOBJECTMODULE_DEFINE(CRuObjectModule_Entity_CollectorFX, CRuEntity_CollectorFX)

// ************************************************************************************************************************************************************

#pragma managed(pop)

#endif
