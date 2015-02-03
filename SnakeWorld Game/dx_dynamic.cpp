#include "dx_dynamic.h"


IDirect3D9* pD3D9 = NULL;
IDXGIFactory* pDXGIFactory = NULL;


// Function prototypes
typedef IDirect3D9* (WINAPI * LPDIRECT3DCREATE9) (UINT);
typedef INT         (WINAPI * LPD3DPERF_BEGINEVENT)(D3DCOLOR, LPCWSTR);
typedef INT         (WINAPI * LPD3DPERF_ENDEVENT)(void);
typedef VOID        (WINAPI * LPD3DPERF_SETMARKER)(D3DCOLOR, LPCWSTR);
typedef VOID        (WINAPI * LPD3DPERF_SETREGION)(D3DCOLOR, LPCWSTR);
typedef BOOL        (WINAPI * LPD3DPERF_QUERYREPEATFRAME)(void);
typedef VOID        (WINAPI * LPD3DPERF_SETOPTIONS)( DWORD dwOptions );
typedef DWORD       (WINAPI * LPD3DPERF_GETSTATUS)( void );
typedef HRESULT     (WINAPI * LPCREATEDXGIFACTORY)(REFIID, void ** );
typedef HRESULT     (WINAPI * LPD3D10CREATEDEVICE)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT32, ID3D10Device** );
typedef HRESULT     (WINAPI * LPD3D10CREATEDEVICE1)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1, UINT, ID3D10Device1** );
typedef HRESULT     (WINAPI * LPD3D10CREATESTATEBLOCK)( ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock );
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKUNION)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKINTERSECT)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKDIFFERENCE)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKENABLECAPTURE)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKDISABLECAPTURE)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKENABLEALL)(D3D10_STATE_BLOCK_MASK *pMask);
typedef HRESULT     (WINAPI * LPD3D10STATEBLOCKMASKDISABLEALL)(D3D10_STATE_BLOCK_MASK *pMask);
typedef BOOL        (WINAPI * LPD3D10STATEBLOCKMASKGETSETTING)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry);


// Module and function pointers
static HMODULE s_hModD3D9 = NULL;
static LPDIRECT3DCREATE9 s_DynamicDirect3DCreate9 = NULL;
static LPD3DPERF_BEGINEVENT s_DynamicD3DPERF_BeginEvent = NULL;
static LPD3DPERF_ENDEVENT s_DynamicD3DPERF_EndEvent = NULL;
static LPD3DPERF_SETMARKER s_DynamicD3DPERF_SetMarker = NULL;
static LPD3DPERF_SETREGION s_DynamicD3DPERF_SetRegion = NULL;
static LPD3DPERF_QUERYREPEATFRAME s_DynamicD3DPERF_QueryRepeatFrame = NULL;
static LPD3DPERF_SETOPTIONS s_DynamicD3DPERF_SetOptions = NULL;
static LPD3DPERF_GETSTATUS s_DynamicD3DPERF_GetStatus = NULL;
static HMODULE s_hModDXGI = NULL;
static LPCREATEDXGIFACTORY s_DynamicCreateDXGIFactory = NULL;
static HMODULE s_hModD3D10 = NULL;
static HMODULE s_hModD3D101 = NULL;
static LPD3D10CREATESTATEBLOCK s_DynamicD3D10CreateStateBlock = NULL;
static LPD3D10CREATEDEVICE s_DynamicD3D10CreateDevice = NULL;
static LPD3D10CREATEDEVICE1 s_DynamicD3D10CreateDevice1 = NULL;
static LPD3D10STATEBLOCKMASKUNION s_DynamicD3D10StateBlockMaskUnion = NULL;
static LPD3D10STATEBLOCKMASKINTERSECT s_DynamicD3D10StateBlockMaskIntersect = NULL;
static LPD3D10STATEBLOCKMASKDIFFERENCE s_DynamicD3D10StateBlockMaskDifference = NULL;
static LPD3D10STATEBLOCKMASKENABLECAPTURE s_DynamicD3D10StateBlockMaskEnableCapture = NULL;
static LPD3D10STATEBLOCKMASKDISABLECAPTURE s_DynamicD3D10StateBlockMaskDisableCapture = NULL;
static LPD3D10STATEBLOCKMASKENABLEALL s_DynamicD3D10StateBlockMaskEnableAll = NULL;
static LPD3D10STATEBLOCKMASKDISABLEALL s_DynamicD3D10StateBlockMaskDisableAll = NULL;
static LPD3D10STATEBLOCKMASKGETSETTING s_DynamicD3D10StateBlockMaskGetSetting = NULL;


