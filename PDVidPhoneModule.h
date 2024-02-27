#pragma once

#include "ModuleBase.h"
#include "Image.h"

class CPDVidPhoneModule : public CModuleBase
{
public:
	CPDVidPhoneModule();
	~CPDVidPhoneModule();

	virtual void Resize(int width, int height);
	virtual void Render();

protected:
	virtual void Initialize();

	CImage* _pBackground;
	ID3D11Buffer* _vertexBuffer;

	// Input related
	virtual void BeginAction();
	virtual void Back();
};
