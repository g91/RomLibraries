#include <windows.h>
#include "textureimage.h"

#define CLEAR_OLD_TEXTURE_TIME				60.0f

using namespace std;

set<CTextureImage*>			CTextureImage::s_haveRuTexture;

//--------------------------------------------------------------------------------------
CTextureImage::CTextureImage()
{
	m_fileNameStr			= NULL;
	m_isRightPath			= false;
	m_pRuTexture			= NULL;
	m_textureWidth			= 16;
	m_textureHeight			= 16;
	m_pExtTexture			= NULL;
}


//--------------------------------------------------------------------------------------
CTextureImage::~CTextureImage()
{
	Release();
}

//--------------------------------------------------------------------------------------
void CTextureImage::Initialize()
{
	s_haveRuTexture.clear();
}

//--------------------------------------------------------------------------------------
void CTextureImage::Shutdown()
{
	s_haveRuTexture.clear();
}


//--------------------------------------------------------------------------------------
void CTextureImage::SetRuTexture( IRuTexture* pTexture )
{
	m_pExtTexture = pTexture;
}

//--------------------------------------------------------------------------------------
IRuTexture* CTextureImage::GetRuTexture()
{
	return m_pRuTexture;
}

//--------------------------------------------------------------------------------------
inline BOOL CTextureImage::BeginRender(bool allowBinding)
{	
	if ( m_pRuTexture == NULL && !LoadRuTexture() )
		return FALSE;

	// 設定貼圖
	g_ruUIRenderer->SetTexture(m_pRuTexture, allowBinding);
	return TRUE;
}

//--------------------------------------------------------------------------------------
inline void CTextureImage::SetAlphaMode( DWORD alphaMode )
{
	// 設定貼圖效果
	switch ( alphaMode )
	{
	case ALPHA_MODE_DISABLE:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_FLAT);
		break;
	case ALPHA_MODE_BLEND:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_FILTER);
		break;
	case ALPHA_MODE_ADD:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_ADD);
		break;
	case ALPHA_MODE_LUMINANCE:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_LUMINANCE);
		break;
	case ALPHA_MODE_MASK:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_MASK);
		break;
	case ALPHA_MODE_MASKED:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_MASKED);
		break;
	case ALPHA_MODE_MASK_Z:
		g_ruUIRenderer->SetEffectType(ru2DFXTYPE_BLIT_MASK_Z);
		break;
	}
}

//--------------------------------------------------------------------------------------
inline void CTextureImage::SetEffectParameter( TextureEffectParameter effectParam, DWORD parameter )
{
	switch ( effectParam )
	{
	case TXPARAM_STRENGTH:
		g_ruUIRenderer->SetEffectParameter( ru2DFXPARAM_STRENGTH, parameter );
		break;
	case TXPARAM_COLOR:
		g_ruUIRenderer->SetEffectParameter( ru2DFXPARAM_COLOR, parameter );
		break;
	case TXPARAM_ROTATE:
		g_ruUIRenderer->SetEffectParameter( ru2DFXPARAM_ROTATE, parameter );
		break;
	}
}

//--------------------------------------------------------------------------------------
inline void CTextureImage::Submit(CRuRectangle &destRect, CRuRectangle &texCoords, DWORD color)
{
	if ( m_pRuTexture )
	{
		CRuRectangle srcRect = texCoords;

		srcRect.m_left		*= m_textureWidth;
		srcRect.m_right		*= m_textureWidth;
		srcRect.m_top		*= m_textureHeight;
		srcRect.m_bottom	*= m_textureHeight;

		g_ruUIRenderer->Submit(1, &srcRect, &destRect, &color);	// 繪製區塊
	}
}

//--------------------------------------------------------------------------------------
inline void CTextureImage::Submit(float x, float y, DWORD color)
{
	if ( m_pRuTexture )
	{
		CRuRectangle dest, src;

		dest.m_left		= x;
		dest.m_top		= y;
		dest.m_right	= x + m_textureWidth;
		dest.m_bottom	= y + m_textureHeight;

		src.m_left		= 0.0f;
		src.m_right		= (float)m_textureWidth;
		src.m_top		= 0.0f;
		src.m_bottom	= (float)m_textureHeight;

		g_ruUIRenderer->Submit(1, &src, &dest, &color);			// 繪製區塊
	}
}

