#ifndef _MYWORLD_H_
#define _MYWORLD_H_

#include "gameworld.h"
#include "mysnake.h"
#include "myapple.h"
#include "butterfly.h"
#include "segmentsmodel.h"
#include "terrain.h"
#include "device.h"
#include "soundmanager.h"
#include "camera.h"
#include "myscenetree.h"
#include "skybox.h"
#include "downloader.h"

//
// for chat message receiving events
//
class CMyGameWorldEvents
{
public:

	virtual void	OnChatMessageReceived( CString&strFrom, CString&strMessage ){}

};

//////////////////////////////////////////////////////////////////////////
// the game world object
class CMyGameWorld: public CGameWorld
{
public:

	CMyGameWorld();
	virtual ~CMyGameWorld();

	bool	CreateWorld( CDXDevice&dev, CDownloader*pDownloader, CMyGameWorldEvents*pEvents );

	void	FrameMove( CDXDevice&dev, CSoundManager&snd, double dStep );

	void	Render( CDXDevice&dev );

	// create new snake object for this world
	virtual CSnake*	NewSnake( bool bUserControlled );

	// create new apple object for this world
	virtual CApple*	NewApple( double x, double y );

	void	ResetProjection( CDXDevice&dev );

	inline	void	SetGrassDetail( float flDetail )
	{
		m_grassmodels.flGrassDetail = flDetail;
	}
	inline	void	EnableShadows( bool bEnable )
	{
		m_bShadowsEnabled = bEnable;
	}

	//
	

private:

	//
	// camera
	//
	CCamera		m_camera;
	D3DXVECTOR3	m_vecCameraEye, m_vecCameraEyeSpeed, m_vecCameraAt, m_vecCameraAtSpeed, m_vecCameraUp, m_vecCameraUpSpeed;
	float		m_dViewDistance;	// relative view distance ( to m_dWorldRefreshDistance ) // [0.0; 1.0]

	void		UpdateCameraMove( double dStep, bool bSetImmediately );
	void		SetTargetCamera_3D( D3DXVECTOR3&vecCameraEyeTo, D3DXVECTOR3&vecCameraAtTo, D3DXVECTOR3&vecCameraUpTo );
	void		SetTargetCamera_Top( D3DXVECTOR3&vecCameraEyeTo, D3DXVECTOR3&vecCameraAtTo, D3DXVECTOR3&vecCameraUpTo );

	void		UpdateCamera( CDXDevice&dev, CSoundManager&snd );
	void		UpdateCamera( CDXDevice&dev );

	//
	// moving
	//

	void		UpdateFrameLength( CDXDevice&dev, double dStep );	// update the effect variable containing frame length in seconds

	virtual void	DoSnakesMove( double dStep ){ /*nothing*/ }	// replaced, do nothing
	virtual void	DoSnakesMove( CDXDevice&dev, CSoundManager&snd, double dStep );	// new DoSnakesMove

	void	DoApplesMove( CDXDevice&dev, double dStep, CBoundBox&keepAliveBox, CSoundManager&snd );	// animate apples

	float m_flSnakingAnimation;	// 0..1: left    -1..0: right

	//
	//  rendering
	//
	void		ClearRenderTarget( CDXDevice&dev );

	void		RenderShadows( CDXDevice&dev );
	void		RenderShadows10( CDX10Device&dev );

	bool			CreateShadow10( CDX10Device&dev );
	ID3D10Buffer	*m_pShadowVB;
	ID3D10EffectTechnique	*m_pShadowTechnique;
	ID3D10InputLayout		*m_pShadowInputLayout;

	bool m_bShadowsEnabled;

	void		RenderSnakesName( CDXDevice &dev );
	void		RenderTop10Snakes( CDXDevice &dev );

	//
	// HUD
	//

	void	HUDFrameMove( double dStep );
	void	HUDRender( CDXDevice&dev );

	float	m_flMessageAnim, m_flShowLengthAfter, m_flShowBestAfter;
	CList<CString>	m_messages;
	void	AddMessage( CString&str );

	// skybox
	CSkyBox			m_skybox;

	// snakes
	CSegmentsModelContainer	m_segmentsmodels;

	// apple model
	CAppleModelContainer	m_applemodel;

	// butterfly
	CButterflyModelContainer	m_butterflymodel;// butterfly model
	ButterflyList				m_butterflies;	// list of butterflies
	CList<CTextureLoader>		m_butterfliestextures;	// list of butterfly textures

	bool	CreateButterflies(CDXDevice&dev);
	void	CreateButterfliesTextures(CDXDevice&dev);

	void	ButterfliesFrameMove( double dStep, CBoundBox&keepAliveBox );
	
	// terrain
	CTerrainContainer		m_terrainmodel;
	CGrassContainer			m_grassmodels;
	CStoneWallContainer		m_stonewallmodels;

	CSceneBaseTree			*m_pSceneTree;
	void					ResetSceneTree();

	//
	// events
	//
	virtual void OnSceneDefReceived();
	virtual void OnUserSnakeInitialized();
	virtual void OnDead( CSnake*pKilledSnake, CSnake*pKilledBy );	// snake dead
	virtual void OnMessage( CString&str );	// message from server
	virtual void OnChatMessage( CString&strUserName, CString&strMessage );	// chat message from the user

	CMyGameWorldEvents	*m_pEvents;

	//
	// downloader
	//
	CDownloader	*m_pDownloader;

};


#endif//_MYWORLD_H_