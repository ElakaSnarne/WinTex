#include "InventoryModule.h"
#include "Globals.h"
#include "GameController.h"
#include "Items.h"
#include "UAKMNewsPaperModule.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "UAKMGame.h"
#include "UAKMEncodedMessageModule.h"
#include "UAKMTornNoteModule.h"
#include "LocationModule.h"

#define INVENTORY_WIDTH	108.0f
#define INVENTORY_HEIGHT 80.0f

#define EXAMINE_ITEM_ID				0
#define EXAMINE_ADD_ITEM_ID			1
#define EXAMINE_PARAMETER_A_INDEX	2
#define EXAMINE_PARAMETER_A_VALUE	3
#define EXAMINE_ASK_ABOUT1			4
#define EXAMINE_ASK_ABOUT2			5
#define EXAMINE_TRAVEL1				6
#define EXAMINE_TRAVEL2				7
#define EXAMINE_FILE				8
#define EXAMINE_ENTRY				9
#define EXAMINE_DESCRIPTION			10
#define EXAMINE_FLAGS				14
#define EXAMINE_RATE				15
#define EXAMINE_HINT_STATE			17

#define EXAMINE_FLAG_VIDEO			1
#define EXAMINE_FLAG_IMAGE			2
#define EXAMINE_FLAG_TEXT			4
#define EXAMINE_FLAG_SPECIAL		8

#define ITEM_NOTE_SCRAPS			33
#define ITEM_SHREDDED_NOTE			57
#define ITEM_NEWSPAPER				94
#define ITEM_ENCODED_NOTE			95
#define ITEM_NOTE_SCRAPS_2			139

int CInventoryModule::_selectedItemId = -1;
int CInventoryModule::_draggingItemId = -1;
int CInventoryModule::_mouseOverItemId = -1;
ULONGLONG CInventoryModule::_lastItemClick = 0;

CDXButton* CInventoryModule::_pBtnExamine = NULL;
CDXButton* CInventoryModule::_pBtnUse = NULL;
CDXButton* CInventoryModule::_pBtnResume = NULL;

CDXText CInventoryModule::_text;

CDXImageButton* CInventoryModule::_pBtnUp = NULL;
CDXImageButton* CInventoryModule::_pBtnDown = NULL;

ID3D11Buffer* CInventoryModule::_selectionRectangle = NULL;

LPBYTE CInventoryModule::_examData = NULL;
int CInventoryModule::_examStructSize = 0;
CAnimBase* CInventoryModule::_anim = NULL;

WCHAR CInventoryModule::_examFileName[11];

int CInventoryModule::ExamineItemOnResume = -1;

int CInventoryModule::_lineAdjustment = 0;
int CInventoryModule::_lineCount = 0;
int CInventoryModule::_visibleLineCount = 0;

D3D11_RECT CInventoryModule::_limitedRect;

CInventoryModule::CInventoryModule() : CModuleBase(ModuleType::Inventory)
{
	_selectedItemId = -1;
	_draggingItemId = -1;
	_mouseOverItemId = -1;
	_dragging = FALSE;
	_mouseDownPoint.x = -1;
	_mouseDownPoint.y = -1;
	ExamineItemOnResume = -1;

	_text.SetColours(0xff000000, 0xff00c300, 0xff24ff00, 0xff000000);
}

CInventoryModule::~CInventoryModule()
{
	if (_anim != NULL)
	{
		delete _anim;
		_anim = NULL;
	}

	if (_pBtnExamine != NULL)
	{
		delete _pBtnExamine;
		_pBtnExamine = NULL;
	}

	if (_pBtnUse != NULL)
	{
		delete _pBtnUse;
		_pBtnUse = NULL;
	}

	if (_pBtnResume != NULL)
	{
		delete _pBtnResume;
		_pBtnResume = NULL;
	}

	if (_pBtnUp != NULL)
	{
		delete _pBtnUp;
		_pBtnUp = NULL;
	}

	if (_pBtnDown != NULL)
	{
		delete _pBtnDown;
		_pBtnDown = NULL;
	}

	CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Crosshair].SetPosition(dx.GetWidth() / 2, dx.GetHeight() / 2);
}

void CInventoryModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2;
	_cursorPosY = dx.GetHeight() / 2;

	if (_pBtnExamine == NULL)
	{
		// Create UI
		char* pExam = "Examine";
		char* pRes = "Resume";
		char* pUse = "Use";
		float maxw = max(max(TexFont.PixelWidth(pExam), TexFont.PixelWidth(pRes)), TexFont.PixelWidth(pUse));

		_pBtnExamine = new CDXButton(pExam, maxw, 32.0f * pConfig->FontScale, OnExamine);
		_pBtnUse = new CDXButton(pUse, maxw, 32.0f * pConfig->FontScale, OnUse);
		_pBtnResume = new CDXButton(pRes, maxw, 32.0f * pConfig->FontScale, OnResume);

		_pBtnExamine->SetPosition(0, dx.GetHeight() - 40 * pConfig->FontScale);
		_pBtnUse->SetPosition((dx.GetWidth() - _pBtnUse->GetWidth()) / 2, dx.GetHeight() - 40 * pConfig->FontScale);
		_pBtnResume->SetPosition(dx.GetWidth() - _pBtnResume->GetWidth(), dx.GetHeight() - 40 * pConfig->FontScale);

		ZeroMemory(&_fullRect, sizeof(D3D11_RECT));
		_fullRect.top = 0;
		_fullRect.left = 0;
		_fullRect.bottom = dx.GetHeight();
		_fullRect.right = dx.GetWidth();

		ZeroMemory(&_limitedRect, sizeof(D3D11_RECT));
		_limitedRect.top = 3.0f * dx.GetHeight() / 4.0f;
		_limitedRect.left = 0;
		_limitedRect.bottom = dx.GetHeight() - 10.0f;
		_limitedRect.right = dx.GetWidth() - 40 - _pBtnResume->GetWidth();

		_visibleLineCount = (_limitedRect.bottom - _limitedRect.top) / (TexFont.Height() * pConfig->FontScale);

		// Text up/down buttons
		_pBtnUp = new CDXImageButton(2, ScrollUp);
		_pBtnUp->SetPosition(_limitedRect.right, _limitedRect.top);
		_pBtnDown = new CDXImageButton(3, ScrollDown);
		_pBtnDown->SetPosition(_limitedRect.right, _limitedRect.bottom - _pBtnDown->GetHeight() - 10.0f);

		// Create selection rectangle vertex buffer
		COLOURED_VERTEX_ORTHO* pVB = new COLOURED_VERTEX_ORTHO[5];
		if (pVB != NULL)
		{
			float x1 = 1.0f;
			float x2 = x1 + INVENTORY_WIDTH - 2.0f;
			float y1 = 0.5f;
			float y2 = y1 - INVENTORY_HEIGHT - 2.0f;

			pVB[0].position = XMFLOAT4(x1, y1, 0.0f, 0.0f);
			pVB[0].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pVB[1].position = XMFLOAT4(x2, y1, 0.0f, 0.0f);
			pVB[1].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pVB[2].position = XMFLOAT4(x2, y2 - 0.3f, 0.0f, 0.0f);	// DX bug?
			pVB[2].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pVB[3].position = XMFLOAT4(x1, y2, 0.0f, 0.0f);
			pVB[3].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pVB[4].position = XMFLOAT4(x1, y1, 0.0f, 0.0f);
			pVB[4].colour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;
			vbDesc.ByteWidth = sizeof(COLOURED_VERTEX_ORTHO) * 5;
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = pVB;
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vbDesc, &vData, &_selectionRectangle, "InventorySelection");

			delete[] pVB;
		}

		// Load inventory data file
		_examData = CLZ::Decompress(L"EXAM.LZ").Data;
		_examStructSize = *(int*)_examData;

		// TODO: PD examine structure size is 0x17 bytes
		// PD offset = ?

		CopyMemory(_examFileName, L"EXAM000.AP", 22);
	}

	if (_selectedItemId >= 0)
	{
		// Trigger examine
		_pBtnExamine->Click();
	}
}

void CInventoryModule::Dispose()
{
}

