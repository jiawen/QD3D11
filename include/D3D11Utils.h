#ifndef D3D11UTILS_H
#define D3D11UTILS_H

#include <D3D11.h>
#include <D3DX11.h>
#include <d3dx11effect.h>

#include <QVector>
#include <DXGI.h>

#include <common/Reference.h>
#include <cameras/Camera.h>
#include <imageproc/Image1i.h>
#include <imageproc/Image1f.h>
#include <imageproc/Image4f.h>
#include <imageproc/Image4ub.h>

#include "DynamicVertexBuffer.h"
#include "StaticDataBuffer.h"
#include "StaticStructuredBuffer.h"
#include "VertexPosition4f.h"
#include "VertexPosition4fTexture2f.h"
#include "VertexPosition4fColor4f.h"
#include "VertexPosition4fNormal3fTexture2f.h"

class DynamicTexture2D;

class D3D11Utils
{
public:

	// Returns a QVector of DXGI adapter on this machine
	// Be sure to release the pointers
	// TODO: make a class for this that auto-releases?
	static QVector< IDXGIAdapter* > getDXGIAdapters();

	static D3D11_VIEWPORT createViewport( int width, int height );
	static D3D11_VIEWPORT createViewport( const Vector2i& wh );
	static D3D11_VIEWPORT createViewport( int topLeft, int topRight, int width, int height, float zMin, float zMax );

	// creates a unit box [0,1]^3
	static QVector< VertexPosition4fNormal3fTexture2f > createBox( bool normalsPointOutward = true );

	template< typename T >
	static ID3D11InputLayout* createInputLayout( ID3D11Device* pDevice, ID3DX11EffectPass* pPass )
	{
		ID3D11InputLayout* pInputLayout;

		D3DX11_PASS_DESC passDesc;
		pPass->GetDesc( &passDesc );
		HRESULT hr = pDevice->CreateInputLayout( T::s_layout, T::numElements(), passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &pInputLayout );
		if( SUCCEEDED( hr ) )
		{
			return pInputLayout;
		}
		else
		{
			return NULL;
		}
	}

	// Create a DynamicVertexBuffer of 6 vertices
	// each vertex is a VertexPosition4fColor4f
	static Reference< DynamicVertexBuffer > createAxes( ID3D11Device* pDevice );

	// writes a set of axes into buffer
	static void writeAxes( VertexPosition4fColor4f* vertexArray );

	// Create a DynamicVertexBuffer of 6 vertices (2 triangles) for a fullscreen quad
	// the vertices are in clip coordinates (-1,-1) --> (1,1)
	// so the projection matrix should be identity (i.e., don't use one)	
	// each vertex is a VertexPosition4f
	static Reference< DynamicVertexBuffer > createFullScreenQuad( ID3D11Device* pDevice );

	// Create a DynamicVertexBuffer of 6 vertices (2 triangles) for a screen aligned quad
	// each vertex is a VertexPosition4fTexture2f
	static Reference< DynamicVertexBuffer > createScreenAlignedQuad( float x, float y, float width, float height, ID3D11Device* pDevice );

	// writes a fullscreen quadrilateral (6 vertices, 2 triangles) into buffer
	// clip coordinates are -1 to 1
	static void writeFullScreenQuad( VertexPosition4f* vertexArray );

	// writes a screen aligned quadrilateral (6 vertices, 2 triangles) into buffer
	// position is set to (x,y) --> (x+width, y+height), z = 0, w = 1, (x,y) is the bottom left corner (y-axis points up)	
	static void writeScreenAlignedQuad( float x, float y, float width, float height, VertexPosition4f* vertexArray );

	// writes a screen aligned quadrilateral (6 vertices, 2 triangles) into buffer
	// position is set to (x,y) --> (x+width, y+height), z = 0, w = 1, (x,y) is the bottom left corner (y-axis points up)
	// by default, texture coordinates are flipped upside down ((0,0) at the top left corner)
	static void writeScreenAlignedQuad( float x, float y, float width, float height, VertexPosition4fTexture2f* vertexArray, bool flipUV = true );

	// Loads a texture from a standard image file, 8-bits per color channel
	// By default, Image4ub loads images such that the bottom left has coordinates (0,0) and is at location 0 memory
	// set flipUV to true to flip it up/down
	static Reference< DynamicTexture2D > createTextureFromFile( ID3D11Device* pDevice, QString filename, bool flipUV = true );

	static Reference< DynamicTexture2D > createTextureFromImage( ID3D11Device* pDevice, Reference< Image4ub > im, bool flipUV = true );
	static Reference< DynamicTexture2D > createTextureFromImage( ID3D11Device* pDevice, Reference< Image4f > im, bool flipUV = true );
	static Reference< DynamicTexture2D > createTextureFromImage( ID3D11Device* pDevice, Reference< Image1f > im, bool flipUV = true );

	static void copyImageToTexture( Reference< Image1f > im, Reference< DynamicTexture2D > tex, bool flipUV = true );
	static void copyImageToTexture( Reference< Image4f > im, Reference< DynamicTexture2D > tex, bool flipUV = true );
	static void copyImageToTexture( Reference< Image4ub > im, Reference< DynamicTexture2D > tex, bool flipUV = true );

	static void copyTextureToImage( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, Reference< Image1f > im );
	static void copyTextureToImage( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, Reference< Image1i > im );
	static void copyTextureToImage( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, Reference< Image4ub > im );
	static void copyTextureToImage( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, Reference< Image4f > im );

	static void saveTextureToPFM( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, QString filename );
	static void saveTextureToPFM4( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, QString filename );
	static void saveTextureToPNG( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, QString filename, bool scale = false, float factor = 1.f );
	static void saveTextureToTXT( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, QString filename );

	static bool saveFloat2BufferToTXT( ID3D11Device* pDevice, Reference< StaticDataBuffer > pBuffer, QString filename );
	static bool saveFloat2BufferToTXT( ID3D11Device* pDevice, Reference< StaticStructuredBuffer > pBuffer, QString filename );

	// HACK
	static void saveTextureToBinary( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, QString filename );

	// HACK
	template< typename T >
	static void saveSTLVectorToBinary( const std::vector< T >& input, const char* filename )
	{
		FILE* fp = fopen( filename, "wb" );
		uint size = static_cast< uint >( input.size() );
		fwrite( &size, sizeof( uint ), 1, fp );
		fwrite( &( input[ 0 ] ), sizeof( T ), size, fp );
		fflush( fp );
		fclose( fp );
	}

};


#endif // D3D11UTILS_H
