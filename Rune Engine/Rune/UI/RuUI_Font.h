#pragma once

#include <map>
#include <vector>
#include <list>
#include <string>

#include "../Engine/Base/RuCore.h"

#include "../Renderer/RuRenderer.h"
#include "../Renderer/RendererNode/RuRendererNode_UI.h"

#pragma managed(push, off)

using namespace std;

// --------------------------------------------------------------------------------------
class CRuFontType;
class CRuFontTexture;
class CRuFontTexture2;
class CRuFontChar;
class CRuFontString;

// --------------------------------------------------------------------------------------
//
// 字型陰影參數
typedef struct
{
	DWORD color;
	short x, y;
} RuFontShadow;

// --------------------------------------------------------------------------------------
// 字型寬度
enum RuFontWeight
{
	ruFONTWEIGHT_THIN		= 0,
	ruFONTWEIGHT_NORMAL,
	ruFONTWEIGHT_BOLD,
};

enum RuFontOutline
{
	ruFONTOUTLINE_NONE		= 0,
	ruFONTOUTLINE_NORMAL,
	ruFONTOUTLINE_THICK
};

typedef void (*RuFontTextureCallBack)	();

// --------------------------------------------------------------------------------------
struct RuFontBase
{	
	int						index;
	char*					fontStr;	
	RuFontOutline			outline;
	RuFontWeight			weight;

	// Windows-based font variables
	HFONT					hFont;

	// FreeType-based font variables
	CRuMemoryStream*		m_fontStream;
};

// --------------------------------------------------------------------------------------
class CRuFontType
{
public:
							CRuFontType();
	virtual					~CRuFontType();

	// static function
	static	void			Initialize();
	static	void			Shutdown();	
	static	CRuFontType*	GetFontType( const char *fontNameStr, int height, RuFontWeight weight, RuFontOutline outline );

	// member function
	void					PaintToMem(const WCHAR wchar, const WCHAR* toneSymbol, IRuTexture *dstTexture, INT32 x, INT32 y, SIZE &size);
	void					PaintToMem(WCHAR* text, int length, IRuTexture *dstTexture, INT32 x, INT32 y, SIZE &size);
	const char*				GetFontName()					{ return m_fontBase->fontStr; }
	RuFontWeight			GetWeight()						{ return m_fontBase->weight; }
	RuFontOutline			GetOutLine()					{ return m_fontBase->outline; }
	int						GetFontBaseIndex()				{ return m_fontBase->index; }
	int						GetSerial()						{ return m_serial; }
	int						GetHeight()						{ return m_height; }
	void					GetFontSize(WCHAR wchar, WCHAR* toneSymbol, SIZE& outSize);
	void					GetFontSize(const WCHAR* text, int length, SIZE& outSize);
	
	int						GetRealHeight()					{ return GetHeight(); }

protected:
	static	bool						s_useFreeType;
	static	CRuArrayList<CRuFontType*>	s_fontType;
	static	vector<RuFontBase*>			s_fontBase;

	// Windows-based font variables
	static	HDC							s_hDC;					//!< DC Handle
	static	HFONT						s_hCurrentFont;			//!< Font Handle
	static	HBITMAP						s_hbmBitmap;			//!< Bitmap Handle
	static	DWORD*						s_pBitmapBits;			//!< Bitmap Pointer
	static	CRuImageFX_Resampler_32		s_imageResampler;		//!< Image sampler

	// FreeType-based font variables
	static void*						s_fontLibrary;			//!< FreeType font library

	// Common variables
	RuFontBase*							m_fontBase;				//!< Base font
	int									m_serial;				//!< Serial number
	int									m_height;				//!< Font height

	// FreeType-based font variables
	void*								m_fontFace;				//!< FreeType font face handle
};

// --------------------------------------------------------------------------------------
class CRuFontTexture
{
public:
	CRuFontTexture();
	virtual ~CRuFontTexture();	