void CInventoryModule::Render()
{
	if (ExamineItemOnResume >= 0)
	{
		_selectedItemId = ExamineItemOnResume;
		ExamineItemOnResume = -1;
		_pBtnExamine->Click();
		return;
	}

	dx.Clear(0.0f, 0.0f, 0.0f);

	dx.DisableZBuffer();
	CConstantBuffers::Setup2D(dx);

	if (_anim == NULL)
	{
		// Render images from inventory
		int itemCount = CGameController::GetItemCount();

		// Find maximum width of all inventory names, use as right side margin
		int w = dx.GetWidth();
		int h = dx.GetHeight();

		float inventoryWidth = INVENTORY_WIDTH;
		float inventoryHeight = INVENTORY_HEIGHT;
		int numberOfItemsPerRow = w / inventoryWidth;
		float margin = w - numberOfItemsPerRow * inventoryWidth;
		float ox = margin / 2;
		float oy = -inventoryHeight + 10.0f;

		int currentSelection = _mouseOverItemId;
		_mouseOverItemId = -1;

		for (int i = 0; i < itemCount; i++)
		{
			if ((i % numberOfItemsPerRow) == 0)
			{
				ox = margin / 2;
				oy += inventoryHeight;
			}

			int id = CGameController::GetItemId(i);
			BOOL mouseOver = (_cursorPosX >= (ox - 5) && _cursorPosX < (ox + inventoryWidth - 5) && _cursorPosY >= (oy - 5) && _cursorPosY < (oy + inventoryHeight - 3));	// Offsetting by 5, box is 10 pixels wider and taller than image
			if (mouseOver)
			{
				_mouseOverItemId = id;
				if (currentSelection != id)
				{
					_lastItemClick = 0;
				}
			}

			// Do not render beyond what is visible
			if ((oy + inventoryHeight) <= h)
			{
				if (_selectedItemId != id || !_dragging || (_mouseDownPoint.x == _cursorPosX && _mouseDownPoint.y == _cursorPosY))
				{
					CItems::RenderItemImage(id, ox, oy, mouseOver);
				}

				if (_selectedItemId == id && !_dragging)
				{
					// Render selection rectangle
					UINT stride = sizeof(COLOURED_VERTEX_ORTHO);
					UINT offset = 0;
					dx.SetVertexBuffers(0, 1, &_selectionRectangle, &stride, &offset);
					CShaders::SelectColourShader();
					dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

					XMMATRIX wm = XMMatrixTranslation(ox - 5, 5 - oy, 0.0f);
					CConstantBuffers::SetWorld(dx, &wm);
					dx.Draw(5, 0);
				}
			}

			ox += inventoryWidth;
		}

		// Need 3 buttons, Examine, Use, Close (plus possibly scroll up/down)
		_pBtnExamine->Render();
		_pBtnUse->Render();

		if (_mouseOverItemId >= 0 && !_dragging)
		{
			int namePixels = CItems::GetItemNameWidth(_mouseOverItemId);
			float namex = min(max(0.0f, _cursorPosX - namePixels / 2), w - namePixels);
			CItems::RenderItemName(_mouseOverItemId, namex - 10.0f, _cursorPosY + 16.0f, TRUE);
		}

		// TODO: Need optionally visible buttons to scroll inventory list
		// TODO: Leave margin at bottom for buttons
	}
	else
	{
		// Render animation
		CAnimationController::UpdateAndRender(_anim);
	}

	if (_anim != NULL)
	{
		// Limit view, make scrollable if text too large
		dx.SetScissorRect(_limitedRect);
		// Render text at bottom, top line may need to be modified
		_text.Render(10.0f, dx.GetHeight() - _text.Height() - 10.0f + _lineAdjustment * TexFont.Height() * pConfig->FontScale);
		dx.SetScissorRect(_fullRect);

		if (_lineCount > _visibleLineCount)
		{
			_pBtnUp->Render();
			_pBtnDown->Render();
		}
	}

	_pBtnResume->Render();

	if (_dragging && (_mouseDownPoint.x != _cursorPosX || _mouseDownPoint.y != _cursorPosY))
	{
		// Render item at cursor
		CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Crosshair].SetPosition(_cursorPosX, _cursorPosY);
		CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Crosshair].Render();
		CItems::RenderItemImage(_selectedItemId, _cursorPosX, _cursorPosY, FALSE);
	}
	else
	{
		CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Arrow].SetPosition(_cursorPosX, _cursorPosY);
		CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Arrow].Render();
	}

	dx.EnableZBuffer();

	dx.Present(1, 0);
}

BOOL CInventoryModule::CheckButton(CDXButton* btn, float x, float y)
{
	if (btn->HitTest(x, y))
	{
		btn->Click();
		return TRUE;
	}

	return FALSE;
}

short comboTable[] = { 0,5,26,1,4,0,1,5,129,4,129,26,7,37,39,9,12,2,10,77,78,14,15,49,17,35,138,18,112,114,27,28,52,42,43,85,45,46,88,45,48,87,46,87,107,48,88,107,58,59,90,60,90,93,61,70,102,65,66,108,66,73,110,66,96,132,73,131,96,79,80,76,91,103,95,110,131,132,111,114,124 };

short comboScoreTable[] = { 8,85,14,76,6,108,10,93 };

