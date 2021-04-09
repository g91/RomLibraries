#ifndef _RUMOJO_HELPER_
#define _RUMOJO_HELPER_

#include "../Mojo/RuMojo_Base.h"
#include "../Engine/Geometry/RuMeshBase.h"

#pragma unmanaged

// ************************************************************************************************************************************************************

// Height field manipulation
BOOL				Mojo_ResetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, REAL height);
BOOL				Mojo_PaintHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL delta, REAL refHeight, BOOL adaptivePainting, RuFalloffFunction falloffFunction);
BOOL				Mojo_NoiseHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL variation);
BOOL				Mojo_SetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL height, REAL brushStrength, RuFalloffFunction falloffFunction);
BOOL				Mojo_SmoothHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &center, REAL radius, REAL delta);
BOOL				Mojo_OffsetHeight(CRuMojo_AttributeParcel<REAL> *hfParcel, REAL offset);

// Height field mesh building
CRuMesh*			Mojo_BuildMeshFromHeightField(CRuMojo_AttributeParcel<REAL> *hfParcel, CRuMojo_AttributeParcel<RUCOLOR> *cfParcel, CRuMojo_AttributeParcel<BYTE> *flagParcel, BYTE flagValue);

// Height field serialization
BOOL				Mojo_SerializeFrom(CRuMojo_AttributeParcel<REAL> **hfParcel, IRuStream *stream);
BOOL				Mojo_SerializeTo(CRuMojo_AttributeParcel<REAL> *hfParcel, IRuStream *stream);

// Height field evaluation
REAL				Mojo_GetHeightAtPoint(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point);
REAL				Mojo_GetHeightAtPoint_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point);
REAL				Mojo_GetSlopeAtPoint(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point);
REAL				Mojo_GetSlopeAtPoint_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point);

REAL				Mojo_GetValleyDepth_Multisampled(CRuMojo_AttributeParcel<REAL> *hfParcel, const CRuVector3 &point);

// Mask painting
BOOL				Mojo_ResetFlags(CRuMojo_AttributeParcel<BYTE> *parcel, BYTE flagValue, BYTE flagMask);
BOOL				Mojo_PaintFlags(CRuMojo_AttributeParcel<BYTE> *parcel, const CRuVector3 &center, REAL radius, BYTE flagValue, BYTE flagMask);
BOOL				Mojo_PaintValue(CRuMojo_AttributeParcel<BYTE> *parcel, const CRuVector3 &center, REAL radius, REAL value, REAL power, RuFalloffFunction falloffFunction, BYTE flagMask);

// Color painting
BOOL				Mojo_PaintColor(CRuMojo_AttributeParcel<RUCOLOR> *parcel, const CRuVector3 &center, REAL radius, RUCOLOR color, REAL delta, RuFalloffFunction falloffFunction);
RUCOLOR				Mojo_PickColor(CRuMojo_AttributeParcel<RUCOLOR> *parcel, const CRuVector3 &center);

// ************************************************************************************************************************************************************

#pragma managed

#endif