//
// DX 9
//
bool	D3D9_Init()
{
	if( pD3D9 == NULL )
	{
		// This may fail if Direct3D 9 isn't installed
		// This may also fail if the Direct3D headers are somehow out of sync with the installed Direct3D DLLs
		pD3D9 = D3D9_Dynamic_Direct3DCreate9( D3D_SDK_VERSION );
		if( pD3D9 == NULL )
		{
			// If still NULL, then D3D9 is not available
			return false;
		}
	}

	return true;
}
IDirect3D9*	D3D9_GetDirect3D()
{
	if ( D3D9_Init() )
		return pD3D9;
	else
		return NULL;
}
// Ensure function pointers are initialized
static bool D3D9_EnsureAPIs( void )
{
	// If the module is non-NULL, this function has already been called.  Note
	// that this doesn't guarantee that all ProcAddresses were found.
	if( s_hModD3D9 != NULL )
		return true;

	// This may fail if Direct3D 9 isn't installed
	s_hModD3D9 = LoadLibrary( L"d3d9.dll" );
	if( s_hModD3D9 != NULL )
	{
		s_DynamicDirect3DCreate9 = (LPDIRECT3DCREATE9)GetProcAddress( s_hModD3D9, "Direct3DCreate9" );
		s_DynamicD3DPERF_BeginEvent = (LPD3DPERF_BEGINEVENT)GetProcAddress( s_hModD3D9, "D3DPERF_BeginEvent" );
		s_DynamicD3DPERF_EndEvent = (LPD3DPERF_ENDEVENT)GetProcAddress( s_hModD3D9, "D3DPERF_EndEvent" );
		s_DynamicD3DPERF_SetMarker = (LPD3DPERF_SETMARKER)GetProcAddress( s_hModD3D9, "D3DPERF_SetMarker" );
		s_DynamicD3DPERF_SetRegion = (LPD3DPERF_SETREGION)GetProcAddress( s_hModD3D9, "D3DPERF_SetRegion" );
		s_DynamicD3DPERF_QueryRepeatFrame = (LPD3DPERF_QUERYREPEATFRAME)GetProcAddress( s_hModD3D9, "D3DPERF_QueryRepeatFrame" );
		s_DynamicD3DPERF_SetOptions = (LPD3DPERF_SETOPTIONS)GetProcAddress( s_hModD3D9, "D3DPERF_SetOptions" );
		s_DynamicD3DPERF_GetStatus = (LPD3DPERF_GETSTATUS)GetProcAddress( s_hModD3D9, "D3DPERF_GetStatus" );
	}

	return s_hModD3D9 != NULL;
}

void	D3D9_EnsureCleanup()
{
	SAFE_RELEASE(pD3D9);
}


IDirect3D9 * WINAPI D3D9_Dynamic_Direct3DCreate9(UINT SDKVersion) 
{
	if( D3D9_EnsureAPIs() && s_DynamicDirect3DCreate9 != NULL )
		return s_DynamicDirect3DCreate9( SDKVersion );
	else
		return NULL;
}

int WINAPI D3D9_Dynamic_D3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_BeginEvent != NULL )
		return s_DynamicD3DPERF_BeginEvent( col, wszName );
	else
		return -1;
}

int WINAPI D3D9_Dynamic_D3DPERF_EndEvent( void )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_EndEvent != NULL )
		return s_DynamicD3DPERF_EndEvent();
	else
		return -1;
}

