// OpenGLBase.cpp - A multi-threaded OpenGL base application for GPWiki tutorials.
// Windows version.

#include "stdafx.h"
#ifdef _WINDOWS
#include "Windows/Framework.h"
#endif
#include "MainObj.h"
#include "Touches.h"

#pragma comment (lib , "opengl32.lib") // Makes VC link the GL libs, 
#pragma comment (lib , "glu32.lib")    // other compliers will have to do it manually

// Globals
HINSTANCE gInst;
HWND hGLWin;
HWND hRot;
HDC  GLDC;
HDC  GLDC2;
int  volatile RunLevel = 1;
bool Keys[256];  // Key monitor
MouseInfo Mouse; // Mouse monitor
MouseInfo LastMouse; // Mouse monitor

HDC glGLDC;
HBITMAP glBM;
HBITMAP glOldBM;

int ScrX;
int ScrY;
int BPP;

#define WINDOW_TITLE_NORMAL		(_T("OpenGL game"))
#define WINDOW_TITLE_ROTATED	(_T("OpenGL game (rotated)"))

bool RegisterWin();
bool StartGL();
bool RenderProc(LPVOID lpParam);

HANDLE info_mutex = 0;

MainObj* _mainObj;


// application entry point
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	MSG msg;
	//DEVMODE ScrRes;
	HANDLE hThr;
	DWORD Res;

	// Set the global instance
	gInst = hInstance;

	// Store the current screen resolution
	//EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&ScrRes);

	// Register our Window Class
	if(!RegisterWin())
	{
		MessageBox(NULL,_T("Register Window Class Failed!"),_T("Error"),MB_OK | MB_ICONERROR);
		return 0;
	}

	info_mutex = CreateMutex(NULL, FALSE, NULL);

	// Start GL Window
	ScrX = 320;
	ScrY = 480;
	BPP = 32;
	if(!StartGL())
	{
		MessageBox(NULL,_T("GL Startup Failed!"),_T("Error"),MB_OK | MB_ICONERROR);
		return 0;
	}

	// Launch rendering thread
	hThr=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RenderProc,0,0,NULL);

	if(hThr) 
	{
		RunLevel=1;
		// Main message loop:
		while(RunLevel) 
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	// Shutdown and cleanup

	// Wait for thread to stop
	Res=STILL_ACTIVE;

	while(Res==STILL_ACTIVE)
		GetExitCodeThread(hThr,&Res);

	CloseHandle(info_mutex);

	// Close window
	if(hGLWin)
		DestroyWindow(hGLWin);
	if(hRot)
		DestroyWindow(hRot);

	UnregisterClass(_T("OpenGLBaseWin"), gInst);
	UnregisterClass(_T("OpenGLBaseRotated"), gInst);

	// Restore Original Screen Mode
	//	ChangeDisplaySettings(&ScrRes,CDS_RESET);

	return (int) msg.wParam;
}
WINDOWPLACEMENT wp;
WINDOWPLACEMENT wp2;
void Align()
{
	uint x_edge = GetSystemMetrics(SM_CXEDGE);
	uint y_edge = GetSystemMetrics(SM_CYEDGE);
	uint caption = GetSystemMetrics(SM_CYCAPTION);
	// main window next to rot window
	GetWindowPlacement(hGLWin, &wp);
	GetWindowPlacement(hRot, &wp2);
	if (wp.rcNormalPosition.left != wp2.rcNormalPosition.right)
	{
		wp.rcNormalPosition.left = wp2.rcNormalPosition.right;
		wp.rcNormalPosition.right = wp.rcNormalPosition.left + x_edge + x_edge + 320;
		wp.rcNormalPosition.top = wp2.rcNormalPosition.top - 160;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + y_edge + y_edge + caption + 480;
		SetWindowPlacement(hGLWin, &wp);
		//BringWindowToTop(hGLWin);
		//BringWindowToTop(hRot);
	}
}
void Align2()
{
	uint x_edge = GetSystemMetrics(SM_CXEDGE);
	uint y_edge = GetSystemMetrics(SM_CYEDGE);
	uint caption = GetSystemMetrics(SM_CYCAPTION);
	// rot window next to main window
	GetWindowPlacement(hRot, &wp);
	GetWindowPlacement(hGLWin, &wp2);
	if (wp.rcNormalPosition.right != wp2.rcNormalPosition.left)
	{
		wp.rcNormalPosition.right = wp2.rcNormalPosition.left;
		wp.rcNormalPosition.left = wp.rcNormalPosition.right - (x_edge + x_edge + 480);
		wp.rcNormalPosition.top = wp2.rcNormalPosition.top + 160;
		wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + (y_edge + y_edge + caption + 320);
		SetWindowPlacement(hRot, &wp);
		//BringWindowToTop(hRot);
		//BringWindowToTop(hGLWin);
	}
}

