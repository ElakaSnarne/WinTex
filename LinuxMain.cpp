#include "Platform.h"

#ifdef PLATFORM_LINUX

#include "Globals.h"
#include "Configuration.h"
#include "GameController.h"
#include "AnimationController.h"
#include "UAKMGame.h"
#include "PDGame.h"
#include "DXScreen.h"
#include "DXControls.h"
#include "DXSound.h"
#include "MIDIPlayer.h"
#include "PDMIDIPlayer.h"
#include "ModuleController.h"
#include "ConstantBuffers.h"
#include "InputMapping.h"
#include <iostream>
#include <unistd.h> 
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

BOOL _runDXThread = TRUE;
std::vector<SDL_GameController*> _gamepads;

int MapSDLKeyToVK(SDL_Keycode sym) {
    if (sym >= SDLK_a && sym <= SDLK_z) return 'A' + (sym - SDLK_a);
    if (sym >= SDLK_0 && sym <= SDLK_9) return '0' + (sym - SDLK_0);
    if (sym == SDLK_UP) return VK_UP;
    if (sym == SDLK_DOWN) return VK_DOWN;
    if (sym == SDLK_LEFT) return VK_LEFT;
    if (sym == SDLK_RIGHT) return VK_RIGHT;
    if (sym == SDLK_SPACE) return VK_SPACE;
    if (sym == SDLK_RETURN) return VK_RETURN;
    if (sym == SDLK_ESCAPE) return VK_ESCAPE;
    if (sym == SDLK_BACKSPACE) return VK_BACK;
    if (sym == SDLK_TAB) return VK_TAB;
    if (sym == SDLK_LSHIFT || sym == SDLK_RSHIFT) return VK_SHIFT;
    if (sym == SDLK_LCTRL || sym == SDLK_RCTRL) return VK_CONTROL;
    if (sym == SDLK_LALT || sym == SDLK_RALT) return VK_MENU;
    if (sym == SDLK_INSERT) return VK_INSERT;
    if (sym == SDLK_DELETE) return VK_DELETE;
    if (sym == SDLK_HOME) return VK_HOME;
    if (sym == SDLK_END) return VK_END;
    if (sym == SDLK_PAGEUP) return VK_PRIOR;
    if (sym == SDLK_PAGEDOWN) return VK_NEXT;
    if (sym >= SDLK_F1 && sym <= SDLK_F12) return VK_F1 + (sym - SDLK_F1);
    return 0;
}

