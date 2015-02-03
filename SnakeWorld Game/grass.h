#ifndef _GRASS_H_
#define _GRASS_H_

//////////////////////////////////////////////////////////////////////////
//
#include "device.h"
#include "list.h"
#include "grasstextureloader.h"
#include "textureloader.h"

class CGrassContainer;

//////////////////////////////////////////////////////////////////////////
// one stalk of grass
// the model is created later in geometry shader
struct VertexGrass
{
	D3DXVECTOR3	vecPos;		// grass stalk position

	D3DXVECTOR3 vecSeg[2];	// the physical model, normalized grass stalk segments direction

	D3DXVECTOR3 vecDefSeg;	// grass stalk default (normalized) segment0 direction

	D3DXVECTOR3 vecSegSpeed[2];	// the speed of grass segments

	D3DXVECTOR2 vecDirection;	// stalk orientation
	//D3DXVECTOR2 vecDefDirection;// stalk default orientation

	float		flSegHeight;		// segment height = stalk height / 3
};

//////////////////////////////////////////////////////////////////////////
// DX9 grass model, classic billboards
struct VertexGrass9
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};


//////////////////////////////////////////////////////////////////////////
// one grass plane ( TERRAIN_SIZE × TERRAIN_SIZE )
class CGrassModel
{
public:

	CGrassModel( CGrassContainer*pContainer );
	virtual ~CGrassModel();

	virtual void	RenderGrass( CDXDevice&dev, float flDetail, D3DXMATRIX&mat ) = 0;
	virtual void	FrameMove( double dStep ) = 0;

	void	Release();

protected:

	CGrassContainer	*m_pContainer;	// grass models owner
};

// DX10 grass
class CGrassModel10: public CGrassModel
{
public:

	CGrassModel10( CDX10Device&dev, CGrassContainer*pContainer, UINT n );
	virtual ~CGrassModel10();

	virtual void	RenderGrass( CDXDevice&dev, float flDetail, D3DXMATRIX&mat );
	virtual void	FrameMove( double dStep );

	
protected:

	bool	CreateModel( CDX10Device&dev, UINT n );
	void	InitModel( VertexGrass*pGrass, UINT n );

	ID3D10Buffer	*m_pVB;			// grass vertex buffer
	UINT			m_nVertexes;	// grass vertex buffer vertexes count
};

// DX9 grass
class CGrassModel9: public CGrassModel
{
public:

	CGrassModel9( CDX9Device&dev, CGrassContainer*pContainer );
	virtual ~CGrassModel9();

	virtual void	RenderGrass( CDXDevice&dev, float flDetail, D3DXMATRIX&mat );
	virtual void	FrameMove( double dStep );

	D3DXMATRIX	mat;
	float		flDetail;

protected:

	
};


//////////////////////////////////////////////////////////////////////////
//
typedef CList<CGrassModel>	GrassList;

//////////////////////////////////////////////////////////////////////////
// grass container
// manages created not used grass vertex buffers
// manages the DirectX effect techniques and variables
class CGrassContainer
{
public:
	CGrassContainer();
	~CGrassContainer();

	bool	CreateContainer( CDXDevice*pdev );	// initialize the container

	bool	CreateModel( CGrassModel**ppNewModel );	// get new grass model
	bool	ReleaseModel( CGrassModel*pModel );		// release grass model

	void	FinalizeGrassRender9( CDX9Device&dev );	// finalize grass rendering (after everything else is rendered)

	// grass moving
	void			ClearGrassMoveMap();			// clears the grass move map 
	void			BeginRenderToGrassMoveMap();	// set as render target
	void			EndRenderToGrassMoveMap();		// restores back the original RT and DST
	void			SetGrassMoveMapDirty(){ m_bGrassMoveMapCleared = false; }

	ID3D10Buffer*	AnimateAndSwapGrass10( ID3D10Buffer*pVB, UINT nVertexes );	// animate grass and return new animated vertex buffer

	float	flGrassDetail;	// 0 - 1
	
	// DX objects
	// DX10
	CGrassTextureLoader		m_texture;		// grass texture array

	ID3D10InputLayout		*pGrassInputLayout;			// grass vertex layout

	ID3D10EffectTechnique	*pGrassRenderTechnique;		// grass render
	
	ID3D10EffectShaderResourceVariable	*pVarGrassTextureArray;	// diffuse texture variable

	ID3D10ShaderResourceView*m_pGrassMoveMapView;

	// DX9
	IDirect3DVertexDeclaration9	*pGrassDecl9;
	D3DXHANDLE	pGrassTechnique9;
	IDirect3DVertexBuffer9	*pGrassVB9;
	IDirect3DIndexBuffer9	*pGrassIB9;
	UINT		nGrass9Vertices, nGrass9Faces;
	
	CTextureLoader	m_texture9;
	CList<CGrassModel9>	m_grassesToRender9;

protected:

	bool	CreateContainer10( CDX10Device*pdev );	// initialize the container
	bool	CreateContainer9( CDX9Device*pdev );	// initialize the container

	// grass animation buffers
	// temporary vertex buffer length must be equal to other vertex buffers length used for rendering grass
	ID3D10Buffer	*m_pTmpVB;			// grass vertex buffer used for animation
	UINT			m_nTmpVertexes;		// grass vertex buffer used for animation vertexes count

	ID3D10EffectTechnique	*pGrassAnimateTechnique;		// grass animation

	CDXDevice	*m_pDevice;		// pointer to the render device

	GrassList	m_freeModels;	// array of not used models


	// grass move map
	ID3D10Texture2D			*m_pRTGrassMoveMap;
	ID3D10RenderTargetView	*m_pRTGrassMoveMapView;
	bool					m_bGrassMoveMapCleared;

	// used between BeginRenderToGrassMoveMap and EndRenderToGrassMoveMap
	ID3D10RenderTargetView* pOrigRT;
	ID3D10DepthStencilView* pOrigDS;
	D3D10_VIEWPORT			OrigVP;
};

#endif//_GRASS_H_