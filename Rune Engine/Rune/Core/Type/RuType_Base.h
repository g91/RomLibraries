/*!
	@file RuType_Base.h

	Copyright (c) 2004-2009 Runewaker Entertainment Corp. All rights reserved.
*/

#pragma once

#ifndef NULL
	#define NULL 0
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x0500
#endif

#ifndef _WIN32_WINDOWS
	#define _WIN32_WINDOWS 0x0400
#endif

#ifndef _WIN32_IE						// Allow use of features specific to IE 4.0 or later.
	#define _WIN32_IE 0x0500			// Change this to the appropriate value to target IE 5.0 or later.
#endif

// Disable useless and utterly lame warnings
#pragma warning(disable : 4793)														// Disable native code generation warnings
#pragma warning(disable : 4819)														// Disable character cannot be represented by current code page warning
#pragma warning(disable : 4845)														// Disable CLR warning about unable to import an already imported symbol (this is amazingly lame)
#pragma warning(disable : 4949)														// Disable #pragma unmanaged only having effect with CLR on warnings (god this is an amazingly retarded warning)
#pragma warning(disable : 4996)														// Disable deprecated warnings

#pragma managed(push, off)

#include <assert.h>
#include <windows.h>
#include <mmsystem.h>
#include <objbase.h>
#include <xmmintrin.h>
#include <shlobj.h>

// Required boost components
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

// Library includes
#pragma comment(lib, "winmm.lib")

// ************************************************************************************************************************************************************

#ifndef RUAPI
	#if defined(WIN32) || defined(_WIN32)
		#ifdef RU_DLL_EXPORTS
			#define RUAPI __declspec(dllexport)
		#else
			#define RUAPI
//			#define RUAPI __declspec(dllimport) <-- DLL headers must have this
		#endif
	#else
		#define RUAPI
	#endif
#endif

// ************************************************************************************************************************************************************

typedef signed __int32			INT;
typedef signed __int8			INT8;
typedef signed __int16			INT16;
typedef signed __int32			INT32;
typedef signed __int64			INT64;
typedef unsigned __int32		UINT;
typedef unsigned __int8			UINT8;
typedef unsigned __int16		UINT16;
typedef unsigned __int32		UINT32;
typedef unsigned __int64		UINT64;
typedef unsigned __int8			BYTE;
typedef unsigned __int16		WORD;
typedef unsigned long			DWORD;
typedef float					REAL;
typedef void*					POINTER;

#ifdef _WIN64
	typedef unsigned __int64	PTRVALUE;
#else
	typedef unsigned __int32	PTRVALUE;
#endif

// ************************************************************************************************************************************************************

typedef UINT32 RUCOLOR;				//!< A 32-bit packed color value in the format 0xAARRGGBB. Identical to D3DCOLOR.

// ************************************************************************************************************************************************************

/*!
	Base floating point color type.

	@author John Tang
	@version 2005.12.08
*/
class CRuColor4
{
public:
	union
	{
		struct
		{
			REAL					m_r;
			REAL					m_g;
			REAL					m_b;
			REAL					m_a;
		};

		REAL						m_color[4];										// Color component array as RGBA
	};

public:
								CRuColor4() : m_a(1.0f), m_r(1.0f), m_g(1.0f), m_b(1.0f) { }
								CRuColor4(RUCOLOR color) : m_a((color >> 24 & 0xFF) / 255.0f), m_r((color >> 16 & 0xFF) / 255.0f), m_g((color >> 8 & 0xFF) / 255.0f), m_b((color & 0xFF) / 255.0f) { }
								CRuColor4(const CRuColor4 &color) : m_a(color.m_a), m_r(color.m_r), m_g(color.m_g), m_b(color.m_b) { }
								CRuColor4(UINT8 a, UINT8 r, UINT8 g, UINT8 b) : m_a(a / 255.0f), m_r(r / 255.0f), m_g(g / 255.0f), m_b(b / 255.0f) { }
								CRuColor4(REAL a, REAL r, REAL g, REAL b) : m_a(a), m_r(r), m_g(g), m_b(b) { }