int MapSDLScancodeToWinScan(SDL_Scancode sc) {
    switch (sc) {
        case SDL_SCANCODE_W: return 0x11;
        case SDL_SCANCODE_A: return 0x1E;
        case SDL_SCANCODE_S: return 0x1F;
        case SDL_SCANCODE_D: return 0x20;
        case SDL_SCANCODE_Q: return 0x10;
        case SDL_SCANCODE_E: return 0x12;
        case SDL_SCANCODE_R: return 0x13;
        case SDL_SCANCODE_T: return 0x14;
        case SDL_SCANCODE_Y: return 0x15;
        case SDL_SCANCODE_U: return 0x16;
        case SDL_SCANCODE_I: return 0x17;
        case SDL_SCANCODE_O: return 0x18;
        case SDL_SCANCODE_P: return 0x19;
        case SDL_SCANCODE_F: return 0x21;
        case SDL_SCANCODE_G: return 0x22;
        case SDL_SCANCODE_H: return 0x23;
        case SDL_SCANCODE_J: return 0x24;
        case SDL_SCANCODE_K: return 0x25;
        case SDL_SCANCODE_L: return 0x26;
        case SDL_SCANCODE_Z: return 0x2C;
        case SDL_SCANCODE_X: return 0x2D;
        case SDL_SCANCODE_C: return 0x2E;
        case SDL_SCANCODE_V: return 0x2F;
        case SDL_SCANCODE_B: return 0x30;
        case SDL_SCANCODE_N: return 0x31;
        case SDL_SCANCODE_M: return 0x32;
        case SDL_SCANCODE_LSHIFT: return 0x2A;
        case SDL_SCANCODE_RSHIFT: return 0x36;
        case SDL_SCANCODE_LCTRL: return 0x1D;
        case SDL_SCANCODE_RCTRL: return 0x1D; // Windows maps both to 1D but with extended bit for right
        case SDL_SCANCODE_LALT: return 0x38;
        case SDL_SCANCODE_RALT: return 0x38;
        case SDL_SCANCODE_ESCAPE: return 0x01;
        case SDL_SCANCODE_RETURN: return 0x1C;
        case SDL_SCANCODE_SPACE: return 0x39;
        case SDL_SCANCODE_UP: return 0x48;
        case SDL_SCANCODE_DOWN: return 0x50;
        case SDL_SCANCODE_LEFT: return 0x4B;
        case SDL_SCANCODE_RIGHT: return 0x4D;
        case SDL_SCANCODE_TAB: return 0x0F;
        case SDL_SCANCODE_BACKSPACE: return 0x0E;
        case SDL_SCANCODE_INSERT: return 0x52;
        case SDL_SCANCODE_DELETE: return 0x53;
        case SDL_SCANCODE_HOME: return 0x47;
        case SDL_SCANCODE_END: return 0x4F;
        case SDL_SCANCODE_PAGEUP: return 0x49;
        case SDL_SCANCODE_PAGEDOWN: return 0x51;
        default: return 0;
    }
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        gDataPath = argv[1];
    }

    BOOL uakm = TRUE; // Default to UAKM for now
    isUAKM = TRUE;
    
    pConfig = new CConfiguration(L"Under A Killing Moon");
    
    // Init DX
    // Force windowed mode for now
    if (!dx.Init(NULL, 1280, 720, TRUE, FALSE)) {
        std::cerr << "Failed to init DX" << std::endl;
        return 1;
    }
    
    SDL_ShowCursor(SDL_DISABLE);

    CDXControls::Init();
    
    // Initial 2D Setup
    CConstantBuffers::Setup2D(dx);

    if (uakm)
        pMIDI = new CMIDIPlayer();
    else
        pMIDI = new CPDMIDIPlayer();

    CGameController::Init();
    CAnimationController::Init();
    
    CGameBase* pGame = NULL;
    if (uakm)
        pGame = new CUAKMGame();

    if (pGame)
    {
        CGameController::StartGame(pGame);
        
        CInputMapping::LoadControlsMap();
        CModuleController::MainThreadId = 1; // Dummy ID for Linux

        // Game Loop
        Uint32 lastTime = SDL_GetTicks();
        SDL_Event event;

        while (_runDXThread)
        {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    _runDXThread = FALSE;
                }
                else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                        CModuleController::GotFocus();
                    }
                    else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        CModuleController::LostFocus();
                    }
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                         // _runDXThread = FALSE; // Don't quit on ESC, let the game handle it
                    } 
                    
                    int vk = MapSDLKeyToVK(event.key.keysym.sym);
                    int scan = MapSDLScancodeToWinScan(event.key.keysym.scancode);
                    // Construct lParam like Windows: 
                    // 0-15: Repeat count (1)
                    // 16-23: Scan code
                    // 24: Extended key
                    // 29: Context code
                    // 30: Previous key state
                    // 31: Transition state
                    LPARAM lParam = (scan << 16) | 1; 
                    if (vk != 0) CModuleController::KeyDown(vk, lParam);
                }
                else if (event.type == SDL_KEYUP) {
                    int vk = MapSDLKeyToVK(event.key.keysym.sym);
                    int scan = MapSDLScancodeToWinScan(event.key.keysym.scancode);
                    LPARAM lParam = (scan << 16) | 0xC0000001; // Transition state 1, Previous state 1
                    if (vk != 0) CModuleController::KeyUp(vk, lParam);
                }
                else if (event.type == SDL_MOUSEMOTION) {
                    POINT pt;
                    if (SDL_GetRelativeMouseMode()) {
                        pt.x = (dx.GetWidth() / 2) + event.motion.xrel;
                        pt.y = (dx.GetHeight() / 2) + event.motion.yrel;
                    } else {
                        pt.x = event.motion.x;
                        pt.y = event.motion.y;
                    }
                    CModuleController::MouseMove(pt);
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    POINT pt;
                    pt.x = event.button.x;
                    pt.y = event.button.y;
                    int btn = (event.button.button == SDL_BUTTON_LEFT) ? -1 : (event.button.button == SDL_BUTTON_MIDDLE) ? 0 : 1;
                    CModuleController::MouseDown(pt, btn);
                }
                else if (event.type == SDL_MOUSEBUTTONUP) {
                    POINT pt;
                    pt.x = event.button.x;
                    pt.y = event.button.y;
                    int btn = (event.button.button == SDL_BUTTON_LEFT) ? -1 : (event.button.button == SDL_BUTTON_MIDDLE) ? 0 : 1;
                    CModuleController::MouseUp(pt, btn);
                }
                else if (event.type == SDL_MOUSEWHEEL) {
                    // Windows WM_MOUSEWHEEL uses increments of 120 (WHEEL_DELTA)
                    // SDL uses steps.
                    CModuleController::MouseWheel(event.wheel.y * 120);
                }
                else if (event.type == SDL_CONTROLLERDEVICEADDED) {
                    SDL_GameController* pad = SDL_GameControllerOpen(event.cdevice.which);
                    if (pad) {
                        _gamepads.push_back(pad);
                        std::cout << "Gamepad added: " << SDL_GameControllerName(pad) << std::endl;
                    }
                }
                else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
                    SDL_GameController* pad = SDL_GameControllerFromInstanceID(event.cdevice.which);
                    if (pad) {
                        SDL_GameControllerClose(pad);
                        for (auto it = _gamepads.begin(); it != _gamepads.end(); ++it) {
                            if (*it == pad) {
                                _gamepads.erase(it);
                                break;
                            }
                        }
                         std::cout << "Gamepad removed" << std::endl;
                    }
                }
                else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
                    InputSource source = InputSource::JoystickButton;
                    int offset = 48 + event.cbutton.button; // Map SDL buttons starting at DI offset 48
                    int data = (event.type == SDL_CONTROLLERBUTTONDOWN) ? 0x80 : 0x00;
                    if (data & 0x80) data = (data & 0x7f) | 0x80000000; // Match Windows behavior for button press
                    
                    CModuleController::GamepadInput(source, offset, data);
                }
                else if (event.type == SDL_CONTROLLERAXISMOTION) {
                    InputSource source = InputSource::JoystickAxis;
                    int offset = -1;
                    switch (event.caxis.axis) {
                        case SDL_CONTROLLER_AXIS_LEFTX: offset = 0; break;
                        case SDL_CONTROLLER_AXIS_LEFTY: offset = 4; break;
                        case SDL_CONTROLLER_AXIS_RIGHTX: offset = 12; break; // lRx
                        case SDL_CONTROLLER_AXIS_RIGHTY: offset = 16; break; // lRy
                        case SDL_CONTROLLER_AXIS_TRIGGERLEFT: offset = 8; break; // lZ
                        case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: offset = 20; break; // lRz
                    }
                    
                    if (offset != -1) {
                         // Scale -32768..32767 to -1000..1000
                         int val = (int)(event.caxis.value * 1000.0f / 32767.0f);
                         CModuleController::GamepadInput(source, offset, val);
                    }
                }
            }

            Uint32 currentTime = SDL_GetTicks();
            Uint32 deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            CGameController::Tick(deltaTime);
            CModuleController::Render();
            
            // Small sleep to prevent 100% CPU usage
            SDL_Delay(1);
        }
        
        if (pGame) delete pGame;
    }
    
    // Close gamepads
    for (auto pad : _gamepads) {
        SDL_GameControllerClose(pad);
    }
    _gamepads.clear();
    
    dx.Dispose();
    
    return 0;
}

#endif
