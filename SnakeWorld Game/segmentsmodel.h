#ifndef _SEGMENTSMODEL_H_
#define _SEGMENTSMODEL_H_

#include "snakeobject.h"
#include "list.h"
#include "boundbox.h"

#include "device.h"
#include "camera.h"

#include "textureloader.h"

#include "scenetreemoving.h"

//////////////////////////////////////////////////////////////////////////
// vertex struct

// snake body line strip
struct VertexSnakeBody
{
	D3DXVECTOR3	Pos;		// vertex position
	D3DXVECTOR3	Normal;		// side vector
	float		Distance;	// distance from the segments block begin
};


// eye indexed triangle list
struct VertexEye
{
	D3DXVECTOR3 Pos;		// vertex position
	D3DXVECTOR3 Normal;		// vertex normal
};


//////////////////////////////////////////////////////////////////////////
class CSegmentsModelContainer;


//////////////////////////////////////////////////////////////////////////
// base segment model
class CSegmentsModel: public ISceneTree_MovableObject
{
public:

	CSegmentsModel(CSegmentsModelContainer*pContainer);
	virtual ~CSegmentsModel();

	virtual void	Reset();

	virtual void	RenderObject( CDXDevice&dev, CCamera&camera ) = 0;
	virtual void	RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation ) = 0;
	virtual void	RenderShadowVolume( CDXDevice&dev ) = 0;

	// update the model vertexes by given super segment (its piece of shit)
	// update the tree node (moves into the best node)
	virtual bool	UpdateModel(
		CVector&vecTail, CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS, CVector&vecHead,
		ISceneTree_Moving*pSceneTreeRoot,
		float flDistanceFromTail, float flDistanceFromHead,
		CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture );

	virtual CBoundBox&	GetBoundingBox(){ return m_boundBox; }
	inline float		GetLength(){ return m_flLength; }

	// move the object into the containers free-to-use list
	void	Release();

protected:

	// update vertexes by given super-segment
	bool	UpdateInnerModel( CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS );	// ... - ...
	bool	UpdateHeadModel( CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CVector&vecHead );				// ... ->
	bool	UpdateTailModel( CVector&vecTail, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS );				// >- ...
	bool	UpdateHeadTailModel( CVector&vecTail, CSnakeSuperSegment*pSS, CVector&vecHead );					// >->

	virtual void	UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture);

	virtual bool	UpdateModel( CVector&vecFirst, CVector&vecFirstDir, CSnakeSuperSegment*pSS, CVector&vecEnd ) = 0;

	// rendering parameters
	float m_flDistanceFromTail, m_flDistanceFromHead;

	// bounding box
	CBoundBox		m_boundBox;

	bool			m_bWantUpdate;

	float			m_flLength;

	// models manager
	CSegmentsModelContainer	*m_pContainer;
};

//////////////////////////////////////////////////////////////////////////
// snake super segment model for DX10
class CSegmentsModel10: public CSegmentsModel
{
public:

	CSegmentsModel10( CSegmentsModelContainer*pContainer, CDX10Device&dev );
	virtual ~CSegmentsModel10();

	virtual void	Reset();

	virtual void	RenderObject( CDXDevice&dev, CCamera&camera );
	virtual void	RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev );

protected:

	// create new vertex buffer
	bool	CreateVB( CDX10Device&dev );

	virtual void	UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture);
	virtual bool	UpdateModel( CVector&vecFirst, CVector&vecFirstDir, CSnakeSuperSegment*pSS, CVector&vecEnd );

	// the model
	ID3D10Buffer	*m_pVertexBuffer;
	UINT			m_nVertexes;
	

	// rendering parameters
	ID3D10ShaderResourceView *m_pSnakeTextureView, *m_pSnakeNormalTextureView;
};


//////////////////////////////////////////////////////////////////////////
// snake super segment model for DX9
class CSegmentsModel9: public CSegmentsModel
{
public:

	CSegmentsModel9( CSegmentsModelContainer*pContainer, CDX9Device&dev );
	virtual ~CSegmentsModel9();

	virtual void	Reset();

	virtual void	RenderObject( CDXDevice&dev, CCamera&camera );
	virtual void	RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev );

protected:

	// create new vertex buffer
	bool	CreateVB( CDX9Device&dev );

	virtual void	UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture);
	virtual bool	UpdateModel( CVector&vecFirst, CVector&vecFirstDir, CSnakeSuperSegment*pSS, CVector&vecEnd );

	IDirect3DTexture9	*m_pSnakeTexture, *m_pSnakeNormal;

	IDirect3DVertexBuffer9	*m_pVertexBuffer;
	UINT					m_nVertexes;
};


//////////////////////////////////////////////////////////////////////////
// list of super segments
typedef CList<CSegmentsModel>	SegmentsModelList;

//////////////////////////////////////////////////////////////////////////
// base snake head model
class CSnakeHeadModel
{
public:
	CSnakeHeadModel(){}
	virtual ~CSnakeHeadModel(){}

	virtual void	RenderModel( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, CCamera&camera, CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture ) = 0;
	virtual void	RenderGrassMove( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, D3DXVECTOR3&vecGrassTranslation ) = 0;
	virtual void	RenderShadowVolume( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength ) = 0;
	
protected:

