#include "DXText.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"

CDXText::CDXText()
{
	_vertexBuffer = NULL;

	// TODO: Allow tab character
	// TODO: Compress multiple spaces? (This is currently done, but should be optional)

	_lines = 0;

	_printableCharacters = 0;
	_width = 0.0f;

	// Default colours, white without border
	_colour1 = 0;
	_colour2 = -1;
	_colour3 = -1;
	_colour4 = 0;
}

CDXText::~CDXText()
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}
}

void CDXText::Render(float x, float y)
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* pRV = TexFont.GetTextureRV();
	XMMATRIX wm = XMMatrixTranslation(floor(x), -floor(y), -1.0f);

	CShaders::SelectTexFontShader();

	CDXFont::SelectFontColour(_colour1, _colour2, _colour3, _colour4);
	CConstantBuffers::SetWorld(dx, &wm);
	dx.SetShaderResources(0, 1, &pRV);
	dx.Draw(_printableCharacters * 6, 0);
}

CDXText::CWordList::CWordList()
{
	_text = NULL;
	_chars = NULL;
	_pixels = NULL;
	_next = NULL;
	_last = NULL;
}

CDXText::CWordList::CWordList(char* text, int chars, float pixels)
{
	_text = text;
	_chars = chars;
	_pixels = pixels;
	_next = NULL;
	_last = NULL;
}

void CDXText::CWordList::Add(char* text, int chars, float pixels)
{
	CWordList* pWL = new CWordList(text, chars, pixels);
	if (_next == NULL) _next = pWL;
	if (_last != NULL) _last->_next = pWL;
	_last = pWL;
}

CDXText::CWordList::~CWordList()
{
	while (_next != NULL)
	{
		CWordList* next = _next;
		_next = next->_next;
		next->_next = NULL;
		delete next;
	}
}

CDXText::CWordList* CDXText::CWordList::Next()
{
	return _next;
}

float CDXText::CWordList::Pixels()
{
	return _pixels;
}

char* CDXText::CWordList::Text()
{
	return _text;
}

int CDXText::CWordList::Chars()
{
	return _chars;
}

void CDXText::SetText(LPCWSTR text, Alignment alignment)
{
	Rect rc;
	rc.Top = 0;
	rc.Left = 10.0f;
	rc.Bottom = -dx.GetHeight();
	rc.Right = dx.GetWidth() - 10.0f;
	SetText(text, rc, alignment);
}

void CDXText::SetText(char* text, Alignment alignment)
{
	Rect rc;
	rc.Top = 0;
	rc.Left = 10.0f;
	rc.Bottom = -dx.GetHeight();
	rc.Right = dx.GetWidth() - 10.0f;
	SetText(text, rc, alignment);
}

