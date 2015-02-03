#include "headmodel.h"

//////////////////////////////////////////////////////////////////////////
// vertex struct

// eye indexed triangle list
struct VertexEye
{
	D3DXVECTOR3 Pos;		// vertex position
	D3DXVECTOR3 Normal;		// vertex normal
};


//////////////////////////////////////////////////////////////////////////
// snake head model
CSnakeHeadModel::CSnakeHeadModel()
{
	m_pVertexBuffer = NULL;
	m_nVertexes = 0;

	m_pEyeVB = m_pEyeIB = NULL;
	m_nEyeIndices = 0;
}


//////////////////////////////////////////////////////////////////////////
// snake head model dtor
CSnakeHeadModel::~CSnakeHeadModel()
{
	SAFE_RELEASE( m_pEyeIB );
	SAFE_RELEASE( m_pEyeVB );

	SAFE_RELEASE( m_pVertexBuffer );
}


//////////////////////////////////////////////////////////////////////////
// create the model
bool	CSnakeHeadModel::CreateModel( CDX10Device&dev )
{
	return 
		InitializeEffect(dev) &&
		CreateHeadModel(dev) &&
		CreateEyeModel(dev);
}


//////////////////////////////////////////////////////////////////////////
// initialize the dx techniques, variables
bool	CSnakeHeadModel::InitializeEffect( CDX10Device&dev )
{
	// snake eye
	D3D10_INPUT_ELEMENT_DESC layouteye[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layouteye)/sizeof(layouteye[0]);

	// Create the input layout
	pSnakeEyeTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(m_pDevice->pd3dDevice->CreateInputLayout( layouteye, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pSnakeEyeInputLayout )) )
		return false;	// unable to create the input layout
}


//////////////////////////////////////////////////////////////////////////
// create the head model
bool	CSnakeHeadModel::CreateHeadModel( CDX10Device&dev )
{
	//
	// head
	//
	VertexSnakeBody	pts[] =
	{
		// position					// size					// texture u
		{D3DXVECTOR3(+0.5f,0,0), D3DXVECTOR3(0,1.0f,0.8f), 0.0f},
		{D3DXVECTOR3(-0.7f,0,0), D3DXVECTOR3(0,0.95f,1.0f), 0.7f},
		{D3DXVECTOR3(-1.0f,0,0), D3DXVECTOR3(0,0.9f,0.9f), 1.0f},
		{D3DXVECTOR3(-1.8f,0.1f,0), D3DXVECTOR3(0,0.7f,0.8f), 1.5f},
		{D3DXVECTOR3(-2.6f,0.2f,0), D3DXVECTOR3(0,0.4f,0.5f), 2.6f},
		{D3DXVECTOR3(-2.7f,0.2f,0), D3DXVECTOR3(0,0.1f,0.2f), 2.7f},
	};

	m_nVertexes = sizeof( pts ) / sizeof( pts[0] );

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexSnakeBody ) * m_nVertexes;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA	InitData;

	InitData.pSysMem = pts;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
		return false;	// failed to create

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// create the eye model
bool	CSnakeHeadModel::CreateEyeModel( CDX10Device&dev )
{
	//
	// eye
	//
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

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexSnakeBody ) * npts;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA	InitData;

	InitData.pSysMem = pts;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pEyeVB ) ) )
		return false;	// failed to create

	delete[] pts;	//

	// IB
	m_nEyeIndices = (2*nc + nc)*3;
	DWORD *ids = new DWORD[m_nEyeIndices];

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


	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * m_nEyeIndices;	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	InitData.pSysMem = ids;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pEyeIB ) ) )
		return false;	// failed to create

	delete [] ids;

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// render the model
// texture must be already set
// flBodySegmentsDistanceFromTail,flBodySegmentsDistanceFromHead,flBodySegmentsMove must be already set
void	CSnakeHeadModel::RenderModel(
									 CDX10Device&dev,
									 CSegmentsModelContainer*pContainer,
									 D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength,
									 float flMoveAnim, CCamera&camera )
{
	if (!m_nVertexes || !m_pVertexBuffer)	return;

	// test if the head is visible in the view frustum
	CBoundBox boundbox( vecPosition );
	boundbox += 3.0f;
	if ( !camera.CullBlock( &boundbox ) )
		return;	// not visible

	// set the shader variables
	pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( flBodyLength );
	pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( 0.0f );
	pContainer->pSnakeVarBodySegmentsMove->SetFloat( flMoveAnim );

	//////////////////////////////////////////////////////////////////////////
	// SNAKE HEAD

	// setup matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
	matWorld.m[3][0] = vecPosition.x;
	matWorld.m[3][1] = vecPosition.y;
	matWorld.m[3][2] = vecPosition.z;

	dev.pVarMatWorld->SetMatrix( (float*)&matWorld );

	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( pContainer->pSnakeInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	UINT stride = sizeof(VertexSnakeBody), offset = 0;

	// the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// Render snake head
	pContainer->pSnakeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pContainer->pSnakeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->Draw( m_nVertexes, 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	// SNAKE EYE

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( pContainer->pSnakeEyeInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	stride = sizeof(VertexEye);
	offset = 0;

	// the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pEyeVB, &stride, &offset );
	dev.pd3dDevice->IASetIndexBuffer( m_pEyeIB, DXGI_FORMAT_R32_UINT, 0 );

	// setup matrix
	float flDstTail = flBodyLength + 1.0f;
	float flDstHead = - 1.0f;
	for ( int iEye = 0; iEye < 2; ++iEye )
	{
		SetupEyeMatrix( matWorld, vecPosition, flDirection, iEye, 
			0.75f *
			( min( flDstTail*0.07f, 1.0f ) * (1.0f / (abs(flDstTail*0.8f - flDstHead*1.2f)*( 0.05f ) + 1.0f) + 1.0f) )	// LINE COPIED FROM THE EFFECTS.FX <-- SnakeBodyVS::flRad
			);
		dev.pVarMatWorld->SetMatrix( (float*)&matWorld );

		// Render snake eye
		pContainer->pSnakeEyeTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			pContainer->pSnakeEyeTechnique->GetPassByIndex( p )->Apply(0);
			dev.pd3dDevice->DrawIndexed( m_nEyeIndices, 0, 0 );
		}
	}


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