short comboHintStateScoreTable[] = { 0,11,2,75,26,12,39,108,49,135,52,145,76,234,78,227,85,263,88,342,90,373,93,374,95,189,102,418,107,343,108,469,110,453,114,379,124,380,132,454,138,209,255,65535 };

void CInventoryModule::OnExamine(LPVOID data)
{
	if (_selectedItemId >= 0)
	{
		_anim = NULL;

		ExminationData* pExam = (ExminationData*)(_examData + 4 + _selectedItemId * _examStructSize);

		_examFileName[5] = (WCHAR)('0' + (pExam->File >> 4));

		char* pDesc = (char*)(pExam->DescriptionOffset + _examData);
		pAddCaptions->clear();

		std::list<CCaption*>* pOld = pDisplayCaptions;
		pDisplayCaptions = pAddCaptions;
		pAddCaptions = pOld;
		ClearCaptions(pOld);
		Rect rect{ 0.0f, 0.0f, 1000.0f, _limitedRect.right - _limitedRect.left };
		_text.SetText(pDesc, rect);
		_lineCount = _text.Lines();
		_lineAdjustment = max(0, min(_lineCount - _visibleLineCount, _lineCount));
		UpdateButtons();

		if ((pExam->Flags & EXAMINE_FLAG_VIDEO) != 0)
		{
			BinaryData bd = LoadEntry(_examFileName, pExam->Entry);
			_anim = CAnimationController::Load(bd, 2);
		}
		else if ((pExam->Flags & EXAMINE_FLAG_IMAGE) != 0)
		{
			DoubleData dd = LoadDoubleEntry(_examFileName, pExam->Entry);
			_anim = CAnimationController::LoadImage(dd, 320, 240, 2);
		}
		else if (pExam->Flags == 0)
		{
			// Special examine module required (newspaper, shredded/torn notes etc)
			if (_selectedItemId == ITEM_NOTE_SCRAPS || _selectedItemId == ITEM_NOTE_SCRAPS_2 || _selectedItemId == ITEM_SHREDDED_NOTE)
			{
				CModuleController::Push(new CUAKMTornNoteModule(_selectedItemId));
			}
			else if (_selectedItemId == ITEM_NEWSPAPER)
			{
				// Show newspaper
				CModuleController::Push(new CUAKMNewsPaperModule());
			}
			else if (_selectedItemId == ITEM_ENCODED_NOTE)
			{
				CModuleController::Push(new CUAKMEncodedMessageModule());
			}
		}

		int extraScore = 0;
		if (_selectedItemId == 55 || _selectedItemId == 76 || _selectedItemId == 94 || _selectedItemId == 106 || _selectedItemId == 134)
		{
			extraScore = 4;
		}
		else if (_selectedItemId == 85)
		{
			extraScore = 14;
		}

		CGameController::SetItemExamined(_selectedItemId, extraScore);
	}
}

void CInventoryModule::OnUse(LPVOID data)
{
	// Set current item, resume
	CGameController::SetCurrentItemId(_selectedItemId);
	// Set current action to be "Use"
	CLocationModule::CurrentAction = ACTION_USE;
	OnResume(NULL);
}

void CInventoryModule::OnResume(LPVOID data)
{
	if (_anim == NULL)
	{
		CModuleController::Pop(CModuleController::CurrentModule);
	}
	else
	{
		delete _anim;
		_anim = NULL;

		ExminationData* pExam = (ExminationData*)(_examData + 4 + _selectedItemId * _examStructSize);
		if (pExam->ItemId != _selectedItemId)
		{
			// Transform item (remove old, add new)
			CGameController::SetItemState(_selectedItemId, 2);
			CGameController::SetItemState(pExam->ItemId, 1);
		}
		if (pExam->AddItemId != 0xff)
		{
			CGameController::SetItemState(pExam->AddItemId, 1);
		}
		if (pExam->AskAbout1 != 0xff)
		{
			CGameController::SetAskAboutState(pExam->AskAbout1, 1);
		}
		if (pExam->AskAbout2 != 0xff)
		{
			CGameController::SetAskAboutState(pExam->AskAbout2, 1);
		}
		if (pExam->ParameterAIndex != 0xff)
		{
			CGameController::SetParameter(pExam->ParameterAIndex, pExam->ParameterAValue);
		}
		if (pExam->Travel1 != 0xff)
		{
			CGameController::SetData(UAKM_SAVE_TRAVEL + pExam->Travel1, 1);
		}
		if (pExam->Travel2 != 0xff)
		{
			CGameController::SetData(UAKM_SAVE_TRAVEL + pExam->Travel2, 1);
		}
		if (pExam->HintState != 0xffff)
		{
			CGameController::SetHintState(pExam->HintState, 1, 1);
		}
	}
}