	// static function
	static	bool			Initialize( RuFontTextureCallBack func );
	static	void			Shutdown();
	static	void			ReleaseAllFontIndex();

	static	void			PaintBegin();
	static	int				GetFreeFontCapacity()				{ return s_freeFont.Count(); }
	static	IRuTexture*		GetRuTextureD3D()					{ return s_pRuTexture; }
	static	INT32			GetRuTextureWidth()					{ return s_pRuTexture->GetLevelWidth(0); }
	static	INT32			GetRuTextureHeight()				{ return s_pRuTexture->GetLevelHeight(0); }	
	static	CRuFontTexture*	GetRuFontTexture(CRuFontType* fontType, WCHAR wchar, WCHAR* toneSymbol=NULL);
	static	CRuFontTexture* GetBlockTexture()					{ return s_blockTexture; }

	// member function
	void					Clear();
	void					SetToneSymbol(const WCHAR* toneSymbol);
	CRuRectangle			GetRectangle()						{ return m_rect; }

protected:
	static	CRuFontTexture*	GetFreeFontTexture();
	static	bool			Recycle( CRuFontTexture* pTexture );
	static	void			Rally(bool removeAll=false);
	static	BOOL			RallyCallback(const void *key, const void *data, void *userData);

protected:
	static	CRuHashMap<INT64, CRuFontTexture*>	s_fontTexIndex;
	static	CRuStack<CRuFontTexture*>			s_freeFont;
	static	CRuFontTexture*						s_unitTexture;
	static	IRuTexture*							s_pRuTexture;
	static	bool								s_isLockRally;
	static	RuFontTextureCallBack				s_rallyFunc;
	static	DWORD*								s_pPixels;
	static	CRuArrayList<INT64>					s_removedFontTexList;
	static	CRuFontTexture*						s_blockTexture;
	
	WCHAR*										m_toneSymbol;
	CRuRectangle								m_rect;
};

// --------------------------------------------------------------------------------------
class CRuFontTexture2
{
public:
	CRuFontTexture2();
	virtual ~CRuFontTexture2();	

	// static function
	static	bool				Initialize( RuFontTextureCallBack func );
	static	void				Shutdown();
	static	void				ReleaseAllFontIndex();
	static	void				ReleaseFontTexture(CRuFontTexture2* fontTexture);

	static	void				PaintBegin();
	static	IRuTexture*			GetRuTextureD3D()					{ return s_pRuTexture; }
	static	INT32				GetRuTextureWidth()					{ return s_pRuTexture->GetLevelWidth(0); }
	static	INT32				GetRuTextureHeight()				{ return s_pRuTexture->GetLevelHeight(0); }		
	static	CRuFontTexture2*	GetBlockTexture();
	static	CRuFontTexture2*	GetRuFontTexture(CRuFontString* pFontString);

	// member function
	void						Release();
	CRuRectangle				GetRectangle()						{ return m_rect; }

protected:
	static	bool				GetFreeUnitTexture(int unitWidth, int &x, int &y);
	static	void				Rally(bool removeAll=false);

protected:
	static	IRuTexture*						s_pRuTexture;
	static	bool							s_isLockRally;
	static	RuFontTextureCallBack			s_rallyFunc;
	static	CRuFontTexture2*				s_blockTexture;
	static	CRuFontTexture2*				s_useBlock[32][128];
	static	int								s_blockXSize;
	static	int								s_blockYSize;
	static	int								s_rallyYLast;

	int										m_x;
	int										m_y;
	int										m_length;
	CRuRectangle							m_rect;
	CRuFontString*							m_pFontString;
};

// --------------------------------------------------------------------------------------
class CRuFontChar
{
public:
	CRuFontChar();
	virtual ~CRuFontChar();

