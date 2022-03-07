#include "HintModule.h"
#include "GameController.h"
#include "Utilities.h"

CHintModule::CHintModule() : CModuleBase(ModuleType::Hints)
{
	_vertexBuffer = NULL;
	_hintVertexBuffer = NULL;
	_categoryVertexBuffer = NULL;

	_pCurrentHintCategory = NULL;

	_colBlack = 0;
	_colBlue = 0;
	_colCategory = 0;
	_colOrange = 0;
	_colGreen = 0;
	_colScore = 0;
	_colHighlight = 0;
	_colShade = 0;
}

CHintModule::~CHintModule()
{
}

void CHintModule::Resize(int width, int height)
{
}

void CHintModule::BeginAction()
{
	// Check mode
	// If in catalog mode, check if a category has been selected
	// If in category mode, check if an available hint has been selected

	float y = (_hintTexture.Height() + 2) * pConfig->FontScale;
	float hittesth = (_blankTexture.Height() - 2) * pConfig->FontScale;
	float h = max(_blankTexture.Height() + 2, TexFont.Height()) * pConfig->FontScale;
	float boxw = _blankTexture.Width() * pConfig->FontScale;
	float hittestw = (_blankTexture.Width() - 2) * pConfig->FontScale;
	float x = 9.0f * pConfig->FontScale;

	if (_pCurrentHintCategory == NULL)
	{
		// Check if a hint category (questionmark box) was clicked (and select the category)

		for (auto it : _activeHintCategories)
		{
			if (_cursorPosX >= x && _cursorPosX < (x + hittestw) && _cursorPosY >= y && _cursorPosY < (y + hittesth))
			{
				_pCurrentHintCategory = it;

				Rect rect{ 0, 0, 1000, dx.GetWidth() - (x + boxw) * 2 };

				for (auto hit : _pCurrentHintCategory->Hints)
				{
					int state = hit->GetState();
					if (state == -1)
					{
						state = CGameController::GetHintState(hit->GetHintIndex());
						hit->SetState(state);
						hit->Prepare(state, _colBlack, _colOrange, _colGreen, _colBlue, rect);
					}
				}

				return;
			}

			y += h;
		}

		// Check if the cancel/return button was clicked instead
		if (_pBtnResume->GetMouseOver())
		{
			CModuleController::Pop(this);
		}
	}
	else
	{
		// Check if a hint (questionmark box) was clicked (and show hint)
		for (auto it : _pCurrentHintCategory->Hints)
		{
			// Only allow clicking on first hint that has hint state 0
			int state = it->GetState();
			if (state == 0)
			{
				if (_cursorPosX >= (x + boxw) && _cursorPosX < (x + boxw + hittestw) && _cursorPosY >= y && _cursorPosY < (y + hittesth))
				{
					it->SetState(2);
					int hintIndex = it->GetHintIndex();
					CGameController::SetHintState(hintIndex, CGameController::GetHintState(hintIndex) | 2, 0);
					CGameController::AddScore(-4);

					/*
seg000:001A1CFA                 mov     ebx, ds:dword_1F100B[ecx*4]
seg000:001A1D01                 shl     ebx, 1
seg000:001A1D03                 add     ebx, 1
seg000:001A1D06                 bts     ds:HintStates, ebx
seg000:001A1D0D                 sub     ds:Score, 4     ; Lose 4 points when using a hint
					*/
					return;
				}

				break;
			}
			else if (state == 2)
			{
				break;
			}

			y += max(h, it->Height());
		}

		if (_pBtnDirectory->GetMouseOver())
		{
			_pCurrentHintCategory = NULL;
		}
	}
}

void CHintModule::Back()
{
	if (_pCurrentHintCategory == NULL)
	{
		CModuleController::Pop(this);
	}
	else
	{
		_pCurrentHintCategory = NULL;
	}
}