// TODO: When combining, should select the new item

void CInventoryModule::Resize(int width, int height)
{
}

void CInventoryModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnExamine->SetMouseOver(_pBtnExamine->HitTest(x, y) != NULL);
	_pBtnUse->SetMouseOver(_pBtnUse->HitTest(x, y) != NULL);
	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);
	_pBtnUp->SetMouseOver(_pBtnUp->HitTest(x, y) != NULL);
	_pBtnDown->SetMouseOver(_pBtnDown->HitTest(x, y) != NULL);
}

void CInventoryModule::BeginAction()
{
	float x = _cursorPosX;
	float y = _cursorPosY;

	if (_anim == NULL)
	{
		if (CheckButton(_pBtnExamine, x, y) || CheckButton(_pBtnUse, x, y) || CheckButton(_pBtnResume, x, y));
		else
		{
			// Check if mouse is over an item
			if (_mouseOverItemId >= 0)
			{
				_selectedItemId = _mouseOverItemId;
				_mouseDownPoint.x = _cursorPosX;
				_mouseDownPoint.y = _cursorPosY;
				_dragging = TRUE;

				ULONGLONG now = GetTickCount64();
				if ((now - _lastItemClick) < 500)
				{
					_dragging = FALSE;
					_pBtnExamine->Click();
				}

				_lastItemClick = now;
			}
		}
	}
	else
	{
		if (CheckButton(_pBtnResume, x, y) || CheckButton(_pBtnUp, x, y) || CheckButton(_pBtnDown, x, y));
	}
}

void CInventoryModule::EndAction()
{
	if (_dragging)
	{
		if (_selectedItemId != _mouseOverItemId)
		{
			// Check for combination
			for (int c = 0; c < 27; c++)
			{
				if ((comboTable[c * 3 + 0] == _selectedItemId && comboTable[c * 3 + 1] == _mouseOverItemId) || (comboTable[c * 3 + 0] == _mouseOverItemId && comboTable[c * 3 + 1] == _selectedItemId))
				{
					// Remove 2 items, add new
					// TODO: Should actually replace item 2 (mouse over)
					int newItemId = comboTable[c * 3 + 2];
					CGameController::SetItemState(_selectedItemId, 2);
					CGameController::SetItemState(_mouseOverItemId, 2);
					CGameController::SetItemState(newItemId, 1);

					for (int s = 0; s < 4; s++)
					{
						if (comboScoreTable[s * 2 + 1] == newItemId)
						{
							CGameController::AddScore(comboScoreTable[s * 2 + 0]);
							break;
						}
					}

					int i = 0;
					while (comboHintStateScoreTable[i * 2] != 0xff)
					{
						if (comboHintStateScoreTable[i * 2] == newItemId)
						{
							CGameController::SetHintState(comboHintStateScoreTable[i * 2 + 1], 2, 1);
							if (newItemId == 95)
							{
								CGameController::SetHintState(398, 2, 0);
							}
							break;
						}
						i++;
					}

					_selectedItemId = newItemId;

					break;
				}
			}
		}

		_dragging = FALSE;
	}
}

void CInventoryModule::Back()
{
	OnResume(NULL);
}

void CInventoryModule::ScrollUp(LPVOID data)
{
	_lineAdjustment = max(0, min(_lineCount - _visibleLineCount, _lineAdjustment + 1));
	UpdateButtons();
}

void CInventoryModule::ScrollDown(LPVOID data)
{
	_lineAdjustment = max(0, _lineAdjustment - 1);
	UpdateButtons();
}

void CInventoryModule::UpdateButtons()
{
	BOOL tooManyLines = ((_visibleLineCount - _lineCount) < 0);
	_pBtnUp->SetEnabled(tooManyLines && _lineAdjustment != (_lineCount - _visibleLineCount));
	if (!_pBtnUp->GetEnabled())
	{
		_pBtnUp->SetMouseOver(FALSE);
	}

	_pBtnDown->SetEnabled(tooManyLines && _lineAdjustment > 0);
	if (!_pBtnDown->GetEnabled())
	{
		_pBtnDown->SetMouseOver(FALSE);
	}
}

void CInventoryModule::Next()
{
	ScrollDown(NULL);
}

void CInventoryModule::Prev()
{
	ScrollUp(NULL);
}