//--------------------------------------------------------------------------------------
void CTextureImage::Release()
{
	set<CTextureImage*>::iterator iter = s_haveRuTexture.find( this );
	if ( iter != s_haveRuTexture.end() )
		s_haveRuTexture.erase( iter );

	ReleaseRuTexture();

	if ( m_fileNameStr )
	{
		delete [] m_fileNameStr;
		m_fileNameStr = NULL;
	}

	m_isRightPath	= false;
}

//--------------------------------------------------------------------------------------
void CTextureImage::Draw( CRuRectangle &destRect, CRuRectangle &texCoords, DWORD alphaMode, DWORD color )
{	
	if ( BeginRender() )
	{		
		SetAlphaMode(alphaMode);
		SetEffectParameter(TXPARAM_ROTATE, TRM_NONE);
		Submit(destRect, texCoords, color);
	}
}

//--------------------------------------------------------------------------------------
void CTextureImage::Draw(float x, float y, DWORD alphaMode, DWORD color)
{
	if ( BeginRender() )
	{
		SetAlphaMode(alphaMode);
		SetEffectParameter(TXPARAM_ROTATE, TRM_NONE);
		Submit(x, y, color);
	}
}

//--------------------------------------------------------------------------------------
void CTextureImage::SetFileName( const char *fileNameStr )
{
	if ( m_fileNameStr && fileNameStr && strcmp(m_fileNameStr, fileNameStr) == 0 )
		return;

	ReleaseRuTexture();
	if ( m_fileNameStr )
	{
		delete [] m_fileNameStr;
		m_fileNameStr = NULL;
	}

	if ( fileNameStr )
	{
		int len = (int)strlen(fileNameStr);

		if ( len > 256 )
		{
			m_isRightPath = FALSE;
		}
		else
		{
			m_isRightPath = TRUE;
			m_fileNameStr = new char[strlen(fileNameStr) + 1];
			strcpy( m_fileNameStr, fileNameStr );
		}
	}
}

//--------------------------------------------------------------------------------------
void CTextureImage::RemoveTexture()
{
	if ( m_pRuTexture ) {
		g_ruResourceManager->RemoveTexture(m_pRuTexture);
		m_pRuTexture = NULL;
		//ReleaseRuTexture();
	}
}

