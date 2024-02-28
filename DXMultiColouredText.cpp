#include "DXMultiColouredText.h"
#include "Globals.h"

void CDXMultiColouredText::CMCWordList::Add(char* text, int chars, float pixels, int col)
{
	CMCWordList* pWL = new CMCWordList(text, chars, pixels, col);
	if (_next == NULL) _next = pWL;
	if (_last != NULL) _last->SetNext(pWL);
	_last = pWL;
}

CDXMultiColouredText::CMCWordList::CMCWordList(char* text, int chars, float pixels, int col) : CWordList(text, chars, pixels)
{
	Colour = col;
}

CDXMultiColouredText::CDXMultiColouredText()
{
}

CDXMultiColouredText::~CDXMultiColouredText()
{
}

void CDXMultiColouredText::SetColours(int colour1, int colour2, int colour3)
{
	_colour1 = colour1;
	_colour2 = colour2;
	_colour3 = colour3;
}

void CDXMultiColouredText::Render(float x, float y)
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(MULTICOLOURED_FONT_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* pRV = TexFont.GetTextureRV();
	XMMATRIX wm = XMMatrixTranslation(floor(x), -floor(y), 0.0f);

	CShaders::SelectMultiColouredFontShader();

	CDXFont::SelectFontColour(_colour1, _colour2, _colour3, _colour4);
	CConstantBuffers::SetWorld(dx, &wm);
	dx.SetShaderResources(0, 1, &pRV);
	dx.Draw(_printableCharacters * 6, 0);
}

void CDXMultiColouredText::SetText(LPCWSTR text, Rect rect)
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	_lines = 0;
	CMCWordList wl;
	LPCWSTR scan = text;
	LPCWSTR start = scan;
	float pixels = 0.0f;
	_printableCharacters = 0;

	float* pWidths = TexFont.Widths();

	int currentCol = _colour1;

	while (TRUE)
	{
		auto c = *(scan++);
		if (c == 0) break;
		else if (c == 0x20 || c == '<' || c == '~' || c == '>' || c == '@')
		{
			// Space, add word to list
			auto len = scan - start - 1;
			if (len > 0)
			{
				wl.Add((char*)start, static_cast<int>(len), pixels, currentCol);
			}
			start = scan;
			pixels = 0.0f;

			if (c == '<')
			{
				currentCol = _colour2;
			}
			else if (c == '~')
			{
				currentCol = _colour3;
			}
			else if (c == '>' || c == '@')
			{
				currentCol = _colour1;
			}
		}
		else if (c > 0x20 && c <= 0x7f)
		{
			pixels += ((float)pWidths[c - 0x20]) * pConfig->FontScale;
			_printableCharacters++;
		}
	}

	// Add last word to list
	auto len = scan - start - 1;
	if (len > 0)
	{
		wl.Add((char*)start, static_cast<int>(len), pixels, currentCol);
	}

	if (_printableCharacters > 0)
	{
		MULTICOLOURED_FONT_VERTEX* pVB = new MULTICOLOURED_FONT_VERTEX[6 * _printableCharacters];
		if (pVB != NULL)
		{
			// Calculate how many lines are required
			_lines = 0;
			CMCWordList* pWL = (CMCWordList*)wl.Next();
			float maxw = rect.Right - rect.Left;
			float pixelsLeft = 0.0f;
			CMCWordList* print = NULL;
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
					pWL = (CMCWordList*)pWL->Next();
					wordsInLine = 1;
				}

				while (pWL != NULL && (pixelsLeft - (pWL->Pixels() + pWidths[0])) > -0.01)	// Enough room for the next word?
				{
					wordsInLine++;
					pixelsLeft -= pWL->Pixels() + pWidths[0];
					pWL = (CMCWordList*)pWL->Next();
				}

				// Print this line...
				float sx = rect.Left;

				while (print != NULL && wordsInLine-- > 0)
				{
					float r = print->Red();
					float g = print->Green();
					float b = print->Blue();
					XMFLOAT4 col = { r, g, b, 1.0f };

					sx = floor(sx);
					for (int c = 0; c < print->Chars(); c++)
					{
						auto ch = ((WCHAR*)print->Text())[c];
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
							pVB[cix].texture = XMFLOAT2(x1, y1);
							pVB[cix++].colour = col;
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy, -1.5f);
							pVB[cix].texture = XMFLOAT2(x2, y1);
							pVB[cix++].colour = col;
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy - fh, -1.5f);
							pVB[cix].texture = XMFLOAT2(x2, y2);
							pVB[cix++].colour = col;

							pVB[cix].position = XMFLOAT3(sx, sy, -1.5f);
							pVB[cix].texture = XMFLOAT2(x1, y1);
							pVB[cix++].colour = col;
							pVB[cix].position = XMFLOAT3(sx + scaledFx, sy - fh, -1.5f);
							pVB[cix].texture = XMFLOAT2(x2, y2);
							pVB[cix++].colour = col;
							pVB[cix].position = XMFLOAT3(sx, sy - fh, -1.5f);
							pVB[cix].texture = XMFLOAT2(x1, y2);
							pVB[cix++].colour = col;

							sx += scaledFx;
						}
					}

					sx += pWidths[0];
					print = (CMCWordList*)print->Next();
				}

				if (_width < (sx - pWidths[0] - rect.Left))
				{
					_width = sx - pWidths[0] - rect.Left;
				}

				sy -= TexFont.Height() * pConfig->FontScale;

				wordsInLine = 0;
				pixelsLeft = 0.0f;
			}

			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;
			vbDesc.ByteWidth = sizeof(MULTICOLOURED_FONT_VERTEX) * 6 * _printableCharacters;
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = pVB;
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "MultiColouredText");

			delete[] pVB;
		}
	}
}
