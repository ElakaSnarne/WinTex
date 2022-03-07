#include "GameBase.h"
#include "Utilities.h"
#include "DXScreen.h"
#include "VideoModule.h"
#include "GameController.h"
#include "LocationModule.h"

CGameBase::CGameBase()
{
	_gameData = NULL;
	ZeroMemory(Timers, sizeof(Timers));
}

CGameBase::~CGameBase()
{
	for (int i = 0; i < 12; i++)
	{
		CModuleController::Cursors[i].Dispose();
	}
}

void CGameBase::LoadFromDMap(int entry)
{
	CModuleController::Push(new CVideoModule(VideoType::Scripted, entry));
}

void CGameBase::LoadFromMap(int entry, int startupPosition)
{
	CModuleController::Push(GetLocationModule(entry, startupPosition));
}

BOOL CGameBase::LoadIcons(BinaryData bd)
{
	// Internally an AP file, should now extract all the icons
	int count = *(short*)bd.Data;

	BYTE colourTranslationTable[64];
	for (int i = 0; i < 64; i++)
	{
		colourTranslationTable[i] = (BYTE)(4.04762 * i);
	}

	BinaryData palette_bd = LoadEntry(L"GRAPHICS.AP", 0);
	if (palette_bd.Data != NULL && palette_bd.Length > 0)
	{
		float cx = dx.GetWidth() / 2.0f;
		float cy = dx.GetHeight() / 2.0f;

		for (int i = 0; i < (count - 2) && i < 12; i++)
		{
			PBYTE pIcon = bd.Data + GetInt(bd.Data, 2 + i * 4, 4);
			int size = (int)bd.Data + GetInt(bd.Data, 6 + i * 4, 4) - (int)pIcon;
			PBYTE pEnd = pIcon + size;
			if (size > 1)
			{
				// Decompress icon, make memory texture and load to bitmap
				memcpy(palette_bd.Data + 3, pIcon, 21);
				PBYTE pPalette = palette_bd.Data;
				pIcon += 21;
				std::list<CDXBitmap*> icons;
				while (pIcon < pEnd)
				{
					int w = GetInt(pIcon, 2, 2);
					int h = GetInt(pIcon, 4, 2);
					int dataSize = GetInt(pIcon, 9, 4);

					// Create new texture, set pixels manually
					CDXBitmap* pB = new CDXBitmap(w, h);
					icons.push_back(pB);

					CTexture* pTexture = pB->GetTexture();
					ID3D11Texture2D* pTex = pTexture->GetTexture();
					D3D11_MAPPED_SUBRESOURCE subRes;
					dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);

					PBYTE pSrc = pIcon + 16;
					PBYTE pDst = (PBYTE)subRes.pData;
					ZeroMemory(pDst, h * subRes.RowPitch);

					for (int y = 0; y < h; y++)
					{
						int o = GetInt(pSrc, 0, 2);
						int l = GetInt(pSrc, 2, 2);

						for (int x = 0; x < l; x++)
						{
							int c = pSrc[4 + x];
							if (c > 0)
							{
								pDst[(o + x) * 4 + 2] = colourTranslationTable[pPalette[c * 3 + 0] & 0x3f];
								pDst[(o + x) * 4 + 1] = colourTranslationTable[pPalette[c * 3 + 1] & 0x3f];
								pDst[(o + x) * 4 + 0] = colourTranslationTable[pPalette[c * 3 + 2] & 0x3f];
								pDst[(o + x) * 4 + 3] = 0xff;
							}
						}

						pSrc += 4 + l;
						pDst += subRes.RowPitch;
					}

					dx.Unmap(pTex, 0);

					pIcon += dataSize;
				}

				CModuleController::Cursors[i].SetIcons((CAnimatedCursor::CursorType)i, icons);
				CModuleController::Cursors[i].SetPosition(cx, cy);
			}
		}

		delete[] palette_bd.Data;

		return TRUE;
	}

	return FALSE;
}