	void			SetupEyeMatrix( D3DXMATRIX&matOut, D3DXVECTOR3&vecPosition, float flDirection, int iEye, float flEyeRad );
	void			CreateEyeVertexes( VertexEye**ppVertexesOut, UINT*pVertexCountOut, DWORD**ppIndexesOut, UINT*pIndexCountOut );
	
};

// DX10 snake head model
class CSnakeHeadModel10: public CSnakeHeadModel
{
public:
	CSnakeHeadModel10(CDX10Device&dev);
	virtual ~CSnakeHeadModel10();

	virtual void	RenderModel( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, CCamera&camera, CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture );
	virtual void	RenderGrassMove( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength );

protected:

	bool	CreateModel( CDX10Device&dev );

	// the model
	ID3D10Buffer	*m_pVertexBuffer;
	UINT			m_nVertexes;

	bool			CreateHeadModel( CDX10Device&dev );

	// eye
	ID3D10Buffer	*m_pEyeVB;
	ID3D10Buffer	*m_pEyeIB;
	UINT			m_nEyeIndices;

	bool			CreateEyeModel( CDX10Device&dev );
};

// DX9 snake head model
class CSnakeHeadModel9: public CSnakeHeadModel
{
public:
	CSnakeHeadModel9(CDX9Device&dev);
	virtual ~CSnakeHeadModel9();

	virtual void	RenderModel( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, CCamera&camera, CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture );
	virtual void	RenderGrassMove( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength );

protected:

	// head
	IDirect3DVertexBuffer9	*m_pVertexBuffer;
	UINT					m_nVertexes;

	bool		CreateHeadModel( CDX9Device&dev );

	// eyes
	IDirect3DVertexBuffer9 *m_pEyeVB;
	IDirect3DIndexBuffer9  *m_pEyeIB;
	UINT	m_nEyeVertexCount, m_nEyeIndexCount;

	bool		CreateEyeModel( CDX9Device&dev );
	
};


//////////////////////////////////////////////////////////////////////////
// snake head instance
class CSnakeHeadModelObject: public ISceneTree_MovableObject
{
public:
	CSnakeHeadModelObject();
	virtual ~CSnakeHeadModelObject();

	virtual void			UpdateHead(
		CSegmentsModelContainer*pContainer,
		D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength,
		ISceneTree_Moving*pSceneTreeRoot,
		CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture );

	virtual void	RenderObject( CDXDevice&dev, CCamera&camera );
	virtual void	RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev );

	virtual CBoundBox&	GetBoundingBox(){ return m_bound; }

protected:

	// render parameters
	CSegmentsModelContainer*m_pContainer;
	D3DXVECTOR3	m_vecPosition;
	float m_flDirection, m_flBodyLength;

	CTextureLoader*pSnakeTexture;
	CTextureLoader*pNormalTexture;

	CBoundBox	m_bound;
};

//////////////////////////////////////////////////////////////////////////
// super segment models container
class CSegmentsModelContainer
{
public:
	
	CSegmentsModelContainer();
	~CSegmentsModelContainer();

	//
	// initialize the manager
	//

	bool	CreateContainer( CDXDevice*pdev );		// initialize the container

	//
	// create/release super segment model
	//

	bool	CreateModel( CSegmentsModel**ppNewModel );	// get new super segment model
	bool	ReleaseModel( CSegmentsModel*pModel );		// release super segment model


	//
	// used snake head model
	//

	CSnakeHeadModel		*m_pSnakeHeadModel;	// snake head model

	//
	// snake segments technique DX10
	//

	ID3D10EffectTechnique	*pSnakeTechnique;			// snake body render technique
	ID3D10EffectTechnique	*pAdvanceSnakeTechnique;	// snake body advance technique
	ID3D10EffectTechnique	*pSnakeEyeTechnique;		// snake eye render technique

	ID3D10EffectTechnique	*pSnakeGrassMoveTechnique;	// snake body grass moving mask technique
	ID3D10EffectTechnique	*pSnakeShadowVolumeTechnique;	// snake body shadow volume technique

	ID3D10InputLayout		*pSnakeInputLayout;			// snake body vertex layout
	ID3D10InputLayout		*pSnakeEyeInputLayout;		// snake eye vertex layout

	ID3D10Buffer			*m_pAdvanceSnakeBodyVertexBuffer;	// advanced snake body segments model

	ID3D10EffectScalarVariable	*pSnakeVarBodySegmentsDistanceFromTail;	// distance of the beginning of segments model from the tail
	ID3D10EffectScalarVariable	*pSnakeVarBodySegmentsDistanceFromHead;	// distance of the beginning of segments model from the head

	//
	// DX9
	//
	D3DXHANDLE		pSnakeTechnique9, pEyeTechnique9;	// snake body render technique
	D3DXHANDLE		pVarDistanceFromTail9, pVarDistanceFromHead9;	// distances of current super-segment
	IDirect3DVertexDeclaration9	*pSnakeDecl9, *pEyeDecl9;	// snake body vertex declaration
	IDirect3DIndexBuffer9	*pSnakeBodyIB9;		// snake body index buffer (same for all vertex buffers)

protected:

	bool	CreateContainer10( CDX10Device*pdev );
	bool	CreateContainer9( CDX9Device*pdev );

	CDXDevice			*m_pDevice;		// pointer to the render device

	SegmentsModelList	m_freeModels;	// array of not used buffers

};


#endif//_SEGMENTSMODEL_H_