void CHintModule::Initialize()
{
	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_cursorPosX = w / 2;
	_cursorPosY = h / 2;

	int categoryCount = CGameController::GetHintCategoryCount();
	for (int hc = 0; hc < categoryCount; hc++)
	{
		int hcs = CGameController::GetHintCategoryState(hc);
		if (hcs == 1)
		{
			// Hint category is active/open
			CHintCategory* pHC = CGameController::GetHintCategory(hc);
			pHC->Reset();
			_activeHintCategories.push_back(pHC);
		}
	}

	int palette[256];

	// Load hint graphics
	CFile file;
	if (file.Open(L"HINT.AP"))
	{
		int size = file.Size();
		LPBYTE buffer = new BYTE[size];
		if (buffer != NULL)
		{
			file.Read(buffer, size);

			// Read palette
			LPBYTE pPal = buffer + GetInt(buffer, 2, 4);
			for (int c = 0; c < 256; c++)
			{
				double r = pPal[c * 3 + 0];
				double g = pPal[c * 3 + 1];
				double b = pPal[c * 3 + 2];
				int ri = (byte)((r * 255.0) / 63.0);
				int gi = (byte)((g * 255.0) / 63.0);
				int bi = (byte)((b * 255.0) / 63.0);
				int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
				palette[c] = col;
			}

			// Compressed image (active categories page)
			// Compressed image (hint page)
			// AP data with images
			int categoriesPageOffset = GetInt(buffer, 6, 4);
			int hintsPageOffset = GetInt(buffer, 10, 4);
			int buttonsOffset = GetInt(buffer, 14, 4);
			BinaryData bdCategoriesPage = CLZ::Decompress(buffer, categoriesPageOffset, hintsPageOffset - categoriesPageOffset);
			//BinaryData bdHintsPage = CLZ::Decompress(buffer, hintsPageOffset, buttonsOffset - hintsPageOffset);

			// TODO: Create textures of directory page (to get hints and directory images)
			_hintTexture.Init(bdCategoriesPage.Data, bdCategoriesPage.Length, 0, palette, -1, NULL, 11, 9, 93, 42, true, 640, 480);
			_categoryTexture.Init(bdCategoriesPage.Data, bdCategoriesPage.Length, 0, palette, -1, NULL, 108, 9, 421, 42, true, 640, 480);

			delete[] bdCategoriesPage.Data;
			//delete[] bdHintsPage.Data;

			// TODO: Create textures and vertex buffers
			// Entry 3.0 is blank
			// Entry 3.1 is dot
			// Entry 3.2 is check
			// Entry 3.8 is questionmark button

			_blankTexture.Init(buffer + buttonsOffset + GetInt(buffer, buttonsOffset + 2, 4), 0, 0, &palette[0], 0, "");
			_dotTexture.Init(buffer + buttonsOffset + GetInt(buffer, buttonsOffset + 6, 4), 0, 0, &palette[0], 0, "");
			_checkTexture.Init(buffer + buttonsOffset + GetInt(buffer, buttonsOffset + 10, 4), 0, 0, &palette[0], 0, "");
			_questionmarkTexture.Init(buffer + buttonsOffset + GetInt(buffer, buttonsOffset + 34, 4), 0, 0, &palette[0], 0, "");

			// Create vertex buffer of same dimension as symbols
			CreateTexturedRectangle(0.0f, 0.0f, -_blankTexture.Height() * pConfig->FontScale, _blankTexture.Width() * pConfig->FontScale, &_vertexBuffer, "");
			CreateTexturedRectangle(0.0f, 0.0f, -_hintTexture.Height() * pConfig->FontScale, _hintTexture.Width() * pConfig->FontScale, &_hintVertexBuffer, "");
			CreateTexturedRectangle(0.0f, 0.0f, -_categoryTexture.Height() * pConfig->FontScale, _categoryTexture.Width() * pConfig->FontScale, &_categoryVertexBuffer, "");

			delete[] buffer;
		}
	}

	_colBlack = palette[11];
	_colBlue = palette[12];
	_colCategory = palette[13];
	_colOrange = palette[14];
	_colGreen = palette[15];
	_colScore = palette[33];

	_colHighlight = palette[29];	// 29 or 65
	_colShade = palette[27];		// 27 or 67

	Rect rect1{ 0, 0, h, w };
	Rect rect2{ 0, 0, h, w };
	Rect rect3{ 0, 0, h, w };
	// TODO: Calculate position and space reserved for category? Need one for when it's in the list and one for when it's the selected category

	for (auto it : _activeHintCategories)
	{
		// Create buffers for category and (active) hints
		it->Prepare(_colBlack, _colBlue, _colCategory, _colOrange, _colGreen, _colScore, _colHighlight, _colShade, rect1, rect2, rect3);
	}

	char* pResume = "Resume";
	char* pDirectory = "Directory";

	float maxbtnw = max(TexFont.PixelWidth(pResume), TexFont.PixelWidth(pDirectory));

	_pBtnResume = new CDXButton(pResume, maxbtnw, 10.0f);
	_pBtnDirectory = new CDXButton(pDirectory, maxbtnw, 10.0f);

	_pBtnResume->SetPosition(w - maxbtnw - 38 * pConfig->FontScale, h - 46 * pConfig->FontScale);
	_pBtnDirectory->SetPosition(w - maxbtnw - 38 * pConfig->FontScale, h - 46 * pConfig->FontScale);
}

