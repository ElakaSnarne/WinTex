#include "PDVidPhoneModule.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "resource.h"
#include "GameController.h"
#include "PDGame.h"
#include <codecvt>
#include <algorithm>

#define VIDPHONE_SCREEN				5
#define VIDPHONE_BUTTONS			7
#define VIDPHONE_SCREEN_RESET		9

#define VIDPHONE_MODE_DEFAULT		0
#define VIDPHONE_MODE_DIALLING		1
#define VIDPHONE_MODE_IN_CALL		2

#define VIDPHONE_IMAGE_DIAL_DOWN	15
#define VIDPHONE_IMAGE_DIAL_UP		19
#define VIDPHONE_IMAGE_EXIT_DOWN	27
#define VIDPHONE_IMAGE_EXIT_UP		31
#define VIDPHONE_IMAGE_MESSAGE_DOWN	39
#define VIDPHONE_IMAGE_MESSAGE_UP	43

#define VIDPHONE_PHONE_NUMBERS		67
#define VIDPHONE_SOUNDS				68

CPDVidPhoneModule::CPDVidPhoneModule() : CFullScreenModule(ModuleType::VidPhone)
{
	_callMap[1] = 0x1b;
	_callMap[6] = 0x25;
	_callMap[8] = 0x14;
	_callMap[12] = 0xc;
	_callMap[14] = 0x15;
	_callMap[16] = 0x28;
	_callMap[17] = 0x11;

	_messages.push_back({ 12, 5 });
	_messages.push_back({ 12, 7 });
	_messages.push_back({ 27, 17 });
	_messages.push_back({ 27, 12 });
	_messages.push_back({ 27, 13 });
	_messages.push_back({ 27, 14 });
	_messages.push_back({ 27, 16 });
	_messages.push_back({ 12, 12 });
	_messages.push_back({ 20, 50 });

	_topCallIndex = 0;
	_mode = VIDPHONE_MODE_DEFAULT;
	_callerIndex = -1;

	_scriptEngine = CGameController::GetScriptEngine();

	_screenResetData = NULL;

	_diallingIndex = -1;

	_sourceVoice = NULL;
	_readyForNextTone = FALSE;
	_nextToneTime = NULL;

	_highlighted = NULL;
}

CPDVidPhoneModule::~CPDVidPhoneModule()
{
	for (auto pb : _phonebook)
	{
		delete pb;
	}

	if (_screenResetData != NULL)
	{
		delete[] _screenResetData;
		_screenResetData = NULL;
	}

	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}

	Dispose();
}

void CPDVidPhoneModule::Initialize()
{
	CFullScreenModule::Initialize();

	CAnimationController::Clear();

	_pdRawFont.Init(IDR_RAWFONT_PD);
	_uakmRawFont.Init(IDR_RAWFONT_UAKM);

	DoubleData dd = LoadDoubleEntry(L"GRAPHICS.AP", VIDPHONE_SCREEN);
	ReadPalette(dd.File1.Data);
	for (int i = 0; i < 256; i++)
	{
		_originalPalette[i] = _palette[i];
	}

	delete[] dd.File1.Data;

	_screen = dd.File2.Data;
	FillMemory(_screen + 640 * 366, 640 * 114, 0);

	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_cursorPosX = static_cast<float>(w) / 2.0f;
	_cursorPosY = static_cast<float>(h) / 2.0f;

	_cursorMinX = 0;
	_cursorMaxX = w;
	_cursorMinY = 0;
	_cursorMaxY = h;

	BinaryData bd = LoadEntry(L"GRAPHICS.AP", VIDPHONE_BUTTONS);
	if (bd.Data != NULL)
	{
		_data = bd.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}
	}

	bd = LoadEntry(L"GRAPHICS.AP", VIDPHONE_SCREEN_RESET);
	if (bd.Data != NULL)
	{
		_screenResetData = bd.Data;
	}

	UpdatePhonebook();
	RenderPhonebook();

	RenderEnterPhoneNumber();

	RenderItem(VIDPHONE_IMAGE_DIAL_DOWN, 296, 325);
	RenderItem(VIDPHONE_IMAGE_MESSAGE_DOWN, 124, 325);

	UpdateTexture();

	DialogueOptions[0].SetClick(DialogueOptionA);
	DialogueOptions[1].SetClick(DialogueOptionB);
	DialogueOptions[2].SetClick(DialogueOptionC);
}

