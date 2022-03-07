#include "DXAdapter.h"

CDXAdapter::CDXAdapter(IDXGIAdapter* pAdapter)
{
	_numModes = 0;
	_displayModeList = NULL;

	_pAdapter = pAdapter;
	pAdapter->GetDesc(&_adapterDesc);

	IDXGIOutput* adapterOutput;
	if (pAdapter->EnumOutputs(0, &adapterOutput) == S_OK)
	{
		if (adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &_numModes, NULL) == S_OK)
		{
			_displayModeList = new DXGI_MODE_DESC[_numModes];
			if (adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &_numModes, _displayModeList) == S_OK)
			{
				for (int i = 0; i < _numModes; i++)
				{
					for (int j = i + 1; j < _numModes; j++)
					{
						if (_displayModeList[i].Width == _displayModeList[j].Width && _displayModeList[i].Height == _displayModeList[j].Height)
						{
							for (int k = j + 1; k < _numModes; k++)
							{
								_displayModeList[k - 1] = _displayModeList[k];
							}

							j--;
							_numModes--;
						}
					}
				}
			}
		}

		adapterOutput->Release();
	}
}

CDXAdapter::~CDXAdapter()
{
	if (_pAdapter != NULL)
	{
		_pAdapter->Release();
		_pAdapter = NULL;
	}

	if (_displayModeList != NULL)
	{
		delete _displayModeList;
		_displayModeList = NULL;
	}
}