void CHintModule::Render()
{
	// Render original hint GUI or create a new? (If allowing font scale, may need to use a new, but can use parts of the original graphics)

	dx.Clear(0.73f, 0.73f, 0.73f);

	float y = 0.0f;// *pConfig->FontScale;
	float h = max(_blankTexture.Height() + 2, TexFont.Height()) * pConfig->FontScale;
	float boxw = _blankTexture.Width() * pConfig->FontScale;
	float x = 8.0f * pConfig->FontScale;

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	XMMATRIX wm;
	ID3D11ShaderResourceView* pRV = NULL;

	if (_pCurrentHintCategory == NULL)
	{
		// Render categories

		CShaders::SelectOrthoShader();
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dx.SetVertexBuffers(0, 1, &_categoryVertexBuffer, &stride, &offset);
		wm = XMMatrixTranslation((dx.GetWidth() - _categoryTexture.Width() * pConfig->FontScale) / 2, -y, -0.5f);
		CConstantBuffers::SetWorld(dx, &wm);
		pRV = _categoryTexture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		y += (_hintTexture.Height() + 2) * pConfig->FontScale;

		for (auto it : _activeHintCategories)
		{
			it->Render(x + boxw, y, true);

			dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
			dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			CShaders::SelectOrthoShader();
			XMMATRIX wm = XMMatrixTranslation(x, -y + 2 * pConfig->FontScale, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			ID3D11ShaderResourceView* pRV = _questionmarkTexture.GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(4, 0);

			y += h;
		}

		_pBtnResume->Render();
	}
	else
	{
		// Render category at top

		float cw = _pCurrentHintCategory->Width();
		float yoffset = ((_hintTexture.Height() - TexFont.Height()) * pConfig->FontScale) / 2.0f;
		_pCurrentHintCategory->Render((dx.GetWidth() - cw) / 2, y + yoffset, false);
		//y += TexFont.Height() * pConfig->FontScale * 2;

		y += (_hintTexture.Height() + 2) * pConfig->FontScale;

		// Render hints
		bool questionMarkUsed = false;
		for (auto it : _pCurrentHintCategory->Hints)
		{
			int state = it->GetState();

			ID3D11ShaderResourceView* pRV1 = _blankTexture.GetTextureRV();
			ID3D11ShaderResourceView* pRV2 = _dotTexture.GetTextureRV();

			if (state == 0 && !questionMarkUsed)
			{
				pRV2 = _questionmarkTexture.GetTextureRV();
				questionMarkUsed = true;
			}
			else if ((state & 1) != 0)
			{
				pRV1 = _checkTexture.GetTextureRV();
				it->Render(x + boxw * 2, y);
			}
			else if ((state & 2) != 0)
			{
				it->Render(x + boxw * 2, y);
				questionMarkUsed = true;
			}

			UINT stride = sizeof(TEXTURED_VERTEX);
			UINT offset = 0;
			dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
			dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			CShaders::SelectOrthoShader();
			XMMATRIX wm = XMMatrixTranslation(x, -y + 2 * pConfig->FontScale, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			dx.SetShaderResources(0, 1, &pRV1);
			dx.Draw(4, 0);

			wm = XMMatrixTranslation(x + boxw, -y + 2 * pConfig->FontScale, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			dx.SetShaderResources(0, 1, &pRV2);
			dx.Draw(4, 0);

			y += max(h, it->Height());
		}

		_pBtnDirectory->Render();
	}

	y = 0.0f;// *pConfig->FontScale;
	CShaders::SelectOrthoShader();
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dx.SetVertexBuffers(0, 1, &_hintVertexBuffer, &stride, &offset);
	wm = XMMatrixTranslation(0, -y, -0.5f);
	CConstantBuffers::SetWorld(dx, &wm);
	pRV = _hintTexture.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	dx.Draw(4, 0);

	// Render cursor
	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();

	dx.Present(1, 0);
}

void CHintModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);
	_pBtnDirectory->SetMouseOver(_pBtnDirectory->HitTest(x, y) != NULL);
}