void CPDVidPhoneModule::Resize(int width, int height)
{
}

void CPDVidPhoneModule::Render()
{
	if (_mode == VIDPHONE_MODE_IN_CALL)
	{
		if (CAnimationController::Exists() && CAnimationController::IsDone() && _scriptState.WaitingForInput == FALSE && _scriptState.ExecutionPointer == -1)
		{
			// Reset screen
			_mode = VIDPHONE_MODE_DEFAULT;
			RenderRaw(_screenResetData, 62, 42, 296, 228);

			// Reset vidphone buttons
			RenderItem(VIDPHONE_IMAGE_DIAL_DOWN, 296, 325);
			RenderItem(VIDPHONE_IMAGE_EXIT_UP, 38, 325);
			RenderItem(VIDPHONE_IMAGE_MESSAGE_DOWN, 124, 325);	// TODO: Render up if messages exist

			_callerIndex = -1;
			for (auto pb : _phonebook)
			{
				pb->IsSelected = FALSE;
			}

			RenderPhonebook();

			RenderEnterPhoneNumber();

			UpdateTexture();
		}
	}
	else if (_mode == VIDPHONE_MODE_DIALLING && GetTickCount64() > _nextToneTime)
	{
		if (_diallingIndex == -1)
		{
			_mode = VIDPHONE_MODE_IN_CALL;

			LoadVideo(_callerIndex);
		}
		else if (_readyForNextTone)
		{
			PlayNextTone();
		}
	}

	if (CAnimationController::Exists())
	{
		CAnimationController::SetOutputBuffer(_screen, 640, 480, 62, 42, _palette, 0x60, 256);
		if (CAnimationController::UpdateAndRender())
		{
			UpdateTexture();
		}

		RenderScreen();

		CAnimationController::RenderCaptions(-1.0f);

		if (_scriptState.WaitingForInput && CAnimationController::IsDone())
		{
			if (DialogueOptionsCount >= 1)
			{
				DialogueOptions[0].Render();
			}
			if (DialogueOptionsCount >= 2)
			{
				DialogueOptions[1].Render();
			}
			if (DialogueOptionsCount >= 3)
			{
				DialogueOptions[2].Render();
			}

			if (_scriptState.AskAbout || _scriptState.Offer || _scriptState.Buy)
			{
				BOOL recreate = ((_scriptState.AskAbout && CGameController::AskAboutChanged) || (_scriptState.Offer && CGameController::ItemsChanged) || (_scriptState.Buy && CGameController::BuyChanged));

				if (_scriptState.TopItemOffset < 0 || recreate)
				{
					int valToFind = -1;

					std::vector<ListBoxItem> items;
					if (_scriptState.AskAbout && !_scriptState.AskingAboutBuyables)
					{
						int askAboutCount = CGameController::GetAskAboutCount();
						for (int i = 0; i < 256 && i < askAboutCount; i++)
						{
							ListBoxItem lbi;
							lbi.Id = CGameController::GetAskAboutId(i);
							lbi.Text = CGameController::GetAskAboutName(lbi.Id);
							items.push_back(lbi);
						}

						CGameController::AskAboutChanged = FALSE;
						valToFind = 4;
					}
					else if (_scriptState.Offer)
					{
						int itemCount = CGameController::GetItemCount();
						for (int i = 0; i < 256 && i < itemCount; i++)
						{
							ListBoxItem lbi;
							lbi.Id = CGameController::GetItemId(i);
							lbi.Text = CGameController::GetItemName(lbi.Id);
							items.push_back(lbi);
						}

						CGameController::ItemsChanged = FALSE;
						valToFind = 7;
					}
					else if (_scriptState.Buy || (_scriptState.AskAbout && _scriptState.AskingAboutBuyables))
					{
						_askAboutBase = 0x3f;

						int itemCount = CGameController::GetBuyableItemCount();
						for (int i = 0; i < 256 && i < itemCount; i++)
						{
							ListBoxItem lbi;
							lbi.Id = CGameController::GetBuyableItemId(i);
							lbi.Text = CGameController::GetBuyableItemName(lbi.Id);
							items.push_back(lbi);
						}

						CGameController::BuyChanged = FALSE;
						valToFind = _scriptState.Buy ? 6 : 4;
					}

					_scriptState.TopItemOffset = 0;
					// Make list appear over the correct button
					int ix = 0;
					if (DialogueOptions[1].GetValue() == valToFind)
					{
						ix = 1;
					}
					else if (DialogueOptions[2].GetValue() == valToFind)
					{
						ix = 2;
					}
					_listBox.Init(items, floor(DialogueOptions[ix].GetX() + DialogueOptions[ix].GetWidth() / 2.0f));
				}

				_listBox.Render();
			}

			// Render arrow cursor
			CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
			CModuleController::Cursors[0].Render();
		}
	}
	else
	{
		RenderScreen();
	}

	if (!CAnimationController::HasAnim() || CAnimationController::IsDone())
	{
		if (_scriptState.WaitingForMediaToFinish)
		{
			_scriptEngine->Resume(&_scriptState, TRUE);
		}
		else if (_scriptState.ExecutionPointer == -1)
		{
			CAnimationController::Clear();
		}
	}
}

