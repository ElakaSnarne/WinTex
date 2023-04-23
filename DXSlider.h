#pragma once
#include "DXControl.h"
#include "DXText.h"
#include "Texture.h"

class CDXSlider : public CDXControl
{
public:
	CDXSlider(char* text, float minValue, float maxValue, float step, float* pValue, int precision, float sliderX);
	~CDXSlider();

	static void Init();
	static void Dispose();

	virtual void Render();

	void SetValue(float value) { *_pValue = value; UpdateValueText(); }
	float GetValue() { return *_pValue; }

	void UpdateValueText();
	void CalculateSliderPosition();
	virtual CDXControl* HitTest(float x, float y);
	void Drag(float x, float y);

	virtual void SetColours(int colour1, int colour2, int colour3, int colour4);

protected:
	static CTexture _sliderTexBackground;
	static CTexture _sliderTexSlider;
	static CTexture _sliderTexSliderMouseOver;
	CDXText* _pTLabel;
	CDXText* _pTValue;

	float _textX;
	float _textY;
	float _sliderX;

	float _sliderPosition;

	float* _pValue;
	float _step;
	int _precision;
	float _minimum;
	float _maximum;
};

