#include "UAKMTornNoteModule.h"
#include "Globals.h"
#include "GameController.h"
#include "Utilities.h"
#include "UAKMGame.h"
#include "InventoryModule.h"

int TornNoteDistances[] = { -22, 0, -57, 0, -44, 0, 123, -10, -117, 4, 117, -46, -46, 12, -47, -8, -48, -8, 39, -17, 102, -13, 0, -14, -67, 2, -37, 6, 93, -47, -94, 26, 105, -28, -36, -20, -71, -6, -29, 12, 136, -42, -52, 2, -93, 5 };

CUAKMTornNoteModule* CUAKMTornNoteModule::pUAKMTNM = NULL;

CUAKMTornNoteModule::CUAKMTornNoteModule(int item) : CModuleBase(ModuleType::TornNote)
{
	pUAKMTNM = this;

	_cursorMinX = 0;
	_cursorMaxX = dx.GetWidth() - 1;
	_cursorMinY = 0;
	_screenHeight = static_cast<float>(dx.GetHeight());
	_cursorMaxY = static_cast<int>(_screenHeight - 1.0f);

	_item = item;
	_newItem = -1;

	_numberOfImages = 0;
	_pImageData = NULL;

	_positionOffset = 0;
	_vertexBuffer = NULL;

	float width = 440.0f, height = 280.0f;
	float screenWidth = (float)dx.GetWidth();
	float sx = screenWidth / width;
	float sy = _screenHeight / height;
	_scale = min(sx, sy);

	_selectedScrap = NULL;
	_completed = FALSE;

	_timeToExit = 0;
}

CUAKMTornNoteModule::~CUAKMTornNoteModule()
{
	Dispose();
}

void CUAKMTornNoteModule::Dispose()
{
	if (_pImageData != NULL)
	{
		delete _pImageData;
		_pImageData = NULL;
	}

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	// Clear unordered maps
	for (auto it : _images)
	{
		delete it.second;
	}

	_images.clear();
}

float rotationAngles[] = { 0.0f, -XM_PI / 2.0f, -XM_PI, XM_PI / 2.0f };

void CUAKMTornNoteModule::Render()
{
	if (_vertexBuffer != NULL)
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		dx.DisableZBuffer();

		CConstantBuffers::Setup2D(dx);

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();

		for (int i = 0; i < _numberOfImages; i++)
		{
			CNoteScrap* pN = _zImages[i];
			float w = pN->Width;
			float h = pN->Height;
			// Scale, rotate, translate by correct dimension depending on rotation, translate x & y
			float itx = (pN->Orientation == 0 || pN->Orientation == 2) ? w / 2.0f : h / 2.0f;
			float ity = (pN->Orientation == 0 || pN->Orientation == 2) ? h / 2.0f : w / 2.0f;
			float x = itx + pN->X;
			float y = ity + pN->Y;

			XMMATRIX wm = XMMatrixScaling(w, h, 1.0f) * XMMatrixRotationZ(rotationAngles[pN->Orientation]) * XMMatrixTranslation(x, -y, 0.0f);

			CConstantBuffers::SetWorld(dx, &wm);
			ID3D11ShaderResourceView* pRV = pN->Texture.GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(4, 0);
		}

		if (_completed)
		{
			_caption.Render(0.0f, dx.GetHeight() - 10.0f - _caption.Height());
		}

		_pBtnResume->Render();

		CModuleController::Cursors[0].SetPosition((float)_cursorPosX, (float)_cursorPosY);
		CModuleController::Cursors[0].Render();

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}

	if (_completed &&  GetTickCount64() >= _timeToExit)
	{
		CInventoryModule::ExamineItemOnResume = _newItem;
		CModuleController::Pop(this);
	}
}

void CUAKMTornNoteModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	int palIx = -1;
	if (_item == 33)
	{
		palIx = 21;
		_numberOfImages = 28;
		_positionOffset = UAKM_SAVE_PUZZLE_DATA;
		CGameController::SetHintState(188, 2, 0);
	}
	else if (_item == 57)
	{
		palIx = 24;
		_numberOfImages = 31;
		_positionOffset = UAKM_SAVE_PUZZLE_DATA + 168;
		CGameController::SetHintState(361, 2, 0);
	}
	else if (_item == 139)
	{
		palIx = 26;
		_numberOfImages = 26;
		_positionOffset = UAKM_SAVE_PUZZLE_DATA + 354;
		CGameController::SetHintState(247, 2, 0);
	}

	_caption.SetColours(0, 0xff00c300, 0xff24ff00, 0);

	_caption.SetText(L"That completes it!  Now that it's assembled, I think I'll glue the pieces together so that they won't move.", CDXText::Alignment::Justify);

	if (palIx >= 0)
	{
		// Load palette
		BinaryData bdPal = LoadEntry(L"GRAPHICS.AP", palIx);
		if (bdPal.Data != NULL)
		{
			for (int c = 0; c < 256; c++)
			{
				double r = bdPal.Data[c * 3 + 0];
				double g = bdPal.Data[c * 3 + 1];
				double b = bdPal.Data[c * 3 + 2];
				int ri = (byte)((r * 255.0) / 63.0);
				int gi = (byte)((g * 255.0) / 63.0);
				int bi = (byte)((b * 255.0) / 63.0);
				int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
				_palette[c] = col;
			}

			delete[] bdPal.Data;
		}

		// Extract images
		BinaryData bdImages = LoadEntry(L"GRAPHICS.AP", palIx + 1);
		if (bdImages.Data != NULL)
		{
			_pImageData = bdImages.Data;
			int count = GetInt(_pImageData, 0, 2) - 1;
			if (_item == 139)
			{
				// No need to load the placeholders
				count = 26;
			}

			for (int i = 0; i < count; i++)
			{
				LPBYTE pImage = _pImageData + GetInt(_pImageData, 2 + i * 4, 4);
				int width = GetInt(pImage, 0, 2);
				int height = GetInt(pImage, 2, 2);
				CNoteScrap* id = new CNoteScrap();
				id->RawImage = pImage;
				id->OriginalWidth = width;
				id->OriginalHeight = height;
				id->Width = width * _scale;
				id->Height = height * _scale;
				id->Texture.Init(width, height);

				D3D11_MAPPED_SUBRESOURCE subRes;
				ID3D11Texture2D* pTexture = id->Texture.GetTexture();
				if (SUCCEEDED(dx.Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
				{
					int inPtr = 4;
					int* pTex = (int*)subRes.pData;
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int pix = pImage[inPtr++];
							pTex[y * subRes.RowPitch / 4 + x] = (pix != 0) ? _palette[pix] : 0;
						}
					}

					dx.Unmap(pTexture, 0);
				}

				if (i < _numberOfImages)
				{
					// Get position, orientation and z-order from save data
					id->Offset = _positionOffset + i * 6;

					id->X = (CGameController::GetData(id->Offset + 0) + CGameController::GetData(id->Offset + 1) * 256) * _scale;
					id->Y = (CGameController::GetData(id->Offset + 2) + CGameController::GetData(id->Offset + 3) * 256) * _scale;
					id->Z = CGameController::GetData(id->Offset + 4);
					//id->Z = i;
					id->Orientation = CGameController::GetData(id->Offset + 5);
				}

				_images[i] = id;
			}

			for (int z = 0; z < count; z++)
			{
				for (int i = 0; i < count; i++)
				{
					if (_images[i]->Z == z)
					{
						_zImages.push_back(_images[i]);
					}
				}
			}
		}
	}

	// Create vertex buffer
	CreateTexturedRectangle(0.5f, -0.5f, -0.5f, 0.5f, &_vertexBuffer, "NoteVertexBuffer");

	// Resume-button
	char* pResume = "Resume";
	_pBtnResume = new CDXButton(pResume, TexFont.PixelWidth(pResume), 32.0f * pConfig->FontScale, OnResume);
	_pBtnResume->SetPosition(dx.GetWidth() - _pBtnResume->GetWidth(), dx.GetHeight() - 40 * pConfig->FontScale);

	// Torn note
	// Palette in GRAPHICS.AP 20 or 21
	// Images in GRAPHICS.AP 22
	// 24 images + 4 images for corners

	// 2nd torn note
	// Palette in GRAPHICS.AP 26
	// Images in GRAPHICS.AP 27
	// 26 images + 4 images for corners

	// User has to click on the actual non-transparent pixel to select the tile, so the raw image must be kept in memory for hit testing
}