void CGameBase::BuildFileList(int resource)
{
	DWORD xmlSize = 0;
	PBYTE xml = GetResource(resource, L"XML", &xmlSize);
	PWCHAR wcxml = new WCHAR[xmlSize + 1];
	if (wcxml != NULL)
	{
		memset(wcxml, 0, xmlSize + 1);
		size_t wcsize = 0;
		int test = mbstowcs_s(&wcsize, wcxml, xmlSize + 1, (char*)xml, xmlSize);
		CComPtr<IXMLDOMDocument> doc;
		VARIANT_BOOL vb = VARIANT_FALSE;
		CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_ALL, __uuidof(IXMLDOMDocument), (void**)&doc);
		if (doc->loadXML(wcxml, &vb) == S_OK)
		{
			CComPtr<IXMLDOMNodeList> nodeList;

			// Extract file list
			doc->selectNodes(L"GameData/Files/File", &nodeList);
			CComPtr<IXMLDOMNode> node;
			long ix = 0;
			while (nodeList->get_item(ix, &node) == S_OK)
			{
				CComQIPtr<IXMLDOMElement> element(node);
				CComVariant va;
				element->getAttribute(L"Name", &va);
				CGameController::SetFileName(ix++, va.bstrVal);
				node.Release();
			}
			nodeList.Release();

			// Extract ask about list
			doc->selectNodes(L"GameData/AskAbout/Topic", &nodeList);
			ix = 0;
			while (nodeList->get_item(ix, &node) == S_OK)
			{
				CComQIPtr<IXMLDOMElement> element(node);
				CComVariant va;
				element->getAttribute(L"Name", &va);
				CGameController::SetAskAboutName(ix++, va.bstrVal);
				node.Release();
			}
			nodeList.Release();

			// Extract item list
			doc->selectNodes(L"GameData/Items/Item", &nodeList);
			ix = 0;
			while (nodeList->get_item(ix, &node) == S_OK)
			{
				CComQIPtr<IXMLDOMElement> element(node);
				CComVariant va;
				element->getAttribute(L"Name", &va);
				CGameController::SetItemName(ix++, va.bstrVal);
				node.Release();
			}
			nodeList.Release();

			// Extract situation descriptions
			CComPtr<IXMLDOMNode> locSitNode = NULL;
			doc->selectSingleNode(L"GameData/SaveSituations/L", &locSitNode);
			if (locSitNode != NULL)
			{
				CComQIPtr<IXMLDOMElement> locSit(locSitNode);
				CComVariant va;
				HRESULT hr = locSit->getAttribute(L"Default", &va);
				CGameController::SetSituationDescriptionL(-1, va.bstrVal);

				locSit->selectNodes(L"LD", &nodeList);
				ix = 0;
				while (nodeList->get_item(ix, &node) == S_OK)
				{
					CComQIPtr<IXMLDOMElement> element(node);
					element->getAttribute(L"Name", &va);
					CGameController::SetSituationDescriptionL(ix++, va.bstrVal);
					node.Release();
				}
				nodeList.Release();

				locSitNode.Release();
			}

			CComPtr<IXMLDOMNode> diaSitNode = NULL;
			doc->selectSingleNode(L"GameData/SaveSituations/D", &diaSitNode);
			if (diaSitNode != NULL)
			{
				CComQIPtr<IXMLDOMElement> diaSit(diaSitNode);
				CComVariant va;
				diaSit->getAttribute(L"Default", &va);
				CGameController::SetSituationDescriptionD(-1, va.bstrVal);

				diaSit->selectNodes(L"DD", &nodeList);
				ix = 0;
				while (nodeList->get_item(ix, &node) == S_OK)
				{
					CComQIPtr<IXMLDOMElement> element(node);
					element->getAttribute(L"Name", &va);
					CGameController::SetSituationDescriptionD(ix++, va.bstrVal);
					node.Release();
				}
				nodeList.Release();

				diaSitNode.Release();
			}
		}

		delete[] wcxml;
	}
}
