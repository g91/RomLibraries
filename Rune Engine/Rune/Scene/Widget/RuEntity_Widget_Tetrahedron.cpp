/*!
	@file RuEntity_Widget_Tetrahedron.cpp

	Copyright (c) 2004-2007 All rights reserved
*/

#include "../../Scene/Widget/RuEntity_Widget_Tetrahedron.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

static const BYTE tetrahedronStreamBytes[] =
{
	0x5e,0x34,0x86,0xc7,0x99,0x4d,0x16,0x95,0x0d,0x08,0x10,0xa2,0x48,0xab,0xc9,0x59,0xff,0xff,0xff,0xff,
	0x03,0x00,0x00,0x00,0x5e,0x34,0x86,0xc7,0x99,0x4d,0x16,0x95,0x0d,0x08,0x10,0xa2,0x48,0xab,0xc9,0x59,
	0x01,0x00,0x00,0x00,0x5e,0x34,0x86,0xc7,0x99,0x4d,0x16,0x95,0x0d,0x08,0x10,0xa2,0x48,0xab,0xc9,0x59,
	0x01,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x68,0x65,0x64,0x72,0x61,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,
	0x80,0x3f,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0x00,0x00,0x00,0x48,0x60,0xc6,0xa8,0x47,0x41,0x23,0x2d,0xc5,0x3a,0x0d,0xab,0x00,0x27,
	0x81,0xd4,0xff,0xff,0xff,0xff,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x18,0x00,
	0x00,0x00,0x08,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,
	0x05,0x00,0x06,0x00,0x07,0x00,0x08,0x00,0x09,0x00,0x0a,0x00,0x0b,0x00,0x0c,0x00,0x0d,0x00,0x0e,0x00,
	0x0f,0x00,0x10,0x00,0x11,0x00,0x12,0x00,0x13,0x00,0x14,0x00,0x15,0x00,0x16,0x00,0x17,0x00,0x9c,0x09,
	0x2a,0x3b,0xac,0x49,0x1e,0xaf,0x66,0x5c,0x3b,0x89,0x3b,0xce,0x01,0x72,0xff,0xff,0xff,0xff,0x02,0x00,
	0x00,0x00,0x29,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x39,0x21,0x2f,0x00,0x00,
	0x80,0xbf,0x00,0x00,0x38,0x27,0xf3,0x04,0x35,0xbf,0x86,0x89,0xe6,0x23,0xf3,0x04,0x35,0x3f,0xf3,0x04,
	0x35,0xbf,0x00,0x00,0x80,0xa5,0xf3,0x04,0x35,0xbf,0xf3,0x04,0x35,0x3f,0xf0,0x00,0xe4,0x2e,0xf3,0x04,
	0x35,0x3f,0x08,0x39,0x21,0x2f,0x00,0x00,0x80,0xbf,0x00,0x00,0x38,0x27,0xf3,0x04,0x35,0x3f,0xc7,0x00,
	0xe4,0x2e,0xf3,0x04,0x35,0xbf,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0xf3,0x04,
	0x35,0x3f,0xc7,0x00,0xe4,0x2e,0xf3,0x04,0x35,0xbf,0xf3,0x04,0x35,0xbf,0x00,0x00,0x80,0xa5,0xf3,0x04,
	0x35,0xbf,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,0x00,0x00,0xf3,0x04,0x35,0xbf,0x86,0x89,
	0xe6,0x23,0xf3,0x04,0x35,0x3f,0xf3,0x04,0x35,0x3f,0xf0,0x00,0xe4,0x2e,0xf3,0x04,0x35,0x3f,0xf3,0x04,
	0x35,0x3f,0xc7,0x00,0xe4,0x2e,0xf3,0x04,0x35,0xbf,0x08,0x39,0x21,0x2f,0x00,0x00,0x80,0xbf,0x00,0x00,
	0x38,0x27,0xf3,0x04,0x35,0xbf,0x00,0x00,0x80,0xa5,0xf3,0x04,0x35,0xbf,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x3f,0x00,0x00,0x00,0x00,0xf3,0x04,0x35,0xbf,0x00,0x00,0x80,0xa5,0xf3,0x04,0x35,0xbf,0xf3,0x04,
	0x35,0xbf,0x86,0x89,0xe6,0x23,0xf3,0x04,0x35,0x3f,0xf3,0x04,0x35,0xbf,0x86,0x89,0xe6,0x23,0xf3,0x04,
	0x35,0x3f,0x08,0x39,0x21,0x2f,0x00,0x00,0x80,0xbf,0x00,0x00,0x38,0x27,0xf3,0x04,0x35,0x3f,0xf0,0x00,
	0xe4,0x2e,0xf3,0x04,0x35,0x3f,0xf3,0x04,0x35,0x3f,0xf0,0x00,0xe4,0x2e,0xf3,0x04,0x35,0x3f,0xf3,0x04,
	0x35,0x3f,0xc7,0x00,0xe4,0x2e,0xf3,0x04,0x35,0xbf,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x3f,0x00,0x00,
	0x00,0x00,0xec,0x05,0x51,0xbf,0x3a,0xcd,0x13,0xbf,0x5e,0x8d,0xe8,0x24,0xec,0x05,0x51,0xbf,0x3a,0xcd,
	0x13,0xbf,0x5e,0x8d,0xe8,0x24,0xec,0x05,0x51,0xbf,0x3a,0xcd,0x13,0xbf,0x5e,0x8d,0xe8,0x24,0xec,0x05,
	0x51,0x3f,0x3a,0xcd,0x13,0xbf,0x00,0x00,0x00,0x00,0xec,0x05,0x51,0x3f,0x3a,0xcd,0x13,0xbf,0x00,0x00,
	0x00,0x00,0xec,0x05,0x51,0x3f,0x3a,0xcd,0x13,0xbf,0x00,0x00,0x00,0x00,0x02,0x2a,0x3a,0xae,0x3a,0xcd,
	0x13,0x3f,0xec,0x05,0x51,0xbf,0x02,0x2a,0x3a,0xae,0x3a,0xcd,0x13,0x3f,0xec,0x05,0x51,0xbf,0x02,0x2a,
	0x3a,0xae,0x3a,0xcd,0x13,0x3f,0xec,0x05,0x51,0xbf,0x07,0x2a,0x3a,0xae,0x3a,0xcd,0x13,0x3f,0xec,0x05,
	0x51,0x3f,0x07,0x2a,0x3a,0xae,0x3a,0xcd,0x13,0x3f,0xec,0x05,0x51,0x3f,0x07,0x2a,0x3a,0xae,0x3a,0xcd,
	0x13,0x3f,0xec,0x05,0x51,0x3f,0xf5,0x29,0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,0x51,0xbf,0xf5,0x29,
	0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,0x51,0xbf,0xf5,0x29,0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,
	0x51,0xbf,0xec,0x05,0x51,0xbf,0x3a,0xcd,0x13,0x3f,0x5e,0x8d,0xe8,0xa4,0xec,0x05,0x51,0xbf,0x3a,0xcd,
	0x13,0x3f,0x5e,0x8d,0xe8,0xa4,0xec,0x05,0x51,0xbf,0x3a,0xcd,0x13,0x3f,0x5e,0x8d,0xe8,0xa4,0x1a,0x2a,
	0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,0x51,0x3f,0x1a,0x2a,0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,
	0x51,0x3f,0x1a,0x2a,0x3a,0x2e,0x3a,0xcd,0x13,0xbf,0xec,0x05,0x51,0x3f,0xec,0x05,0x51,0x3f,0x3a,0xcd,
	0x13,0x3f,0xcb,0xe7,0x05,0xa5,0xec,0x05,0x51,0x3f,0x3a,0xcd,0x13,0x3f,0xcb,0xe7,0x05,0xa5,0xec,0x05,
	0x51,0x3f,0x3a,0xcd,0x13,0x3f,0xcb,0xe7,0x05,0xa5,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0x01,0x00,0x00,0x00,0xbe,0x23,0xdc,0xb8,0x40,0x4d,0x22,0x3f,0xee,0x28,
	0xa9,0xae,0xbc,0xec,0xc9,0x4a,0x01,0x00,0x00,0x00,0xf3,0x04,0x35,0xbf,0x00,0x00,0x80,0xbf,0xf3,0x04,
	0x35,0xbf,0xf3,0x04,0x35,0x3f,0x00,0x00,0x80,0x3f,0xf3,0x04,0x35,0x3f,0x08,0x00,0x00,0x00,0x01,0x00,
	0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,
	0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05,0x00,0x06,0x00,
	0x07,0x00,0xeb,0xf7,0xe4,0xce,0x27,0x41,0xfa,0x85,0xe8,0x1b,0x75,0xa4,0xcc,0xad,0xa6,0x5b,0xff,0xff,
	0xff,0xff,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,
	0x00,0x00,0x40,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x49,0x00,
	0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4a,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,
	0x80,0x3f,0x41,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x4c,0x00,0x00,0x00,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00
};

