#pragma once

#include "ModuleBase.h"
#include <list>
#include "D3D11-NoWarn.h"
#include "DXButton.h"
#include "AnimBase.h"
#include "DXImageButton.h"

class CInventoryModule : public CModuleBase
{
public:
	CInventoryModule();
	virtual ~CInventoryModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

	static int ExamineItemOnResume;

protected:
	virtual void Initialize();

	BOOL CheckButton(CDXButton* btn, float x, float y);

	static int _selectedItemId;
	static int _draggingItemId;
	static int _mouseOverItemId;
	POINT _mouseDownPoint;
	BOOL _dragging;
	static ULONGLONG _lastItemClick;

	static CDXButton* _pBtnExamine;
	static CDXButton* _pBtnUse;
	static CDXButton* _pBtnResume;

	static void OnExamine(LPVOID data);
	static void OnUse(LPVOID data);
	static void OnResume(LPVOID data);

	static ID3D11Buffer* _selectionRectangle;

	static LPBYTE _examData;
	static int _examStructSize;

	enum ExminationFlag
	{
		Video = 1,
		Image = 2,
		Text = 4,
		Unknown = 8
	};

	#pragma pack(1)
	struct ExminationData
	{
		BYTE ItemId;
		BYTE AddItemId;
		BYTE ParameterAIndex;
		BYTE ParameterAValue;
		BYTE AskAbout1;
		BYTE AskAbout2;
		BYTE Travel1;
		BYTE Travel2;
		BYTE File;
		BYTE Entry;
		LONG DescriptionOffset;
		BYTE Flags;
		WORD Rate;
		WORD HintState;
	};
	#pragma pack(8)

	static WCHAR _examFileName[11];
	static CAnimBase* _anim;

	static CDXText _text;

	static CDXImageButton* _pBtnUp;
	static CDXImageButton* _pBtnDown;

	D3D11_RECT _fullRect;
	static D3D11_RECT _limitedRect;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void EndAction();
	virtual void Back();
	virtual void Next();
	virtual void Prev();

	static void ScrollUp(LPVOID data);
	static void ScrollDown(LPVOID data);
	static void UpdateButtons();

	static int _lineAdjustment;
	static int _lineCount;
	static int _visibleLineCount;
};