// Message Handler for our Window
LRESULT CALLBACK GLWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	BOOL mutex = false;
	if (Msg == WM_KEYDOWN || Msg == WM_KEYUP || Msg== WM_MOUSEMOVE|| Msg==WM_LBUTTONDOWN || Msg==WM_LBUTTONUP||Msg==WM_RBUTTONDOWN||Msg==WM_RBUTTONUP)
	{
		DWORD dwWaitResult = WaitForSingleObject(info_mutex, 5000L);
		if (dwWaitResult == WAIT_TIMEOUT || dwWaitResult == WAIT_ABANDONED)
			return 0;
		mutex = true;
	}
	__try
	{
		switch(Msg) 
		{
		case WM_WINDOWPOSCHANGED:
			Align2();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

			// Grab inputs
		case WM_KEYDOWN:
			Keys[wParam] = TRUE;
			return 0;

		case WM_KEYUP:							
			Keys[wParam] = FALSE;	
			return 0;

		case WM_MOUSEMOVE:
			{
				Mouse.Mx = LOWORD(lParam);
				Mouse.My = 480 - HIWORD(lParam);
				return 0;
			}

		case WM_LBUTTONDOWN:
			Mouse.Mleft=TRUE;
			return 0;

		case WM_LBUTTONUP:
			Mouse.Mleft=FALSE;
			return 0;

		case WM_RBUTTONDOWN:
			Mouse.Mright=TRUE;
			return 0;

		case WM_RBUTTONUP:
			Mouse.Mright=FALSE;
			return 0;

		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
	}
	__finally
	{
		if (mutex) ReleaseMutex(info_mutex);
	}

	return 0;
}