//--------------------------------------------------------------------------------------
BOOL CTextureImage::GetSize( SIZE &size )
{
	if ( m_pRuTexture )
	{
		size.cx = m_textureWidth;
		size.cy = m_textureHeight;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------
const CTextureImage& CTextureImage::operator=( const CTextureImage& src )
{
	SetFileName( src.m_fileNameStr );
	return *this;
}

//--------------------------------------------------------------------------------------
bool CTextureImage::CheckMem()
{
	if ( s_haveRuTexture.size() >= MAX_TEXTUREIMAGE )
	{
		static bool flag = true;

		set<CTextureImage*>::iterator iterSta = s_haveRuTexture.begin();
		int count = (int)s_haveRuTexture.size() / 2;
		if ( !flag )
		{
			for ( int i = 0; i < count; i++ )
				iterSta++;
		}

		flag = !flag;
		set<CTextureImage*>::iterator iterEnd = iterSta;

		for ( int i = 0; i < count; i++, iterEnd++ )
		{
			(*iterEnd)->ReleaseRuTexture();
		}

		s_haveRuTexture.erase( iterSta, iterEnd );
	}

	return true;
}

//--------------------------------------------------------------------------------------
inline void CTextureImage::ReleaseRuTexture()
{
	if ( m_pRuTexture )
	{
		m_pRuTexture->Release();		
		m_pRuTexture = NULL;
	}	

	//OutputDebugStringA( "CTextureImage::ReleaseRuTexture\n" );

}

//--------------------------------------------------------------------------------------
inline BOOL CTextureImage::LoadRuTexture()
{
	if( m_pExtTexture )
	{
		//OutputDebugStringA( "CTextureImage::LoadRuTexture\n" );
		ReleaseRuTexture();

		m_pRuTexture = m_pExtTexture;
		m_textureWidth = m_pRuTexture->GetLevelWidth( 0 );
		m_textureHeight = m_pRuTexture->GetLevelHeight( 0 );

		//pair<set<CTextureImage*>::iterator, bool> hr;
		//hr = s_haveRuTexture.insert( this );	

		//assert( m_pRuTexture != NULL );
		return TRUE;
	}

	if ( m_isRightPath && CheckMem() )
	{
		ReleaseRuTexture();

		if ( m_fileNameStr && m_fileNameStr[0] ) {
			// 取得貼圖
			if ( (m_pRuTexture = g_ruResourceManager->LoadTexture( m_fileNameStr )) != NULL )
			{
				m_textureWidth = m_pRuTexture->GetLevelWidth( 0 );
				m_textureHeight = m_pRuTexture->GetLevelHeight( 0 );

				pair<set<CTextureImage*>::iterator, bool> hr;
				hr = s_haveRuTexture.insert( this );			
				return TRUE;
			}
#ifdef KALYDO
			else
			{
				// wait for the download
				return FALSE;
			}
#endif // KALYDO
		}

		m_isRightPath = FALSE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------
CTextureBackdrop::CTextureBackdrop()
{
	m_tileSize = 0.0f;
	m_edgeSize = 0.0f;
}

//--------------------------------------------------------------------------------------
CTextureBackdrop::~CTextureBackdrop()
{
}

//--------------------------------------------------------------------------------------
void CTextureBackdrop::Draw(CRuRectangle &destRect, DWORD tileColor, DWORD edgeColor)
{	
	CRuRectangle texCoords(0.0f, 0.0f, 1.0f, 1.0f);	
	CRuRectangle insideRect;
	
	// Draw Tile
	if ( m_tileSize > 0.0f && m_tileTexture.BeginRender(true) )
	{
		CRuRectangle tileRect;
		insideRect.m_left	= destRect.m_left + m_backgroundOffset.m_left;
		insideRect.m_top	= destRect.m_top + m_backgroundOffset.m_top;
		insideRect.m_right	= destRect.m_right - m_backgroundOffset.m_right;
		insideRect.m_bottom	= destRect.m_bottom - m_backgroundOffset.m_bottom;
		
		m_tileTexture.SetAlphaMode(ALPHA_MODE_BLEND);
		m_tileTexture.SetEffectParameter(TXPARAM_ROTATE, TRM_NONE);
		m_tileTexture.Submit(insideRect, texCoords, tileColor);

		/*
		for ( FLOAT y = insideRect.m_top; y < insideRect.m_bottom; y += m_tileSize )
		{	
			tileRect.m_top = y;
			if ( y + m_tileSize > insideRect.m_bottom )
			{				
				tileRect.m_bottom = insideRect.m_bottom;
				texCoords.m_bottom = (insideRect.m_bottom - y) / m_tileSize;
			}
			else
				tileRect.m_bottom = y + m_tileSize;

			for ( FLOAT x = insideRect.m_left; x < insideRect.m_right; x += m_tileSize )
			{
				tileRect.m_left = x;
				if ( x + m_tileSize > insideRect.m_right )
				{				
					tileRect.m_right = insideRect.m_right;
					texCoords.m_right = (insideRect.m_right - x) / m_tileSize;
				}
				else
					tileRect.m_right = x + m_tileSize;

				m_tileTexture.Submit(tileRect, texCoords, tileColor);
			}
		}
		*/
	}

	// Draw Edge
	if ( m_edgeSize > 0.0f && m_edgeTexture.BeginRender(true) )
	{
		CRuRectangle edgeRect;
		insideRect.m_left		= destRect.m_left + m_edgeSize;
		insideRect.m_top		= destRect.m_top + m_edgeSize;
		insideRect.m_right		= destRect.m_right - m_edgeSize;
		insideRect.m_bottom		= destRect.m_bottom - m_edgeSize;
		m_edgeTexture.SetAlphaMode(ALPHA_MODE_BLEND);
		m_edgeTexture.SetEffectParameter(TXPARAM_ROTATE, TRM_NONE);
		texCoords.Set( 0.0f, 0.0f, 1.0f, 1.0f );
		
		// left & right
		for ( FLOAT y = insideRect.m_top; y < insideRect.m_bottom; y += m_edgeSize )
		{
			edgeRect.m_top = y;
			if ( y + m_edgeSize < insideRect.m_bottom )
				edgeRect.m_bottom = y + m_edgeSize;
			else				
				edgeRect.m_bottom = insideRect.m_bottom;		
			
			edgeRect.m_left		= destRect.m_left;
			edgeRect.m_right	= destRect.m_left + m_edgeSize;
			texCoords.m_left	= 0.0f;
			texCoords.m_right	= 0.125f;
			m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );

			edgeRect.m_left		= destRect.m_right - m_edgeSize;
			edgeRect.m_right	= destRect.m_right;
			texCoords.m_left	= 0.125f;
			texCoords.m_right	= 0.25f;
			m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );
		}

		// top & bottom
		//m_edgeTexture.SetEffectParameter(TXPARAM_ROTATE, TRM_CLOCKWISE_90);
		for ( FLOAT x = insideRect.m_left; x < insideRect.m_right; x += m_edgeSize )
		{
			edgeRect.m_left	= x;
			if ( x + m_edgeSize < insideRect.m_right )
				edgeRect.m_right = x + m_edgeSize;
			else
				edgeRect.m_right = insideRect.m_right;
			
			edgeRect.m_top		= destRect.m_top;
			edgeRect.m_bottom	= destRect.m_top + m_edgeSize;				
			texCoords.m_left	= 0.25f;
			texCoords.m_right	= 0.375f;
			m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );

			edgeRect.m_top		= destRect.m_bottom - m_edgeSize;
			edgeRect.m_bottom	= destRect.m_bottom;
			texCoords.m_left	= 0.375f;
			texCoords.m_right	= 0.5f;
			m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );
		}

		// top left
		m_edgeTexture.SetEffectParameter( TXPARAM_ROTATE, TRM_NONE );
		edgeRect.Set( destRect.m_left, destRect.m_top, destRect.m_left + m_edgeSize, destRect.m_top + m_edgeSize );
		texCoords.m_left	= 0.5f;
		texCoords.m_right	= 0.625f;
		m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );

		// top right
		edgeRect.Set( destRect.m_right - m_edgeSize, destRect.m_top, destRect.m_right, destRect.m_top + m_edgeSize );
		texCoords.m_left	= 0.625f;
		texCoords.m_right	= 0.75f;
		m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );

		// bottom left
		edgeRect.Set( destRect.m_left, destRect.m_bottom - m_edgeSize, destRect.m_left + m_edgeSize, destRect.m_bottom );
		texCoords.m_left	= 0.75f;
		texCoords.m_right	= 0.875f;
		m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );

		// bottom right
		edgeRect.Set( destRect.m_right - m_edgeSize, destRect.m_bottom - m_edgeSize, destRect.m_right, destRect.m_bottom );
		texCoords.m_left	= 0.875f;
		texCoords.m_right	= 1.0f;
		m_edgeTexture.Submit( edgeRect, texCoords, edgeColor );
	}
}

//--------------------------------------------------------------------------------------
const CTextureBackdrop& CTextureBackdrop::operator=( const CTextureBackdrop& src )
{
	m_tileTexture		= src.m_tileTexture;
	m_edgeTexture		= src.m_edgeTexture;
	m_backgroundOffset	= src.m_backgroundOffset;
	m_tileSize			= src.m_tileSize;
	m_edgeSize			= src.m_edgeSize;
	return *this;
}