#ifndef _APPLEMODEL_H_
#define _APPLEMODEL_H_

#include "device.h"
#include "list.h"
#include "textureloader.h"

//////////////////////////////////////////////////////////////////////////
// apple vertex
struct VertexApple
{
	D3DXVECTOR3	Pos;	// vertex position
	D3DXVECTOR3	Norm;	// vertex normal
	D3DXVECTOR2	Tex;	// vertex texture coordinates
};


//////////////////////////////////////////////////////////////////////////
// apple abstract model
class CAppleModel
{
public:
	CAppleModel(){}
	virtual ~CAppleModel(){}

	virtual bool	CreateModel( CDXDevice&dev ) = 0;

	virtual void	RenderApple( CDXDevice&dev, D3DXMATRIX&matWorld ) = 0;
	virtual void	RenderAppleGrassMove( CDXDevice&dev, D3DXMATRIX&matWorld, D3DXVECTOR3&vecGrassTranslation ) = 0;
	virtual void	RenderAppleShadowVolume( CDXDevice&dev, D3DXMATRIX&matWorld ) = 0;

protected:

	bool	GenerateAppleModel( VertexApple**ppv, UINT*pnv, WORD**ppi, UINT*pni );

};

//////////////////////////////////////////////////////////////////////////
// apple DX10 model
class CAppleModel10: public CAppleModel
{
public:
	CAppleModel10();
	virtual ~CAppleModel10();

	virtual bool	CreateModel( CDXDevice&dev );

	virtual void	RenderApple( CDXDevice&dev, D3DXMATRIX&matWorld );
	virtual void	RenderAppleGrassMove( CDXDevice&dev, D3DXMATRIX&matWorld, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderAppleShadowVolume( CDXDevice&dev, D3DXMATRIX&matWorld );

protected:

	ID3D10Buffer*pVB,*pIB;
	UINT		nIB;

	// rendering
	ID3D10InputLayout			*m_pAppleInputLayout;		// wall vertex layout

	ID3D10EffectTechnique		*m_pAppleTechnique;			// wall render technique	
	ID3D10EffectTechnique		*m_pAppleGrassMoveTechnique;	// wall grass move render technique	
	ID3D10EffectTechnique		*m_pAppleShadowVolumeTechnique;	// wall ShadowVolume render technique	

	CTextureLoader	m_texture;
};


//////////////////////////////////////////////////////////////////////////
// apple DX9 model
class CAppleModel9: public CAppleModel
{
public:
	CAppleModel9();
	virtual ~CAppleModel9();

	virtual bool	CreateModel( CDXDevice&dev );

	virtual void	RenderApple( CDXDevice&dev, D3DXMATRIX&matWorld );
	virtual void	RenderAppleGrassMove( CDXDevice&dev, D3DXMATRIX&matWorld, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderAppleShadowVolume( CDXDevice&dev, D3DXMATRIX&matWorld );

protected:

	D3DXHANDLE		m_pAppleTechnique;
	IDirect3DVertexDeclaration9	*m_pAppleDecl;

	IDirect3DVertexBuffer9	*pVB;
	IDirect3DIndexBuffer9	*pIB;
	UINT	nIB, nVB;

	CTextureLoader	m_texture;
};

//////////////////////////////////////////////////////////////////////////
// apple model container
class CAppleModelContainer
{
public:
	CAppleModelContainer();
	virtual ~CAppleModelContainer();

	bool	CreateContainer( CDXDevice&dev );

	CAppleModel*	GetModel(){ return pModel; }

protected:
	CAppleModel	*pModel;
};

#endif//_APPLEMODEL_H_