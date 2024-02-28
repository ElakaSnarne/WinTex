#include "DXFont.h"
#include "Globals.h"
#include "File.h"

CDXFont::CDXFont() : CTexture::CTexture()
{
}

CDXFont::CDXFont(PBYTE pFont, DWORD size) : CTexture::CTexture()
{
	Init(pFont, size);
}

void CDXFont::Init(PBYTE pFont, DWORD size)
{
	if (CTexture::Init(pFont, size, "FONT"))
	{
		// Load same texture again, this time to a CPU accessible texture, lock data and examine...
		D3DX11_IMAGE_LOAD_INFO li;
		ZeroMemory(&li, sizeof(li));
		li.MipLevels = 1;
		li.Usage = D3D11_USAGE_STAGING;
		li.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
		li.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		ID3D11Texture2D* pTempTexture;

		if (SUCCEEDED(D3DX11CreateTextureFromMemory(dx.GetDevice(), pFont, size, &li, NULL, (ID3D11Resource**)&pTempTexture, NULL)))
		{
			D3D11_TEXTURE2D_DESC desc;
			pTempTexture->GetDesc(&desc);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			dx.Map(pTempTexture, 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedResource);

			// Select font size based on screen

			int w = dx.GetWidth();
			int scanHeight = desc.Height / 4;	// TexFont has 4 layers
			_y1 = 0.75f;
			_y2 = 1.0f;

			int offset = 0;
			int tallest = 0;
			int charwidth = 16;
			int charheight = 16;
			LPBYTE pData = (LPBYTE)mappedResource.pData;
			for (int c = 0; c < 224; c++)
			{
				int fp = charheight;
				int lp = 0;
				int cw = -1;

				// Find width of character
				for (int y = 0; y < charheight; y++)
				{
					for (int x = 0; x < charwidth; x++)
					{
						for (int l = 0; l < 4; l++)
						{
							int r = pData[offset + ((y + l * charheight) * desc.Width + x) * 4 + 0];
							int g = pData[offset + ((y + l * charheight) * desc.Width + x) * 4 + 1];
							int b = pData[offset + ((y + l * charheight) * desc.Width + x) * 4 + 2];
							int a = pData[offset + ((y + l * charheight) * desc.Width + x) * 4 + 3];

							if (a > 0)
							{
								if (x > cw)
								{
									cw = x;
								}
								if (y < fp)
								{
									fp = y;
								}
								if (y > lp)
								{
									lp = y;
								}
							}
						}
					}
				}

				if (cw < 0) cw = 0;

				_widths[c] = (float)(cw + 1);

				if ((lp - fp) > tallest)
				{
					tallest = lp - fp;
				}

				offset += charwidth * 4;
			}

			_widths[0] = 5;	// Hardcoding space to be 5 pixels

			_height = tallest + 1.0f;
			_y1 = 0.75;
			_y2 = ((float)3 * charheight + _height) / (float)desc.Height;

			dx.Unmap(pTempTexture, 0);

			pTempTexture->Release();
			pTempTexture = NULL;
		}
	}
}

CDXFont::~CDXFont()
{
}

float CDXFont::PixelWidth(char* text)
{
	float pixels = 0.0f;

	while (*text)
	{
		char ch = *(text++);
		if (ch >= 0x20 && ch <= 0x7f)
		{
			ch -= 0x20;
			pixels += _widths[ch] * pConfig->FontScale;
		}
	}

	return pixels;
}

float CDXFont::PixelWidth(char ch)
{
	return _widths[ch] * pConfig->FontScale;
}

