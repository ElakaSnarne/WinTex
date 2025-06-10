#pragma once

#include "ModuleBase.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"
#include "DXButton.h"

class CPDNewspaperModule : public CModuleBase
{
public:
	CPDNewspaperModule();
	virtual ~CPDNewspaperModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

	static CPDNewspaperModule* pPDNPM;

protected:
	virtual void Initialize();

	int _display;
	int _highLight;

	float _left;
	float _right;
	float _top;
	float _bottom;
	float _scale;

	int _palette[256];

	CDXButton* _pBtnResume;
	static void OnResume(LPVOID data);

	class CNewsPaperView
	{
	public:
		CNewsPaperView()
		{
			Data = NULL;
			Buffer = NULL;
		}

		~CNewsPaperView()
		{
			if (Data != NULL)
			{
				delete[] Data;
				Data = NULL;
			}

			if (Buffer != NULL)
			{
				Buffer->Release();
				Buffer = NULL;
			}
		}

		LPBYTE Data = NULL;
		int Width = 0;
		int Height = 0;
		CTexture Texture;
		ID3D11Buffer* Buffer = NULL;
	};

	std::unordered_map<int, CNewsPaperView*> _newsPaper;

	void UpdateTexture(CNewsPaperView* np);

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();
};
