#include "segmentsmodel.h"

#define HEAD_POINTS	(5)


//////////////////////////////////////////////////////////////////////////
// base super segment
CSegmentsModel::CSegmentsModel(CSegmentsModelContainer*pContainer)
{
	m_pContainer = pContainer;

	Reset();
}
CSegmentsModel::~CSegmentsModel()
{

}
//////////////////////////////////////////////////////////////////////////
// reset the state to uninitialized
void	CSegmentsModel::Reset()
{
	m_bWantUpdate = true;
}
//////////////////////////////////////////////////////////////////////////
// release super segment model
// move it to the list of models free to use
void	CSegmentsModel::Release()
{
	if (m_pContainer)
		m_pContainer->ReleaseModel( this );

	// remove from the tree
	RemoveFromTreeNode();
}
//////////////////////////////////////////////////////////////////////////
// update the model
bool	CSegmentsModel::UpdateModel(
									CVector&vecTail, CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS, CVector&vecHead,
									ISceneTree_Moving*pSceneTreeRoot,
									float flDistanceFromTail, float flDistanceFromHead,
									CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture )
{
	if ( !pSS )
		return false;

	// update rendering parameters
	UpdateParams(flDistanceFromTail,flDistanceFromHead,SnakeTexture,NormalTexture);

	//
	bool bRet = false;

	// update the model
	if ( pPrevSS )
	{	// do not use tail

		if ( pNextSS )
		{	// do not use head
			bRet = UpdateInnerModel( pPrevSS, pSS, pNextSS );
		}
		else
		{	// use head
			bRet = UpdateHeadModel( pPrevSS, pSS, vecHead );
		}
	}
	else
	{	// use tail

		if ( pNextSS )
		{	// do not use head
			bRet = UpdateTailModel( vecTail, pSS, pNextSS );
		}
		else
		{	// use head
			bRet = UpdateHeadTailModel( vecTail, pSS, vecHead );
		}
	}

	// move into the best scene tree node
	MoveToTreeNode( pSceneTreeRoot );

	// return the update-model result
	return bRet;
}

bool	CSegmentsModel::UpdateInnerModel( CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS )
{
	if (m_bWantUpdate)
	{
		CVector	vecFirst = pSS->Segments()[0]->Position();
		CVector vecFirstDir = vecFirst - pPrevSS->Segments().Last()->Position();

		m_bWantUpdate = false;

		return UpdateModel( pSS->Segments()[0]->Position(), vecFirstDir, pSS, pNextSS->Segments()[0]->Position() );
	}
	else
		return true;
}
bool	CSegmentsModel::UpdateHeadModel( CSnakeSuperSegment*pPrevSS, CSnakeSuperSegment*pSS, CVector&vecHead )
{
	m_bWantUpdate = true;

	CVector	vecFirst = pSS->Segments()[0]->Position();
	CVector vecFirstDir = vecFirst - pPrevSS->Segments().Last()->Position();

	return UpdateModel( pSS->Segments()[0]->Position(), vecFirstDir, pSS, vecHead );
}
bool	CSegmentsModel::UpdateTailModel( CVector&vecTail, CSnakeSuperSegment*pSS, CSnakeSuperSegment*pNextSS )
{
	m_bWantUpdate = true;

	CVector vecTailDir = ( pSS->Segments().Count() > 1 ) ? (pSS->Segments()[1]->Position() - vecTail) : ( pNextSS->Segments()[0]->Position() - vecTail );

	return UpdateModel( vecTail, vecTailDir, pSS, pNextSS->Segments()[0]->Position() );
}
bool	CSegmentsModel::UpdateHeadTailModel( CVector&vecTail, CSnakeSuperSegment*pSS, CVector&vecHead )
{
	m_bWantUpdate = true;

	CVector vecTailDir = ( pSS->Segments().Count() > 1 ) ? (pSS->Segments()[1]->Position() - vecTail) : ( vecHead - vecTail );

	return UpdateModel( vecTail, vecTailDir, pSS, vecHead );
}

//////////////////////////////////////////////////////////////////////////
// update rendering parameters
void	CSegmentsModel::UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture)
{
	m_flDistanceFromTail = flDistanceFromTail;
	m_flDistanceFromHead = flDistanceFromHead;
}




