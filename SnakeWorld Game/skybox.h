#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "device.h"
#include "camera.h"
#include "textureloader.h"


// sky box vertex struct
struct VertexSkyBox
{
	D3DXVECTOR3	Pos;	// vertex position
	D3DXVECTOR2 Tex;	// texture mapping
};


// skybox object
class CSkyBox
{
public:
	CSkyBox();
	virtual ~CSkyBox();

	// create the sky box object, initialize vertex buffer, load textures
	virtual bool CreateSkyBox( CDXDevice&dev );

	// render the sky box
	virtual void Render( CDXDevice&dev, CCamera&camera );

protected:

	virtual bool CreateSkyBox10( CDX10Device&dev );
	bool	CreateSkyEffect10( CDX10Device&dev );
	bool	CreateSkyboxTechnique10( CDX10Device&dev );
	virtual void Render10( CDX10Device&dev, CCamera&camera, D3DXMATRIX&matViewProj );

	virtual bool CreateSkyBox9( CDX9Device&dev );
	virtual void Render9( CDX9Device&dev, CCamera&camera, D3DXMATRIX&matViewProj );
	
	void	InitModel( VertexSkyBox*vertexes, DWORD*indexes );

	CTextureLoader		m_texture;

	//
	// DX10
	//

	ID3D10Buffer	*m_pSkyboxVB, *m_pSkyboxIB;	// Sky box model
	
	ID3D10InputLayout		*m_pSkyboxInputLayout;		// Sky box vertex layout

	// effect
	ID3D10Effect	*m_pSkyEffect;

	ID3D10EffectTechnique	*m_pSkyboxTechnique;			// Sky box render technique	

	ID3D10EffectMatrixVariable	*m_pVarViewProjMatrix;		// view x projection transform
	ID3D10EffectShaderResourceVariable *m_pVarTexture;		// texture variable

	//
	// DX9
	//
	ID3DXEffect		*m_pSkyEffect9;
	D3DXHANDLE		m_pSkyboxTechnique9, m_pVarViewprojTransform9, m_pVarTexture9;
	IDirect3DVertexDeclaration9	*m_pSkyboxDecl9;
	IDirect3DVertexBuffer9	*m_pSkyboxVB9;
	IDirect3DIndexBuffer9	*m_pSkyboxIB9;
};

#endif//_SKYBOX_H_