	inline CRuColor4&			Set(UINT8 a, UINT8 r, UINT8 g, UINT8 b) { m_a = a / 255.0f; m_r = r / 255.0f; m_g = g / 255.0f; m_b = b / 255.0f; return *this; }
	inline CRuColor4&			Set(REAL a, REAL r, REAL g, REAL b) { m_a = a; m_r = r; m_g = g; m_b = b; return *this; }
	inline CRuColor4&			Set(RUCOLOR color) { m_a = ((color >> 24) & 0xFF) / 255.0f; m_r = ((color >> 16) & 0xFF) / 255.0f; m_g = ((color >> 8) & 0xFF) / 255.0f; m_b = (color & 0xFF) / 255.0f; return *this; }

	inline CRuColor4&			Clamp() { m_a = m_a > 1.0f ? 1.0f : m_a; m_r = m_r > 1.0f ? 1.0f : m_r; m_g = m_g > 1.0f ? 1.0f : m_g; m_b = m_b > 1.0f ? 1.0f : m_b; return *this; };
	inline DWORD				GetARGB() const { return (((UINT32) (m_a * 255.0f) & 0xFF) << 24) | (((UINT32) (m_r * 255.0f) & 0xFF) << 16) | (((UINT32) (m_g * 255.0f) & 0xFF) << 8) | ((UINT32) (m_b * 255.0f) & 0xFF); }
	inline REAL*				GetFloatArray() { return m_color; }

	// Arithmetic operator overloads
	inline CRuColor4			operator*(const REAL multiplier) const { return CRuColor4(m_a * multiplier, m_r * multiplier, m_g * multiplier, m_b * multiplier); }
	inline CRuColor4			operator*(const CRuColor4 &color) const { return CRuColor4(m_a * color.m_a, m_r * color.m_r, m_g * color.m_g, m_b * color.m_b); }
	inline CRuColor4			operator+(const CRuColor4 &color) const { return CRuColor4(m_a + color.m_a, m_r + color.m_r, m_g + color.m_g, m_b + color.m_b); }
	inline CRuColor4			operator-(const CRuColor4 &color) const { return CRuColor4(m_a - color.m_a, m_r - color.m_r, m_g - color.m_g, m_b - color.m_b); }

	// Arithmetic update operator overloads
	inline CRuColor4&			operator*=(const REAL multiplier) { m_a *= multiplier; m_r *= multiplier; m_g *= multiplier; m_b *= multiplier; return *this; }
	inline CRuColor4&			operator/=(const REAL divisor) { m_a /= divisor; m_r /= divisor; m_g /= divisor; m_b /= divisor; return *this; }
	inline CRuColor4&			operator+=(const CRuColor4 &color) { m_a += color.m_a; m_r += color.m_r; m_g += color.m_g; m_b += color.m_b; return *this; }
	inline CRuColor4&			operator-=(const CRuColor4 &color) { m_a -= color.m_a; m_r -= color.m_r; m_g -= color.m_g; m_b -= color.m_b; return *this; }
};

// ************************************************************************************************************************************************************

/*!
	IRuReferenceCounted

	Base class for all reference counted objects. This is designed for single-inheritance
	object hierarchies only. Any use of multiple-inheritance should be limited to additional
	interfaces. A class should never inherit from two base classes deriving from
	IRuReferenceCounted.

	Subclasses can optionally implement the Dispose function. This function is called when
	the reference count reaches zero, ahead of the invocation of the destructor. This allows
	subclasses to perform any house cleaning that cannot be done in the destructor.

	@author Ceranos
	@version Initial
*/
class IRuReferenceCounted
{
protected:
	volatile LONG			m_refCount;												//!< Reference count

public:
							IRuReferenceCounted();									//!< Initial reference count is always 1
	virtual					~IRuReferenceCounted();									//!< Default destructor

	// Reference counting interface
	virtual INT32			AddRef();												//!< Increment reference count
	virtual INT32			Release();												//!< Decrement reference count
	virtual INT32			GetRefCount() const;									//!< Retrieve reference count

	// Disposal
	virtual void			Dispose();												//!< Called when object reference count reaches zero and is about to be destroyed
};

