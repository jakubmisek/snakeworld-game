#include "grass.h"

//////////////////////////////////////////////////////////////////////////
// grass DX9 ctor
CGrassModel9::CGrassModel9( CDX9Device&dev, CGrassContainer*pContainer )
:CGrassModel(pContainer)
{
	
}


//////////////////////////////////////////////////////////////////////////
// grass DX9 dtor
CGrassModel9::~CGrassModel9()
{

}


//////////////////////////////////////////////////////////////////////////
// grass DX9 render
void	CGrassModel9::RenderGrass( CDXDevice&dev, float flDetail, D3DXMATRIX&mat )
{
	this->mat = mat;	// save the matrix
	this->flDetail = flDetail;

	// grass must be rendered after everything else (alphablending)
	m_pContainer->m_grassesToRender9.Add( this );
}


//////////////////////////////////////////////////////////////////////////
// grass DX9 frame move
void	CGrassModel9::FrameMove( double dStep )
{

}


//////////////////////////////////////////////////////////////////////////
// render grasses (dx9)
void	CGrassContainer::FinalizeGrassRender9( CDX9Device&dev )
{
	if ( !pGrassIB9 || !pGrassVB9 )
		return;

	dev.pEffect->SetTexture( dev.pVarDiffuseTexture, m_texture9.GetTexture9() );

	dev.pd3dDevice->SetVertexDeclaration(pGrassDecl9);
	dev.pd3dDevice->SetStreamSource(0,pGrassVB9,0,sizeof(VertexGrass9));
	dev.pd3dDevice->SetIndices(pGrassIB9);

	dev.pEffect->SetTechnique( pGrassTechnique9 );

	
	for ( CList<CGrassModel9>::iterator it = m_grassesToRender9; !it.EndOfList(); ++it )
	{

		float flDetail = it->flDetail * flGrassDetail;	// final grass detail
		flDetail *= flDetail;

		if (flDetail > 1.0f)	flDetail = 1.0f;

		
		if ( flDetail <= 0.0f )
			continue;

		dev.pEffect->SetMatrix( dev.pVarWorldTransform, &it->mat  );

		UINT cPasses;
		if (SUCCEEDED(dev.pEffect->Begin( &cPasses, 0 ) ))
		{
			for( UINT p = 0; p < cPasses; ++p )
			{
				if (SUCCEEDED(dev.pEffect->BeginPass( p ) ))
				{
					dev.pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nGrass9Vertices, 0, (UINT)( (float)nGrass9Faces * flDetail ) );

					dev.pEffect->EndPass();
				}
			}

			dev.pEffect->End();
		}
	}

	m_grassesToRender9.Clear(false);
}

