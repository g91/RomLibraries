#include "../../Engine/Instantiator/RuMeshInstantiators_Common.h"

#pragma managed(push, off)

// ************************************************************************************************************************************************************

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Static, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Static, "CRuMeshInstantiator_Static", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Static, 0xBD66BA49, 0x06F443ee, 0x9F4D4E67, 0x764FB8AA)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_StaticColored, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_StaticColored, "CRuMeshInstantiator_StaticColored", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_StaticColored, 0xEF6041C0, 0x74884df1, 0xA7F5B2C7, 0x8D0ADC63)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_StaticColoredOld, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_StaticColoredOld, "CRuMeshInstantiator_StaticColoredOld", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_StaticColoredOld, 0xEF6041C0, 0x74884df1, 0xA7F5B2C7, 0x8D0ADC63)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Prelit, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Prelit, "CRuMeshInstantiator_Prelit", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Prelit, 0x883070B1, 0xD6A54d74, 0xA8616206, 0x0ADE889E)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_PrelitNoTex, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_PrelitNoTex, "CRuMeshInstantiator_PrelitNoTex", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_PrelitNoTex, 0x66DC3863, 0x890647dc, 0x9C3FFC67, 0x9CC9B606)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Skinned, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Skinned, "CRuMeshInstantiator_Skinned", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Skinned, 0x975B13DF, 0x0B694246, 0x9D6602B6, 0xD8692996)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_CompressedTerrain, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_CompressedTerrain, "CRuMeshInstantiator_CompressedTerrain", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_CompressedTerrain, 0x02A47032, 0x8A0044ba, 0xB2DE49B6, 0xEE24718C)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Position, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Position, "CRuMeshInstantiator_Position", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Position, 0xC9D89EB5, 0x1D414550, 0x95B735FF, 0x26C0608C)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Normal, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Normal, "CRuMeshInstantiator_Normal", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Normal, 0x6DAA1F83, 0xF78944e9, 0xBE588C10, 0xFD5FD8B5)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Normal_UBYTE4, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Normal_UBYTE4, "CRuMeshInstantiator_Normal_UBYTE4", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Normal_UBYTE4, 0x9CEDABFD, 0xCA67414c, 0x8E0313AB, 0xF49F18D5)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_Diffuse, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_Diffuse, "CRuMeshInstantiator_Diffuse", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_Diffuse, 0x801B7639, 0xC765447a, 0xAFD22BAC, 0x22773F04)

ruRTTI_DEFINE_SUBCLASS(CRuMeshInstantiator_UV0, IRuMeshInstantiator)
ruHEAP_DEFINE_SUBCLASS(CRuMeshInstantiator_UV0, "CRuMeshInstantiator_UV0", "IRuMeshInstantiator")
ruCLASSGUID_DEFINE(CRuMeshInstantiator_UV0, 0xB93A7DB5, 0x64C24da7, 0x9CB52867, 0xA2555863)

// ************************************************************************************************************************************************************

inline UINT16 RuRealToHalfI(UINT32 i)
{
	INT32 s =  (i >> 16) & 0x00008000;
	INT32 e = ((i >> 23) & 0x000000FF) - (127 - 15);
	INT32 m =   i        & 0x007FFFFF;

	if(e <= 0)
	{
		if(e < -10)
		{
			return 0;
		}
		else
		{
			m = (m | 0x00800000) >> (1 - e);
			return s | (m >> 13);
		}
	}
	else if(e == 0xFF - (127 - 15))
	{
		if(m == 0)	// Inf
		{
			return s | 0x7C00;
		}
		else		// NaN
		{
			m >>= 13;
			return s | 0x7C00 | m | (m == 0);
		}
	}
	else
	{
		if(e > 30)	// Overflow
		{
			return s | 0x7C00;
		}

		return s | (e << 10) | (m >> 13);
	}
}