// ************************************************************************************************************************************************************

class CRuRTTI
{
protected:
	const char*				m_typeName;
	const CRuRTTI*			m_baseType;

public:
							CRuRTTI(const char *typeName, const CRuRTTI *baseType);

	// Type queries
	const char*				GetTypeName() const;
	BOOL					IsSubClassOf(const CRuRTTI &type) const;
	BOOL					operator==(const CRuRTTI &type) const;
	BOOL					operator!=(const CRuRTTI &type) const;

protected:
	// Prevent copying
							CRuRTTI(const CRuRTTI &obj);
	CRuRTTI&				operator=(const CRuRTTI & obj);
};

#define ruRTTI_DECLARE																										\
	public:																													\
		virtual const CRuRTTI&		GetType() const;																		\
		static const CRuRTTI&		Type();

#define ruRTTI_DEFINE_ROOTCLASS(name)																						\
	const CRuRTTI &name::GetType() const { return Type(); }																	\
	const CRuRTTI &name::Type() { static CRuRTTI classType(#name, NULL); return classType; }													

#define ruRTTI_DEFINE_SUBCLASS(name, parent)																				\
	const CRuRTTI &name::GetType() const { return Type(); }																	\
	const CRuRTTI &name::Type() { static CRuRTTI classType(#name, &parent::Type()); return classType; }

#define ruRTTI_DEFINE_SUBCLASS_TEMPLATE(name, templateParam, parent)														\
	template<class templateParam>																							\
	const CRuRTTI &name<templateParam>::GetType() const { return Type(); }													\
	template<class templateParam>																							\
	const CRuRTTI &name<templateParam>::Type() { static CRuRTTI classType(#name, &parent::Type()); return classType; }													

// ************************************************************************************************************************************************************

#pragma pack(1)

struct CDXT1Block
{
	UINT16					m_color0;
	UINT16					m_color1;
	UINT32					m_block;
};

#pragma pack()

// ************************************************************************************************************************************************************

inline IRuReferenceCounted::IRuReferenceCounted()
:	m_refCount(1)
{
}

inline IRuReferenceCounted::~IRuReferenceCounted()
{
}

inline INT32 IRuReferenceCounted::AddRef()
{
/*
	// Increment reference count
	++m_refCount;

	// Return reference count
	return m_refCount;
*/

	return InterlockedIncrement(&m_refCount);
}

inline INT32 IRuReferenceCounted::Release()
{
	// Decrement reference count
	INT32 newRefCount = InterlockedDecrement(&m_refCount);

	// Automatically delete the reference counted resource
	if(newRefCount <= 0)
	{
		// Call dispose function to allow object to take care of any clean-up before destruction
		Dispose();

		// Destroy object
		delete this;

		return 0;
	}

	return newRefCount;
/*
	// Decrement reference count
	--m_refCount;

	// Automatically delete the reference counted resource
	if(m_refCount <= 0)
	{
		// Call dispose function to allow object to take care of any clean-up before destruction
		Dispose();

		// Destroy object
		delete this;

		return 0;
	}

	return m_refCount;
*/
}

inline INT32 IRuReferenceCounted::GetRefCount() const
{
	return m_refCount;
}

inline void IRuReferenceCounted::Dispose()
{
}

// ************************************************************************************************************************************************************

inline CRuRTTI::CRuRTTI(const char *typeName, const CRuRTTI *baseType)
:	m_typeName(typeName),
	m_baseType(baseType)
{
}

inline const char *CRuRTTI::GetTypeName() const
{
	return m_typeName;
}

inline BOOL CRuRTTI::IsSubClassOf(const CRuRTTI &type) const
{
	const CRuRTTI *curType = this;

	while(curType != NULL)
	{
		if(curType == &type)
			return TRUE;

		curType = curType->m_baseType;
	}

	return FALSE;
}

inline BOOL CRuRTTI::operator==(const CRuRTTI &type) const
{
	return (this == &type);
}

inline BOOL CRuRTTI::operator!=(const CRuRTTI &type) const
{
	return (this != &type);
}

// ************************************************************************************************************************************************************

#pragma managed(pop)
