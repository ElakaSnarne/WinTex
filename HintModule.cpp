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

void CHintModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);
	_pBtnDirectory->SetMouseOver(_pBtnDirectory->HitTest(x, y) != NULL);
}
