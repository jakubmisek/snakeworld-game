#ifndef _STONEWALL_H_
#define _STONEWALL_H_

#include "device.h"
#include "list.h"
#include "textureloader.h"


//////////////////////////////////////////////////////////////////////////
// stone wall container
class CStoneWallContainer;


//////////////////////////////////////////////////////////////////////////
// stone vertex
struct VertexWall
{
	D3DXVECTOR3	Pos;	// vertex position
	D3DXVECTOR3	Norm;	// vertex normal
	D3DXVECTOR2	Tex;	// vertex texture coordinates
};

//////////////////////////////////////////////////////////////////////////
// stone wall model
class CStoneWallModel
{
public:
	CStoneWallModel( CStoneWallContainer*pContainer );
	virtual ~CStoneWallModel();

	virtual void	RenderStoneWall( CDXDevice&dev ) = 0;
	virtual void	RenderStoneWallGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation ) = 0;
	virtual void	RenderStoneWallShadowVolume( CDXDevice&dev ) = 0;

	void	Release();

	D3DXMATRIX		m_matWorld;	// stone wall transform matrix

protected:
	CStoneWallContainer	*m_pContainer;

	void	CreateStoneWallVertexes( VertexWall**ppWallVBOut, UINT*pWallVBCount, DWORD**ppWallIBOut, UINT*pWallIBCount );
	void	InitStone( VertexWall*pv, DWORD*pi, DWORD basevertex, D3DXVECTOR3&vecPos, float flHeight, float flRadius );
	
};

class CStoneWallModel10: public CStoneWallModel
{
public:
	CStoneWallModel10( CDX10Device&dev, CStoneWallContainer*pContainer );
	virtual ~CStoneWallModel10();

	virtual void	RenderStoneWall( CDXDevice&dev );
	virtual void	RenderStoneWallGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderStoneWallShadowVolume( CDXDevice&dev );

protected:

	bool	InitModel( CDX10Device&dev );
	

	ID3D10Buffer*pVB,*pIB;
	UINT		nIB;

};

class CStoneWallModel9: public CStoneWallModel
{
public:
	CStoneWallModel9( CDX9Device&dev, CStoneWallContainer*pContainer );
	virtual ~CStoneWallModel9();

	virtual void	RenderStoneWall( CDXDevice&dev );
	virtual void	RenderStoneWallGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderStoneWallShadowVolume( CDXDevice&dev );

protected:

	bool	InitModel( CDX9Device&dev );


	IDirect3DVertexBuffer9	*pVB;
	IDirect3DIndexBuffer9	*pIB;
	UINT	nIB, nVB;
};



//////////////////////////////////////////////////////////////////////////
// list of stone wall models
typedef CList<CStoneWallModel> StoneWallModels;


//////////////////////////////////////////////////////////////////////////
// stone wall models container
class CStoneWallContainer
{
public:

	CStoneWallContainer();
	~CStoneWallContainer();

	bool	CreateContainer( CDXDevice*pdev );			// create the container, initialize the techniques and variables

	bool	CreateStoneWall( CStoneWallModel**ppNewModel, D3DXMATRIX&matWorld );	// create new random stone wall

	void	ReleaseModel( CStoneWallModel*pModel );			// move the wall model into the list of free models

	CTextureLoader		m_texture, m_normaltexture;

	//
	// DX10
	//

	// rendering
	ID3D10InputLayout			*m_pWallInputLayout;		// wall vertex layout

	ID3D10EffectTechnique		*m_pWallTechnique;			// wall render technique	
	ID3D10EffectTechnique		*m_pWallGrassMoveTechnique;	// wall grass move render technique	
	ID3D10EffectTechnique		*m_pWallShadowVolumeTechnique;	// wall ShadowVolume render technique	


	//
	// DX9
	//
	D3DXHANDLE		m_pWallTechnique9;
	IDirect3DVertexDeclaration9	*m_pWallDecl9;
	
protected:

	bool	CreateContainer10( CDX10Device*pdev );			// create the container, initialize the techniques and variables
	bool	CreateContainer9( CDX9Device*pdev );			// create the container, initialize the techniques and variables

	CDXDevice			*m_pDevice;				// 3d device

	StoneWallModels		m_freeModels;			// free models
};



#endif//_STONEWALL_H_

