#pragma once

#include "DXControl.h"
#include "DXText.h"
#include "Texture.h"
#include "SaveGameInfo.h"

class CSaveGameControl : public CDXControl
{
public:
	CSaveGameControl(void(*onClick)(SaveGameInfo), bool isSafe = false);
	virtual ~CSaveGameControl();

	static void Init();
	static void Dispose();

	virtual void Render();
	virtual void MouseEnter();
	virtual void MouseMove();
	virtual void MouseLeave();
	virtual void MouseButtonDown();
	virtual void MouseButtonUp();
	virtual void KeyDown();
	virtual void KeyUp();
	virtual void GotFocus();
	virtual void LostFocus();

	void SetMouseOver(BOOL mouseOver);

	void SetInfo(SaveGameInfo info);
	SaveGameInfo GetInfo();
	void SetFileName(std::wstring fileName);
	void SetComment(std::string comment);

	virtual void Click();

	void SetPDColours();

	float GetColumn2();
	float GetColumn4();
	float GetXLimit();

protected:
	void(*_clicked)(SaveGameInfo info);

	bool _isSave;

	static CTexture _texBackground;
	static CTexture _texMouseOver;

	SaveGameInfo _info;

	CDXText _tFileName1;
	CDXText _tFileName2;
	CDXText _tPlayer1;
	CDXText _tPlayer2;
	CDXText _tDay1;
	CDXText _tDay2;
	CDXText _tDateTime1;
	CDXText _tDateTime2;
	CDXText _tLocation1;
	CDXText _tLocation2;
	CDXText _tComment1;
	CDXText _tComment2;
};