void CPDVidPhoneModule::RenderScreen()
{
	if (_vertexBuffer != NULL)
	{
		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
		CModuleController::Cursors[0].Render();

		dx.EnableZBuffer();
	}
}

void CPDVidPhoneModule::BeginAction()
{
	if (_inputEnabled)
	{
		int x = static_cast<int>((_cursorPosX - _left) / _scale);
		int y = static_cast<int>((_cursorPosY - _top) / _scale);

		if (_mode == VIDPHONE_MODE_DEFAULT)
		{
			if (x >= 450 && x < 640 && y >= 1 && y < 142)
			{
				// Check if a phonebook entry has been clicked
				for (auto pb : _phonebook)
				{
					pb->IsSelected = (y >= pb->Box.top && y < pb->Box.bottom);
					if (pb->IsSelected)
					{
						_callerIndex = pb->Index;
					}
				}

				RenderPhonebook();

				RenderItem(_callerIndex > -1 ? VIDPHONE_IMAGE_DIAL_UP : VIDPHONE_IMAGE_DIAL_DOWN, 296, 325);

				UpdateTexture();
			}

			if (y >= 328 && y <= 365)
			{
				if (x >= 38 && x <= 120)
				{
					// Exit
					CModuleController::Pop(this);
				}
				else if (x >= 123 && x <= 206)
				{
					// Message
				}
				else if (x >= 209 && x <= 292)
				{
					// Hint
				}
				else if (x >= 296 && x <= 378)
				{
					// Dial
					if (_callerIndex >= 0)
					{
						_mode = VIDPHONE_MODE_DIALLING;

						// Disable vidphone buttons
						RenderItem(VIDPHONE_IMAGE_DIAL_DOWN, 296, 325);
						RenderItem(VIDPHONE_IMAGE_EXIT_DOWN, 38, 325);
						RenderItem(VIDPHONE_IMAGE_MESSAGE_DOWN, 124, 325);
						UpdateTexture();

						// Start playing dialling sounds
						_diallingIndex = _callerIndex * 20;
						_readyForNextTone = TRUE;
					}
				}
			}
		}
		else if (_mode == VIDPHONE_MODE_IN_CALL)
		{
#ifndef DEBUG
			if (CGameController::CanCancelVideo())
#endif
			{
				CAnimationController::Skip();

				if (_scriptState.WaitingForInput && (_scriptState.AskAbout || _scriptState.Offer || _scriptState.Buy))
				{
					int hitId = _listBox.HitTestLB(x, y);
					if (hitId >= 0)
					{
						// Set item id and option and resume script
						int option = _scriptState.AskAbout ? 4 : _scriptState.Offer ? 7 : _scriptState.Buy ? 6 : -1;
						CGameController::SetSelectedItem(hitId + ((option == 4) ? _askAboutBase : 0));
						_scriptState.SelectedOption = option;
						_scriptState.SelectedValue = hitId + ((option == 4) ? _askAboutBase : 0);
						_scriptEngine->Resume(&_scriptState, TRUE);
					}
				}

				for (int i = 0; i < 3; i++)
				{
					if (DialogueOptionsCount > i && DialogueOptions[i].HitTest(x, y) != NULL)
					{
						DialogueOptions[i].Click();
						break;
					}
				}
			}
		}
	}
}

