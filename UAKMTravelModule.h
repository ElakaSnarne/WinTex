#pragma once

#include "ModuleBase.h"
#include <unordered_map>
#include <D3D11.h>
#include "Texture.h"
#include <list>
#include <string>
#include "DXText.h"
#include "Globals.h"

class CUAKMTravelModule : public CModuleBase
{
public:
	CUAKMTravelModule();
	virtual ~CUAKMTravelModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

protected:
	virtual void Initialize();

	int _palette[256];

	class CTravelImage
	{
	public:
		float Top;
		float Left;
		float Bottom;
		float Right;
		CTexture Texture;
		ID3D11Buffer* Buffer;

		void Render();
		void Render(float x, float y);

		BOOL HitTest(float x, float y) { return (x >= Left && x < Right && y >= -Top && y < -Bottom); }
	};

	std::unordered_map<int, CTravelImage*> _images;

	float _scale;
	float _offsetX;
	float _offsetY;

	int _selectedLocation;
	int _selectedSubLocation;
	int _selectedSubLocationEntry;

	class CSubLocation
	{
	public:
		int ParentLocation;
		std::string Text;
		CDXText RealText;
		float Top;
		float Left;
		float Bottom;
		float Right;

		CSubLocation(int parent, std::string text) { Left = Top = Right = Bottom = 0.0f; ParentLocation = parent; Text = text; RealText.SetText((char*)text.c_str()); }
	};

	std::list<CSubLocation*> _subLocations;

	ID3D11Buffer* _selectionIndicator;

	// Input related
	virtual void BeginAction();
	virtual void Back();
};
