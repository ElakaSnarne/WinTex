#include "PDInventoryModule.h"

//#include "Globals.h"
#include "GameController.h"
#include "Items.h"
//#include "PDNewsPaperModule.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "PDGame.h"
//#include "LocationModule.h"
#include "PDExamStruct.h"

//short pdComboTable[] = { 0,5,26,1,4,0,1,5,129,4,129,26,7,37,39,9,12,2,10,77,78,14,15,49,17,35,138,18,112,114,27,28,52,42,43,85,45,46,88,45,48,87,46,87,107,48,88,107,58,59,90,60,90,93,61,70,102,65,66,108,66,73,110,66,96,132,73,131,96,79,80,76,91,103,95,110,131,132,111,114,124 };
//
//short pdComboScoreTable[] = { 8,85,14,76,6,108,10,93 };
//
//short pdComboHintStateScoreTable[] = { 0,11,2,75,26,12,39,108,49,135,52,145,76,234,78,227,85,263,88,342,90,373,93,374,95,189,102,418,107,343,108,469,110,453,114,379,124,380,132,454,138,209,255,-1 };

wchar_t* hex = L"0123456789ABCDEF";

CPDInventoryModule::CPDInventoryModule() : CInventoryModule()
{
	_text.SetColours(0xff000000, 0xff000000, 0xff00ff00, 0xff000000);
	CItems::SetTextColours(0, 0, -1, 0);
}

void CPDInventoryModule::Examine()
{
	if (_selectedItemId >= 0)
	{
		_anim = NULL;

		PDExamStruct* pExam = (PDExamStruct*)(_examData + 8 + _selectedItemId * sizeof(PDExamStruct));

		_examFileName[4] = hex[(pExam->ExamFileNumber >> 4) & 0xf];
		_examFileName[5] = hex[(pExam->ExamFileNumber) & 0xf];

		char* pDesc = (char*)(pExam->DescriptionOffset + _examData);
		pAddCaptions->clear();

		std::list<CCaption*>* pOld = pDisplayCaptions;
		pDisplayCaptions = pAddCaptions;
		pAddCaptions = pOld;
		ClearCaptions(pOld);
		Rect rect{ 0.0f, 0.0f, 1000.0f, static_cast<float>(_limitedRect.right - _limitedRect.left) };
		_text.SetText(pDesc, rect);
		_lineCount = _text.Lines();
		_lineAdjustment = max(0, min(_lineCount - _visibleLineCount, _lineCount));
		UpdateButtons();

		if ((pExam->Flags & EXAMINE_FLAG_VIDEO) != 0)
		{
			BinaryData bd = LoadEntry(_examFileName, pExam->ExamEntryNumber);
			_anim = CAnimationController::Load(bd, 2);
		}
		else if ((pExam->Flags & EXAMINE_FLAG_IMAGE) != 0)
		{
			DoubleData dd = LoadDoubleEntry(_examFileName, pExam->ExamEntryNumber);
			_anim = CAnimationController::LoadImage(dd, 320, 240, 2);
		}
		else if (pExam->Flags == 0)
		{
			//// Special examine module required (newspaper, shredded/torn notes etc)
			//if (_selectedItemId == ITEM_NOTE_SCRAPS || _selectedItemId == ITEM_NOTE_SCRAPS_2 || _selectedItemId == ITEM_SHREDDED_NOTE)
			//{
			//	CModuleController::Push(new CUAKMTornNoteModule(_selectedItemId));
			//}
			//else if (_selectedItemId == ITEM_NEWSPAPER)
			//{
			//	// Show newspaper
			//	CModuleController::Push(new CUAKMNewsPaperModule());
			//}
			//else if (_selectedItemId == ITEM_ENCODED_NOTE)
			//{
			//	CModuleController::Push(new CUAKMEncodedMessageModule());
			//}
		}

		int extraScore = 0;
		//if (_selectedItemId == 55 || _selectedItemId == 76 || _selectedItemId == 94 || _selectedItemId == 106 || _selectedItemId == 134)
		//{
		//	extraScore = 4;
		//}
		//else if (_selectedItemId == 85)
		//{
		//	extraScore = 14;
		//}

		CGameController::SetItemExamined(_selectedItemId, extraScore);
	}
}

void CPDInventoryModule::Resume()
{
	if (_anim == NULL)
	{
		CModuleController::Pop(CModuleController::CurrentModule);
	}
	else
	{
		delete _anim;
		_anim = NULL;

		PDExamStruct* pExam = (PDExamStruct*)(_examData + 8 + _selectedItemId * sizeof(PDExamStruct));
		if (pExam->Id != _selectedItemId)
		{
			// Transform item (remove old, add new)
			CGameController::SetItemState(_selectedItemId, 2);
			CGameController::SetItemState(pExam->Id, 1);
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
			CGameController::SetData(PD_SAVE_TRAVEL + pExam->Travel1, 1);
		}
		if (pExam->Travel2 != 0xff)
		{
			CGameController::SetData(PD_SAVE_TRAVEL + pExam->Travel2, 1);
		}
		if (pExam->HintState != 0xffff)
		{
			CGameController::SetHintState(pExam->HintState, 1, 1);
		}
	}
}