void CPDVidPhoneModule::Back()
{
	if (_mode == VIDPHONE_MODE_DEFAULT)
	{
		CModuleController::Pop(this);
	}
}

void CPDVidPhoneModule::UpdatePhonebook()
{
	// Names come from save situation, dialog table
	int y = 1;
	int phonebookSize = CGameController::GetWord(PD_SAVE_VIDPHONE_ENTRIES_COUNT);
	for (int i = 0; i < phonebookSize; i++)
	{
		int index = CGameController::GetData(PD_SAVE_VIDPHONE_ENTRIES + i * 2);
		Phonebook* pBP = new Phonebook();
		pBP->Index = index;
		_phonebook.push_back(pBP);
	}
}

void CPDVidPhoneModule::RenderPhonebook()
{
	// Names come from save situation, dialog table
	int y = 1;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	std::unordered_map<int, int> colourMap;
	for (auto pb : _phonebook)
	{
		std::wstring name = CGameController::GetSituationDescriptionD(pb->Index + 1);
		colourMap[2] = pb->IsSelected ? 15 : 13;
		pb->Box = _pdRawFont.Render(_screen, 640, 480, 462, y, (char*)conv.to_bytes(name).c_str(), colourMap, -1, -1, TRUE);
		y += _pdRawFont.GetHeight();
	}
}

void CPDVidPhoneModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	x = _cursorPosX;
	y = _cursorPosY;

	if (_scriptState.WaitingForInput)
	{
		// Check mouse over options buttons (only the ones that are visible)
		for (int i = 0; i < 3; i++)
		{
			if (DialogueOptionsCount > i)
			{
				// Check if mouse is over button...
				DialogueOptions[i].SetMouseOver(DialogueOptions[i].HitTest(x, y) != NULL);
			}
		}

		if (_scriptState.AskAbout || _scriptState.Offer || _scriptState.Buy)
		{
			_listBox.HitTestLB(x, y);
		}
	}
	else if (_mode == VIDPHONE_MODE_DEFAULT)
	{
		BOOL update = FALSE;
		if (_highlighted != NULL)
		{
			DrawRectangle(_highlighted->Box.left - 3, _highlighted->Box.top - 1, _highlighted->Box.right + 3, _highlighted->Box.bottom - 2, 0);
			update = TRUE;
		}

		if (x >= 450 && x < 640 && y >= 1 && y < 142)
		{
			// Check if a phonebook entry has been clicked
			for (auto pb : _phonebook)
			{
				if (y >= pb->Box.top && y < pb->Box.bottom)
				{
					DrawRectangle(pb->Box.left - 3, pb->Box.top - 1, pb->Box.right + 3, pb->Box.bottom - 2, 15);
					_highlighted = pb;
					update = TRUE;
				}
			}
		}

		if (update)
		{
			UpdateTexture();
		}
	}
}

void CPDVidPhoneModule::DialogueOptionA(LPVOID data)
{
	SelectOption(DialogueOptions[0].GetValue());
}

