#include "butterflymodel.h"


#define	MODEL_FACES		(2*2)	// 2 quads
#define	MODEL_INDEXES	(MODEL_FACES*3)	// every triangle has 3 vertexes
#define	MODEL_VERTEXES	(6)	// number of vertexes


// creates the butterfly model
bool	CButterflyModel::GenerateModel( VertexButterfly**ppv, UINT*pnv, WORD**ppi, UINT*pni )
{
	// check arguments
	if ( !ppv || !ppi || !pnv || !pni )
		return false;

	// allocate arrays
	UINT		nv = MODEL_VERTEXES;
	UINT		ni = MODEL_INDEXES;
	VertexButterfly *pv = new VertexButterfly[ nv ];
	WORD		*pi = new WORD[ ni ];

	//
	// init model
	//

	//   1---3---5
	//   |   |   |
	//   0---2---4
	//

	// vertexes
	pv[0].Pos = D3DXVECTOR3(-1,0,0);
	pv[1].Pos = D3DXVECTOR3(-1,0,1);
	pv[2].Pos = D3DXVECTOR3(+0,0,0);
	pv[3].Pos = D3DXVECTOR3(+0,0,1);
	pv[4].Pos = D3DXVECTOR3(+1,0,0);
	pv[5].Pos = D3DXVECTOR3(+1,0,1);

	pv[0].Tex = D3DXVECTOR2(0,1);
	pv[1].Tex = D3DXVECTOR2(0,0);
	pv[2].Tex = D3DXVECTOR2(1,1);
	pv[3].Tex = D3DXVECTOR2(1,0);
	pv[4].Tex = D3DXVECTOR2(0,1);
	pv[5].Tex = D3DXVECTOR2(0,0);

	for (unsigned int i = 0; i < nv; ++i)
		pv[i].Weight = - pv[i].Pos.x;	// edges of the wings

	// indexes
	WORD	*curi = pi;

	curi[0] = 0;
	curi[1] = 1;
	curi[2] = 2;
	curi += 3;

	curi[0] = 2;
	curi[1] = 1;
	curi[2] = 3;
	curi += 3;

	curi[0] = 2;
	curi[1] = 3;
	curi[2] = 4;
	curi += 3;

	curi[0] = 4;
	curi[1] = 3;
	curi[2] = 5;
	curi += 3;

	//
	*ppv = pv;
	*pnv = nv;
	*ppi = pi;
	*pni = ni;

	return true;
}


// init container
CButterflyModelContainer::CButterflyModelContainer()
{
	m_pModel = NULL;
}


// CButterflyModelContainer dtor
CButterflyModelContainer::~CButterflyModelContainer()
{
	SAFE_DELETE(m_pModel);
}


// create model
bool	CButterflyModelContainer::CreateContainer(CDXDevice &dev)
{
	// create model object
	if ( dev.pDX10 )
	{
		m_pModel = new CButterflyModel10();
	}
	else if ( dev.pDX9 )
	{
		m_pModel = new CButterflyModel9();
	}

	// create model
	if ( m_pModel )
	{
		if ( m_pModel->CreateModel( dev ) )
			return true;
	}

	// not created
	return false;
}