#pragma once

#include "DXBase.h"

class CDXAdapter : CDXBase
{
public:
	CDXAdapter(IDXGIAdapter* pAdapter);
	~CDXAdapter();

	DXGI_ADAPTER_DESC _adapterDesc;
	unsigned int _numModes;
	DXGI_MODE_DESC* _displayModeList;

private:
	IDXGIAdapter* _pAdapter;
};