void CPDVidPhoneModule::DialogueOptionB(LPVOID data)
{
	SelectOption(DialogueOptions[1].GetValue());
}

void CPDVidPhoneModule::DialogueOptionC(LPVOID data)
{
	SelectOption(DialogueOptions[2].GetValue());
}

void CPDVidPhoneModule::SelectOption(int option)
{
	if (CModuleController::CurrentModule != NULL && CModuleController::CurrentModule->Type == ModuleType::VidPhone)
	{
		((CPDVidPhoneModule*)CModuleController::CurrentModule)->SelectDialogueOption(option);
	}
}

void CPDVidPhoneModule::SelectDialogueOption(int option)
{
	_scriptState.TopItemOffset = -1;
	_scriptEngine->SelectDialogueOption(&_scriptState, option);
}

void CPDVidPhoneModule::LoadVideo(int caller)
{
	int dmapIndex = _callMap[caller];

	// Get DMAPData, copy script, reset pointer or set to active script
	_scriptEngine->_mapEntry = CModuleController::pDMap->Get(dmapIndex);
	std::wstring fileName = CGameController::GetFileName(_scriptEngine->_mapEntry->ScriptFileIndex);
	if (fileName != L"")
	{
		BinaryData bd = LoadEntry(fileName.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);
		//_scriptState = CGameController::GetScriptState();
		_scriptState.Init(bd.Data, bd.Length, fileName.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);

		_scriptState.ExecutionPointer = _scriptState.GetScript(50);	// 50 on startup, 51 on exit?

		if (_scriptState.ExecutionPointer < 0)
		{
			_scriptState.ExecutionPointer = 0;
		}

		_scriptEngine->Resume(&_scriptState);
	}

	_askAboutBase = 0;
}

void CPDVidPhoneModule::RenderEnterPhoneNumber()
{
	Fill(135, 143, 284, 170, 0);
	DrawRectangle(135, 143, 284, 170, 13);

	// Inside green rectangle (150x28) from 136x172
	// Colours 8 & 9, UAKM font
	std::unordered_map<int, int> colourMap;
	colourMap[2] = 9;
	colourMap[3] = 8;
	_uakmRawFont.Render(_screen, 640, 460, 144, 144, "Enter phone number", colourMap, -1);
	_uakmRawFont.Render(_screen, 640, 460, 174, 157, "now please", colourMap, -1);
}

void CPDVidPhoneModule::PlayNextTone()
{
	if (_diallingIndex >= 0)
	{
		LPBYTE pNumbers = _files[VIDPHONE_PHONE_NUMBERS];
		if (pNumbers != NULL)
		{
			while (pNumbers[_diallingIndex] != '.')
			{
				char n = pNumbers[_diallingIndex++];
				if (n >= '0' && n <= '9')
				{
					LPBYTE wave = _files[VIDPHONE_SOUNDS + n - '0'];
					if (wave != NULL)
					{
						if (_sourceVoice == NULL)
						{
							char formatBuff[64];
							WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
							pwfx->wFormatTag = GetInt(wave, 0x14, 2);
							pwfx->nChannels = GetInt(wave, 0x16, 2);
							pwfx->nSamplesPerSec = GetInt(wave, 0x18, 4);
							pwfx->nAvgBytesPerSec = GetInt(wave, 0x1c, 4);
							pwfx->nBlockAlign = 2;
							pwfx->wBitsPerSample = GetInt(wave, 0x22, 2);
							pwfx->cbSize = 0;
							_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
						}

						if (_sourceVoice != NULL)
						{
							_sourceVoice->Start(0, 0);

							XAUDIO2_BUFFER buf = { 0 };
							buf.AudioBytes = GetInt(wave, 0x28, 4);
							buf.pAudioData = wave + 0x28;
							_sourceVoice->SubmitSourceBuffer(&buf);

							_readyForNextTone = FALSE;
						}
					}
					break;
				}
			}

			if (pNumbers[_diallingIndex] == '.')
			{
				_diallingIndex = -1;
			}
		}
	}
}