	// static function
	static	void			Initialize( RuFontTextureCallBack func );
	static	void			Shutdown();
	static	int				GetToneSymbolNumber( WCHAR *toneSymbol );
	static	CRuFontChar*	GetFontChar( CRuFontType *pFontType, WCHAR wchar, WCHAR *toneSymbol=NULL );
	static	void			PaintBegin()			{ CRuFontTexture::PaintBegin(); }	
	static	bool			DrawText( CRuFontType* pFontType, float x, float y, int color, const WCHAR* textStr );
	static	bool			DrawTextA( CRuFontType* pFontType, float x, float y, int color, const char* textStr );	

	// member function
	void					Paint( float x, float y, float scale, DWORD color, RuFontShadow *pShadow = NULL, DWORD blockColor=0,bool bOutline = false );	
	const CRuFontType*		GetFontType()			{ return m_pFontType; }
	WCHAR					GetChar()				{ return m_char; }	
	float					GetXSize()				{ return m_x; }
	float					GetYSize()				{ return m_y; }
	CRuFontTexture*			GetFontTexture()		{ return CRuFontTexture::GetRuFontTexture(m_pFontType, m_char, m_toneSymbol); }

protected:
	static	CRuHashMap<INT64, CRuFontChar *>	s_fontChar;	
	CRuFontType*							m_pFontType;
	float									m_x, m_y;
	WCHAR									m_char;
	WCHAR*									m_toneSymbol;

	// Run-time
	CRuRectangle							m_runtime_SrcRects[6];
	CRuRectangle							m_runtime_DestRects[6];
	DWORD									m_runtime_Colors[6];
};

// --------------------------------------------------------------------------------------
class CRuFontString
{
public:
	CRuFontString();
	virtual ~CRuFontString();

	// static function
	static	void			Initialize( RuFontTextureCallBack func );
	static	void			Shutdown();
	static	CRuFontString*	GetFontString( CRuFontType *pFontType, const WCHAR* text, int length, BOOL isReleaseWhenTextureNull = FALSE );
	static	void			PaintBegin()			{ CRuFontTexture2::PaintBegin(); }
	static	bool			DrawText( CRuFontType* pFontType, float x, float y, int color, const WCHAR* textStr );
	static	bool			DrawTextA( CRuFontType* pFontType, float x, float y, int color, const char* textStr );
	//static	void			SplitString( const WCHAR* text, int length, list<wstring> &outVec );
	static	void			SplitString( const WCHAR* text, int size, vector<wstring> &outVec );
	static	void			ReleaseFontString(CRuFontString* pFontString);
	static	const WCHAR*	CharNext(const WCHAR* inString, int length = -1);

	// member function
	void					Paint( float x, float y, float scale, DWORD color, RuFontShadow *pShadow = NULL, DWORD blockColor = 0, bool bOutline = false, bool RTLChangeSign = false );
	void					SetText(const WCHAR* text, int length);
	CRuFontType*			GetFontType()			{ return m_pFontType; }
	WCHAR*					GetText()				{ return m_text; }
	int						GetTextLength()			{ return m_textLength; }
	float					GetXSize()				{ return m_x; }
	float					GetYSize()				{ return m_y; }
	bool					GetFontPaintSize(SIZE& size);
	CRuFontTexture2*		GetFontTexture();
	WORD					GetFirstWordType();
	int						Compare(const WCHAR* dest);	

protected:
	static map<wstring, CRuFontString*>		s_fontString;
	static bool								s_shutdownLocked;

	float									m_x, m_y;
	CRuFontType*							m_pFontType;
	CRuFontTexture2*						m_pFontTexture;
	WCHAR*									m_text;
	int										m_textLength;	
	BOOL									m_isReleaseSelf;
};

extern RUAPI char* TransToChar(const WCHAR* text);
extern RUAPI WCHAR* TransToWChar(const char* text);
extern RUAPI WCHAR* Utf8ToWChar(const char* text);
extern RUAPI WCHAR* MultiByteToWChar(const char* s);
extern RUAPI bool IsToneSymbol(WCHAR wch);

#pragma managed(pop)
