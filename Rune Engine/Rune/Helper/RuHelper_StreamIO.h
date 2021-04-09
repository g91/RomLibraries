/*!
	@project Rune
	@file RuHelper_StreamIO.h

	Copyright (c) 2004-2005 All rights reserved

	@author John Tang
	@date 2006/02/08
*/

#ifndef _RUHELPER_STREAMIO_H_
#define _RUHELPER_STREAMIO_H_

#include "../Core/Type/RuType_Object.h"
#include "../Core/Type/RuType_BitField.h"
#include "../Core/Stream/RuStreamIO.h"
#include "../Core/Math/RuMathGeometry.h"
#include "../Core/Math/RuMathVector.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

template <typename DataType>
BOOL SerializeFrom(IRuStream *stream, DataType *key)
{
	return (stream->Read(key, sizeof(DataType)) == sizeof(DataType));
}

template <typename DataType>
BOOL SerializeTo(IRuStream *stream, DataType *key)
{
	return (stream->Write(key, sizeof(DataType)) == ruSEC_OK);
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuColor4 *key)
{
	stream->Read(&key->m_a, sizeof(REAL));
	stream->Read(&key->m_r, sizeof(REAL));
	stream->Read(&key->m_g, sizeof(REAL));
	stream->Read(&key->m_b, sizeof(REAL));

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuColor4 *key)
{
	stream->Write(&key->m_a, sizeof(REAL));
	stream->Write(&key->m_r, sizeof(REAL));
	stream->Write(&key->m_g, sizeof(REAL));
	stream->Write(&key->m_b, sizeof(REAL));

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuVector3 *key)
{
	stream->Read(&key->m_x, sizeof(REAL));
	stream->Read(&key->m_y, sizeof(REAL));
	stream->Read(&key->m_z, sizeof(REAL));

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuVector3 *key)
{
	stream->Write(&key->m_x, sizeof(REAL));
	stream->Write(&key->m_y, sizeof(REAL));
	stream->Write(&key->m_z, sizeof(REAL));

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuQuaternion *key)
{
	stream->Read(&key->m_x, sizeof(REAL));
	stream->Read(&key->m_y, sizeof(REAL));
	stream->Read(&key->m_z, sizeof(REAL));
	stream->Read(&key->m_w, sizeof(REAL));

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuQuaternion *key)
{
	stream->Write(&key->m_x, sizeof(REAL));
	stream->Write(&key->m_y, sizeof(REAL));
	stream->Write(&key->m_z, sizeof(REAL));
	stream->Write(&key->m_w, sizeof(REAL));

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuMatrix4x4 *key)
{
	stream->Read(key->m_elements, sizeof(REAL) * 16);

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuMatrix4x4 *key)
{
	stream->Write(key->m_elements, sizeof(REAL) * 16);

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuAABB *key)
{
	stream->Read(&key->Minima().m_x, sizeof(REAL));
	stream->Read(&key->Minima().m_y, sizeof(REAL));
	stream->Read(&key->Minima().m_z, sizeof(REAL));
	stream->Read(&key->Maxima().m_x, sizeof(REAL));
	stream->Read(&key->Maxima().m_y, sizeof(REAL));
	stream->Read(&key->Maxima().m_z, sizeof(REAL));

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuAABB *key)
{
	stream->Write(&key->Minima().m_x, sizeof(REAL));
	stream->Write(&key->Minima().m_y, sizeof(REAL));
	stream->Write(&key->Minima().m_z, sizeof(REAL));
	stream->Write(&key->Maxima().m_x, sizeof(REAL));
	stream->Write(&key->Maxima().m_y, sizeof(REAL));
	stream->Write(&key->Maxima().m_z, sizeof(REAL));

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuOBB *key)
{
	stream->Read(&key->Center().m_x, sizeof(REAL));
	stream->Read(&key->Center().m_y, sizeof(REAL));
	stream->Read(&key->Center().m_z, sizeof(REAL));

	for(INT32 i = 0; i < 3; ++i)
	{
		stream->Read(&key->Axis()[i].m_x, sizeof(REAL));
		stream->Read(&key->Axis()[i].m_y, sizeof(REAL));
		stream->Read(&key->Axis()[i].m_z, sizeof(REAL));
	}

	for(INT32 i = 0; i < 3; ++i)
	{
		stream->Read(&key->Extents()[i], sizeof(REAL));
	}

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuOBB *key)
{
	stream->Write(&key->Center().m_x, sizeof(REAL));
	stream->Write(&key->Center().m_y, sizeof(REAL));
	stream->Write(&key->Center().m_z, sizeof(REAL));

	for(INT32 i = 0; i < 3; ++i)
	{
		stream->Write(&key->Axis()[i].m_x, sizeof(REAL));
		stream->Write(&key->Axis()[i].m_y, sizeof(REAL));
		stream->Write(&key->Axis()[i].m_z, sizeof(REAL));
	}

	for(INT32 i = 0; i < 3; ++i)
	{
		stream->Write(&key->Extents()[i], sizeof(REAL));
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, char **key)
{
	// Read key string length
	INT32 keyLength = 0;
	stream->Read(&keyLength, sizeof(INT32));

	if(keyLength > 0)
	{
		// Read key string
		*key = ruNEW char [keyLength];
		stream->Read(*key, keyLength);
	}
	else
	{
		*key = ruNEW char [1];
		(*key)[0] = NULL;
	}

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, char **key)
{
	if(*key)
	{
		// Write key string length
		INT32 keyLength = static_cast<INT32>(strlen(*key)) + 1;
		stream->Write(&keyLength, sizeof(INT32));

		// Write key string
		stream->Write(*key, keyLength);
	}
	else
	{
		INT32 keyLength = 0;
		stream->Write(&keyLength, sizeof(INT32));
	}

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, const char **key)
{
	if(*key)
	{
		// Write key string length
		INT32 keyLength = static_cast<INT32>(strlen(*key)) + 1;
		stream->Write(&keyLength, sizeof(INT32));

		// Write key string
		stream->Write(*key, keyLength);
	}
	else
	{
		INT32 keyLength = 0;
		stream->Write(&keyLength, sizeof(INT32));
	}

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, RuPropertyType *key)
{
	RuPropertyTypeSerializer propertySerializer(stream);
	*key = propertySerializer.SerializeFrom();

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, RuPropertyType *key)
{
	RuPropertyTypeSerializer propertySerializer(stream);
	boost::apply_visitor(propertySerializer, *key);

	return TRUE;
}

// ************************************************************************************************************************************************************

template <>
inline BOOL SerializeFrom(IRuStream *stream, CRuBitField *key)
{
	INT32 numBits = 0;
	INT32 bitFieldArraySize = 0;

	stream->Read(&numBits, sizeof(INT32));
	stream->Read(&bitFieldArraySize, sizeof(INT32));

	key->SetNumBits(numBits);

	UINT32 *bitFieldArray = key->GetBitFieldArray();

	stream->Read(bitFieldArray, sizeof(UINT32) * bitFieldArraySize);

	return TRUE;
}

template <>
inline BOOL SerializeTo(IRuStream *stream, CRuBitField *key)
{
	INT32 numBits = key->GetNumBits();
	INT32 bitFieldArraySize = key->GetBitFieldArraySize();
	UINT32 *bitFieldArray = key->GetBitFieldArray();

	stream->Write(&numBits, sizeof(INT32));
	stream->Write(&bitFieldArraySize, sizeof(INT32));
	stream->Write(bitFieldArray, sizeof(UINT32) * bitFieldArraySize);

	return TRUE;
}

// ************************************************************************************************************************************************************

#pragma managed

#endif
