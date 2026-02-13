#include "GameBase.h"
#include "Utilities.h"
#include "DXScreen.h"
#include "VideoModule.h"
#include "GameController.h"
#include "LocationModule.h"
#include "ResumeGameModule.h"
#include <string>

CGameBase::CGameBase()
{
	_gameData = NULL;
	ZeroMemory(Timers, sizeof(Timers));
	_hintCategoryCount = 0;
	_allowCancelVideo = TRUE;

	_selectedItem = 0;
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
			int size = static_cast<int>(bd.Data + GetInt(bd.Data, 6 + i * 4, 4) - pIcon);
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

void CGameBase::ReadGameXMLInfo(int resource)
{
	DWORD xmlSize = 0;
	PBYTE xml = GetResource(resource, L"XML", &xmlSize);
#ifdef PLATFORM_WINDOWS
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

			// Extract buyable item list
			doc->selectNodes(L"GameData/BuyableItems/Item", &nodeList);
			ix = 0;
			while (nodeList->get_item(ix, &node) == S_OK)
			{
				CComQIPtr<IXMLDOMElement> element(node);
				CComVariant va;
				element->getAttribute(L"Name", &va);
				CGameController::SetBuyableItemName(ix++, va.bstrVal);
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

			// TODO: Read hints
			CComPtr<IXMLDOMNode> hintCategoriesNode = NULL;
			doc->selectSingleNode(L"GameData/Hints", &hintCategoriesNode);
			if (hintCategoriesNode != NULL)
			{
				CComVariant va;

				hintCategoriesNode->selectNodes(L"HintCategory", &nodeList);
				_hintCategoryCount = 0;
				while (nodeList->get_item(_hintCategoryCount++, &node) == S_OK)
				{
					CComQIPtr<IXMLDOMElement> element(node);
					element->getAttribute(L"Index", &va);
					int categoryIndex = (int)wcstol(va.bstrVal, 0, 10);
					if (_hintCategoryCount < categoryIndex)
					{
						_hintCategoryCount = categoryIndex;
					}

					element->getAttribute(L"Title", &va);
					std::wstring title = va.bstrVal;
					CHintCategory* pCategory = new CHintCategory(categoryIndex, title);

					// TODO: Create new hint category list
					//CGameController::SetSituationDescriptionD(ix++, va.bstrVal);
					// TODO: Get category hints

					CComPtr<IXMLDOMNode> hintNode = NULL;
					CComPtr<IXMLDOMNodeList> hintsNodeList;
					node->selectNodes(L"Hint", &hintsNodeList);
					int hintix = 0;
					while (hintsNodeList->get_item(hintix++, &hintNode) == S_OK)
					{
						CComQIPtr<IXMLDOMElement> hintElement(hintNode);
						hintElement->getAttribute(L"Index", &va);
						int hintIndex = (int)wcstol(va.bstrVal, 0, 10);
						BSTR bstr = NULL;
						hintElement->get_text(&bstr);
						if (bstr != NULL)
						{
							std::wstring hint(bstr);
							// Add hint to category
							pCategory->AddHint(hintIndex, hint);

							::SysFreeString(bstr);
						}
						hintNode.Release();
					}
					hintsNodeList.Release();

					node.Release();

					_hintCategories[categoryIndex] = pCategory;
				}
				nodeList.Release();

				hintCategoriesNode.Release();
			}
		}

		delete[] wcxml;
	}
#else
    if (xml == NULL || xmlSize == 0) return;
    std::string sXml((char*)xml, xmlSize);
    
    // Helper to extract attribute
    auto GetAttr = [&](const std::string& tag, const std::string& attr) {
        std::string needle = attr + "=\"";
        size_t start = tag.find(needle);
        if (start == std::string::npos) return std::string("");
        start += needle.length();
        size_t end = tag.find("\"", start);
        if (end == std::string::npos) return std::string("");
        return tag.substr(start, end - start);
    };

    // Files
    {
        size_t start = sXml.find("<Files>");
        size_t end = sXml.find("</Files>");
        if (start != std::string::npos && end != std::string::npos) {
            std::string section = sXml.substr(start, end - start);
            size_t pos = 0;
            long ix = 0;
            while ((pos = section.find("<File ", pos)) != std::string::npos) {
                size_t tagEnd = section.find("/>", pos);
                if (tagEnd == std::string::npos) break;
                std::string tag = section.substr(pos, tagEnd - pos);
                std::string val = GetAttr(tag, "Name");
                if (!val.empty()) CGameController::SetFileName(ix++, ToWString(val).c_str());
                pos = tagEnd;
            }
        }
    }
    
    // AskAbout
    {
        size_t start = sXml.find("<AskAbout>");
        size_t end = sXml.find("</AskAbout>");
        if (start != std::string::npos && end != std::string::npos) {
            std::string section = sXml.substr(start, end - start);
            size_t pos = 0;
            long ix = 0;
            while ((pos = section.find("<Topic ", pos)) != std::string::npos) {
                size_t tagEnd = section.find("/>", pos);
                if (tagEnd == std::string::npos) break;
                std::string tag = section.substr(pos, tagEnd - pos);
                std::string val = GetAttr(tag, "Name");
                if (!val.empty()) CGameController::SetAskAboutName(ix++, ToWString(val).c_str());
                pos = tagEnd;
            }
        }
    }

    // Items
    {
        size_t start = sXml.find("<Items>");
        size_t end = sXml.find("</Items>");
        if (start != std::string::npos && end != std::string::npos) {
            std::string section = sXml.substr(start, end - start);
            size_t pos = 0;
            long ix = 0;
            while ((pos = section.find("<Item ", pos)) != std::string::npos) {
                size_t tagEnd = section.find("/>", pos);
                if (tagEnd == std::string::npos) break;
                std::string tag = section.substr(pos, tagEnd - pos);
                std::string val = GetAttr(tag, "Name");
                if (!val.empty()) CGameController::SetItemName(ix++, ToWString(val).c_str());
                pos = tagEnd;
            }
        }
    }

    // BuyableItems
    {
        size_t start = sXml.find("<BuyableItems>");
        size_t end = sXml.find("</BuyableItems>");
        if (start != std::string::npos && end != std::string::npos) {
            std::string section = sXml.substr(start, end - start);
            size_t pos = 0;
            long ix = 0;
            while ((pos = section.find("<Item ", pos)) != std::string::npos) {
                size_t tagEnd = section.find("/>", pos);
                if (tagEnd == std::string::npos) break;
                std::string tag = section.substr(pos, tagEnd - pos);
                std::string val = GetAttr(tag, "Name");
                if (!val.empty()) CGameController::SetBuyableItemName(ix++, ToWString(val).c_str());
                pos = tagEnd;
            }
        }
    }

    // SaveSituations
    {
        size_t start = sXml.find("<SaveSituations>");
        if (start != std::string::npos) {
            size_t lStart = sXml.find("<L ", start);
            size_t lEnd = sXml.find("</L>", lStart);
            if (lStart != std::string::npos && lEnd != std::string::npos) {
                std::string lSec = sXml.substr(lStart, lEnd - lStart);
                size_t defPos = lSec.find("Default=\"");
                if (defPos != std::string::npos) {
                    size_t qEnd = lSec.find("\"", defPos + 9);
                    std::string defVal = lSec.substr(defPos + 9, qEnd - (defPos + 9));
                    CGameController::SetSituationDescriptionL(-1, ToWString(defVal).c_str());
                }
                size_t pos = 0;
                long ix = 0;
                while ((pos = lSec.find("<LD ", pos)) != std::string::npos) {
                     size_t tagEnd = lSec.find("/>", pos);
                     std::string tag = lSec.substr(pos, tagEnd - pos);
                     std::string val = GetAttr(tag, "Name");
                     if (!val.empty()) CGameController::SetSituationDescriptionL(ix++, ToWString(val).c_str());
                     pos = tagEnd;
                }
            }
            
            size_t dStart = sXml.find("<D ", start);
            size_t dEnd = sXml.find("</D>", dStart);
            if (dStart != std::string::npos && dEnd != std::string::npos) {
                std::string dSec = sXml.substr(dStart, dEnd - dStart);
                size_t defPos = dSec.find("Default=\"");
                if (defPos != std::string::npos) {
                    size_t qEnd = dSec.find("\"", defPos + 9);
                    std::string defVal = dSec.substr(defPos + 9, qEnd - (defPos + 9));
                    CGameController::SetSituationDescriptionD(-1, ToWString(defVal).c_str());
                }
                size_t pos = 0;
                long ix = 0;
                while ((pos = dSec.find("<DD ", pos)) != std::string::npos) {
                     size_t tagEnd = dSec.find("/>", pos);
                     std::string tag = dSec.substr(pos, tagEnd - pos);
                     std::string val = GetAttr(tag, "Name");
                     if (!val.empty()) CGameController::SetSituationDescriptionD(ix++, ToWString(val).c_str());
                     pos = tagEnd;
                }
            }
        }
    }

    // Hints
    {
        size_t start = sXml.find("<Hints>");
        size_t end = sXml.find("</Hints>");
        if (start != std::string::npos && end != std::string::npos) {
             std::string section = sXml.substr(start, end - start);
             size_t pos = 0;
             _hintCategoryCount = 0;
             while ((pos = section.find("<HintCategory ", pos)) != std::string::npos) {
                 size_t startTagEnd = section.find(">", pos);
                 size_t endTag = section.find("</HintCategory>", startTagEnd);
                 std::string catTag = section.substr(pos, startTagEnd - pos);
                 std::string catContent = section.substr(startTagEnd + 1, endTag - (startTagEnd + 1));
                 
                 std::string idxStr = GetAttr(catTag, "Index");
                 std::string title = GetAttr(catTag, "Title");
                 int catIdx = 0;
                 try { catIdx = std::stoi(idxStr); } catch(...) {}
                 if (_hintCategoryCount < catIdx) _hintCategoryCount = catIdx;
                 
                 CHintCategory* pCategory = new CHintCategory(catIdx, ToWString(title));
                 
                 size_t hPos = 0;
                 while ((hPos = catContent.find("<Hint ", hPos)) != std::string::npos) {
                     size_t hStartTagEnd = catContent.find(">", hPos);
                     size_t hEndTag = catContent.find("</Hint>", hStartTagEnd);
                     std::string hTag = catContent.substr(hPos, hStartTagEnd - hPos);
                     std::string hText = catContent.substr(hStartTagEnd + 1, hEndTag - (hStartTagEnd + 1));
                     
                     std::string hIdxStr = GetAttr(hTag, "Index");
                     int hIdx = 0;
                     try { hIdx = std::stoi(hIdxStr); } catch(...) {}
                     
                     pCategory->AddHint(hIdx, ToWString(hText));
                     hPos = hEndTag;
                 }
                 
                 _hintCategories[catIdx] = pCategory;
                 pos = endTag;
             }
        }
    }
#endif
}

void CGameBase::Start()
{
	Init();

	if (CFile::Exists(L"GAMES\\SAVEGAME.000"))
	{
		CModuleController::Push(new CResumeGameModule());
	}
	else
	{
		CModuleController::Push(new CVideoModule(VideoType::Single, L"TITLE.AP", 0));
	}
}

CHintCategory* CGameBase::GetHintCategory(int index)
{
	CHintCategory* pCategory = _hintCategories[index];
	return pCategory;
}
