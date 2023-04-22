#include "AnimBase.h"
#include "Utilities.h"
#include "Globals.h"

CAnimBase::CAnimBase()
{
	_pInputBuffer = NULL;
	_pVideoOutputBuffer = NULL;
	_inputBufferLength = 0;
	_videoFramePointer = 0;
	_audioFramePointer = 0;
	_pPalette = NULL;

	_width = 0;
	_height = 0;
	_rate = 0;
	_frameTime = 0;
	_lastFrameUpdate = 0;
	_frame = 0;

	_vertexBuffer = NULL;

	_sourceVoice = NULL;
	_remainingAudioLength = 0;

	_audioFramesQueued = 0;
	_audioFramesProcessed = 0;
	_videoFramesProcessed = 0;

	_framePointer = 0;
	_done = FALSE;

	for (int i = 0; i < 64; i++)
	{
		_colourTranslationTable[i] = (BYTE)(4.04762 * i);
	}
}

CAnimBase::~CAnimBase()
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}

	if (_pInputBuffer != NULL)
	{
		delete[] _pInputBuffer;
		_pInputBuffer = NULL;
	}

	if (_pPalette != NULL)
	{
		delete[] _pPalette;
		_pPalette = NULL;
	}

	if (_pVideoOutputBuffer != NULL)
	{
		delete[] _pVideoOutputBuffer;
		_pVideoOutputBuffer = NULL;
	}

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}
}

BOOL CAnimBase::Init(LPBYTE pData, int length)
{
	_pInputBuffer = pData;
	_inputBufferLength = length;
	_screenWidth = dx.GetWidth();
	_screenHeight = dx.GetHeight();
	_pPalette = new int[256];
	ZeroMemory(_pPalette, sizeof(int) * 256);

	return TRUE;
}

void CAnimBase::CreateBuffers(int width, int height, int factor)
{
	if (width > 0 && height > 0)
	{
		int bufferHeight = (((height + 7) / 8) * 8);

		_pVideoOutputBuffer = new BYTE[width * bufferHeight];
		ZeroMemory(_pVideoOutputBuffer, width * bufferHeight);

		// Setup vertex buffer, keep aspect ratio
		float sx = (float)_screenWidth / (float)(width * factor);
		float sy = (float)_screenHeight / (float)(height * factor);
		float scale = min(sx, sy);
		float sw = width * scale;
		float sh = height * scale;
		float ox = (_screenWidth - sw);
		float oy = (_screenHeight - sh);

		float left, right, top, bottom;

		left = floor((float)(ox / 2.0f)) + 0.5f;
		right = floor(left + sw) + 0.5f;
		top = floor((float)(-oy / 2.0f)) + 0.5f;
		bottom = floor(top - sh) + 0.5f;

		TEXTURED_VERTEX* vertices = new TEXTURED_VERTEX[4];
		if (vertices != NULL)
		{
			vertices[0].position = XMFLOAT3(right, top, 0.0f);
			vertices[0].texture = XMFLOAT2(1.0f, 0.0f);

			vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
			vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

			vertices[2].position = XMFLOAT3(left, top, 0.0f);
			vertices[2].texture = XMFLOAT2(0.0f, 0.0f);

			vertices[3].position = XMFLOAT3(left, bottom, 0.0f);
			vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 4;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer, "AnimBaseBuffer");

			delete[] vertices;
		}
	}
}

void CAnimBase::Render()
{
	if (_vertexBuffer != NULL)
	{
		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();

		XMMATRIX wm = XMMatrixIdentity();

		CShaders::SelectOrthoShader();
		CConstantBuffers::SetWorld(dx, &wm);

		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		dx.EnableZBuffer();
	}
}

BOOL CAnimBase::Init(BinaryData bd)
{
	return Init(bd.Data, bd.Length);
}

void CAnimBase::Update()
{
	if (!_done)
	{
		ULONGLONG tick = GetTickCount64();
		ULONGLONG diff = tick - _lastFrameUpdate;
		if (diff >= _frameTime)
		{
			_frame++;

			//wchar_t buffer[10];
			//OutputDebugString(L"Decoding video frame ");
			//OutputDebugString(_itow(_frame, buffer, 10));
			//OutputDebugString(L" @ ");
			//OutputDebugString(_itow(_videoFramePointer, buffer, 16));
			//OutputDebugString(L"\r\n");
			//if (DecodeVideoFrame())
			int test = _framePointer;
			//if (_lock.Lock())
			{
				if (DecodeFrame())
				{
					// Replace texture if new video frame is required (frame time has lapsed, video frame exists)
					ID3D11Texture2D* pTex = _texture.GetTexture();
					if (pTex != NULL)
					{
						D3D11_MAPPED_SUBRESOURCE subRes;
						if (SUCCEEDED(dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
						{
							int* pScr = (int*)subRes.pData;
							for (int y = 0; y < _height; y++)
							{
								for (int x = 0; x < _width; x++)
								{
									pScr[y * subRes.RowPitch / 4 + x] = _pPalette[_pVideoOutputBuffer[y * _width + x]];
								}
							}

							dx.Unmap(pTex, 0);
						}
						else
						{
							int debug = 0;
						}
					}
				}

				if (_lastFrameUpdate == 0) _lastFrameUpdate = diff - _frameTime;
				_lastFrameUpdate += _frameTime;

				//if (_framePointer >= test && _audioFramesProcessed >= _audioFramesQueued)
				//if (_framePointer == test && _audioFramesProcessed == _audioFramesQueued)
				if ((_framePointer == 0 || _framePointer >= _inputBufferLength) && _audioFramesProcessed == _audioFramesQueued)
				{
					_done = TRUE;
				}

				//_lock.Release();
			}
		}
	}
}

void CAnimBase::Skip()
{
	// Any audio must be stopped
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_audioFramesProcessed = _audioFramesQueued;
	}
	_audioBuffers.clear();
	_done = TRUE;
}

void CAnimBase::Resize(int width, int height)
{
	float factor = 1.0f;

	if (width > 0 && height > 0)
	{
		if (_vertexBuffer != NULL)
		{
			_vertexBuffer->Release();
			_vertexBuffer = NULL;
		}

		_screenWidth = width;
		_screenHeight = height;

		// Recreate vertex buffer
		float sx = (float)_screenWidth / (float)(_width * factor);
		float sy = (float)_screenHeight / (float)(_height * factor);
		float scale = min(sx, sy);
		float sw = _width * scale;
		float sh = _height * scale;
		float ox = (_screenWidth - sw);
		float oy = (_screenHeight - sh);

		float left, right, top, bottom;

		left = floor((float)(ox / 2.0f)) + 0.5f;
		right = floor(left + sw) + 0.5f;
		top = floor((float)(-oy / 2.0f)) + 0.5f;
		bottom = floor(top - sh) + 0.5f;

		TEXTURED_VERTEX* vertices = new TEXTURED_VERTEX[4];
		if (vertices != NULL)
		{
			vertices[0].position = XMFLOAT3(right, top, 0.0f);
			vertices[0].texture = XMFLOAT2(1.0f, 0.0f);

			vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
			vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

			vertices[2].position = XMFLOAT3(left, top, 0.0f);
			vertices[2].texture = XMFLOAT2(0.0f, 0.0f);

			vertices[3].position = XMFLOAT3(left, bottom, 0.0f);
			vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 4;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer, "AnimBaseBuffer");

			delete[] vertices;
		}
	}
}