static const INT32 tetrahedronStreamBytesLength = 1166;

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuEntity_Widget_Tetrahedron, CRuEntity)
ruHEAP_DEFINE_SUBCLASS(CRuEntity_Widget_Tetrahedron, "CRuEntity_Widget_Tetrahedron", "CRuEntity")
ruCLASSGUID_DEFINE(CRuEntity_Widget_Tetrahedron, 0xF87B962F, 0x03784322, 0xA97B420C, 0xA6A5E577)

// ************************************************************************************************************************************************************

CRuEntity_Widget_Tetrahedron::CRuEntity_Widget_Tetrahedron()
{
	CRuEntity *hedronEntity = g_ruResourceManager->LoadEntity("0xF87B962F, 0x03784322, 0xA97B420C, 0xA6A5E577");

	if(hedronEntity == NULL)
	{
		// Copy tetrahedron model stream into a memory stream
		CRuMemoryStream *memStream = ruNEW CRuMemoryStream();
		memStream->SetBufferSize(tetrahedronStreamBytesLength);
		memStream->SetStreamSize(tetrahedronStreamBytesLength);
		memcpy(memStream->GetBuffer(), tetrahedronStreamBytes, tetrahedronStreamBytesLength);

		// Construct tetrahedron model from the memory stream
		memStream->Seek(0, ruSSM_Begin);
		hedronEntity = static_cast<CRuEntity *>(RuObjectFactory().CreateObject(memStream));

		// Insert tetrahedron model into resource manager's database
		g_ruResourceManager->AddEntity("0xF87B962F, 0x03784322, 0xA97B420C, 0xA6A5E577", hedronEntity);

		// Release memory stream
		delete memStream;
	}

	// Attach to node
	this->AddChild(hedronEntity);

	// Release local copy
	ruSAFE_RELEASE(hedronEntity);
}

CRuEntity_Widget_Tetrahedron::~CRuEntity_Widget_Tetrahedron()
{
}

// ************************************************************************************************************************************************************

#pragma managed
