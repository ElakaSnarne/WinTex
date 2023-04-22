#include <windows.h>
#include <windowsx.h>
#include "D3D11-NoWarn.h"
#include "D3DX11-NoWarn.h"
#include "D3DX10-NoWarn.h"
#include "LZ.h"
#include "File.h"
#include "Utilities.h"
#include "PTF.h"
#include "DirectX.h"
#include "DXText.h"
#include "Globals.h"
#include "UAKMGame.h"
#include "PDGame.h"
#include "DXScreen.h"
#include "DXShader.h"
#include "resource.h"
#include "Configuration.h"
#include "AnimationController.h"
#include "GameController.h"
#include "Gamepad.h"

#include "VideoModule.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

DWORD WINAPI Direct3DThread(LPVOID lpParameter);
DWORD WINAPI TimerThread(LPVOID lpParameter);
BOOL _runDXThread = TRUE;

void Dispose()
{
	CGamepadController::Dispose();
	CDXControls::Dispose();
	CConstantBuffers::Dispose();
	CShaders::Dispose();
	dx.Dispose();
}

BOOL InitD3D()
{
	if (dx.Init(_hWnd, pConfig->Width, pConfig->Height, !pConfig->FullScreen, pConfig->AnisotropicFilter) == FALSE)
	{
		//MessageBox(_hWnd, dx.ErrorMessage, L"InitD3D", MB_OK);
		return FALSE;
	}

	CDXControls::Init();

	CConstantBuffers::Setup2D(dx);	// Prepare for 2D rendering of title animation

	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
		PostThreadMessage(CModuleController::MainThreadId, message, wParam, lParam);
		return 0;
		break;
	case WM_DESTROY:
	{
		// close the application entirely
		_runDXThread = FALSE;
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYUP:
	case WM_KEYDOWN:
	{
		PostThreadMessage(CModuleController::D3DThreadId, message, wParam, lParam);
		return 0;
		break;
	}
	case WM_SIZE:
	{
		dx.Resize(lParam & 0x7fff, (lParam >> 16) & 0x7fff);
		break;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (CoInitializeEx(NULL, COINIT_MULTITHREADED) == S_OK)
	{
		int sw = GetSystemMetrics(SM_CXSCREEN);
		int sh = GetSystemMetrics(SM_CYSCREEN);

		BOOL uakm = FALSE, pd = FALSE;
		HICON hIcon = NULL;
		std::wstring windowTitle = L"";
		if (CFile::Exists(L"TEX3.EXE"))
		{
			uakm = TRUE;
			isUAKM = TRUE;
			windowTitle = L"Tex Murphy: Under a Killing Moon v1.0";
			hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_UAKM));
			pConfig = new CConfiguration(L"Under A Killing Moon");
		}
		else if (CFile::Exists(L"TEX4.EXE"))
		{
			pd = TRUE;
			isUAKM = FALSE;
			windowTitle = L"Tex Murphy: The Pandora Directive v0.0";
			hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_PD));
			pConfig = new CConfiguration(L"The Pandora Directive");
		}

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = hIcon;
		wc.lpszClassName = L"WinTex";
		RegisterClassEx(&wc);

		RECT wr = { 0, 0, pConfig->Width, pConfig->Height };
		AdjustWindowRect(&wr, WS_CAPTION, FALSE);

		int windowX = (sw - pConfig->Width) / 2 + wr.left;
		int windowY = (sh - pConfig->Height) / 2 + wr.top;
		int windowWidth = wr.right - wr.left;
		int windowHeight = wr.bottom - wr.top;

		// create the window and use the result as the handle
		_hWnd = CreateWindowEx(0,
			L"WinTex",									// name of the window class
			windowTitle.c_str(),						// title of the window in release mode
			WS_CAPTION,									// window style
			windowX,									// x-position of the window
			windowY,									// y-position of the window
			windowWidth,								// width of the window
			windowHeight,								// height of the window
			NULL,										// we have no parent window, NULL
			NULL,										// we aren't using menus, NULL
			hInstance,									// application handle
			NULL);										// used with multiple windows, NULL

		ShowWindow(_hWnd, nCmdShow);
		ShowCursor(FALSE);

		SetCursorPos(windowX + windowWidth / 2, windowY + windowHeight / 2);

		if (InitD3D())
		{
			try
			{
				if (uakm)
				{
					pMIDI = new CMIDIPlayer();
				}
				else if (pd)
				{
					pMIDI = new CPDMIDIPlayer();
				}

				CGameController::Init();
				CAnimationController::Init();

				CGamepadController::Init(_hWnd);

				//MessageBox(NULL, L"Creating game", NULL, 0);

				CGameBase* pGame = NULL;

				// Check which game folder we're in
				if (uakm)
				{
					pGame = new CUAKMGame();
				}
				else if (pd)
				{
					pGame = new CPDGame();
				}

				if (pGame != NULL)
				{
					CGameController::StartGame(pGame);

					CInputMapping::LoadControlsMap();

					CModuleController::MainThreadId = GetCurrentThreadId();

					HANDLE hThread = CreateThread(NULL, 1048576, Direct3DThread, NULL, 0, &CModuleController::D3DThreadId);
					HANDLE hTimerThread = CreateThread(NULL, 1048576, TimerThread, NULL, 0, &CModuleController::TimerThreadId);

					MSG msg = { 0 };
					while (_runDXThread)
					{
						if (GetMessage(&msg, NULL, 0, 0))
						{
							if (msg.message == WM_CLOSE)
							{
								_runDXThread = FALSE;
								break;
							}
							else if (msg.message == WM_ACTIVATEAPP) {
								if (msg.wParam == TRUE) {
									CModuleController::GotFocus();
								}
								else {
									CModuleController::LostFocus();
								}
							}
							else if (msg.message == WM_DISPLAYCHANGE);// OutputDebugString(L"Display Change\r\n");
							else if (msg.message == WM_MOUSEMOVE && GetFocus() == _hWnd)
							{
								POINT pt;
								pt.x = (msg.lParam) & 0xffff;
								pt.y = (msg.lParam >> 16) & 0xffff;
								CModuleController::MouseMove(pt);
							}
							else if (msg.message == WM_NCMOUSEMOVE);
							else if (msg.message == WM_NCMOUSELEAVE);
							else if (msg.message == WM_DWMNCRENDERINGCHANGED);
							else if (msg.message == WM_PAINT);
							else if (msg.message == WM_TIMER);
							else if (msg.message == WM_NCLBUTTONDOWN);
							else if (msg.message == WM_LBUTTONDOWN || msg.message == WM_MBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
							{
								POINT pt;
								pt.x = (msg.lParam) & 0xffff;
								pt.y = (msg.lParam >> 16) & 0xffff;
								CModuleController::MouseDown(pt, (msg.message == WM_LBUTTONDOWN) ? -1 : (msg.message == WM_MBUTTONDOWN) ? 0 : 1);
							}
							else if (msg.message == WM_LBUTTONUP || msg.message == WM_MBUTTONUP || msg.message == WM_RBUTTONUP)
							{
								POINT pt;
								pt.x = (msg.lParam) & 0xffff;
								pt.y = (msg.lParam >> 16) & 0xffff;
								CModuleController::MouseUp(pt, (msg.message == WM_LBUTTONUP) ? -1 : (msg.message == WM_MBUTTONUP) ? 0 : 1);
							}
							else if (msg.message == WM_MOUSEWHEEL)
							{
								CModuleController::MouseWheel(static_cast<int>(msg.wParam));
							}
							else if (msg.message == WM_KEYDOWN)
							{
								CModuleController::KeyDown(msg.wParam, msg.lParam);
							}
							else if (msg.message == WM_KEYUP)
							{
								CModuleController::KeyUp(msg.wParam, msg.lParam);
							}
							else if (msg.message == WM_SIZE)
							{
								// Resize buffers
								dx.Resize(::pConfig->Width, ::pConfig->Height);
							}
							else if (msg.message == UM_GAMEPAD)
							{
								InputSource source = (InputSource)(msg.wParam >> 16);
								CModuleController::GamepadInput(source, msg.wParam & 0xffff, static_cast<int>(msg.lParam));
							}
							else
							{
								//OutputDebugString(L"Message: ");
								//OutputDebugString(_itow(msg.message, buffer, 16));
								//OutputDebugString(L"\r\n");
							}

							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}

					WaitForSingleObject(hThread, INFINITE);
					CloseHandle(hThread);

					CAnimationController::Clear();

					if (pGame != NULL)
					{
						delete pGame;
						pGame = NULL;
					}
				}
				else if (!uakm && !pd)
				{
					MessageBox(NULL, L"Unable to determine game", NULL, 0);
				}
				else
				{
					MessageBox(NULL, L"Unable to start game", NULL, 0);
				}
			}
			catch (...)
			{
				MessageBox(NULL, L"Caught an exception!", NULL, 0);
			}
		}

		//MessageBox(NULL, L"Disposing", NULL, 0);
		Dispose();

		CoUninitialize();
	}
	else
	{
		MessageBox(NULL, L"Failed to initialize COM", L"Disaster!", 0);
	}

	return 0;
}

DWORD WINAPI Direct3DThread(LPVOID lpParameter)
{
	while (_runDXThread)
	{
		try
		{
			CGamepadController::GamepadController->Update();	// Get joystick events
			CModuleController::Render();
		}
		catch (void*)
		{
			int debug = 0;
		}
	}

	return 0;
}

DWORD WINAPI TimerThread(LPVOID lpParameter)
{
	// All this thread does is check timers
	ULONGLONG time = GetTickCount64();

	while (_runDXThread)
	{
		ULONGLONG now = GetTickCount64();
		CGameController::Tick((int)(now - time));
		time = now;

		Sleep(50);
	}

	return 0;
}