void CDXText::SetText(char* text, Rect rect, Alignment alignment)
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	_lines = 0;

	CWordList wl;
	char* scan = text;
	char* start = scan;
	float pixels = 0.0f;
	float spaceWidth = TexFont.SpaceWidth();
	_printableCharacters = 0;

	float* pWidths = TexFont.Widths();

	while (TRUE)
	{
		char c = *(scan++);
		if (c == 0) break;
		else if (c == 0x20 || c == 0xa || c == 0xd)
		{
			// space, add word to list
			int len = scan - start - 1;
			if (len > 0)
			{
				wl.Add(start, len, pixels);
			}

			if (c == 0xa || c == 0xd)
			{
				wl.Add(NULL, 0, 0);
			}

			start = scan;
			pixels = 0.0f;
		}
		else if (c > 0x20 && c <= 0xff)
		{
			pixels += ((float)pWidths[c - 0x20]) * pConfig->FontScale;
			_printableCharacters++;
		}
	}

	// Add last word to list
	int len = scan - start - 1;
	if (len > 0)
	{
		wl.Add(start, len, pixels);
	}

	if (_printableCharacters > 0)
	{
		TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[6 * _printableCharacters];
		if (pVB != NULL)
		{
			// Calculate how many lines are required
			_lines = 0;
			CWordList* pWL = wl.Next();
			float maxw = rect.Right - rect.Left;
			float pixelsLeft = 0.0f;
			CWordList* print = NULL;
			int wordsInLine = 0;

			float sy = 0.0f;
			int cix = 0;
			float fcw = 1 / 224.0f;
			float y1 = TexFont.Y1();
			float y2 = TexFont.Y2();
			float fh = TexFont.Height() * pConfig->FontScale;

			while (pWL != NULL)
			{
				if (pWL->Pixels() == 0)
				{
					// Forced new line
					_lines++;
					pWL = pWL->Next();
				}
				else
				{
					if (pWL->Pixels() > pixelsLeft)
					{
						// New line required
						_lines++;
						pixelsLeft = maxw - pWL->Pixels();
						print = pWL;
						pWL = pWL->Next();
						wordsInLine = 1;
					}

					while (pWL != NULL && pWL->Pixels() > 0 && (pixelsLeft - (pWL->Pixels() + spaceWidth)) > -0.01)	// Enough room for the next word?
					{
						wordsInLine++;
						pixelsLeft -= pWL->Pixels() + spaceWidth;
						pWL = pWL->Next();
					}

					// Print this line...
					float sx = floor(rect.Left);
					float justifyadjust = 0.0f;
					switch (alignment)
					{
						case Alignment::Left:
						{
							break;
						}
						case Alignment::Center:
						{
							sx = pixelsLeft / 2.0f;
							break;
						}
						case Alignment::Right:
						{
							sx = maxw - (pixelsLeft + (wordsInLine > 1 ? spaceWidth : 0));
							break;
						}
						case Alignment::Justify:
						{
							justifyadjust = (wordsInLine > 1 && pWL != NULL) ? pixelsLeft / ((float)(wordsInLine - 1)) : 0.0f;
							break;
						}
						case Alignment::JustifyAlways:
						{
							justifyadjust = (wordsInLine > 1) ? pixelsLeft / ((float)(wordsInLine - 1)) : 0.0f;
							break;
						}
					}

					if (pWL != NULL && pWL->Pixels() == 0)
					{
						justifyadjust = 0.0f;
					}

					while (print != NULL && wordsInLine-- > 0)
					{
						sx = floor(sx);

						for (int c = 0; c < print->Chars(); c++)
						{
							char ch = print->Text()[c];
							if (ch >= 0x20 && ch <= 0x7f)
							{
								ch -= 0x20;
								// Create vertices and indexes for this character...
								float fx = pWidths[ch];

								float x1 = fcw * (float)ch;
								float x2 = x1 + fx / 3584.0f;// 5760.0f;

								fx *= pConfig->FontScale;

								// Create 6 vertices and 6 indexes per char
								float rsx = floor(sx);
								pVB[cix].position = XMFLOAT3(rsx, sy, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x1, y1);
								pVB[cix].position = XMFLOAT3(rsx + fx, sy, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x2, y1);
								pVB[cix].position = XMFLOAT3(rsx + fx, sy - fh, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x2, y2);

								pVB[cix].position = XMFLOAT3(rsx, sy, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x1, y1);
								pVB[cix].position = XMFLOAT3(rsx + fx, sy - fh, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x2, y2);
								pVB[cix].position = XMFLOAT3(rsx, sy - fh, -1.5f);
								pVB[cix++].texture = XMFLOAT2(x1, y2);

								sx += fx;
							}
						}

						sx += spaceWidth + justifyadjust;
						print = print->Next();
					}

					if (_width < (sx - spaceWidth - justifyadjust - rect.Left))
					{
						_width = sx - spaceWidth - justifyadjust - rect.Left;
					}
				}

				sy -= TexFont.Height() * pConfig->FontScale;

				wordsInLine = 0;
				pixelsLeft = 0.0f;
			}

			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;
			vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 6 * _printableCharacters;
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = pVB;
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "Text");

			delete[] pVB;
		}
	}
}

