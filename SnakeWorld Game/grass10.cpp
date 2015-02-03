#include "grass.h"

#define GRASS_WIDTH	(1.0f)	// radius of the grass model // [0.5,2.0]

//////////////////////////////////////////////////////////////////////////
// grass model ctor
CGrassModel10::CGrassModel10( CDX10Device&dev, CGrassContainer*pContainer, UINT n )
:CGrassModel(pContainer)
{


	m_pVB = NULL;
	m_nVertexes = 0;

	CreateModel( dev, n );
}


//////////////////////////////////////////////////////////////////////////
// grass model dtor
CGrassModel10::~CGrassModel10()
{
	SAFE_RELEASE(m_pVB);
	m_nVertexes = 0;
}



//////////////////////////////////////////////////////////////////////////
// create the grass model (point list)
bool	CGrassModel10::CreateModel( CDX10Device&dev, UINT n )
{
	// create grass model
	VertexGrass	*pGrass = new VertexGrass[ n ];

	InitModel( pGrass, n );

	// create & init vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexGrass ) * n;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA	InitData;
	InitData.pSysMem = pGrass;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pVB ) ) )
		return false;	// failed to create

	delete[] pGrass;

	m_nVertexes = n;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// initialize the grass vertexes
void	CGrassModel10::InitModel( VertexGrass*pGrass, UINT n )
{
	VertexGrass*p = pGrass;
	for ( UINT i = 0; i < n; ++i, ++p )
	{
		// position
		p->vecPos = D3DXVECTOR3( ((float)(rand() & 0x7ff) / (float)(0x7fe)) , 0,((float)(rand() & 0x7ff) / (float)0x7fe)  );

		// model
		D3DXVECTOR3 vecSeg( (float)(rand() & 0x7ff) / (float)0x7ff - 0.5f, 1.0f, (float)(rand() & 0x7ff) / (float)0x7ff - 0.5f );
		D3DXVec3Normalize( &p->vecDefSeg, &vecSeg );	// default segment0 direction

		p->vecSeg[0] = p->vecSeg[1] = /*p->vecSeg[2] =*/ p->vecDefSeg;	// model
		p->vecSegSpeed[0] = p->vecSegSpeed[1] = /*p->vecSegSpeed[2] =*/ D3DXVECTOR3(0,0,0);	// segments speed

		// orientation
		D3DXVECTOR2 vecDir = D3DXVECTOR2( p->vecDefSeg.x, p->vecDefSeg.z );
		D3DXVec2Normalize( &vecDir, &vecDir );
		p->vecDirection = /*p->vecDefDirection =*/ vecDir*GRASS_WIDTH;

		// segment height
		p->flSegHeight = 0.5f * ( 2.0f +
			( (float)(rand() & 0x7ff) / (float)0x7ff )*1.0f +
			pow( (float)(rand() & 0x7ff) / (float)0x7ff, 8.0f )*4.0f );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the grass
void	CGrassModel10::RenderGrass( CDXDevice&dev, float flDetail, D3DXMATRIX&mat )
{
	flDetail *= m_pContainer->flGrassDetail;	// final grass detail

	if (!m_nVertexes || !m_pVB || flDetail <= 0.0f )	return;

	if (flDetail > 1.0f)	flDetail = 1.0f;

	// set the shader variables
	m_pContainer->pVarGrassTextureArray->SetResource( m_pContainer->m_texture.GetTexture2DView() );

	//////////////////////////////////////////////////////////////////////////
	// GRASS

	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->pGrassInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	UINT stride = sizeof(VertexGrass), offset = 0;

	// the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );

	// Render the grass
	m_pContainer->pGrassRenderTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->pGrassRenderTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( (UINT)( (float)m_nVertexes * flDetail ), 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// animate the grass
void	CGrassModel10::FrameMove( double dStep )
{
	m_pVB = m_pContainer->AnimateAndSwapGrass10( m_pVB, (UINT)( (float)m_nVertexes * m_pContainer->flGrassDetail ) );
}