Size CDXFont::GetSize(char* text, float maxw)
{
	// TODO: Find the minimum width and height required when maximum width is set (maximum can be exceeded only if a single word will take more space)

	char* scan = text;
	char* start = scan;
	float pixels = 0.0f;
	float maxwordwidth = 0.0f;
	CDXText::CWordList wl;
	int lines = 1;

	while (TRUE)
	{
		char c = *(scan++);
		if (c == 0 || c == 0x20)
		{
			// Space or end of text, add word to list
			int len = static_cast<int>(scan - start - 1);
			if (len > 0)
			{
				wl.Add(start, len, pixels);

				if (pixels > maxwordwidth)
				{
					maxwordwidth = pixels;
				}
			}

			if (c == 0)
			{
				break;
			}

			start = scan;
			pixels = 0.0f;
		}
		else if (c > 0x20 && c <= 0x7f)
		{
			pixels += _widths[c - 0x20] * pConfig->FontScale;
		}
	}

	// Count number of lines required to fit text
	if (maxwordwidth > maxw)
	{
		maxw = maxwordwidth;
	}

	lines = 0;
	float maxLineWidth = 0.0f;
	float spaceWidth = _widths[0] * pConfig->FontScale;
	CDXText::CWordList* pWL = wl.Next();
	while (pWL != NULL)
	{
		float linePixels = 0.0f;
		float pixelsLeft = 0.0f;

		if (pWL->Pixels() > pixelsLeft)
		{
			// New line required
			lines++;
			linePixels = pWL->Pixels();
			pixelsLeft = maxw - linePixels;

			pWL = pWL->Next();
		}

		while (pWL != NULL && pixelsLeft >= (pWL->Pixels() + spaceWidth))	// Enough room for the next word?
		{
			float pix = pWL->Pixels() + spaceWidth;
			linePixels += pix;
			pixelsLeft -= pix;
			pWL = pWL->Next();
		}

		if (linePixels > maxLineWidth)
		{
			maxLineWidth = linePixels;
		}
	}

	if (maxLineWidth < maxw)
	{
		maxw = maxLineWidth;
	}

	Size sz;
	sz.Width = maxw;
	sz.Height = lines * _height;
	return sz;
}

std::list<PointUV> CDXFont::Create(LPCWSTR text, BOOL texFont)
{
	std::list<PointUV> points;

	float y = 0.0f;
	float x = 0.0f;
	float fcw = 1.0f / (texFont ? 224.0f : 96.0f);
	float v1 = TexFont.Y1();
	float v2 = TexFont.Y2();
	float fh = TexFont.Height() * pConfig->FontScale;
	float spaceWidth = _widths[0] * pConfig->FontScale;

	while (*text)
	{
		char c = (char)*text++;
		if (c == 0x20)
		{
			x += spaceWidth;
		}
		else if (c > 0x20 && c <= 0x7f)
		{
			c -= 0x20;

			float fx = _widths[c];
			float scaledFx = fx * pConfig->FontScale;

			float u1 = fcw * (float)c;
			float u2 = u1 + fx / (texFont ? 3584.0f : 5760.0f);

			PointUV pt;
			pt.x = floor(x);
			pt.y = 0.0f;
			pt.u = u1;
			pt.v = v1;
			points.push_back(pt);

			pt.x = floor(x + scaledFx);
			pt.y = 0.0f;
			pt.u = u2;
			pt.v = v1;
			points.push_back(pt);

			pt.x = floor(x + scaledFx);
			pt.y = fh;
			pt.u = u2;
			pt.v = v2;
			points.push_back(pt);

			pt.x = floor(x);
			pt.y = 0.0f;
			pt.u = u1;
			pt.v = v1;
			points.push_back(pt);

			pt.x = floor(x + scaledFx);
			pt.y = fh;
			pt.u = u2;
			pt.v = v2;
			points.push_back(pt);

			pt.x = floor(x);
			pt.y = fh;
			pt.u = u1;
			pt.v = v2;
			points.push_back(pt);

			x += scaledFx;
		}
	}

	return points;
}

void CDXFont::SelectRedFont()
{
	SelectFontColour(195, 0, 0, 255, 24, 0);
}

void CDXFont::SelectGreenFont()
{
	SelectFontColour(0, 195, 0, 36, 255, 0);
}

void CDXFont::SelectBlueFont()
{
	SelectFontColour(0, 150, 255, 0, 207, 255);
}

void CDXFont::SelectYellowFont()
{
	SelectFontColour(255, 199, 8, 255, 255, 4);
}

void CDXFont::SelectWhiteFont()
{
	SelectFontColour(255, 255, 255);
}

void CDXFont::SelectGreyFont()
{
	SelectFontColour(195, 195, 195);
}

void CDXFont::SelectBlackFont()
{
	SelectFontColour(0, 0, 0);
}

void CDXFont::SelectFontColour(int red, int green, int blue, float alpha)
{
	SelectFontColour(red, green, blue, red, green, blue, alpha);
}