void WINAPI D3D9_Dynamic_D3DPERF_SetMarker( D3DCOLOR col, LPCWSTR wszName )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_SetMarker != NULL )
		s_DynamicD3DPERF_SetMarker( col, wszName );
}

void WINAPI D3D9_Dynamic_D3DPERF_SetRegion( D3DCOLOR col, LPCWSTR wszName )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_SetRegion != NULL )
		s_DynamicD3DPERF_SetRegion( col, wszName );
}

BOOL WINAPI D3D9_Dynamic_D3DPERF_QueryRepeatFrame( void )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_QueryRepeatFrame != NULL )
		return s_DynamicD3DPERF_QueryRepeatFrame();
	else
		return FALSE;
}

void WINAPI D3D9_Dynamic_D3DPERF_SetOptions( DWORD dwOptions )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_SetOptions != NULL )
		s_DynamicD3DPERF_SetOptions( dwOptions );
}

DWORD WINAPI D3D9_Dynamic_D3DPERF_GetStatus( void )
{
	if( D3D9_EnsureAPIs() && s_DynamicD3DPERF_GetStatus != NULL )
		return s_DynamicD3DPERF_GetStatus();
	else
		return 0;
}

//
// DX 10
//

static bool D3D10_EnsureAPIs( void )
{
	// If any module is non-NULL, this function has already been called.  Note
	// that this doesn't guarantee that all ProcAddresses were found.
	if( s_hModD3D10 != NULL || s_hModDXGI != NULL || s_hModD3D101 != NULL )
		return true;

	// This may fail if Direct3D 10 isn't installed
	s_hModD3D10 = LoadLibrary( L"d3d10.dll" );
	if( s_hModD3D10 != NULL )
	{
		s_DynamicD3D10CreateStateBlock = (LPD3D10CREATESTATEBLOCK)GetProcAddress( s_hModD3D10, "D3D10CreateStateBlock" );
		s_DynamicD3D10CreateDevice = (LPD3D10CREATEDEVICE)GetProcAddress( s_hModD3D10, "D3D10CreateDevice" );

		s_DynamicD3D10StateBlockMaskUnion = (LPD3D10STATEBLOCKMASKUNION)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskUnion" );
		s_DynamicD3D10StateBlockMaskIntersect = (LPD3D10STATEBLOCKMASKINTERSECT)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskIntersect" );
		s_DynamicD3D10StateBlockMaskDifference = (LPD3D10STATEBLOCKMASKDIFFERENCE)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskDifference" );
		s_DynamicD3D10StateBlockMaskEnableCapture = (LPD3D10STATEBLOCKMASKENABLECAPTURE)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskEnableCapture" );
		s_DynamicD3D10StateBlockMaskDisableCapture = (LPD3D10STATEBLOCKMASKDISABLECAPTURE)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskDisableCapture" );
		s_DynamicD3D10StateBlockMaskEnableAll = (LPD3D10STATEBLOCKMASKENABLEALL)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskEnableAll" );
		s_DynamicD3D10StateBlockMaskDisableAll = (LPD3D10STATEBLOCKMASKDISABLEALL)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskDisableAll" );
		s_DynamicD3D10StateBlockMaskGetSetting = (LPD3D10STATEBLOCKMASKGETSETTING)GetProcAddress( s_hModD3D10, "D3D10StateBlockMaskGetSetting" );
	}

	s_hModDXGI = LoadLibrary( L"dxgi.dll" );
	if( s_hModDXGI )
	{
		s_DynamicCreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress( s_hModDXGI, "CreateDXGIFactory" );
	}

	// This may fail if this machine isn't Windows Vista SP1 or later
	s_hModD3D101 = LoadLibrary( L"d3d10_1.dll" );
	if( s_hModD3D101 != NULL )
	{
		s_DynamicD3D10CreateDevice1 = (LPD3D10CREATEDEVICE1)GetProcAddress( s_hModD3D101, "D3D10CreateDevice1" );
	}

	return (s_hModDXGI!=NULL) && (s_hModD3D10!=NULL);
}


