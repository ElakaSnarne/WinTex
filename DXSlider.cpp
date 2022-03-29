#include "DXSlider.h"
#include "resource.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"
#include <iomanip>
#include <sstream>
#include <string>

CTexture CDXSlider::_sliderTexBackground;
CTexture CDXSlider::_sliderTexSlider;
CTexture CDXSlider::_sliderTexSliderMouseOver;

CDXSlider::CDXSlider(char* text, float minValue, float maxValue, float step, float* pValue, int precision)
{
	_textX = 0;
	_precision = precision;
	_pValue = pValue;
	_w = 112.0f;
	_h = TexFont.Height() * pConfig->FontScale;
	_minimum = minValue;
	_maximum = maxValue;
	_step = step;

	CalculateSliderPosition();

	_pTLabel = new CDXText();
	_pTLabel->SetText(text);
	_pTLabel->SetColours(0xffffffff);

	_pTValue = new CDXText();
	_pTValue->SetColours(0xffffffff);
	_pTValue->Width(_w);
	UpdateValueText();

	TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[12];
	if (pVB != NULL)
	{
		// Background
		float x1 = 250.0f;
		float x2 = x1 + _w;
		float y1 = 0.0f;
		float y2 = -32.0f;
		pVB[0].position.x = x1;
		pVB[0].position.y = y1;
		pVB[0].position.z = -0.5f;
		pVB[0].texture.x = 0.0f;
		pVB[0].texture.y = 0.0f;

		pVB[1].position.x = x2;
		pVB[1].position.y = y1;
		pVB[1].position.z = -0.5f;
		pVB[1].texture.x = 1.0f;
		pVB[1].texture.y = 0.0f;

		pVB[2].position.x = x2;
		pVB[2].position.y = y2;
		pVB[2].position.z = -0.5f;
		pVB[2].texture.x = 1.0f;
		pVB[2].texture.y = 1.0f;

		pVB[3].position.x = x1;
		pVB[3].position.y = y1;
		pVB[3].position.z = -0.5f;
		pVB[3].texture.x = 0.0f;
		pVB[3].texture.y = 0.0f;

		pVB[4].position.x = x2;
		pVB[4].position.y = y2;
		pVB[4].position.z = -0.5f;
		pVB[4].texture.x = 1.0f;
		pVB[4].texture.y = 1.0f;

		pVB[5].position.x = x1;
		pVB[5].position.y = y2;
		pVB[5].position.z = -0.5f;
		pVB[5].texture.x = 0.0f;
		pVB[5].texture.y = 1.0f;

		// Slider element
		x1 = 0.0f;
		x2 = 6.0f;
		y1 = 0.0f;
		y2 = -26.0f;
		pVB[6].position.x = x1;
		pVB[6].position.y = y1;
		pVB[6].position.z = -0.5f;
		pVB[6].texture.x = 0.0f;
		pVB[6].texture.y = 0.0f;

		pVB[7].position.x = x2;
		pVB[7].position.y = y1;
		pVB[7].position.z = -0.5f;
		pVB[7].texture.x = 1.0f;
		pVB[7].texture.y = 0.0f;

		pVB[8].position.x = x2;
		pVB[8].position.y = y2;
		pVB[8].position.z = -0.5f;
		pVB[8].texture.x = 1.0f;
		pVB[8].texture.y = 1.0f;

		pVB[9].position.x = x1;
		pVB[9].position.y = y1;
		pVB[9].position.z = -0.5f;
		pVB[9].texture.x = 0.0f;
		pVB[9].texture.y = 0.0f;

		pVB[10].position.x = x2;
		pVB[10].position.y = y2;
		pVB[10].position.z = -0.5f;
		pVB[10].texture.x = 1.0f;
		pVB[10].texture.y = 1.0f;

		pVB[11].position.x = x1;
		pVB[11].position.y = y2;
		pVB[11].position.z = -0.5f;
		pVB[11].texture.x = 0.0f;
		pVB[11].texture.y = 1.0f;

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 12;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = pVB;
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, text);

		delete[] pVB;
	}

	_type = ControlType::Slider;
}

CDXSlider::~CDXSlider()
{
	if (_pTLabel != NULL)
	{
		delete _pTLabel;
		_pTLabel = NULL;
	}

	if (_pTValue != NULL)
	{
		delete _pTValue;
		_pTValue = NULL;
	}
}

void CDXSlider::Init()
{
	DWORD s1, s2, s3;
	PBYTE p1 = GetResource(IDB_SLIDER, L"PNG", &s1);
	PBYTE p2 = GetResource(IDB_BUTTON, L"PNG", &s2);
	PBYTE p3 = GetResource(IDB_BUTTON_MOUSEOVER, L"PNG", &s3);

	_sliderTexBackground.Init(p1, s1, "SLIDER1");
	_sliderTexSlider.Init(p2, s2, "SLIDER2");
	_sliderTexSliderMouseOver.Init(p3, s3, "SLIDER3");
}

void CDXSlider::Dispose()
{
	_sliderTexBackground.Dispose();
	_sliderTexSlider.Dispose();
	_sliderTexSliderMouseOver.Dispose();
}

void CDXSlider::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	// Render background
	ID3D11ShaderResourceView* pRV = _sliderTexBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);

	// Draw slider element
	wm = XMMatrixTranslation(_x + 250.0f + _sliderPosition, -4.0f - _y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	pRV = _mouseOver ? _sliderTexSliderMouseOver.GetTextureRV() : _sliderTexSlider.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	dx.Draw(6, 6);

	// Draw text
	_pTLabel->Render(_textX + _x, _y + 8.0f);
	_pTValue->Render(_textX + _x + 250.0f, _y + 32.0f);
}

void CDXSlider::UpdateValueText()
{
	Rect rc{ 0,0,TexFont.Height(),_pTValue->Width() };
		std::stringstream stream;
		stream << std::fixed << std::setprecision(_precision) << *_pValue;
		std::string data = stream.str();
		_pTValue->SetText(data.c_str(), rc, CDXText::Alignment::Center);
}

void CDXSlider::CalculateSliderPosition()
{
	float value = min(_maximum, max(_minimum, (_pValue != NULL) ? *_pValue : _minimum));
	float span = _maximum - _minimum;
	if (span > 0.0f)
	{
		_sliderPosition = 106.0f * (value - _minimum) / span;
	}
}

CDXControl* CDXSlider::HitTest(float x, float y)
{
	return (_visible && x >= (_x + 252.0f + _sliderPosition) && y >= (_y + 6.0f) && x < (_x + 258.0f + _sliderPosition) && y < (_y + 30.0f)) ? this : NULL;
}

void CDXSlider::Drag(float x, float y)
{
	float span = _maximum - _minimum;
	if (span > 0.0f && _step > 0.0f)
	{
		float min_x_pos = _x + 252.0f;
		float max_x_pos = _x + 358.0f;

		float pos_x = max(min_x_pos, min(max_x_pos, x)) - min_x_pos;
		int positions = 1 + static_cast<int>(span / _step);
		// Find which step position is the closest
		float pixels_per_step = 106.0f / (positions - 1);
		int pos_index = static_cast<int>(pos_x / pixels_per_step);
		_sliderPosition = (106.0f * pos_index) / (positions - 1);
		if (_pValue != NULL)
		{
			*_pValue = _minimum + _step * pos_index;
			UpdateValueText();
		}
	}
}