// Length of first torn note data  168
// Length of shredded letter data  186
// Length of second torn note data 156
// 6 bytes per image (2 x, 2 y, 1 z order, 1 orientation)

// TODO: Make sure pieces are visible

CUAKMTornNoteModule::CNoteScrap* CUAKMTornNoteModule::HitTest(float mx, float my)
{
	int offset = _item == 33 ? 4 : 0;
	std::vector<CNoteScrap*>::iterator it = _zImages.end();
	std::vector<CNoteScrap*>::iterator start = _zImages.begin() + offset;

	do
	{
		it--;
		CNoteScrap* pN = *it;

		float w = pN->Width;
		float h = pN->Height;
		// Scale, rotate, translate by correct dimension depending on rotation, translate x & y
		float rw = (pN->Orientation == 0 || pN->Orientation == 2) ? w : h;
		float rh = (pN->Orientation == 0 || pN->Orientation == 2) ? h : w;
		float x = pN->X;
		float y = pN->Y;

		// Check if mouse is roughly inside rectangle
		if (mx >= x && mx < (x + rw) && my >= y && my < (y + rh))
		{
			// Translate mouse coordinates, scale and orientation relative to object

			int cx = static_cast<int>((mx - x) / _scale);
			int cy = static_cast<int>((my - y) / _scale);
			if (pN->Orientation == 2)
			{
				// Upside down
				cx = pN->OriginalWidth - cx - 1;
				cy = pN->OriginalHeight - cy - 1;
			}
			else if (pN->Orientation == 1)
			{
				// Rotated right
				int tmp = cx;
				cx = cy;
				cy = pN->OriginalHeight - tmp - 1;
			}
			else if (pN->Orientation == 3)
			{
				// Rotated left
				int tmp = cy;
				cy = cx;
				cx = pN->OriginalWidth - tmp - 1;
			}

			// Now check pixel
			LPBYTE pPixels = pN->RawImage + 4;
			if (pPixels[cy*pN->OriginalWidth + cx] != 0)
			{
				// All items from this point should be pushed down in z-order
				std::vector<CNoteScrap*>::iterator fix = it + 1;
				while (fix != _zImages.end())
				{
					CNoteScrap* pS = *fix++;
					pS->Z--;
					CGameController::SetData(pS->Offset + 4, pS->Z);
				}

				// Remove from queue and place at end
				_zImages.erase(it);
				_zImages.push_back(pN);

				pN->Z = _numberOfImages - 1;
				CGameController::SetData(pN->Offset + 4, pN->Z);

				return pN;
			}
		}

	} while (it != start);

	return NULL;
}

BOOL CUAKMTornNoteModule::CheckCompleted()
{
	if (_item == 33)
	{
		for (int i = 0; i < (_numberOfImages - 5); i++)
		{
			CNoteScrap* pScrap1 = _images[i];
			if (pScrap1->Orientation != 0)
			{
				// Not oriented correctly
				return FALSE;
			}

			CNoteScrap* pScrap2 = _images[i + 1];

			int dx = static_cast<int>(abs((pScrap1->X - pScrap2->X) / _scale - TornNoteDistances[i * 2]));
			if (dx > 5)
			{
				return FALSE;
			}

			int dy = static_cast<int>(abs((pScrap1->Y - pScrap2->Y) / _scale - TornNoteDistances[i * 2 + 1]));
			if (dy > 5)
			{
				return FALSE;
			}
		}
	}
	else if (_item == 57)
	{
		for (int i = 0; i < (_numberOfImages - 1); i++)
		{
			CNoteScrap* pScrap1 = _images[i];
			CNoteScrap* pScrap2 = _images[i + 1];
			int dx = static_cast<int>((pScrap2->X - pScrap1->X) / _scale);
			if (dx < 8 || dx > 12)
			{
				return FALSE;
			}
		}
	}
	else if (_item == 139)
	{
		// This note can never be completed
		return FALSE;
	}

	return TRUE;
}

void CUAKMTornNoteModule::Resize(int width, int height)
{
}

void CUAKMTornNoteModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);

	if (!_completed && _selectedScrap != NULL)
	{
		// Move selected piece
		_selectedScrap->X -= _pt.x - _cursorPosX;
		_selectedScrap->Y -= _pt.y - _cursorPosY;
		_pt.x = static_cast<LONG>(_cursorPosX);
		_pt.y = static_cast<LONG>(_cursorPosY);
	}
}

void CUAKMTornNoteModule::BeginAction()
{
	if (_pBtnResume->HitTest(_cursorPosX, _cursorPosY))
	{
		_pBtnResume->Click();
	}
	else
	{
		if (!_completed)
		{
			CNoteScrap* pScrap = HitTest(_cursorPosX, _cursorPosY);
			if (pScrap != NULL)
			{
				_selectedScrap = pScrap;
				_pt.x = static_cast<LONG>(_cursorPosX);
				_pt.y = static_cast<LONG>(_cursorPosY);
			}
		}
	}
}

void CUAKMTornNoteModule::OnResume(LPVOID data)
{
	pUAKMTNM->Back();
}

void CUAKMTornNoteModule::EndAction()
{
	if (_selectedScrap != NULL && !_completed)
	{
		// Update save area
		int x = static_cast<int>(_selectedScrap->X / _scale);
		int y = static_cast<int>(_selectedScrap->Y / _scale);
		CGameController::SetData(_selectedScrap->Offset, x & 0xff);
		CGameController::SetData(_selectedScrap->Offset + 1, (x >> 8) & 0xff);
		CGameController::SetData(_selectedScrap->Offset + 2, y & 0xff);
		CGameController::SetData(_selectedScrap->Offset + 3, (y >> 8) & 0xff);
		CGameController::SetData(_selectedScrap->Offset + 4, _selectedScrap->Z);
		CGameController::SetData(_selectedScrap->Offset + 5, _selectedScrap->Orientation);

		_selectedScrap = NULL;

		// Check if note is now assembled
		if (_completed = CheckCompleted())
		{
			_timeToExit = GetTickCount64() + (DWORD)(300 * TIMER_SCALE);

			if (_item == 33)
			{
				// Sal's note scraps
				CGameController::SetData(UAKM_SAVE_HINT_STATES + 27, 2);
				CGameController::SetHintState(177, 2, 1);
			}
			else if (_item == 57)
			{
				CGameController::AddScore(12);

				// If day 5 and shredded note, enable travel to bastion of sanctity
				if (CGameController::GetParameter(250) == 5)
				{
					CGameController::SetData(UAKM_SAVE_TRAVEL + 14, 1);
				}

				CGameController::SetHintState(362, 2, 1);
			}

			// Transform item
			_newItem = (_item == 33) ? 103 : (_item == 57) ? 109 : -1;
			if (_newItem >= 0)
			{
				int itemCount = CGameController::GetItemCount();
				for (int i = 0; i < itemCount; i++)
				{
					if (CGameController::GetItemId(i) == _item)
					{
						CGameController::SetData(UAKM_SAVE_INVENTORY + i, _newItem);
					}
				}

				// Set item states, don't have/used for old item, have for new item
				CGameController::SetItemState(_item, 2);
				CGameController::SetItemState(_newItem, 1);
			}
		}

		// Second torn note can never be completed, no item to turn into, no score set
	}
}

void CUAKMTornNoteModule::Back()
{
	CModuleController::Pop(this);
}

void CUAKMTornNoteModule::Cycle()
{
	if (!_completed)
	{
		if (_selectedScrap != NULL)
		{
			_selectedScrap->Orientation = (_selectedScrap->Orientation + 1) & 3;
		}
		else
		{
			CNoteScrap* pScrap = HitTest(_cursorPosX, _cursorPosY);
			if (pScrap != NULL)
			{
				// Right button, rotate selected piece
				pScrap->Orientation = (pScrap->Orientation + 1) & 3;
			}
		}
	}
}

void CUAKMTornNoteModule::Next()
{
	if (_selectedScrap != NULL && !_completed)
	{
		_selectedScrap->Orientation = (_selectedScrap->Orientation + 1) & 3;
	}
}

void CUAKMTornNoteModule::Prev()
{
	if (_selectedScrap != NULL && !_completed)
	{
		_selectedScrap->Orientation = (_selectedScrap->Orientation - 1) & 3;
	}
}