//////////////////////////////////////////////////////////////////////////
// setup eye matrix
void	CSnakeHeadModel::SetupEyeMatrix( D3DXMATRIX&matOut, D3DXVECTOR3&vecPosition, float flDirection, int iEye, float flEyeRad )
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matEye;
	switch (iEye)
	{
	case 0:
		D3DXMatrixRotationY( &matWorld, (float)( -flDirection ) );
		D3DXMatrixTranslation( &matEye, +1.0f, 0.5f, +flEyeRad );
		break;
	case 1:
		D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
		D3DXMatrixTranslation( &matEye, -1.0f, 0.5f, +flEyeRad );
		break;
	}

	D3DXMatrixMultiply( &matOut, &matEye, &matWorld );

	// translation
	matOut.m[3][0] += vecPosition.x;
	matOut.m[3][1] += vecPosition.y;
	matOut.m[3][2] += vecPosition.z;
}



//////////////////////////////////////////////////////////////////////////
// create eye model
void	CSnakeHeadModel::CreateEyeVertexes( VertexEye**ppVertexesOut, UINT*pVertexCountOut, DWORD**ppIndexesOut, UINT*pIndexCountOut )
{
	// VB
	const UINT nc = 8;
	UINT npts = 2*nc + 1;	// points count
	VertexEye	*pts = new VertexEye[npts];
	UINT i,j,l;
	float flSin,flCos;
	// create model
	i = 0;
	for ( l = 0; l < 2; ++l )
	{
		if (l==0)
		{
			flSin = 0.0f;
			flCos = 1.0f;
		}
		else 
		{
			flSin = (float)sin( D3DX_PI * 0.25 );
			flCos = (float)cos( D3DX_PI * 0.25 );
		}

		for (j=0;j<nc;++j,++i)
		{
			float flAngle = ((float)j / (float)nc) * (float)(2.0 * D3DX_PI);
			if (l)	flAngle += (float)( (D3DX_PI) / (double)(nc) );

			pts[i].Pos.x = cos( flAngle )*flCos;
			pts[i].Pos.y = sin( flAngle )*flCos;
			pts[i].Pos.z = flSin;
		}
	}

	// eye top
	pts[i].Pos.x = 0.0f;
	pts[i].Pos.y = 0.0f;
	pts[i].Pos.z = 0.95f;


	// create normals, adjust eye size
	for (i = 0; i < npts; ++i)	// create normals
	{
		D3DXVec3Normalize( &pts[i].Normal, &pts[i].Pos );
		pts[i].Pos *= 0.25f;
		pts[i].Pos.z *= 0.75f;
	}

	// IB
	UINT nEyeIndices = (2*nc + nc)*3;
	DWORD *ids = new DWORD[nEyeIndices];

	// init indexes
	j = 0;
	for (i = 0; i < nc; ++i)
	{
		ids[j++] = i;
		ids[j++] = (i+1)%nc;
		ids[j++] = i+nc;

		ids[j++] = i;
		ids[j++] = i+nc;
		ids[j++] = ((i+nc-1)%nc)+nc;

		ids[j++] = i+nc;
		ids[j++] = ((i+1)%nc)+nc;
		ids[j++] = npts-1;
	}

	//
	*ppVertexesOut = pts;
	*pVertexCountOut = npts;

	*ppIndexesOut = ids;
	*pIndexCountOut = nEyeIndices;
}

//////////////////////////////////////////////////////////////////////////
// head model instance ctor
CSnakeHeadModelObject::CSnakeHeadModelObject()
{

}


//////////////////////////////////////////////////////////////////////////
// head model instance dtor
CSnakeHeadModelObject::~CSnakeHeadModelObject()
{
	m_pContainer = NULL;
}


//////////////////////////////////////////////////////////////////////////
// update the head model instance render parameters
void	CSnakeHeadModelObject::UpdateHead(
	CSegmentsModelContainer*pContainer,
	D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength,
	ISceneTree_Moving*pSceneTreeRoot,
	CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture )
{
	m_pContainer = pContainer;
	m_vecPosition = vecPosition;
	m_flDirection = flDirection;
	m_flBodyLength = flBodyLength;
	
	m_bound = vecPosition;
	m_bound += 3.0f;

	// shader variables
	pSnakeTexture = &SnakeTexture;
	pNormalTexture = &NormalTexture;

	// move into the best scene tree node
	MoveToTreeNode( pSceneTreeRoot );
}


//////////////////////////////////////////////////////////////////////////
// render the head
void	CSnakeHeadModelObject::RenderObject( CDXDevice&dev, CCamera&camera )
{
	if (m_pContainer && m_pContainer->m_pSnakeHeadModel)	// => initialized
	{
		m_pContainer->m_pSnakeHeadModel->RenderModel(dev, m_pContainer, m_vecPosition, m_flDirection, m_flBodyLength, camera,*pSnakeTexture,*pNormalTexture);
	}
}