void CDXText::SetText(LPCWSTR text, Rect rect, Alignment alignment)
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	_lines = 0;

	CWordList wl;
	LPCWSTR scan = text;
	LPCWSTR start = scan;
	float pixels = 0.0f;
	float spaceWidth = TexFont.SpaceWidth();
	_printableCharacters = 0;

	float* pWidths = TexFont.Widths();

	while (TRUE)
	{
		char c = *(scan++);
		if (c == 0) break;
		else if (c == 0x20)
		{
			// Space, add word to list
			int len = scan - start - 1;
			if (len > 0)
			{
				wl.Add((char*)start, len, pixels);
			}
			start = scan;
			pixels = 0.0f;
		}
		else if (c > 0x20 && c <= 0x7f)
		{
			pixels += ((float)pWidths[c - 0x20]) * pConfig->FontScale;
			_printableCharacters++;
		}
	}

	// Add last word to list
	int len = scan - start - 1;
	if (len > 0)
	{
		wl.Add((char*)start, len, pixels);
	}

	if (_printableCharacters > 0)
	{
		TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[6 * _printableCharacters];
		if (pVB != NULL)
		{
			// Calculate how many lines are required
			_lines = 0;
			CWordList* pWL = wl.Next();
			float maxw = rect.Right - rect.Left;
			float pixelsLeft = 0.0f;
			CWordList* print = NULL;
			int wordsInLine = 0;

			float sy = 0.0f;
			int cix = 0;
			float fcw = 1.0f / 224.0f;// 96.0f;
			float y1 = TexFont.Y1();
			float y2 = TexFont.Y2();
			float fh = TexFont.Height() * pConfig->FontScale;

			while (pWL != NULL)
			{
				if (pWL->Pixels() > pixelsLeft)
				{
					// New line required
					_lines++;
					pixelsLeft = maxw - pWL->Pixels();
					print = pWL;
					pWL = pWL->Next();
					wordsInLine = 1;
				}

				while (pWL != NULL && (pixelsLeft - (pWL->Pixels() + spaceWidth)) > -0.01)	// Enough room for the next word?
				{
					wordsInLine++;
					pixelsLeft -= pWL->Pixels() + spaceWidth;
					pWL = pWL->Next();
				}

				// Print this line...
				float sx = rect.Left;
				float justifyadjust = 0.0f;
				switch (alignment)
				{
					case Alignment::Left:
					{
						break;
					}
					case Alignment::Center:
					{
						sx = pixelsLeft / 2.0f;
						break;
					}
					case Alignment::Right:
					{
						sx = maxw - (pixelsLeft + (wordsInLine > 1 ? spaceWidth : 0));
						break;
					}
					case Alignment::Justify:
					{
						justifyadjust = (wordsInLine > 1 && pWL != NULL) ? pixelsLeft / ((float)(wordsInLine - 1)) : 0.0f;
						break;
					}
					case Alignment::JustifyAlways:
					{
						justifyadjust = (wordsInLine > 1) ? pixelsLeft / ((float)(wordsInLine - 1)) : 0.0f;
						break;
					}
				}

				while (print != NULL && wordsInLine-- > 0)
				{
					sx = floor(sx);

					for (int c = 0; c < print->Chars(); c++)
					{
						char ch = ((WCHAR*)print->Text())[c];
						if (ch >= 0x20 && ch <= 0x7f)
						{
							ch -= 0x20;
							// Create vertices and indexes for this character...
							float fx = pWidths[ch];

							float x1 = fcw * (float)ch;
							float x2 = x1 + fx / 3584.0f;// 5760.0f;
							float scaledFx = fx * pConfig->FontScale;

							// Create 6 vertices and 6 indexes per char
							pVB[cix].position = XMFLOAT3(sx, sy, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x1, y1);
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x2, y1);
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy - fh, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x2, y2);

							pVB[cix].position = XMFLOAT3(sx, sy, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x1, y1);
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy - fh, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x2, y2);
							pVB[cix].position = XMFLOAT3(sx, sy - fh, -1.5f);
							pVB[cix++].texture = XMFLOAT2(x1, y2);

							sx += scaledFx;
						}
					}

					sx += spaceWidth + justifyadjust;
					print = print->Next();
				}

				if (_width < (sx - spaceWidth - justifyadjust - rect.Left))
				{
					_width = sx - spaceWidth - justifyadjust - rect.Left;
				}

				sy -= TexFont.Height() * pConfig->FontScale;

				wordsInLine = 0;
				pixelsLeft = 0.0f;
			}

			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;
			vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 6 * _printableCharacters;
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = pVB;
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "Text");

			delete[] pVB;
		}
	}
}

float CDXText::PixelWidth(char* text)
{
	return TexFont.PixelWidth(text);
}

float CDXText::Width()
{
	return _width;
}

void CDXText::Width(float w)
{
	_width = w;
}

float CDXText::Height()
{
	return TexFont.Height() * _lines * pConfig->FontScale;
}

void CDXText::SetColours(int colour)
{
	_colour1 = _colour4 = 0;
	_colour2 = _colour3 = colour;
}

void CDXText::SetColours(int colour1, int colour2)
{
	_colour1 = _colour4 = 0;
	_colour2 = colour1;
	_colour3 = colour2;
}

void CDXText::SetColours(int colour1, int colour2, int colour3, int colour4)
{
	_colour1 = colour1;
	_colour2 = colour2;
	_colour3 = colour3;
	_colour4 = colour4;
}
