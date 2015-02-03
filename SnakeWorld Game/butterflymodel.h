#ifndef _BUTTERFLYMODEL_H_
#define _BUTTERFLYMODEL_H_

#include "device.h"
#include "textureloader.h"

// butterly vertex declaration
struct VertexButterfly
{
	D3DXVECTOR3	Pos;
	D3DXVECTOR2	Tex;
	FLOAT		Weight;
};

// butterfly model interface
class CButterflyModel
{
public:
	CButterflyModel(){}
	virtual ~CButterflyModel(){}

	virtual bool CreateModel( CDXDevice&dev ) = 0;

	virtual void RenderModel( CDXDevice&dev, D3DXMATRIX&matWorld, FLOAT flWingsAnimation, CTextureLoader&texture ) = 0;

protected:

	bool	GenerateModel( VertexButterfly**ppv, UINT*pnv, WORD**ppi, UINT*pni );

};

// DX10 butterfly model
class CButterflyModel10: public CButterflyModel
{
public:
	CButterflyModel10();
	virtual ~CButterflyModel10();

	virtual bool CreateModel( CDXDevice&dev );

	virtual void RenderModel( CDXDevice&dev, D3DXMATRIX&matWorld, FLOAT flWingsAnimation, CTextureLoader&texture );

protected:

	ID3D10Buffer*pVB;	// one vertex, position xyz only

	// rendering
	ID3D10InputLayout			*m_pButterflyInputLayout;		// wall vertex layout

	ID3D10EffectTechnique		*m_pButterflyTechnique;			// wall render technique	

	ID3D10EffectScalarVariable	*pVarAnimation;	// butterfly wings animation
};


// DX9 butterfly model
class CButterflyModel9: public CButterflyModel
{
public:
	CButterflyModel9();
	virtual ~CButterflyModel9();

	virtual bool CreateModel( CDXDevice&dev );

	virtual void RenderModel( CDXDevice&dev, D3DXMATRIX&matWorld, FLOAT flWingsAnimation, CTextureLoader&texture );

protected:

	D3DXHANDLE		m_pButterflyTechnique;
	IDirect3DVertexDeclaration9	*m_pButterflyDecl;

	IDirect3DVertexBuffer9	*pVB;
	IDirect3DIndexBuffer9	*pIB;
	UINT	nIB, nVB;

	D3DXHANDLE			pVarAnimation;	// butterfly wings animation
};


// butterfly model container
class CButterflyModelContainer
{
public:

	CButterflyModelContainer();
	virtual~CButterflyModelContainer();

	// create model
	bool CreateContainer( CDXDevice&dev );

	// get model
	CButterflyModel*GetModel(){ return m_pModel; }

protected:
	CButterflyModel	*m_pModel;
};


#endif//_BUTTERFLYMODEL_H_