inline UINT16 RuRealToHalf(REAL i)
{
	union { REAL f; UINT32 i; } v;
	v.f = i;
	return RuRealToHalfI(v.i);
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Static::CRuMeshInstantiator_Static(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		{ 0,	24,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Static::~CRuMeshInstantiator_Static()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Static::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Static::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Static *vertices = (CRuVertex_Static *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const float *texCoords = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	if(morphTarget)
	{
		const CRuVector3 *position = morphTarget->GetPosition();
		const CRuVector3 *normal = morphTarget->GetNormal();

		if(normal && texCoords)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else if(normal)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else if(texCoords)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = 0.0f;
				vertices[i].ny = 0.0f;
				vertices[i].nz = 0.0f;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = 0.0f;
				vertices[i].ny = 0.0f;
				vertices[i].nz = 0.0f;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_StaticColored::CRuMeshInstantiator_StaticColored(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },	// Position
		{ 0,	12,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },	// Packed normal
		{ 0,	16,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },	// Diffuse Color
		{ 0,	20,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },	// UV0 (Diffuse)
		{ 0,	28,	ruELEMENTTYPE_SHORT2N,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	1 },	// UV1 (Lightmap)
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_StaticColored::~CRuMeshInstantiator_StaticColored()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_StaticColored::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_StaticColored::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Static_Colored *vertices = (CRuVertex_Static_Colored *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const float *texCoords = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);
	const float *uv1 = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_LIGHTMAP);

	if(uv1 == NULL)
	{
		uv1 = texCoords;
	}

	if(morphTarget)
	{
		const CRuVector3 *position = morphTarget->GetPosition();
		const CRuVector3 *normal = morphTarget->GetNormal();
		const DWORD *diffuse = morphTarget->GetDiffuse();

		if(normal && texCoords && diffuse)
		{
			for(INT32 i = 0, j = 0, k = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (normal[i].x * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (normal[i].y * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (normal[i].z * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = diffuse[i];

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];

				vertices[i].u1 = static_cast<INT32>(uv1[k++] * 32767);
				vertices[i].v1 = static_cast<INT32>(uv1[k++] * 32767);
			}
		}
		else if(normal && diffuse)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (normal[i].x * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (normal[i].y * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (normal[i].z * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = diffuse[i];

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;

				vertices[i].u1 = 0;
				vertices[i].v1 = 0;
			}
		}
		else if(normal && texCoords)
		{
			for(INT32 i = 0, j = 0, k = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (normal[i].x * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (normal[i].y * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (normal[i].z * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = 0xFFFFFFFF;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];

				vertices[i].u1 = static_cast<INT32>(uv1[k++] * 32767);
				vertices[i].v1 = static_cast<INT32>(uv1[k++] * 32767);
			}
		}
		else if(diffuse && texCoords)
		{
			for(INT32 i = 0, j = 0, k = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (normal[i].x * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (normal[i].y * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (normal[i].z * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = diffuse[i];

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];

				vertices[i].u1 = static_cast<INT32>(uv1[k++] * 32767);
				vertices[i].v1 = static_cast<INT32>(uv1[k++] * 32767);
			}
		}
		else if(normal)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (normal[i].x * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (normal[i].y * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (normal[i].z * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = 0xFFFFFFFF;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;

				vertices[i].u1 = 0;
				vertices[i].v1 = 0;
			}
		}
		else if(texCoords)
		{
			for(INT32 i = 0, j = 0, k = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (1.0f * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = 0xFFFFFFFF;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];

				vertices[i].u1 = static_cast<INT32>(uv1[k++] * 32767);
				vertices[i].v1 = static_cast<INT32>(uv1[k++] * 32767);
			}
		}
		else if(diffuse)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (1.0f * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = diffuse[i];

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;

				vertices[i].u1 = 0;
				vertices[i].v1 = 0;
			}
		}
		else
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].ny = (BYTE) (1.0f * 127.0f + 127.0f);
				vertices[i].nz = (BYTE) (0.0f * 127.0f + 127.0f);
				vertices[i].nw = 0;

				vertices[i].color0 = 0xFFFFFFFF;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;

				vertices[i].u1 = 0;
				vertices[i].v1 = 0;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_StaticColoredOld::CRuMeshInstantiator_StaticColoredOld(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		{ 0,	24,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	28,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_StaticColoredOld::~CRuMeshInstantiator_StaticColoredOld()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_StaticColoredOld::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_StaticColoredOld::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Static_ColoredOld *vertices = (CRuVertex_Static_ColoredOld *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const float *texCoords = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	if(morphTarget)
	{
		const CRuVector3 *position = morphTarget->GetPosition();
		const CRuVector3 *normal = morphTarget->GetNormal();
		const DWORD *diffuse = morphTarget->GetDiffuse();

		if(normal && texCoords && diffuse)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else if(normal && diffuse)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else if(normal && texCoords)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else if(diffuse && texCoords)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = 0.0f;
				vertices[i].ny = 1.0f;
				vertices[i].nz = 0.0f;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else if(normal)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = normal[i].x;
				vertices[i].ny = normal[i].y;
				vertices[i].nz = normal[i].z;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else if(texCoords)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = 0.0f;
				vertices[i].ny = 1.0f;
				vertices[i].nz = 0.0f;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = texCoords[j++];
				vertices[i].v = texCoords[j++];
			}
		}
		else
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].nx = 0.0f;
				vertices[i].ny = 1.0f;
				vertices[i].nz = 0.0f;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Prelit::CRuMeshInstantiator_Prelit(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		{ 0,	16,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Prelit::~CRuMeshInstantiator_Prelit()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Prelit::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Prelit::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Static_Prelit *vertices = (CRuVertex_Static_Prelit *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const CRuVector3 *position = morphTarget->GetPosition();
	const DWORD *diffuse = morphTarget->GetDiffuse();
	const float *uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	if(morphTarget)
	{
		if(position && diffuse && uv)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = uv[j++];
				vertices[i].v = uv[j++];
			}
		}
		else if(position && diffuse)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else if(position && uv)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = uv[j++];
				vertices[i].v = uv[j++];
			}
		}
		else if(position)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = 0xFFFFFFFF;

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else if(diffuse)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = 0.0f;
				vertices[i].y = 0.0f;
				vertices[i].z = 0.0f;

				vertices[i].diffuse = diffuse[i];

				vertices[i].u = 0.0f;
				vertices[i].v = 0.0f;
			}
		}
		else
		{
			OutputDebugString("Completely invalid mesh... what the fuck... this piece of shit doesn't even have position!\n");
		}


	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_PrelitNoTex::CRuMeshInstantiator_PrelitNoTex(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	12,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_PrelitNoTex::~CRuMeshInstantiator_PrelitNoTex()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_PrelitNoTex::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_PrelitNoTex::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Bare_Colored *vertices = (CRuVertex_Bare_Colored *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const CRuVector3 *position = morphTarget->GetPosition();
	const DWORD *diffuse = morphTarget->GetDiffuse();

	if(morphTarget)
	{
		if(position && diffuse)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = diffuse[i];
			}
		}
		else if(position)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				vertices[i].x = position[i].x;
				vertices[i].y = position[i].y;
				vertices[i].z = position[i].z;

				vertices[i].diffuse = 0xFFFFFFFF;
			}
		}
		else
		{
			OutputDebugString("Hmm 22222m\n");
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Skinned::CRuMeshInstantiator_Skinned(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,		0 },
		{ 0,	12,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,			0 },
		{ 0,	16,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,			0 },
		{ 0,	20,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDINDICES,	0 },
		{ 0,	24,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_BLENDWEIGHT,		0 },
		{ 0,	28,	ruELEMENTTYPE_SHORT2N,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Skinned::~CRuMeshInstantiator_Skinned()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Skinned::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Skinned::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Skin *vertices = (CRuVertex_Skin *) buffer;

	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	const DWORD *diffuse = morphTarget->GetDiffuse();
	const UINT16 *blendIndices = morphTarget->GetBlendIndex();
	const float *blendWeights = morphTarget->GetBlendWeight();
	const float *texCoords = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

	if(morphTarget)
	{
		for(INT32 i = 0, j = 0, k = 0, l = 0; i < mesh->GetNumVertices(); i++)
		{
			vertices[i].x = morphTarget->GetPosition()[i].x;
			vertices[i].y = morphTarget->GetPosition()[i].y;
			vertices[i].z = morphTarget->GetPosition()[i].z;

			vertices[i].nx = (BYTE) (morphTarget->GetNormal()[i].x * 127.0f + 127.0f);
			vertices[i].ny = (BYTE) (morphTarget->GetNormal()[i].y * 127.0f + 127.0f);
			vertices[i].nz = (BYTE) (morphTarget->GetNormal()[i].z * 127.0f + 127.0f);
			vertices[i].nw = 0;

			if(diffuse)
			{
				vertices[i].diffuse = diffuse[i];
			}
			else
			{
				vertices[i].diffuse = 0xFFFFFFFF;
			}

			if(blendIndices)
			{
				vertices[i].i0 = (BYTE) blendIndices[k++];
				vertices[i].i1 = (BYTE) blendIndices[k++];
				vertices[i].i2 = (BYTE) blendIndices[k++];
				vertices[i].i3 = (BYTE) blendIndices[k++];
			}
			else
			{
				vertices[i].i0 = 0;
				vertices[i].i1 = 0;
				vertices[i].i2 = 0;
				vertices[i].i3 = 0;
			}

			if(blendWeights)
			{
				// Calculate rounding error while converting to a UBYTE4 weight format and compensate
				INT32 weights[4] = { (BYTE) (blendWeights[l++] * 255.0f), (BYTE) (blendWeights[l++] * 255.0f), (BYTE) (blendWeights[l++] * 255.0f), (BYTE) (blendWeights[l++] * 255.0f) };
				INT32 weightError = 255 - (weights[0] + weights[1] + weights[2] + weights[3]);

				if(weightError > 0)
				{
					if(weights[0] + weightError <= 255)
					{
						weights[0] += weightError;
					}
					else
					{
						weights[1] += weightError;
					}
				}

				vertices[i].w0 = weights[0];
				vertices[i].w1 = weights[1];
				vertices[i].w2 = weights[2];
				vertices[i].w3 = weights[3];
			}
			else
			{
				vertices[i].w0 = 255;
				vertices[i].w1 = 0;
				vertices[i].w2 = 0;
				vertices[i].w3 = 0;
			}

			INT32 arf = vertices[i].w0 + vertices[i].w1 + vertices[i].w2 + vertices[i].w3;

			if(texCoords)
			{
				vertices[i].u = (INT16) (texCoords[j++] * 32767);
				vertices[i].v = (INT16) (texCoords[j++] * 32767);
			}
			else
			{
				vertices[i].u = 0;
				vertices[i].v = 0;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_CompressedTerrain::CRuMeshInstantiator_CompressedTerrain(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		{ 0,	4,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_CompressedTerrain::~CRuMeshInstantiator_CompressedTerrain()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_CompressedTerrain::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_CompressedTerrain::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVertex_Terrain_Packed *vertices = (CRuVertex_Terrain_Packed *) buffer;
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
	UINT16 *compressedVerts = morphTarget->GetBlendIndex();
	const DWORD *diffuse = morphTarget->GetDiffuse();

	if(morphTarget)
	{
		for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
		{
			vertices[i].x = (BYTE) (compressedVerts[j++]);
			vertices[i].z = (BYTE) (compressedVerts[j++]);
			vertices[i].yH = (BYTE) (compressedVerts[j++]);
			vertices[i].yL = (BYTE) (compressedVerts[j++]);

			if(diffuse)
			{
				vertices[i].diffuse = diffuse[i];
			}
		}
	}
/*
	if(streamIndex == 0)
	{
		CRuVertex_Terrain_Packed *vertices = (CRuVertex_Terrain_Packed *) buffer;
		IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);
		const DWORD *diffuse = morphTarget->GetDiffuse();

		if(morphTarget)
		{
			for(INT32 i = 0, j = 0; i < mesh->GetNumVertices(); i++)
			{
				UINT16 discreteY = (UINT16) ((morphTarget->GetPosition()[i].y - m_chunk_Y_Base) / m_chunk_Y_Increment);

				vertices[i].x = (BYTE) (morphTarget->GetPosition()[i].x / m_chunk_Spacing);
				vertices[i].z = (BYTE) (morphTarget->GetPosition()[i].z / m_chunk_Spacing);
				vertices[i].yH = discreteY / 255;
				vertices[i].yL = discreteY % 255;

				if(diffuse)
				{
					vertices[i].diffuse = diffuse[i];
				}
			}
		}
	}
*/
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Position::CRuMeshInstantiator_Position(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_POSITION,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Position::~CRuMeshInstantiator_Position()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Position::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Position::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVector3 *positionOut = reinterpret_cast<CRuVector3 *>(buffer);
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);

	if(morphTarget)
	{
		const CRuVector3 *position = morphTarget->GetPosition();

		if(position)
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				positionOut[i].m_x = position[i].m_x;
				positionOut[i].m_y = position[i].m_y;
				positionOut[i].m_z = position[i].m_z;
			}
		}
		else
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				positionOut[i].m_x = 0.0f;
				positionOut[i].m_y = 0.0f;
				positionOut[i].m_z = 0.0f;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Normal::CRuMeshInstantiator_Normal(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT3,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Normal::~CRuMeshInstantiator_Normal()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Normal::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Normal::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	CRuVector3 *normalOut = reinterpret_cast<CRuVector3 *>(buffer);
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);

	if(morphTarget)
	{
		const CRuVector3 *normal = morphTarget->GetNormal();

		if(normal)
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				normalOut[i].m_x = normal[i].m_x;
				normalOut[i].m_y = normal[i].m_y;
				normalOut[i].m_z = normal[i].m_z;
			}
		}
		else
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				normalOut[i].m_x = 0.0f;
				normalOut[i].m_y = 1.0f;
				normalOut[i].m_z = 0.0f;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Normal_UBYTE4::CRuMeshInstantiator_Normal_UBYTE4(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_UBYTE4,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_NORMAL,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Normal_UBYTE4::~CRuMeshInstantiator_Normal_UBYTE4()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Normal_UBYTE4::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Normal_UBYTE4::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	BYTE *normalOut = reinterpret_cast<BYTE *>(buffer);
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);

	if(morphTarget)
	{
		const CRuVector3 *normal = morphTarget->GetNormal();

		if(normal)
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				normalOut[j++] = (BYTE) (normal[i].x * 127.0f + 127.0f);
				normalOut[j++] = (BYTE) (normal[i].y * 127.0f + 127.0f);
				normalOut[j++] = (BYTE) (normal[i].z * 127.0f + 127.0f);
				normalOut[j++] = 0;
			}
		}
		else
		{
			for(INT32 i = 0, j = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				normalOut[j++] = (BYTE) (0.0f * 127.0f + 127.0f);
				normalOut[j++] = (BYTE) (1.0f * 127.0f + 127.0f);
				normalOut[j++] = (BYTE) (0.0f * 127.0f + 127.0f);
				normalOut[j++] = 0;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_Diffuse::CRuMeshInstantiator_Diffuse(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_RUCOLOR,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_COLOR,		0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_Diffuse::~CRuMeshInstantiator_Diffuse()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_Diffuse::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_Diffuse::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	DWORD *diffuseOut = reinterpret_cast<DWORD *>(buffer);
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);

	if(morphTarget)
	{
		const DWORD *diffuse = morphTarget->GetDiffuse();

		if(diffuse)
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				diffuseOut[i] = diffuse[i];
			}
		}
		else
		{
			for(INT32 i = 0, numVertices = mesh->GetNumVertices(); i < numVertices; i++)
			{
				diffuseOut[i] = 0xFFFFFFFF;
			}
		}
	}
}

// ************************************************************************************************************************************************************

CRuMeshInstantiator_UV0::CRuMeshInstantiator_UV0(IRuEngine *engine)
:	m_engine(engine),
	m_vertexStreamLayout(NULL)
{
	CRuVertexElement vertexElements[] =
	{
		{ 0,	0,	ruELEMENTTYPE_FLOAT2,		ruELEMENTMETHOD_DEFAULT,	ruELEMENTUSAGE_TEXCOORD,	0 },
		RUVERTEXDECL_END()
	};

	// Construct vertex declaration
	m_vertexStreamLayout = m_engine->CreateVertexStreamLayout(vertexElements, 0);
}

CRuMeshInstantiator_UV0::~CRuMeshInstantiator_UV0()
{
	ruSAFE_RELEASE(m_vertexStreamLayout);
}

const IRuVertexStreamLayout *CRuMeshInstantiator_UV0::GetVertexStreamLayout() const
{
	return m_vertexStreamLayout;
}

void CRuMeshInstantiator_UV0::Instantiate(IRuMesh *mesh, UINT32 morphTargetIndex, void *buffer) const
{
	REAL *uvOut = reinterpret_cast<REAL *>(buffer);
	IRuMorphTarget *morphTarget = mesh->GetMorphTarget(morphTargetIndex);

	if(morphTarget)
	{
		const float *uv = morphTarget->GetTextureCoordinate(ruTEXCHANNEL_DIFFUSEMAP);

		if(uv)
		{
			for(INT32 i = 0, numUVComponents = mesh->GetNumVertices() * 2; i < numUVComponents; ++i)
			{
				uvOut[i] = uv[i];
			}
		}
		else
		{
			for(INT32 i = 0, numUVComponents = mesh->GetNumVertices() * 2; i < numUVComponents; ++i)
			{
				uvOut[i] = 0.0f;
			}
		}
	}
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
