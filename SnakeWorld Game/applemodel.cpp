#include "applemodel.h"
#include "appleparam.h"


#define MODEL_W		(16)	//
#define MODEL_H		(24)	//

#define MODEL_VERTEXES	(MODEL_W * MODEL_H)
#define MODEL_FACES		( (MODEL_W-1)*(MODEL_H-1)*2 )	// quads * 2
#define MODEL_INDEXES	( MODEL_FACES * 3 )

// apple model container init
CAppleModelContainer::CAppleModelContainer()
{
	pModel = NULL;
}


// apple model container dtor
CAppleModelContainer::~CAppleModelContainer()
{
	SAFE_DELETE(pModel);
}


// create apple model container
bool	CAppleModelContainer::CreateContainer( CDXDevice&dev )
{
	// create model object
	if ( dev.pDX10 )
		pModel = new CAppleModel10();
	else if( dev.pDX9 )
		pModel = new CAppleModel9();
	
	// create model
	if ( pModel )
	{
		return pModel->CreateModel( dev );
	}
	else
	{
		return false;
	}
}


// generates apple model
// ppv: output, will points to array with vertexes
// pnv: pointer to UINT, number of vertexes in *ppv
// ppi: output, will points to array with indexes
// pni: pointer to UINT, number of indexes in *ppi
bool	CAppleModel::GenerateAppleModel( VertexApple**ppv, UINT*pnv, WORD**ppi, UINT*pni )
{
	// check arguments
	if ( !ppv || !ppi || !pnv || !pni )
		return false;

	// allocate arrays
	UINT		nv = MODEL_VERTEXES;
	UINT		ni = MODEL_INDEXES;
	VertexApple *pv = new VertexApple[ nv ];
	WORD		*pi = new WORD[ ni ];

	//
	UINT i,j;

	//
	// generate model
	//
	VertexApple	*curv = pv;
	for ( j = 0; j < MODEL_H; ++j )
	for ( i = 0; i < MODEL_W; ++i )
	{
		float fi = (float)i / (float)(MODEL_W-1);	// [0;1]
		float fj = (float)j / (float)(MODEL_H-1);	// [0;1]

		curv->Tex = D3DXVECTOR2( fi, fj );
		
		float fx = cosf( (float)( fi * D3DX_PI * 2.0f ) );
		float fy = sinf( (float)( fi * D3DX_PI * 2.0f ) );
		float fz = sinf( - (float)( (fj - 0.5f) * D3DX_PI ) );
		float fw = (float)sin(fj*D3DX_PI);

		// shape
		curv->Pos = D3DXVECTOR3( fx * fw, fz, fy * fw );
		curv->Pos *= APPLE_RADIUS;
		
		if ( fj < 0.3f )
		{
			float f = ( fj / 0.3f * 0.4f + 0.6f );
			curv->Pos *= f;
		}
		else if ( fj > 0.8f )
		{
			float f = ( (1.0f-fj) / 0.2f * 0.4f + 0.6f );
			curv->Pos *= f;
		}

		// clear normal
		curv->Norm = D3DXVECTOR3(0,0,0);

		++curv;
	}

	// vertexes

	// faces
	WORD	*curi = pi;
	for ( j = 0; j < MODEL_H-1; ++j )
	for ( i = 0; i < MODEL_W-1; ++i )
	{
		UINT v0 = j * MODEL_W + i;
		UINT v1 = v0 + 1;
		UINT v2 = v0 + MODEL_W;
		UINT v3 = v2 + 1;

		curi[0] = v0;
		curi[1] = v1;
		curi[2] = v2;
		curi += 3;

		curi[0] = v2;
		curi[1] = v1;
		curi[2] = v3;
		curi += 3;
	}

	// vertexes normals
	for ( i = 0, curi = pi; i < ni/3; ++i, curi+=3 )	// for each face *pi
	{
		VertexApple *v0 = pv+curi[0], *v1 = pv+curi[1], *v2 = pv+curi[2];

		// face normal
		D3DXVECTOR3	vecNormal, vecEdge1, vecEdge2;

		vecEdge1 = v1->Pos - v0->Pos;
		vecEdge2 = v2->Pos - v0->Pos;

		D3DXVec3Cross( &vecNormal, &vecEdge1, &vecEdge2 );
		D3DXVec3Normalize( &vecNormal, &vecNormal );

		// add to each vertex of current face
		v0->Norm += vecNormal;
		v1->Norm += vecNormal;
		v2->Norm += vecNormal;
	}

	// normalize vertex normals
	for ( i = 0; i < nv; ++i )
	{
		D3DXVec3Normalize( &pv[i].Norm, &pv[i].Norm );
	}


	//
	*ppv = pv;
	*pnv = nv;
	*ppi = pi;
	*pni = ni;

	return true;
}
