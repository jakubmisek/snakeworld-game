#ifndef _DX_DYNAMIC_H_
#define _DX_DYNAMIC_H_

//////////////////////////////////////////////////////////////////////////
// Direct3D9 includes
#include <d3d9.h>
#include <d3dx9.h>

//////////////////////////////////////////////////////////////////////////
// Direct3D10 includes
#include <dxgi.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <d3dx10.h>

// XInput includes
#include <xinput.h>

// HRESULT translation for Direct3D10 and other APIs 
#include <dxerr.h>


//
#include "memtool.h"

//
// DX9 API
//
bool	D3D9_Init();
void	D3D9_EnsureCleanup();
IDirect3D9*	D3D9_GetDirect3D();

IDirect3D9*	WINAPI D3D9_Dynamic_Direct3DCreate9(UINT SDKVersion);

int WINAPI D3D9_Dynamic_D3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName );

int WINAPI D3D9_Dynamic_D3DPERF_EndEvent( void );

void WINAPI D3D9_Dynamic_D3DPERF_SetMarker( D3DCOLOR col, LPCWSTR wszName );

void WINAPI D3D9_Dynamic_D3DPERF_SetRegion( D3DCOLOR col, LPCWSTR wszName );

BOOL WINAPI D3D9_Dynamic_D3DPERF_QueryRepeatFrame( void );

void WINAPI D3D9_Dynamic_D3DPERF_SetOptions( DWORD dwOptions );

DWORD WINAPI D3D9_Dynamic_D3DPERF_GetStatus( void );

//
// DX10 API
//


bool	D3D10_Init();
void	D3D10_EnsureCleanup();
HRESULT WINAPI D3D10_Dynamic_D3D10CreateDevice( IDXGIAdapter* pAdapter,
											  D3D10_DRIVER_TYPE DriverType,
											  HMODULE Software,
											  UINT32 Flags,
											  CONST void* pExtensions,
											  UINT32 SDKVersion,
											  ID3D10Device** ppDevice );
HRESULT WINAPI D3D10_Dynamic_D3D10CreateDevice1( IDXGIAdapter *pAdapter,
												D3D10_DRIVER_TYPE DriverType,
												HMODULE Software,
												UINT Flags,
												D3D10_FEATURE_LEVEL1 HardwareLevel,
												UINT SDKVersion,
												ID3D10Device1 **ppDevice);
HRESULT WINAPI D3D10_Dynamic_D3D10CreateStateBlock( ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock );
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskUnion(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskIntersect(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDifference(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskEnableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDisableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskEnableAll(D3D10_STATE_BLOCK_MASK *pMask);
HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDisableAll(D3D10_STATE_BLOCK_MASK *pMask);
BOOL WINAPI D3D10_Dynamic_D3D10StateBlockMaskGetSetting(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry);
HRESULT D3D10_Dynamic_CreateSwapChain10( ID3D10Device*pd3dDevice, DXGI_SWAP_CHAIN_DESC*sd, IDXGISwapChain**ppSwapChain );


#endif//_DX_DYNAMIC_H_