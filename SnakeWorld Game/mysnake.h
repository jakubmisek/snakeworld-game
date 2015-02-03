#ifndef _MYSNAKE_H_
#define _MYSNAKE_H_

#include "snakeobject.h"
#include "segmentsmodel.h"
#include "camera.h"
#include "device.h"
#include "soundmanager.h"
#include "textureloader.h"
#include "downloader.h"

//////////////////////////////////////////////////////////////////////////
// snake super segment extension
class CSnakeSuperSegmentEx: public CSnakeSuperSegment
{
public:
	CSnakeSuperSegmentEx()
	{
		pModel = 0;
	}

	virtual ~CSnakeSuperSegmentEx()
	{
		SAFE_RELEASE( pModel );
	}

	CSegmentsModel	*pModel;
};


//////////////////////////////////////////////////////////////////////////
// the snake object
class CMySnake: public CSnake
{
public:
	CMySnake( bool bUserControlled, CDownloader*pDownloader );
	virtual ~CMySnake();

	virtual void Move( double dStep );	// old, do not use
	virtual void Move( CDXDevice&dev, CSegmentsModelContainer&segmentsmodels, double dStep, ISceneTree_Moving*pSceneTreeRoot, CSoundManager&snd );

	virtual	void	OnTextureUpdate( CString&szTexture );
	virtual void	OnSnakeKilled();

	virtual bool	WantToDelete();

private:

	virtual void InitializationDone();

	virtual CSnakeSuperSegment*NewSuperSegment(){ return new CSnakeSuperSegmentEx(); }

	void	UpdateSegmentsModels( CDXDevice&dev, CSegmentsModelContainer&segmentsmodels, ISceneTree_Moving*pSceneTreeRoot );
	void	UpdateSounds( CSoundManager&snd );
	
	CTextureLoader	m_texture, m_normaltexture;
	CString			m_texturename;

	CAudioPath		m_audioPath;
	CSoundSegment	*m_pSndHit, *m_pSndSnaking;

	CSnakeHeadModelObject m_headmodel;
	SegmentsModelList	m_segmentsmodels;

	void	HeadDirectionSmoothUpdate(double dStep);
	double	m_dSmoothHeadDirection;

	CDownloader	*m_pDownloader;
};

#endif//_MYSNAKE_H_