void CDXFont::SelectFontColour(int red1, int green1, int blue1, int red2, int green2, int blue2, float alpha)
{
	XMVECTOR colour1 = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR colour2 = { ((float)red1) / 255.0f, ((float)green1) / 255.0f, ((float)blue1) / 255.0f, alpha };
	XMVECTOR colour3 = { ((float)red2) / 255.0f, ((float)green2) / 255.0f, ((float)blue2) / 255.0f, alpha };
	XMVECTOR colour4 = { 0.0f, 0.0f, 0.0f, 0.0f };
	CConstantBuffers::SetTexFont(dx, &colour1, &colour2, &colour3, &colour4);
}

void CDXFont::SelectFontColour(float red, float green, float blue, float alpha)
{
	SelectFontColour(red, green, blue, red, green, blue, alpha);
}

void CDXFont::SelectFontColour(float red1, float green1, float blue1, float red2, float green2, float blue2, float alpha)
{
	XMVECTOR colour1 = { 0.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR colour2 = { red1, green1, blue1, alpha };
	XMVECTOR colour3 = { red2, green2, blue2, alpha };
	XMVECTOR colour4 = { 1.0f, 1.0f, 1.0f, 0.0f };
	CConstantBuffers::SetTexFont(dx, &colour1, &colour2, &colour3, &colour4);
}

void CDXFont::SelectFontColour(int colour)
{
	int red = (colour >> 16) & 0xff;
	int green = (colour >> 8) & 0xff;
	int blue = colour & 0xff;
	int alpha = (colour >> 24) & 0xff;
	SelectFontColour(red, green, blue, ((float)alpha) / 255.0f);
}

void CDXFont::SelectFontColour(int colour1, int colour2)
{
	int red1 = (colour1 >> 16) & 0xff;
	int green1 = (colour1 >> 8) & 0xff;
	int blue1 = colour1 & 0xff;
	int alpha = (colour1 >> 24) & 0xff;
	int red2 = (colour2 >> 16) & 0xff;
	int green2 = (colour2 >> 8) & 0xff;
	int blue2 = colour2 & 0xff;
	SelectFontColour(red1, green1, blue1, red2, green2, blue2, ((float)alpha) / 255.0f);
}

void CDXFont::SelectFontColour(int colour1, int colour2, int colour3, int colour4)
{
	int red1 = (colour1 >> 16) & 0xff;
	int green1 = (colour1 >> 8) & 0xff;
	int blue1 = colour1 & 0xff;
	int alpha1 = (colour1 >> 24) & 0xff;
	int red2 = (colour2 >> 16) & 0xff;
	int green2 = (colour2 >> 8) & 0xff;
	int blue2 = colour2 & 0xff;
	int alpha2 = (colour2 >> 24) & 0xff;
	int red3 = (colour3 >> 16) & 0xff;
	int green3 = (colour3 >> 8) & 0xff;
	int blue3 = colour3 & 0xff;
	int alpha3 = (colour3 >> 24) & 0xff;
	int red4 = (colour4 >> 16) & 0xff;
	int green4 = (colour4 >> 8) & 0xff;
	int blue4 = colour4 & 0xff;
	int alpha4 = (colour4 >> 24) & 0xff;

	XMVECTOR xcolour1 = { ((float)red1) / 255.0f, ((float)green1) / 255.0f, ((float)blue1) / 255.0f, ((float)alpha1) / 255.0f };
	XMVECTOR xcolour2 = { ((float)red2) / 255.0f, ((float)green2) / 255.0f, ((float)blue2) / 255.0f, ((float)alpha2) / 255.0f };
	XMVECTOR xcolour3 = { ((float)red3) / 255.0f, ((float)green3) / 255.0f, ((float)blue3) / 255.0f, ((float)alpha3) / 255.0f };
	XMVECTOR xcolour4 = { ((float)red4) / 255.0f, ((float)green4) / 255.0f, ((float)blue4) / 255.0f, ((float)alpha4) / 255.0f };
	CConstantBuffers::SetTexFont(dx, &xcolour1, &xcolour2, &xcolour3, &xcolour4);
}

float CDXFont::Height()
{
	return _height;
}