//////////////////////////////////////////////////////////////////////////
// render the grass move map
void	CSnakeHeadModelObject::RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{
	if (m_pContainer && m_pContainer->m_pSnakeHeadModel)	// => initialized
	{
		m_pContainer->m_pSnakeHeadModel->RenderGrassMove( dev, m_pContainer, m_vecPosition, m_flDirection, m_flBodyLength, vecGrassTranslation );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the head shadow volume
void	CSnakeHeadModelObject::RenderShadowVolume( CDXDevice&dev )
{
	if (m_pContainer && m_pContainer->m_pSnakeHeadModel)	// => initialized
	{
		m_pContainer->m_pSnakeHeadModel->RenderShadowVolume( dev, m_pContainer, m_vecPosition, m_flDirection, m_flBodyLength );
	}
}


//////////////////////////////////////////////////////////////////////////
// segments model container (manager) ctor
CSegmentsModelContainer::CSegmentsModelContainer()
{
	m_pDevice = NULL;
	m_pSnakeHeadModel = NULL;

	// dx10
	pSnakeTechnique = pSnakeEyeTechnique = NULL;
	pSnakeInputLayout = pSnakeEyeInputLayout = NULL;

	m_pAdvanceSnakeBodyVertexBuffer = NULL;

	// dx9
	pSnakeTechnique9 = NULL;
	pSnakeDecl9 = pEyeDecl9 = NULL;
	pSnakeBodyIB9 = NULL;
}


//////////////////////////////////////////////////////////////////////////
// segments model container dtor
CSegmentsModelContainer::~CSegmentsModelContainer()
{
	SAFE_DELETE(m_pSnakeHeadModel);

	// dx10
	SAFE_RELEASE( pSnakeInputLayout );
	SAFE_RELEASE( pSnakeEyeInputLayout );

	SAFE_RELEASE( m_pAdvanceSnakeBodyVertexBuffer );

	// dx9
	SAFE_RELEASE(pSnakeDecl9);
	SAFE_RELEASE(pEyeDecl9);
	SAFE_RELEASE(pSnakeBodyIB9);
}


//////////////////////////////////////////////////////////////////////////
// initialize the container
bool	CSegmentsModelContainer::CreateContainer( CDXDevice*pdev )
{
	m_pDevice = pdev;

	if (pdev->pDX10)
	{
		if ( !CreateContainer10(pdev->pDX10) )
			return false;

	}
	else if (pdev->pDX9)
	{
		if ( !CreateContainer9(pdev->pDX9))
			return false;

	}

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// create DX10 container
bool	CSegmentsModelContainer::CreateContainer10( CDX10Device*pdev )
{
	//
	// initialize direct 3d resources
	//

	// Obtain the techniques
	pSnakeTechnique = pdev->pEffect->GetTechniqueByName( "RenderSnake" );
	pAdvanceSnakeTechnique = pdev->pEffect->GetTechniqueByName( "AdvanceSnake" );
	pSnakeEyeTechnique = pdev->pEffect->GetTechniqueByName( "RenderSnakeEye" );

	pSnakeGrassMoveTechnique = pdev->pEffect->GetTechniqueByName( "RenderSnakeGrassMoveMask" );
	pSnakeShadowVolumeTechnique = pdev->pEffect->GetTechniqueByName( "RenderSnakeShadowVolume" );

	if ( !pSnakeTechnique || !pAdvanceSnakeTechnique || !pSnakeEyeTechnique ||
		!pSnakeGrassMoveTechnique || !pSnakeShadowVolumeTechnique )
		return false;	// some techniques cannot be obtained

	// Obtain variables
	pSnakeVarBodySegmentsDistanceFromTail = pdev->pEffect->GetVariableByName( "flBodySegmentsDistanceFromTail" )->AsScalar();
	pSnakeVarBodySegmentsDistanceFromHead = pdev->pEffect->GetVariableByName( "flBodySegmentsDistanceFromHead" )->AsScalar();

	// Define the input layouts

	// snake body
	D3D10_INPUT_ELEMENT_DESC layoutbody[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "DISTANCE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layoutbody)/sizeof(layoutbody[0]);

	D3D10_PASS_DESC PassDesc;
	pSnakeTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(pdev->pd3dDevice->CreateInputLayout( layoutbody, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pSnakeInputLayout )) )
		return false;	// unable to create the input layout

	// snake eye
	D3D10_INPUT_ELEMENT_DESC layouteye[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	numElements = sizeof(layouteye)/sizeof(layouteye[0]);

	// Create the input layout
	pSnakeEyeTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(pdev->pd3dDevice->CreateInputLayout( layouteye, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pSnakeEyeInputLayout )) )
		return false;	// unable to create the input layout

	// SO buffer
	// m_pAdvanceSnakeBodyVertexBuffer

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexSnakeBody ) * ( SUPERSEGMENT_CAPACITY * 10 );	// maximum size (see AdvanceSnakeBodyGS)
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;	// vertex buffer, SO
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	if( FAILED( pdev->pd3dDevice->CreateBuffer( &bd, NULL, &m_pAdvanceSnakeBodyVertexBuffer ) ) )
		return false;	// failed to create

	// head model
	m_pSnakeHeadModel = new CSnakeHeadModel10(*pdev);


	return true;
}



//////////////////////////////////////////////////////////////////////////
// create DX9 container
bool	CSegmentsModelContainer::CreateContainer9( CDX9Device*pdev )
{
	//
	// VARIABLES
	//
	pVarDistanceFromHead9 = pdev->pEffect->GetParameterByName(NULL,"flBodySegmentsDistanceFromHead");
	pVarDistanceFromTail9 = pdev->pEffect->GetParameterByName(NULL,"flBodySegmentsDistanceFromTail");

	// TECHNIQUE
	pSnakeTechnique9 = pdev->pEffect->GetTechniqueByName("RenderSnakeBody");
	pEyeTechnique9 = pdev->pEffect->GetTechniqueByName("RenderSnakeEye");

	// vertex declaration

	const D3DVERTEXELEMENT9 DeclBody[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 36,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		{ 0, 48,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(pdev->pd3dDevice->CreateVertexDeclaration( DeclBody, &pSnakeDecl9 )))
		return false;

	const D3DVERTEXELEMENT9 DeclEye[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		D3DDECL_END()
	};

	if (FAILED(pdev->pd3dDevice->CreateVertexDeclaration( DeclEye, &pEyeDecl9 )))
		return false;

	// shared index buffer
#define NBODYPOINTS  (7)
	const UINT nIB = (NBODYPOINTS-1)*(SUPERSEGMENT_CAPACITY+1)*6;
	if( FAILED( pdev->pd3dDevice->CreateIndexBuffer( sizeof(WORD)*nIB, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pSnakeBodyIB9, NULL ) ) )
		return false;

	WORD* pIndexOut, *pi;
	if( FAILED( pSnakeBodyIB9->Lock( 0, sizeof(WORD)*nIB, (void**)&pIndexOut, 0 ) ) )
		return false;

	// index buffer model
	pi = pIndexOut;
	for (UINT i = 0; i < SUPERSEGMENT_CAPACITY; ++i)
	{
		for (UINT s = 0; s < (NBODYPOINTS-1); ++s)
		{
			UINT v[4] = { i * NBODYPOINTS + s, i * NBODYPOINTS + s + 1, (i+1)*NBODYPOINTS + s, (i+1)*NBODYPOINTS + s + 1 };

			pi[0] = v[0];
			pi[1] = v[2];
			pi[2] = v[3];

			pi += 3;

			pi[0] = v[0];
			pi[1] = v[3];
			pi[2] = v[1];

			pi += 3;
		}
	}

	pSnakeBodyIB9->Unlock();

	// head model

	m_pSnakeHeadModel = new CSnakeHeadModel9( *pdev );

	return true;
}
//////////////////////////////////////////////////////////////////////////
// creates new segments model
// in most cases pops existing model from the list of free to use models
bool	CSegmentsModelContainer::CreateModel( CSegmentsModel**ppNewModel )
{
	*ppNewModel = NULL;

	//
	if ( m_freeModels.Count() > 0 )
	{
		// pop the item from the list
		*ppNewModel = m_freeModels[0];
		m_freeModels.Remove(0,false);

		// reset the recycled item state
		(*ppNewModel)->Reset();
	}
	else
	{
		// create new
		if (m_pDevice->pDX10)
		{
			*ppNewModel = new CSegmentsModel10( this, *m_pDevice->pDX10 );
		}
		else if (m_pDevice->pDX9)
		{
			*ppNewModel = new CSegmentsModel9( this, *m_pDevice->pDX9 );
		}

	}

	// done
	return (*ppNewModel != NULL);
}


//////////////////////////////////////////////////////////////////////////
// add the segments model into the list of free to use models
bool	CSegmentsModelContainer::ReleaseModel( CSegmentsModel*pModel )
{
	if (pModel)
	{
		m_freeModels.Add( pModel );
	}

	return true;
}