void D3D10_EnsureCleanup()
{
	SAFE_RELEASE (pDXGIFactory);
}

HRESULT WINAPI D3D10_Dynamic_CreateDXGIFactory( REFIID rInterface, void ** ppOut )
{
	if( D3D10_EnsureAPIs() && s_DynamicCreateDXGIFactory != NULL )
		return s_DynamicCreateDXGIFactory( rInterface, ppOut );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10CreateDevice( IDXGIAdapter* pAdapter,
											  D3D10_DRIVER_TYPE DriverType,
											  HMODULE Software,
											  UINT32 Flags,
											  CONST void* pExtensions,
											  UINT32 SDKVersion,
											  ID3D10Device** ppDevice )
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10CreateDevice != NULL )
		return s_DynamicD3D10CreateDevice( pAdapter, DriverType, Software, Flags, SDKVersion, ppDevice );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10CreateDevice1( IDXGIAdapter *pAdapter,
											   D3D10_DRIVER_TYPE DriverType,
											   HMODULE Software,
											   UINT Flags,
											   D3D10_FEATURE_LEVEL1 HardwareLevel,
											   UINT SDKVersion,
											   ID3D10Device1 **ppDevice)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10CreateDevice1 != NULL )
		return s_DynamicD3D10CreateDevice1( pAdapter, DriverType, Software, Flags, HardwareLevel, SDKVersion, ppDevice );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10CreateStateBlock( ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock )
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10CreateStateBlock != NULL )
		return s_DynamicD3D10CreateStateBlock( pDevice, pStateBlockMask, ppStateBlock );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskUnion(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskUnion != NULL )
		return s_DynamicD3D10StateBlockMaskUnion( pA, pB, pResult );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskIntersect(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskIntersect != NULL )
		return s_DynamicD3D10StateBlockMaskIntersect( pA, pB, pResult );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDifference(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskDifference != NULL )
		return s_DynamicD3D10StateBlockMaskDifference( pA, pB, pResult );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskEnableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskEnableCapture != NULL )
		return s_DynamicD3D10StateBlockMaskEnableCapture( pMask, StateType, RangeStart, RangeLength );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDisableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskDisableCapture != NULL )
		return s_DynamicD3D10StateBlockMaskDisableCapture( pMask, StateType, RangeStart, RangeLength );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskEnableAll(D3D10_STATE_BLOCK_MASK *pMask)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskEnableAll != NULL )
		return s_DynamicD3D10StateBlockMaskEnableAll( pMask );
	else
		return E_FAIL;
}

HRESULT WINAPI D3D10_Dynamic_D3D10StateBlockMaskDisableAll(D3D10_STATE_BLOCK_MASK *pMask)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskDisableAll != NULL )
		return s_DynamicD3D10StateBlockMaskDisableAll( pMask );
	else
		return E_FAIL;
}

BOOL WINAPI D3D10_Dynamic_D3D10StateBlockMaskGetSetting(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry)
{
	if( D3D10_EnsureAPIs() && s_DynamicD3D10StateBlockMaskGetSetting != NULL )
		return s_DynamicD3D10StateBlockMaskGetSetting( pMask, StateType, Entry );
	else
		return E_FAIL;
}

HRESULT D3D10_Dynamic_CreateSwapChain10( ID3D10Device*pd3dDevice, DXGI_SWAP_CHAIN_DESC*sd, IDXGISwapChain**ppSwapChain )
{
	if (D3D10_EnsureAPIs())
		return pDXGIFactory->CreateSwapChain( pd3dDevice, sd, ppSwapChain );
	else
		return E_FAIL;
}


bool	D3D10_Init()
{
	if( pDXGIFactory == NULL )
	{
		D3D10_Dynamic_CreateDXGIFactory( __uuidof( IDXGIFactory ), (LPVOID*)&pDXGIFactory );

		if( pDXGIFactory == NULL )
		{
			return false;
		}

		return true;
	}

	return true;
}