LRESULT CALLBACK GLWinProc2(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	BOOL mutex = false;
	if (Msg == WM_KEYDOWN || Msg == WM_KEYUP || Msg== WM_MOUSEMOVE|| Msg==WM_LBUTTONDOWN || Msg==WM_LBUTTONUP||Msg==WM_RBUTTONDOWN||Msg==WM_RBUTTONUP)
	{
		DWORD dwWaitResult = WaitForSingleObject(info_mutex, 5000L);
		if (dwWaitResult == WAIT_TIMEOUT || dwWaitResult == WAIT_ABANDONED)
			return 0;
		mutex = true;
	}
	__try
	{
		switch(Msg) 
		{
		case WM_WINDOWPOSCHANGED:
			Align();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

			// Grab inputs
		case WM_KEYDOWN:
			Keys[wParam] = TRUE;
			return 0;

		case WM_KEYUP:							
			Keys[wParam] = FALSE;	
			return 0;

		case WM_MOUSEMOVE:
			{
				Mouse.Mx = 320-HIWORD(lParam);
				Mouse.My = 480-LOWORD(lParam);
				return 0;
			}

		case WM_LBUTTONDOWN:
			Mouse.Mleft=TRUE;
			return 0;

		case WM_LBUTTONUP:
			Mouse.Mleft=FALSE;
			return 0;

		case WM_RBUTTONDOWN:
			Mouse.Mright=TRUE;
			return 0;

		case WM_RBUTTONUP:
			Mouse.Mright=FALSE;
			return 0;

		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
	}
	__finally
	{
		if (mutex) ReleaseMutex(info_mutex);
	}

	return 0;
}
// Register a Window Class
bool RegisterWin()
{
	WNDCLASSEX glWin;

	glWin.cbSize=sizeof(WNDCLASSEX); 
	glWin.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC; // Window has its own context
	glWin.lpfnWndProc = GLWinProc;				         
	glWin.cbClsExtra = 0;
	glWin.cbWndExtra = 0;
	glWin.hInstance = gInst;
	glWin.hIcon = LoadIcon(NULL, IDI_WINLOGO);      // Default icon
	glWin.hCursor = LoadCursor(NULL, IDC_ARROW);    // Default pointer
	glWin.hbrBackground = NULL;
	glWin.lpszMenuName = NULL;
	glWin.lpszClassName = _T("OpenGLBaseWin");
	glWin.hIconSm=NULL;

	if(!RegisterClassEx(&glWin))
		return FALSE;

	glWin.cbSize=sizeof(WNDCLASSEX); 
	glWin.style = CS_VREDRAW | CS_HREDRAW;  // | CS_OWNDC; // Window has its own context
	glWin.lpfnWndProc = GLWinProc2;
	glWin.cbClsExtra = 0;
	glWin.cbWndExtra = 0;
	glWin.hInstance = gInst;
	glWin.hIcon = LoadIcon(NULL, IDI_WINLOGO);      // Default icon
	glWin.hCursor = LoadCursor(NULL, IDC_ARROW);    // Default pointer
	glWin.hbrBackground = NULL;
	glWin.lpszMenuName = NULL;
	glWin.lpszClassName = _T("OpenGLBaseRotated");
	glWin.hIconSm=NULL;

	if(RegisterClassEx(&glWin))
		return TRUE;				
	else
		return FALSE;
}

VOID* bits;

bool StartGL()
{
//	DEVMODE ScrMode;
	PIXELFORMATDESCRIPTOR PixFmtReq;
	int PixFmt;
	HGLRC GLRC;


	// Set the screen mode
	//ZeroMemory(&ScrMode,sizeof(DEVMODE));
	//ScrMode.dmSize=sizeof(DEVMODE);
	//ScrMode.dmPelsWidth=ScrX;
	//ScrMode.dmPelsHeight=ScrY;
	//ScrMode.dmBitsPerPel=BPP;
	//ScrMode.dmFields=DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

	//if(ChangeDisplaySettings(&ScrMode,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	//	return FALSE;

	// Create our window

	uint x_edge = GetSystemMetrics(SM_CXEDGE);
	uint y_edge = GetSystemMetrics(SM_CYEDGE);
	uint caption = GetSystemMetrics(SM_CYCAPTION);

	hRot = CreateWindowEx(WS_EX_LEFT | WS_EX_WINDOWEDGE,
		_T("OpenGLBaseRotated"),
		WINDOW_TITLE_ROTATED,
		WS_VISIBLE | WS_BORDER | WS_CAPTION,
		/*(1600-ScrX)/2 + (ScrX + x_edge + x_edge), 1200 - ScrY - 64*/ -1,-1 , ScrY + x_edge + x_edge+2, ScrX + caption + y_edge + y_edge +2,
		NULL, NULL, gInst, NULL);

	hGLWin = CreateWindowEx(WS_EX_LEFT | WS_EX_WINDOWEDGE,
		_T("OpenGLBaseWin"),
		WINDOW_TITLE_NORMAL,
		/*WS_POPUP |*/ WS_VISIBLE | WS_BORDER | WS_CAPTION ,
		/*(1600-ScrX+320) / 2, 1200 - ScrY - 64*/-1,-1, ScrX + x_edge + x_edge+2, ScrY + caption + y_edge + y_edge+2,
		NULL,NULL,gInst,NULL);

	Align2();

	if(hGLWin==NULL)
		return FALSE;

	// Define pixel format for our window
	ZeroMemory(&PixFmtReq,sizeof(PIXELFORMATDESCRIPTOR));
	PixFmtReq.nSize=sizeof (PIXELFORMATDESCRIPTOR);
	PixFmtReq.nVersion=1;
	PixFmtReq.dwFlags= PFD_DRAW_TO_WINDOW |	PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI | PFD_DOUBLEBUFFER;
	PixFmtReq.iPixelType=PFD_TYPE_RGBA;
	PixFmtReq.cColorBits=32; // Color depth as specified in arguments
	PixFmtReq.cDepthBits=24;
	PixFmtReq.iLayerType=PFD_MAIN_PLANE;

	// Get the device context
	GLDC = GetDC(hGLWin);
	GLDC2 = GetDC(hRot);


	BITMAPINFOHEADER bih;
	int size = sizeof(BITMAPINFOHEADER) ;
	memset(&bih, 0, size);

	// Populate bitmapinfo header
	bih.biSize = size;
	bih.biWidth = 320;
	bih.biHeight = 480;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;

	//bits = (BYTE*)malloc(320*480*4);

	glGLDC = ::CreateCompatibleDC(GLDC);
	glBM = ::CreateCompatibleBitmap(GLDC, 480, 480);// (BITMAPINFO*)&bih, DIB_PAL_COLORS, &bits, NULL, 0);
	glOldBM = (HBITMAP)::SelectObject(glGLDC, glBM);

	if(!GLDC) return FALSE;
	if(!glGLDC) return FALSE;

	SetGraphicsMode(GLDC, GM_ADVANCED);

	// Match our specified pixel format to device
	PixFmt=ChoosePixelFormat(GLDC, &PixFmtReq);

	if(PixFmt == 0) return FALSE;

	// Set pixel format 
	if(!SetPixelFormat(GLDC,PixFmt,&PixFmtReq)) return FALSE;

	// Create the OpenGL render context and bind to this thread current
	GLRC = wglCreateContext(GLDC);

	if(!GLRC) return FALSE;
	if(!wglMakeCurrent(GLDC,GLRC)) return FALSE;

	// Clear to black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	FlipBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	FlipBuffers();
	return TRUE;
}

// The render thread start point
bool RenderProc(LPVOID lpParam)
{
	HGLRC glRC;

	// Re-aquire the context as we are in a different thread
	glRC=wglCreateContext(GLDC);
	wglMakeCurrent(GLDC,glRC);

	//TDStartEngine();

	_mainObj = new MainObj();
	_mainObj->Setup(ScrX, ScrY);
	_mainObj->SetOrientation(0);


	// Here's were we bring in the Render function
	Render();

	//TDStopEngine();
	
	_mainObj->TearDown();
	delete _mainObj;

	RunLevel = 0; // Make sure the app stops
	return 0;
}

void FlipBuffers()
{
	SwapBuffers(GLDC);
//	glFlush();
	BitBlt(glGLDC, 0,0 ,320,480, GLDC, 0, 0, SRCCOPY);

	//BitBlt(GLDC, 160, 0, 320, 480, glGLDC, 0, 0, SRCINVERT);

	XFORM xform;
	float a = pi / 3.0f;
	//a = .0f;
	xform.eM11 = cos(a);
	xform.eM12 = -sin(a);
	xform.eM21 = sin(a);
	xform.eM22 = cos(a);

//	xform.eDx = (float)320;
//	xform.eDy = (float)480;
	xform.eDx = (float)100;
	xform.eDy = (float)200;

//	XFORM ident;
//	GetWorldTransform( GLDC2, &ident);
//	SetWorldTransform( GLDC2, &xform );
//	BitBlt(GLDC2, 0, 0, 480, 480, glGLDC, 0, 0, SRCCOPY);
//	SetWorldTransform( GLDC2, &ident);
//

	////BitBlt(
	////	GLDC, 320, 0, 320, 480, GLDC, 0, 0, SRCCOPY);
	POINT pts[3];
	pts[0].x = 0; pts[0].y = 320;	// upper left
	pts[1].x = 0; pts[1].y = 0;	// upper right
	pts[2].x = 480; pts[2].y = 320;  // lower left
	PlgBlt(GLDC2, pts, glGLDC, 0, 0, 320, 480, NULL, 0, 0);
}

int touchnum = 0;
void Render(void)
{
	// Set the background to black
	glClearColor(0,0,0,0);
	//glClear(GL_COLOR_BUFFER_BIT);

	// This loop will run until Esc is pressed

	_mainObj->Restore();
	while(RunLevel)
	{
		if(Keys[VK_ESCAPE]) // Esc Key
			RunLevel=0;

		if (Keys[VK_OEM_2] != 0)
		{
			Keys[VK_OEM_2] = 0;
			_mainObj->SetOrientation(0);
		}
		if (Keys[VK_OEM_COMMA] != 0)
		{
			Keys[VK_OEM_COMMA] = 0;
			_mainObj->SetOrientation(-1);
		}
		if (Keys[VK_OEM_PERIOD] != 0)
		{
			Keys[VK_OEM_PERIOD] = 0;
			_mainObj->SetOrientation(1);
		}

		if (Keys['1'] != 0 || Keys[VK_NUMPAD1] != 0)
		{
			Keys['1'] = 0;
			Keys[VK_NUMPAD1] = 0;
			touchnum = 0;
		}
		if (Keys['2'] != 0 || Keys[VK_NUMPAD2] != 0)
		{
			Keys['2'] = 0;
			Keys[VK_NUMPAD2] = 0;
			touchnum = 1;
		}
		if (Keys['3'] != 0 || Keys[VK_NUMPAD3] != 0)
		{
			Keys['3'] = 0;
			Keys[VK_NUMPAD3] = 0;
			touchnum = 2;
		}
		if (Keys['P'] != 0)
		{
			Keys['P'] = 0;
			_mainObj->Freeze();
		}
		if (Keys['U'] != 0)
		{
			Keys['U'] = 0;
			_mainObj->Thaw();
		}
		if (Keys['N'] != 0)
		{
			Keys['N'] = 0;
			_mainObj->PauseGame();
		}
		if (Keys['M'] != 0)
		{
			Keys['M'] = 0;
			_mainObj->UnpauseGame();
		}

		//if (Keys['S'] != 0)
		//{
		//	Keys['S'] = 0;
		//	_mainObj->Shutdown();
		//}
		//if (Keys['R'] != 0)
		//{
		//	Keys['R'] = 0;
		//	_mainObj->Restore();
		//}


		int started = -1;
		int dragging = -1;
		int ended = -1;

		DWORD dwWaitResult = WaitForSingleObject(info_mutex, 500L);
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			__try
			{
				if (Mouse.Mleft != 0 && LastMouse.Mleft == 0)	// left mouse down (finger 1)
				{
					started = theTouches.TouchStart(Mouse.Mx, Mouse.My);
				}
				else
				{
					if (Mouse.Mleft == 0 && LastMouse.Mleft != 0)	// left mouse up (finger 1)
					{
						ended = theTouches.TouchEnd(Mouse.Mx, Mouse.My);
					}
					else
					{
						if (Mouse.Mleft != 0)
						{
							dragging = theTouches.TouchMove(Mouse.Mx, Mouse.My);
						}
					}
				}

				if (Mouse.Mright!= 0 && LastMouse.Mright == 0)	// left mouse down (finger 1)
				{
					started = theTouches.TouchStart(320 - Mouse.Mx, 480 - Mouse.My);
				}
				else
				{
					if (Mouse.Mright == 0 && LastMouse.Mright != 0)	// left mouse up (finger 1)
					{
						ended = theTouches.TouchEnd(320 - Mouse.Mx, 480 - Mouse.My);
					}
					else
					{
						if (Mouse.Mright != 0)
						{
							dragging = theTouches.TouchMove(320 - Mouse.Mx, 480 - Mouse.My);
						}
					}
				}

				//if (Keys[VK_CONTROL] != 0)
				//{
				//	touch_began[1].x = 320 - touch_began[0].x;
				//	touch_began[1].y = 480 - touch_began[0].y;
				//	touch_began[1].ticks = touch_began[0].ticks;
				//	touch[1].x = 320 - touch[0].x;
				//	touch[1].y = 480 - touch[0].y;
				//	touch[1].ticks = touch[0].ticks;
				//	touch_ended[1].x = 320 - touch_ended[0].x;
				//	touch_ended[1].y = 480 - touch_ended[0].y;
				//	touch_ended[1].ticks = touch_ended[0].ticks;
				//	touch_type[1] = touch_type[0];
				//}

				LastMouse.Mleft = Mouse.Mleft;
				LastMouse.Mright = Mouse.Mright;
				LastMouse.Mx = Mouse.Mx;
				LastMouse.My = Mouse.My;

			}
			__finally
			{
				ReleaseMutex(info_mutex);
			}
		}

		if (started != -1) _mainObj->TouchBegan(started);
		if (dragging != -1) _mainObj->TouchDragging(dragging);
		if (ended != -1) _mainObj->TouchEnded(ended);

		_mainObj->Render();
		// We're using double buffers, so we need to swap to see our stuff
		FlipBuffers();
		Sleep(25);
	}
	_mainObj->Shutdown();
}

void Trace(const char* szFormat, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);

	OutputDebugString(szBuff);
}

std::string Format(const char* szFormat, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	return std::string(szBuff);
}