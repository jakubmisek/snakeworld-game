#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "device.h"
#include "textureloader.h"

//////////////////////////////////////////////////////////////////////////
// terrain parameters
#define TERRAIN_SIZE	(50.0f)		// size of the terrain square (width and depth)	// EQUAL TO EFFECT.FX -> flTerrainSize
#define TERRAIN_PTS		(1)			// amount of vertexes-1 on a side of the terrain square

//////////////////////////////////////////////////////////////////////////
//
class CTerrainContainer
{
public:
	CTerrainContainer();
	virtual ~CTerrainContainer();

	bool	CreateTerrainModel( CDXDevice&dev );

	void	RenderTerrain( CDXDevice&dev );

protected:

	bool		CreateTerrainModel10( CDX10Device&dev );
	bool		CreateTerrainModel9( CDX9Device&dev );

	bool		CreateTerrainTechnique10( CDX10Device&dev );
	bool		CreateTerrainTechnique9( CDX9Device&dev );

	void	RenderTerrain10( CDX10Device&dev );
	void	RenderTerrain9( CDX9Device&dev );

	// texture
	CTextureLoader		m_texture;		// terrain texture

	//
	// DX10
	//

	// terrain
	ID3D10Buffer			*m_pTerrainVertexBuffer;	// terrain vertex buffer
	ID3D10Buffer			*m_pTerrainIndexBuffer;		// terrain index buffer
	ID3D10InputLayout		*m_pTerrainInputLayout;		// terrain vertex layout

	ID3D10EffectTechnique	*m_pTerrainTechnique;			// terrain render technique	

	//
	// DX9
	//
	D3DXHANDLE		m_pTerrainTechnique9;		// terrain render technique

	IDirect3DVertexDeclaration9	*m_pTerrainDecl9;	// vertex declaration
	IDirect3DVertexBuffer9		*m_pTerrainVB9;		// vertex buffer (1 quad)
};

#endif//_TERRAIN_H_