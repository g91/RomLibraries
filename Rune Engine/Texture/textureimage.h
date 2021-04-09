#ifndef _TEXTUREIMAGE_H
#define _TEXTUREIMAGE_H

#include <set>
#include "../Rune/Rune.h"

#define MAX_TEXTUREIMAGE			512

enum TextureAlphaMode {
	ALPHA_MODE_DISABLE			= 0,
	ALPHA_MODE_BLEND,
	ALPHA_MODE_ADD,
	ALPHA_MODE_LUMINANCE,
	ALPHA_MODE_MASK,
	ALPHA_MODE_MASKED,
	ALPHA_MODE_MASK_Z,

	ALPHA_MODE_FORCE_DWORD		= 0x7FFFFFFF
};

enum TextureEffectParameter {
	TXPARAM_STRENGTH			= 0,
	TXPARAM_COLOR,
	TXPARAM_ROTATE,

	TXPARAM_FORCE_DWORD			= 0x7FFFFFFF
};

enum TextureRotateMode {
	TRM_NONE					= ru2DROTATE_NONE,
	TRM_CLOCKWISE_90			= ru2DROTATE_CLOCKWISE_90,
	TRM_CLOCKWISE_180			= ru2DROTATE_CLOCKWISE_180,
	TRM_CLOCKWISE_270			= ru2DROTATE_CLOCKWISE_270,
	TRM_FLIP_U					= ru2DROTATE_FLIP_U,
	TRM_FLIP_V					= ru2DROTATE_FLIP_V,

	TRM_FORCE_DWORD				= 0x7FFFFFFF
};

// --------------------------------------------------------------------------------------
class CTextureImage
{
public:
	CTextureImage();
	virtual ~CTextureImage();

	static	void				Initialize();
	static	void				Shutdown();
	static	void				Update( FLOAT absoluteTime );
	static	int					LoadedCapacity()					{ return (int)s_haveRuTexture.size(); }

	inline BOOL					BeginRender( bool allowBinding = false );
	inline void					SetColor( DWORD color );
	inline void					SetAlphaMode( DWORD alphaMode );
	inline void 				SetEffectParameter( TextureEffectParameter effectParam, DWORD parameter );
	inline void					Submit(CRuRectangle &destRect, CRuRectangle &texCoords, DWORD color);
	inline void					Submit(float x, float y, DWORD color);

	void						Release();
	void						Draw(CRuRectangle &destRect, CRuRectangle &texCoords, DWORD alphaMode, DWORD color);
	void						Draw(float x, float y, DWORD alphaMode, DWORD color);	
	void						SetFileName( const char *fileNameStr );	
	void						RemoveTexture();

	BOOL						GetSize( SIZE &size );
	INT32						GetWidth()							{ return m_textureWidth; }
	INT32						GetHeight()							{ return m_textureHeight; }
	char*						GetFileName()						{ return m_fileNameStr; }	

	const CTextureImage&		operator=( const CTextureImage& src );

	void						SetRuTexture( IRuTexture* pTexture );
	IRuTexture*					GetRuTexture();



protected:
	static	bool				CheckMem();
	inline	void				ReleaseRuTexture();
	inline	BOOL				LoadRuTexture();

protected:
	static std::set<CTextureImage*>	s_haveRuTexture;

	char*						m_fileNameStr;
	bool						m_isRightPath;
	IRuTexture*					m_pRuTexture;
	INT32						m_textureWidth;
	INT32						m_textureHeight;

	IRuTexture*					m_pExtTexture;

};

// --------------------------------------------------------------------------------------
class CTextureBackdrop
{
public:
	CTextureBackdrop();
	~CTextureBackdrop();

	void					Draw(CRuRectangle &destRect, DWORD tileColor, DWORD edgeColor);
	void					SetTileFileName( const char *fileNamestr )	{ m_tileTexture.SetFileName( fileNamestr ); }
	void					SetEdgeFileName( const char *fileNamestr )	{ m_edgeTexture.SetFileName( fileNamestr ); }
	void					SetTileSize( FLOAT size )					{ m_tileSize = size; }
	void					SetEdgeSize( FLOAT size )					{ m_edgeSize = size; }
	void					SetBackgroundOffset( CRuRectangle rc )		{ m_backgroundOffset = rc; }
	CTextureImage&			GetTileTexture()							{ return m_tileTexture; }
	CTextureImage&			GetEdgeTexture()							{ return m_edgeTexture; }
	FLOAT					GetTileSize()								{ return m_tileSize; }
	FLOAT					GetEdgeSize()								{ return m_edgeSize; }
	const CTextureBackdrop&	operator=( const CTextureBackdrop& src );

protected:
	CTextureImage			m_tileTexture;
	CTextureImage			m_edgeTexture;
	CRuRectangle			m_backgroundOffset;
	FLOAT					m_tileSize;
	FLOAT					m_edgeSize;
};

#endif //_TEXTUREBASE_H