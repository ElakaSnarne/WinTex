#pragma once
#include "Platform.h"

#ifdef PLATFORM_LINUX
#define GL_GLEXT_PROTOTYPES 1
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cmath>
#include <cwchar>
#include <iostream>
#include <codecvt>
#include <locale>
#include <thread>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <fcntl.h>
#include <filesystem>
namespace fs = std::filesystem;

extern std::string gDataPath;

inline std::string ResolvePath(std::string filename) {
    std::replace(filename.begin(), filename.end(), '\\', '/');
    if (filename.empty()) return filename;
    if (filename[0] == '/') return filename;
    if (filename.compare(0, 2, "./") == 0) return filename;

    static std::string dataRoot = "";
    static bool searched = false;

    if (!searched) {
        if (!gDataPath.empty()) {
            dataRoot = gDataPath;
        } else {
            dataRoot = ".";
        }
        searched = true;
    }

    // Don't prepend if it already starts with dataRoot
    if (dataRoot != "." && filename.compare(0, dataRoot.length(), dataRoot) == 0) {
        if (filename.length() == dataRoot.length() || filename[dataRoot.length()] == '/') {
            return filename;
        }
    }

    if (fs::exists(filename)) return filename;
    return dataRoot + "/" + filename;
}

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif
#include "stb_image.h"

// Windows Types
typedef int BOOL;
typedef uint8_t BYTE;
typedef unsigned char byte;
typedef BYTE* LPBYTE;
typedef BYTE* PBYTE;
typedef int* PINT;
typedef int* LPINT;
typedef uint16_t WORD;
typedef WORD* LPWORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef long long LONG_PTR;
typedef unsigned long long ULONG_PTR;
typedef DWORD* PDWORD;
typedef DWORD* LPDWORD;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef LONG* PLONG;
typedef void* HINSTANCE;
typedef void* HWND;
typedef void* HICON;
typedef char CHAR;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef const wchar_t* PCWSTR;
typedef wchar_t WCHAR;
typedef wchar_t* PWCHAR;
typedef wchar_t* PWSTR;
typedef const char* LPCTSTR;
typedef uint64_t ULONGLONG;
typedef long long WPARAM;
typedef long long LPARAM;
typedef unsigned int UINT;
typedef uint32_t UINT32;
typedef uint8_t UINT8;
typedef void* HANDLE;
typedef long HRESULT;
typedef int INT;
typedef float FLOAT;

#define TRUE 1
#define FALSE 0
#ifndef NULL
#define NULL 0
#endif
#define MAX_PATH 260
#define S_OK 0
#define E_FAIL -1
#define WINAPI
#define CALLBACK
#define INFINITE 0xFFFFFFFF
#define RGB(r,g,b) ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

struct POINT { LONG x; LONG y; };
typedef POINT* LPPOINT;
struct RECT { LONG left; LONG top; LONG right; LONG bottom; };
typedef RECT* LPRECT;

// Math Stubs
struct XMFLOAT2 { float x, y; XMFLOAT2(float _x=0, float _y=0): x(_x), y(_y){} };
struct XMFLOAT3 { float x, y, z; XMFLOAT3(float _x=0, float _y=0, float _z=0): x(_x), y(_y), z(_z){} };
struct XMFLOAT4 { 
    float x, y, z, w; 
    XMFLOAT4(float _x=0, float _y=0, float _z=0, float _w=0): x(_x), y(_y), z(_z), w(_w){} 
};

struct XMVECTOR { 
    union {
        struct { float x, y, z, w; };
        float m128_f32[4];
    };
    XMVECTOR(float _x=0, float _y=0, float _z=0, float _w=0): x(_x), y(_y), z(_z), w(_w) {
        m128_f32[0] = x; m128_f32[1] = y; m128_f32[2] = z; m128_f32[3] = w;
    } 
};

struct XMMATRIX { 
    float m[4][4]; 
    XMMATRIX() { 
        memset(m, 0, sizeof(m)); 
        m[0][0]=m[1][1]=m[2][2]=m[3][3]=1.0f; 
    } 
};

// Math Functions
inline XMMATRIX XMMatrixIdentity() { return XMMATRIX(); }

inline XMMATRIX XMMatrixTranslation(float x, float y, float z) {
    XMMATRIX M;
    M.m[3][0] = x;
    M.m[3][1] = y;
    M.m[3][2] = z;
    return M;
}

inline XMMATRIX XMMatrixScaling(float x, float y, float z) {
    XMMATRIX M;
    M.m[0][0] = x;
    M.m[1][1] = y;
    M.m[2][2] = z;
    return M;
}

inline XMMATRIX XMMatrixRotationZ(float angle) {
    XMMATRIX M;
    float c = cosf(angle);
    float s = sinf(angle);
    M.m[0][0] = c;  M.m[0][1] = s;
    M.m[1][0] = -s; M.m[1][1] = c;
    return M;
}

inline XMMATRIX XMMatrixRotationX(float angle) {
    XMMATRIX M;
    float c = cosf(angle);
    float s = sinf(angle);
    M.m[1][1] = c;  M.m[1][2] = s;
    M.m[2][1] = -s; M.m[2][2] = c;
    return M;
}

inline XMMATRIX XMMatrixRotationY(float angle) {
    XMMATRIX M;
    float c = cosf(angle);
    float s = sinf(angle);
    M.m[0][0] = c;  M.m[0][2] = -s;
    M.m[2][0] = s;  M.m[2][2] = c;
    return M;
}

inline XMVECTOR XMVectorSubtract(XMVECTOR a, XMVECTOR b) {
    return XMVECTOR(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline XMVECTOR XMVector3Normalize(XMVECTOR v) {
    float len = sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
    if (len > 0) return XMVECTOR(v.x/len, v.y/len, v.z/len, v.w);
    return v;
}

inline XMVECTOR XMVector3Cross(XMVECTOR a, XMVECTOR b) {
    return XMVECTOR(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x,
        0.0f
    );
}

inline XMVECTOR XMVector3Dot(XMVECTOR a, XMVECTOR b) {
    float d = a.x*b.x + a.y*b.y + a.z*b.z;
    return XMVECTOR(d,d,d,d);
}

inline XMMATRIX XMMatrixLookAtLH(XMVECTOR eye, XMVECTOR focus, XMVECTOR up) {
    XMVECTOR zaxis = XMVector3Normalize(XMVectorSubtract(focus, eye));
    XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(up, zaxis));
    XMVECTOR yaxis = XMVector3Cross(zaxis, xaxis);

    XMMATRIX M;
    M.m[0][0] = xaxis.x; M.m[0][1] = yaxis.x; M.m[0][2] = zaxis.x; M.m[0][3] = 0.0f;
    M.m[1][0] = xaxis.y; M.m[1][1] = yaxis.y; M.m[1][2] = zaxis.y; M.m[1][3] = 0.0f;
    M.m[2][0] = xaxis.z; M.m[2][1] = yaxis.z; M.m[2][2] = zaxis.z; M.m[2][3] = 0.0f;
    
    M.m[3][0] = -XMVector3Dot(xaxis, eye).x;
    M.m[3][1] = -XMVector3Dot(yaxis, eye).x;
    M.m[3][2] = -XMVector3Dot(zaxis, eye).x;
    M.m[3][3] = 1.0f;
    return M;
}

inline XMMATRIX XMMatrixOrthographicLH(float w, float h, float zn, float zf) {
    XMMATRIX M;
    M.m[0][0] = 2.0f / w;
    M.m[1][1] = 2.0f / h;
    // Map zn->-1, zf->1 for GL clip space
    M.m[2][2] = 2.0f / (zf - zn);
    M.m[3][2] = -(zf + zn) / (zf - zn);
    M.m[3][3] = 1.0f;
    return M;
}

inline XMMATRIX XMMatrixPerspectiveFovLH(float fov, float aspect, float zn, float zf) {
    XMMATRIX M;
    float h = 1.0f / tanf(fov * 0.5f);
    float w = h / aspect;
    
    M.m[0][0] = w;
    M.m[1][1] = h;
    // Map zn->-1, zf->1 for GL clip space
    M.m[2][2] = (zf + zn) / (zf - zn);
    M.m[2][3] = 1.0f;
    M.m[3][2] = -(2.0f * zf * zn) / (zf - zn);
    M.m[3][3] = 0.0f;
    return M;
}

inline XMMATRIX XMMatrixTranspose(XMMATRIX m) {
    // On Linux/GL, we use Row-Major structs to store Column-Major matrix data (layout match).
    // The "Transpose" call in ConstantBuffers.cpp was intended to convert DX Row-Major to Shader Column-Major.
    // Since our matrices are already in the correct memory layout for GL (when read as columns),
    // we disable this transpose to preserve the layout.
    return m;
}

inline XMVECTOR XMVectorSet(float x, float y, float z, float w) { return XMVECTOR(x,y,z,w); }

inline XMMATRIX operator*(const XMMATRIX& a, const XMMATRIX& b) {
    XMMATRIX R;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            R.m[i][j] = a.m[i][0]*b.m[0][j] +
                        a.m[i][1]*b.m[1][j] +
                        a.m[i][2]*b.m[2][j] +
                        a.m[i][3]*b.m[3][j];
        }
    }
    return R;
}

// D3D11 Stubs
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
template<typename T1, typename T2> inline auto min(T1 a, T2 b) { return (a < b) ? a : b; }
template<typename T1, typename T2> inline auto max(T1 a, T2 b) { return (a > b) ? a : b; }

// GUID
typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, *PLUID;
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID, IID;

struct D3D11_VIEWPORT {
    float TopLeftX;
    float TopLeftY;
    float Width;
    float Height;
    float MinDepth;
    float MaxDepth;
};

struct D3D11_RECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
};

// Forward Declarations
typedef struct ID3D11Device ID3D11Device;
typedef struct ID3D11DeviceContext ID3D11DeviceContext;
typedef struct ID3D11Resource ID3D11Resource;
typedef struct ID3D11Buffer ID3D11Buffer;
typedef struct ID3D11Texture2D ID3D11Texture2D;
typedef struct ID3D11VertexShader ID3D11VertexShader;
typedef struct ID3D11PixelShader ID3D11PixelShader;
typedef struct ID3D11InputLayout ID3D11InputLayout;
typedef struct ID3D11ClassLinkage ID3D11ClassLinkage;
typedef struct ID3D11DepthStencilState ID3D11DepthStencilState;
typedef struct ID3D11RasterizerState ID3D11RasterizerState;
typedef struct ID3D11BlendState ID3D11BlendState;
typedef struct ID3D11SamplerState ID3D11SamplerState;
typedef struct ID3D11ShaderResourceView ID3D11ShaderResourceView;
typedef struct ID3D11RenderTargetView ID3D11RenderTargetView;
typedef struct ID3D11DepthStencilView ID3D11DepthStencilView;

enum D3D11_INPUT_CLASSIFICATION {
    D3D11_INPUT_PER_VERTEX_DATA,
    D3D11_INPUT_PER_INSTANCE_DATA
};

enum DXGI_FORMAT { 
    DXGI_FORMAT_UNKNOWN,
    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_TYPELESS,
    DXGI_FORMAT_R32G32B32_FLOAT,
    DXGI_FORMAT_R32G32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_FLOAT
};

typedef struct D3D11_INPUT_ELEMENT_DESC {
    LPCSTR                     SemanticName;
    UINT                       SemanticIndex;
    DXGI_FORMAT                Format;
    UINT                       InputSlot;
    UINT                       AlignedByteOffset;
    D3D11_INPUT_CLASSIFICATION InputSlotClass;
    UINT                       InstanceDataStepRate;
} D3D11_INPUT_ELEMENT_DESC;

enum D3D11_MAP { D3D11_MAP_READ, D3D11_MAP_WRITE, D3D11_MAP_READ_WRITE, D3D11_MAP_WRITE_DISCARD, D3D11_MAP_WRITE_NO_OVERWRITE };
enum D3D11_USAGE { D3D11_USAGE_DEFAULT, D3D11_USAGE_IMMUTABLE, D3D11_USAGE_DYNAMIC, D3D11_USAGE_STAGING };

#define D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT 14
#define D3D11_APPEND_ALIGNED_ELEMENT 0xffffffff
enum D3D11_PRIMITIVE_TOPOLOGY { 
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
    D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
};

#define D3D11_CPU_ACCESS_READ 0x20000L
#define D3D11_BIND_VERTEX_BUFFER 0x1L
#define D3D11_BIND_SHADER_RESOURCE 0x8L
#define D3D11_BIND_CONSTANT_BUFFER 0x4L
#define D3D11_CPU_ACCESS_WRITE 0x10000L

typedef struct DXGI_SAMPLE_DESC {
    UINT Count;
    UINT Quality;
} DXGI_SAMPLE_DESC;

typedef struct D3D11_TEXTURE2D_DESC {
    UINT Width;
    UINT Height;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
} D3D11_TEXTURE2D_DESC;

typedef struct D3D11_BUFFER_DESC {
    UINT ByteWidth;
    UINT Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
    UINT StructureByteStride;
} D3D11_BUFFER_DESC;

typedef struct D3D11_SUBRESOURCE_DATA {
    const void *pSysMem;
    UINT SysMemPitch;
    UINT SysMemSlicePitch;
} D3D11_SUBRESOURCE_DATA;

typedef struct D3D11_MAPPED_SUBRESOURCE {
    void* pData;
    UINT RowPitch;
    UINT DepthPitch;
} D3D11_MAPPED_SUBRESOURCE;

typedef struct D3DX11_IMAGE_LOAD_INFO {
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT FirstMipLevel;
    UINT MipLevels;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CpuAccessFlags;
    UINT MiscFlags;
    DXGI_FORMAT Format;
    UINT Filter;
    UINT MipFilter;
    void *pSrcInfo;
} D3DX11_IMAGE_LOAD_INFO;

typedef struct ID3D11DeviceChild { virtual void Release() {} virtual void SetPrivateData(GUID guid, UINT dataSize, const void* pData) {} } ID3D11DeviceChild;
enum ResourceType { RT_Buffer, RT_Texture2D };
struct ID3D11Resource : public ID3D11DeviceChild { 
    GLuint glId = 0; 
    std::vector<BYTE> cpuData;
    UINT byteWidth = 0;
    UINT bindFlags = 0;
    UINT width = 0;
    UINT height = 0;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    ResourceType type;
};
struct ID3D11Buffer : public ID3D11Resource { ID3D11Buffer() { type = RT_Buffer; } };
struct ID3D11Texture2D : public ID3D11Resource { 
    ID3D11Texture2D() { type = RT_Texture2D; }
    void GetDesc(D3D11_TEXTURE2D_DESC *pDesc) {
        pDesc->Width = width;
        pDesc->Height = height;
        pDesc->Format = format;
    }
};
struct ID3D11VertexShader : public ID3D11DeviceChild {
    GLuint glId = 0;
    void* GetBufferPointer() { return NULL; }
    size_t GetBufferSize() { return 0; }
};
struct ID3D11PixelShader : public ID3D11DeviceChild { GLuint glId = 0; };
struct ID3D11InputLayout : public ID3D11DeviceChild { GLuint glId = 0; };
struct ID3D11DepthStencilState : public ID3D11DeviceChild {};
struct ID3D11RasterizerState : public ID3D11DeviceChild {};
struct ID3D11BlendState : public ID3D11DeviceChild {};
struct ID3D11SamplerState : public ID3D11DeviceChild {};

struct ID3D11ShaderResourceView : public ID3D11DeviceChild { GLuint glId = 0; };
struct ID3D11RenderTargetView : public ID3D11DeviceChild {};
struct ID3D11DepthStencilView : public ID3D11DeviceChild {};
typedef struct IDXGISwapChain IDXGISwapChain;

typedef struct D3D11_DEPTH_STENCIL_DESC {
    BOOL DepthEnable;
    UINT DepthWriteMask;
    UINT DepthFunc;
    BOOL StencilEnable;
    UINT8 StencilReadMask;
    UINT8 StencilWriteMask;
    struct {
        UINT StencilFailOp;
        UINT StencilDepthFailOp;
        UINT StencilPassOp;
        UINT StencilFunc;
    } FrontFace;
    struct {
        UINT StencilFailOp;
        UINT StencilDepthFailOp;
        UINT StencilPassOp;
        UINT StencilFunc;
    } BackFace;
} D3D11_DEPTH_STENCIL_DESC;

typedef struct D3D11_RASTERIZER_DESC {
    UINT FillMode;
    UINT CullMode;
    BOOL FrontCounterClockwise;
    INT DepthBias;
    FLOAT DepthBiasClamp;
    FLOAT SlopeScaledDepthBias;
    BOOL DepthClipEnable;
    BOOL ScissorEnable;
    BOOL MultisampleEnable;
    BOOL AntialiasedLineEnable;
} D3D11_RASTERIZER_DESC;

typedef struct D3D11_SAMPLER_DESC {
    UINT Filter;
    UINT AddressU;
    UINT AddressV;
    UINT AddressW;
    FLOAT MipLODBias;
    UINT MaxAnisotropy;
    UINT ComparisonFunc;
    FLOAT BorderColor[4];
    FLOAT MinLOD;
    FLOAT MaxLOD;
} D3D11_SAMPLER_DESC;

typedef struct D3D11_BLEND_DESC {
    BOOL AlphaToCoverageEnable;
    BOOL IndependentBlendEnable;
    struct {
        BOOL BlendEnable;
        UINT SrcBlend;
        UINT DestBlend;
        UINT BlendOp;
        UINT SrcBlendAlpha;
        UINT DestBlendAlpha;
        UINT BlendOpAlpha;
        UINT8 RenderTargetWriteMask;
    } RenderTarget[8];
} D3D11_BLEND_DESC;

typedef struct D3D11_DEPTH_STENCIL_VIEW_DESC {
    DXGI_FORMAT Format;
    UINT ViewDimension;
    UINT Flags;
    union {
        struct {
            UINT MipSlice;
        } Texture1D;
        struct {
            UINT MipSlice;
        } Texture1DArray;
        struct {
            UINT MipSlice;
        } Texture2D;
        struct {
            UINT MipSlice;
        } Texture2DArray;
        struct {
            UINT MipSlice;
        } Texture2DMS;
        struct {
            UINT MipSlice;
        } Texture2DMSArray;
    };
} D3D11_DEPTH_STENCIL_VIEW_DESC;

typedef struct D3D11_SHADER_RESOURCE_VIEW_DESC {
    DXGI_FORMAT Format;
    UINT ViewDimension;
    union {
        struct {
            UINT MostDetailedMip;
            UINT MipLevels;
        } Texture2D;
    };
} D3D11_SHADER_RESOURCE_VIEW_DESC;

struct ID3D11Device : public ID3D11DeviceChild {
    HRESULT CreateVertexShader(const void *pShaderBytecode, size_t BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11VertexShader **ppVertexShader) { *ppVertexShader = new ID3D11VertexShader(); return S_OK; }
    HRESULT CreatePixelShader(const void *pShaderBytecode, size_t BytecodeLength, ID3D11ClassLinkage *pClassLinkage, ID3D11PixelShader **ppPixelShader) { *ppPixelShader = new ID3D11PixelShader(); return S_OK; }
    HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, size_t BytecodeLength, ID3D11InputLayout **ppInputLayout) { *ppInputLayout = new ID3D11InputLayout(); return S_OK; }
    HRESULT CreateTexture2D(const D3D11_TEXTURE2D_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Texture2D **ppTexture2D) { 
        *ppTexture2D = new ID3D11Texture2D();
        (*ppTexture2D)->width = pDesc->Width;
        (*ppTexture2D)->height = pDesc->Height;
        (*ppTexture2D)->format = pDesc->Format;
        glGenTextures(1, &(*ppTexture2D)->glId);
        glBindTexture(GL_TEXTURE_2D, (*ppTexture2D)->glId);
        
        GLint internalFormat = GL_RGBA;
        GLenum format = GL_RGBA;
        if (pDesc->Format == DXGI_FORMAT_B8G8R8A8_UNORM) format = GL_BGRA;
        GLenum type = GL_UNSIGNED_BYTE;
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, pDesc->Width, pDesc->Height, 0, format, type, pInitialData ? pInitialData->pSysMem : NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return S_OK; 
    }
    HRESULT CreateBuffer(const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer) { *ppBuffer = new ID3D11Buffer();
        (*ppBuffer)->byteWidth = pDesc->ByteWidth;
        (*ppBuffer)->bindFlags = pDesc->BindFlags;
        (*ppBuffer)->cpuData.resize(pDesc->ByteWidth);

        if (pDesc->BindFlags & D3D11_BIND_CONSTANT_BUFFER) {
            // Check if this is a large buffer that needs UBO (Visibility/Translation/TexFont)
            bool createUBO = false;
            char* name = (char*)pInitialData; // Hack: pInitialData is repurposed as name in implementation calls sometimes, but not here.
            // Wait, CreateBuffer implementation in DirectX.cpp (Windows) takes name.
            // In Win32Compat.h, the signature is:
            // HRESULT CreateBuffer(const D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer)
            // It doesn't take name.
            // But CDirectX::CreateBuffer does take name.
            // The stubs in Win32Compat.h don't have name passed to them?
            // CDirectX::CreateBuffer in DirectX.cpp calls _dev->CreateBuffer(..., ppBuffer).
            // It then calls SetDebugName.
            // Win32Compat.h implementation of ID3D11Device::CreateBuffer doesn't know the name.
            
            // However, we can check byte width to guess?
            // VisibilityBufferType: 4096 * 16 = 65536 bytes
            // TranslationBufferType: 256 * 16 = 4096 bytes
            // TexFontBufferType: 4 * 16 = 64 bytes.
            // TexFont is small. It could be a normal uniform.
            // But if we want to use a block layout in shader, UBO is easier.
            // Or we can just use glUniform4fv in ApplyConstantBuffers if it's small.
            
            // Let's create UBO for all constant buffers to be safe/uniform?
            // Or just always create UBO for constant buffers.
            createUBO = true; 

            if (createUBO) {
                 glGenBuffers(1, &(*ppBuffer)->glId);
                 glBindBuffer(GL_UNIFORM_BUFFER, (*ppBuffer)->glId);
                 if (pInitialData) {
                     glBufferData(GL_UNIFORM_BUFFER, pDesc->ByteWidth, pInitialData->pSysMem, GL_DYNAMIC_DRAW);
                     memcpy((*ppBuffer)->cpuData.data(), pInitialData->pSysMem, pDesc->ByteWidth);
                 } else {
                     glBufferData(GL_UNIFORM_BUFFER, pDesc->ByteWidth, NULL, GL_DYNAMIC_DRAW);
                 }
                 glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        } else {
            // For Vertex/Index buffers
            glGenBuffers(1, &(*ppBuffer)->glId);
            GLenum target = (pDesc->BindFlags & D3D11_BIND_VERTEX_BUFFER) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
            glBindBuffer(target, (*ppBuffer)->glId);
            if (pInitialData) {
                glBufferData(target, pDesc->ByteWidth, pInitialData->pSysMem, (pDesc->Usage == D3D11_USAGE_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
                memcpy((*ppBuffer)->cpuData.data(), pInitialData->pSysMem, pDesc->ByteWidth);
            } else {
                glBufferData(target, pDesc->ByteWidth, NULL, (pDesc->Usage == D3D11_USAGE_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
            }
            glBindBuffer(target, 0);
        }
        return S_OK; 
    }
    HRESULT CreateDepthStencilView(ID3D11Resource *pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D11DepthStencilView **ppDepthStencilView) { *ppDepthStencilView = new ID3D11DepthStencilView(); return S_OK; }
    HRESULT CreateRenderTargetView(ID3D11Resource *pResource, const void *pDesc, ID3D11RenderTargetView **ppRTView) { *ppRTView = new ID3D11RenderTargetView(); return S_OK; }
    HRESULT CreateSamplerState(const D3D11_SAMPLER_DESC *pSamplerDesc, ID3D11SamplerState **ppSamplerState) { *ppSamplerState = new ID3D11SamplerState(); return S_OK; }
    HRESULT CreateBlendState(const D3D11_BLEND_DESC *pBlendStateDesc, ID3D11BlendState **ppBlendState) { *ppBlendState = new ID3D11BlendState(); return S_OK; }
    HRESULT CreateRasterizerState(const D3D11_RASTERIZER_DESC *pRasterizerDesc, ID3D11RasterizerState **ppRasterizerState) { *ppRasterizerState = new ID3D11RasterizerState(); return S_OK; }
    HRESULT CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D11DepthStencilState **ppDepthStencilState) { *ppDepthStencilState = new ID3D11DepthStencilState(); return S_OK; }
    HRESULT CreateShaderResourceView(ID3D11Resource *pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D11ShaderResourceView **ppSRView) { *ppSRView = new ID3D11ShaderResourceView(); return S_OK; }
    void CheckMultisampleQualityLevels(DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels) { *pNumQualityLevels = 1; }
};

struct ID3D11DeviceContext : public ID3D11DeviceChild {
    ID3D11Buffer* vsConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
    D3D11_PRIMITIVE_TOPOLOGY currentTopology;

    ID3D11DeviceContext() {
        memset(vsConstantBuffers, 0, sizeof(vsConstantBuffers));
        currentTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }

    HRESULT Map(ID3D11Resource *pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource) { 
        pMappedResource->RowPitch = 0;
        pMappedResource->DepthPitch = 0;
        pMappedResource->pData = NULL;
        
        if (pResource->type == RT_Buffer) {
            ID3D11Buffer* buf = (ID3D11Buffer*)pResource;
            if (buf->cpuData.size() < buf->byteWidth) buf->cpuData.resize(buf->byteWidth);
            pMappedResource->pData = buf->cpuData.data();
        } else if (pResource->type == RT_Texture2D) {
            ID3D11Texture2D* tex = (ID3D11Texture2D*)pResource;
            // Debug print
            // std::cout << "Mapping Texture: " << tex->width << "x" << tex->height << " Type: " << tex->type << std::endl;
            
            if (tex->width == 0 || tex->height == 0) return E_FAIL;

            pMappedResource->RowPitch = tex->width * 4; // Assume 4 bytes per pixel
            pMappedResource->DepthPitch = 0;
            size_t size = tex->height * pMappedResource->RowPitch;
            if (tex->cpuData.size() < size) tex->cpuData.resize(size);
            pMappedResource->pData = tex->cpuData.data();
        }
        return S_OK; 
    }
    void Unmap(ID3D11Resource *pResource, UINT Subresource) {
        if (pResource->type == RT_Buffer) {
            ID3D11Buffer* buf = (ID3D11Buffer*)pResource;
            if (buf->bindFlags & D3D11_BIND_VERTEX_BUFFER) {
                glBindBuffer(GL_ARRAY_BUFFER, buf->glId);
                glBufferSubData(GL_ARRAY_BUFFER, 0, buf->byteWidth, buf->cpuData.data());
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            } else if ((buf->bindFlags & D3D11_BIND_CONSTANT_BUFFER) && buf->glId != 0) {
                 // Update UBO data
                 glBindBuffer(GL_UNIFORM_BUFFER, buf->glId);
                 glBufferSubData(GL_UNIFORM_BUFFER, 0, buf->byteWidth, buf->cpuData.data());
                 glBindBuffer(GL_UNIFORM_BUFFER, 0);
            }
        } else if (pResource->type == RT_Texture2D) {
            ID3D11Texture2D* tex = (ID3D11Texture2D*)pResource;
            glBindTexture(GL_TEXTURE_2D, tex->glId);
            
            GLenum format = GL_RGBA;
            if (tex->format == DXGI_FORMAT_B8G8R8A8_UNORM) format = GL_BGRA;
            
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->width, tex->height, format, GL_UNSIGNED_BYTE, tex->cpuData.data());
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    void VSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
        for (UINT i = 0; i < NumBuffers; i++) {
            if (StartSlot + i < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) {
                vsConstantBuffers[StartSlot + i] = ppConstantBuffers[i];
            }
        }
    }

    void ApplyConstantBuffers() {
        GLint prog = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
        if (prog == 0) {
            static bool warnedProg = false;
            if (!warnedProg) { std::cerr << "Warning: No active GL program in ApplyConstantBuffers" << std::endl; warnedProg = true; }
            return;
        }

        static int frameCountDebug = 0;
        frameCountDebug++;
        bool debug = (frameCountDebug % 600 == 0); // Print occasionally

        for (UINT slot = 0; slot < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; slot++) {
            ID3D11Buffer* buf = vsConstantBuffers[slot];
            if (!buf) continue;

            if (slot == 0) { // VOP
                GLint locView = glGetUniformLocation(prog, "View");
                GLint locOrtho = glGetUniformLocation(prog, "Ortho"); 
                GLint locProj = glGetUniformLocation(prog, "Projection");
                
                // VOPBufferType structure: mat4 view(0), mat4 ortho(64), mat4 projection(128)
                if (locView != -1) {
                    glUniformMatrix4fv(locView, 1, GL_FALSE, (float*)(buf->cpuData.data()));
                    if (debug) {
                        float* m = (float*)(buf->cpuData.data());
                        //std::cerr << "ApplyCB: View Matrix [0][0]=" << m[0] << " [3][3]=" << m[15] << std::endl;
                    }
                }

                if (locOrtho != -1) {
                    glUniformMatrix4fv(locOrtho, 1, GL_FALSE, (float*)(buf->cpuData.data() + 64)); 
                    if (debug) {
                        float* m = (float*)(buf->cpuData.data() + 64);
                        //std::cerr << "ApplyCB: Ortho Matrix [0][0]=" << m[0] << " [3][3]=" << m[15] << std::endl;
                    }
                }

                if (locProj != -1) {
                    glUniformMatrix4fv(locProj, 1, GL_FALSE, (float*)(buf->cpuData.data() + 128)); 
                    if (debug) {
                        float* m = (float*)(buf->cpuData.data() + 128);
                        //std::cerr << "ApplyCB: Projection Matrix [0][0]=" << m[0] << " [3][3]=" << m[15] << std::endl;
                    }
                }
            } else if (slot == 1) { // World
                GLint locWorld = glGetUniformLocation(prog, "World");
                if (locWorld != -1) {
                    glUniformMatrix4fv(locWorld, 1, GL_FALSE, (float*)(buf->cpuData.data()));
                    if (debug) {
                        float* m = (float*)(buf->cpuData.data());
                        //std::cerr << "ApplyCB: World Matrix [0][0]=" << m[0] << " [3][0]=" << m[12] << " [3][1]=" << m[13] << std::endl;
                    }
                }
                else {
                    static bool warnedWorld = false;
                    if (!warnedWorld) { std::cerr << "Warning: Uniform 'World' not found in program " << prog << std::endl; warnedWorld = true; }
                }
            } else if (slot == 3) { // Visibility
                if (buf->glId != 0) {
                     glBindBufferBase(GL_UNIFORM_BUFFER, 3, buf->glId);
                }
            } else if (slot == 4) { // TexFont
                if (buf->glId != 0) {
                     glBindBufferBase(GL_UNIFORM_BUFFER, 4, buf->glId);
                }
            } else if (slot == 5) { // Translation
                if (buf->glId != 0) {
                     glBindBufferBase(GL_UNIFORM_BUFFER, 5, buf->glId);
                }
            }
        }
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
             std::cerr << "GL Error in ApplyConstantBuffers: " << err << std::endl;
        }
    }
    void PSSetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppConstantBuffers) {
        // PS might use them too, but for our simple shader PS only uses texture.
    }
    void VSSetShader(ID3D11VertexShader *pVertexShader, ID3D11ClassLinkage *const *ppClassInstances, UINT NumClassInstances) {
        if (pVertexShader) glUseProgram(pVertexShader->glId);
    }
    void PSSetShader(ID3D11PixelShader *pPixelShader, ID3D11ClassLinkage *const *ppClassInstances, UINT NumClassInstances) {}
    void IASetInputLayout(ID3D11InputLayout *pInputLayout) {}
    void IASetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets) {
        if (NumBuffers > 0 && ppVertexBuffers[0]) {
            glBindBuffer(GL_ARRAY_BUFFER, ppVertexBuffers[0]->glId);
            UINT stride = pStrides[0];
            glEnableVertexAttribArray(0); 
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
            glEnableVertexAttribArray(1); 
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)12);
        }
    }
    void IASetIndexBuffer(ID3D11Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset) {
        if (pIndexBuffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexBuffer->glId);
    }
    void IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) {
        currentTopology = Topology;
    }
    void Draw(UINT VertexCount, UINT StartVertexLocation) {
        ApplyConstantBuffers();
        GLenum mode = GL_TRIANGLES;
        if (currentTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP) mode = GL_TRIANGLE_STRIP;
        else if (currentTopology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP) mode = GL_LINE_STRIP;
        
        glDrawArrays(mode, StartVertexLocation, VertexCount);
    }
    void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) {
        ApplyConstantBuffers();
        GLenum mode = GL_TRIANGLES;
        if (currentTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP) mode = GL_TRIANGLE_STRIP;
        else if (currentTopology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP) mode = GL_LINE_STRIP;

        glDrawElements(mode, IndexCount, GL_UNSIGNED_INT, (void*)(intptr_t)(StartIndexLocation * 4)); // Offset in bytes
    }
    void PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *const *ppShaderResourceViews) {
        if (NumViews > 0 && ppShaderResourceViews[0]) {
            glActiveTexture(GL_TEXTURE0 + StartSlot);
            glBindTexture(GL_TEXTURE_2D, ppShaderResourceViews[0]->glId);
        }
    }
    void PSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState *const *ppSamplers) {}
    void OMSetRenderTargets(UINT NumViews, ID3D11RenderTargetView *const *ppRenderTargetViews, ID3D11DepthStencilView *pDepthStencilView) {}
    void OMSetDepthStencilState(ID3D11DepthStencilState *pDepthStencilState, UINT StencilRef) {}
    void OMSetBlendState(ID3D11BlendState *pBlendState, const FLOAT BlendFactor[4], UINT SampleMask) {}
    void RSSetViewports(UINT NumViewports, const D3D11_VIEWPORT *pViewports) {
        if (NumViewports > 0) {
            glViewport((GLint)pViewports[0].TopLeftX, (GLint)pViewports[0].TopLeftY, (GLsizei)pViewports[0].Width, (GLsizei)pViewports[0].Height);
        }
    }
    void RSSetScissorRects(UINT NumRects, const D3D11_RECT *pRects) {
        if (NumRects > 0) {
            glScissor(pRects[0].left, pRects[0].top, pRects[0].right - pRects[0].left, pRects[0].bottom - pRects[0].top);
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }
    void RSSetState(ID3D11RasterizerState *pRasterizerState) {}
    void ClearRenderTargetView(ID3D11RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[4]) {
        glClearColor(ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void ClearDepthStencilView(ID3D11DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil) {
        glClearDepth(Depth);
        glClearStencil(Stencil);
        GLbitfield mask = 0;
        if (ClearFlags & 1) mask |= GL_DEPTH_BUFFER_BIT; // D3D11_CLEAR_DEPTH
        if (ClearFlags & 2) mask |= GL_STENCIL_BUFFER_BIT; // D3D11_CLEAR_STENCIL
        if (mask) glClear(mask);
    }
    void Flush() {}
};

static const GUID WKPDID_D3DDebugObjectName = { 0x4299e9, 0x3bea, 0x45d5, 0x8d, 0x44, 0x73, 0x54, 0x37, 0x6f, 0x74, 0x36 };

typedef void ID3DX11ThreadPump;
inline HRESULT D3DX11CreateTextureFromMemory(ID3D11Device* pDevice, const void* pSrcData, size_t SrcDataSize, D3DX11_IMAGE_LOAD_INFO* pLoadInfo, ID3DX11ThreadPump* pPump, ID3D11Resource** ppTexture, HRESULT* pHResult) { 
    if (pSrcData == NULL) return E_FAIL;
    ID3D11Texture2D* tex = new ID3D11Texture2D();
    *ppTexture = tex;
    
    int w, h, n;
    unsigned char *data = stbi_load_from_memory((const stbi_uc*)pSrcData, (int)SrcDataSize, &w, &h, &n, 4);
    
    if (data) {
        std::cerr << "D3DX11CreateTextureFromMemory: Loaded image " << w << "x" << h << " channels=" << n << std::endl;
        tex->width = w;
        tex->height = h;
        tex->format = DXGI_FORMAT_R8G8B8A8_UNORM;

        glGenTextures(1, &tex->glId);
        glBindTexture(GL_TEXTURE_2D, tex->glId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        tex->cpuData.resize(w * h * 4);
        memcpy(tex->cpuData.data(), data, w * h * 4);

        stbi_image_free(data);
    } else {
        std::cerr << "D3DX11CreateTextureFromMemory: Failed to load image. Reason: " << stbi_failure_reason() << std::endl;
        tex->width = 2;
        tex->height = 2;
        tex->format = DXGI_FORMAT_R8G8B8A8_UNORM;
        glGenTextures(1, &tex->glId);
        glBindTexture(GL_TEXTURE_2D, tex->glId);
        unsigned char pixels[] = { 
            255, 0, 0, 255, 255, 0, 0, 255,
            255, 0, 0, 255, 255, 0, 0, 255
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    return S_OK; 
}

inline HRESULT D3DX11CreateTextureFromFile(ID3D11Device* pDevice, LPCWSTR pSrcFile, D3DX11_IMAGE_LOAD_INFO* pLoadInfo, ID3DX11ThreadPump* pPump, ID3D11Resource** ppTexture, HRESULT* pHResult) { 
    if (pSrcFile == NULL) return E_FAIL;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string filename = converter.to_bytes(pSrcFile);

    ID3D11Texture2D* tex = new ID3D11Texture2D();
    *ppTexture = tex;
    
    int w, h, n;
    unsigned char *data = stbi_load(filename.c_str(), &w, &h, &n, 4);
    
    if (data) {
        tex->width = w;
        tex->height = h;
        tex->format = DXGI_FORMAT_R8G8B8A8_UNORM;

        glGenTextures(1, &tex->glId);
        glBindTexture(GL_TEXTURE_2D, tex->glId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        tex->cpuData.resize(w * h * 4);
        memcpy(tex->cpuData.data(), data, w * h * 4);

        stbi_image_free(data);
    } else {
        std::cerr << "D3DX11CreateTextureFromFile: Failed to load image: " << filename << std::endl;
        tex->width = 2;
        tex->height = 2;
        tex->format = DXGI_FORMAT_R8G8B8A8_UNORM;
        glGenTextures(1, &tex->glId);
        glBindTexture(GL_TEXTURE_2D, tex->glId);
        unsigned char pixels[] = { 
            255, 0, 0, 255, 255, 0, 0, 255,
            255, 0, 0, 255, 255, 0, 0, 255
        };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    return S_OK; 
}

struct IUnknown {
    virtual HRESULT QueryInterface(const IID &riid, void **ppvObject) { return S_OK; }
    virtual ULONG AddRef() { return 1; }
    virtual ULONG Release() { return 0; }
};

// DXGI Stubs
struct DXGI_ADAPTER_DESC {
    WCHAR Description[128];
    UINT VendorId;
    UINT DeviceId;
    UINT SubSysId;
    UINT Revision;
    size_t DedicatedVideoMemory;
    size_t DedicatedSystemMemory;
    size_t SharedSystemMemory;
    LUID AdapterLuid;
};

struct DXGI_MODE_DESC {
    UINT Width;
    UINT Height;
    // ... other fields if needed
};

struct IDXGIOutput : public IUnknown {
    virtual HRESULT GetDisplayModeList(DXGI_FORMAT EnumFormat, UINT Flags, UINT *pNumModes, DXGI_MODE_DESC *pDesc) { *pNumModes = 0; return S_OK; }
};

struct IDXGIAdapter : public IUnknown {
    virtual HRESULT GetDesc(DXGI_ADAPTER_DESC *pDesc) { return S_OK; }
    virtual HRESULT EnumOutputs(UINT Output, IDXGIOutput **ppOutput) { *ppOutput = new IDXGIOutput(); return S_OK; }
};

// Memory Functions
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define CopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define FillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))

// Other Windows Functions Stubs
inline int GetSystemMetrics(int) { return 1920; } // Dummy
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

inline void OutputDebugString(LPCWSTR) {}
inline void MessageBox(HWND, LPCWSTR lpText, LPCWSTR lpCaption, UINT) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string text = converter.to_bytes(lpText);
    std::string caption = converter.to_bytes(lpCaption);
    std::cerr << "MessageBox [" << caption << "]: " << text << std::endl;
}
#define MB_OK 0

inline ULONGLONG GetTickCount64() { return SDL_GetTicks64(); }
inline DWORD GetTickCount() { return SDL_GetTicks(); }
inline void Sleep(DWORD dwMilliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(dwMilliseconds)); }

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
inline HANDLE CreateThread(LPVOID lpThreadAttributes, size_t dwStackSize, DWORD (WINAPI *lpStartAddress)(LPVOID), LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) { 
    try {
        std::thread* t = new std::thread([lpStartAddress, lpParameter](){
            lpStartAddress(lpParameter);
        });
        if (lpThreadId) *lpThreadId = 0; // Dummy ID
        return (HANDLE)t;
    } catch (...) {
        return INVALID_HANDLE_VALUE;
    }
}
inline DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) { 
    if (hHandle != INVALID_HANDLE_VALUE && hHandle != NULL) {
        std::thread* t = (std::thread*)hHandle;
        if (t->joinable()) t->join();
    }
    return 0; 
}

inline int MultiByteToWideChar(UINT, DWORD, const char*, int, LPWSTR, int) { return 0; }
#define CP_UTF8 65001

#define XM_PI 3.141592654f
#define XM_2PI 6.283185307f
#define XM_PIDIV2 1.570796327f
#define XM_PIDIV4 0.785398163f

// COM / XAudio2 Stubs
#define STDMETHOD_(type, name) virtual type name
#define WAVE_FORMAT_PCM 1

#define XAUDIO2_DEFAULT_FREQ_RATIO 2.0f
typedef struct tWAVEFORMATEX {
  WORD  wFormatTag;
  WORD  nChannels;
  DWORD nSamplesPerSec;
  DWORD nAvgBytesPerSec;
  WORD  nBlockAlign;
  WORD  wBitsPerSample;
  WORD  cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;

struct IXAudio2VoiceCallback {
    virtual void OnVoiceProcessingPassStart(UINT32) {}
    virtual void OnVoiceProcessingPassEnd() {}
    virtual void OnStreamEnd() {}
    virtual void OnBufferStart(void*) {}
    virtual void OnBufferEnd(void*) {}
    virtual void OnLoopEnd(void*) {}
    virtual void OnVoiceError(void*, HRESULT) {}
};

typedef struct XAUDIO2_BUFFER {
  UINT32     Flags;
  UINT32     AudioBytes;
  const BYTE *pAudioData;
  UINT32     PlayBegin;
  UINT32     PlayLength;
  UINT32     LoopBegin;
  UINT32     LoopLength;
  UINT32     LoopCount;
  void       *pContext;
} XAUDIO2_BUFFER;

#define XAUDIO2_LOOP_INFINITE 255

typedef struct IXAudio2Voice IXAudio2Voice;

struct IXAudio2SourceVoice {
    virtual HRESULT SubmitSourceBuffer(const XAUDIO2_BUFFER *pBuffer, const void *pBufferWMA = NULL) { return S_OK; }
    virtual HRESULT Stop(UINT32 Flags = 0, UINT32 OperationSet = 0) { return S_OK; }
    virtual HRESULT Start(UINT32 Flags = 0, UINT32 OperationSet = 0) { return S_OK; }
    virtual void DestroyVoice() {}
    virtual HRESULT SetVolume(float Volume, UINT32 OperationSet = 0) { return S_OK; }
    virtual HRESULT SetOutputMatrix(IXAudio2Voice *pDestinationVoice, UINT32 SourceChannels, UINT32 DestinationChannels, const float *pLevelMatrix, UINT32 OperationSet = 0) { return S_OK; }
    virtual HRESULT FlushSourceBuffers() { return S_OK; }
};

struct IXAudio2MasteringVoice {
    virtual void SetVolume(float Volume, UINT32 OperationSet = 0) {}
    virtual void DestroyVoice() {}
};

typedef struct IXAudio2 : public IUnknown {
    virtual HRESULT CreateMasteringVoice(IXAudio2MasteringVoice **ppMasteringVoice, UINT32 InputChannels = 0, UINT32 InputSampleRate = 0, UINT32 Flags = 0, UINT32 DeviceIndex = 0, const void *pEffectChain = NULL) { 
        *ppMasteringVoice = new IXAudio2MasteringVoice(); // Stub leak
        return S_OK; 
    }
    virtual HRESULT CreateSourceVoice(IXAudio2SourceVoice **ppSourceVoice, const WAVEFORMATEX *pSourceFormat, UINT32 Flags = 0, float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO, IXAudio2VoiceCallback *pCallback = NULL, const void *pSendList = NULL, const void *pEffectChain = NULL) {
        *ppSourceVoice = new IXAudio2SourceVoice(); // Stub leak
        return S_OK;
    }
} IXAudio2;

inline HRESULT XAudio2Create(IXAudio2 **ppXAudio2, UINT32 Flags = 0, UINT32 XAudio2Processor = 0) {
    *ppXAudio2 = new IXAudio2();
    return S_OK;
}

// MIDI Stubs
typedef HANDLE HMIDIOUT;
typedef HANDLE LPHMIDIOUT;
typedef UINT MMVERSION;
typedef UINT MMRESULT;
typedef ULONG_PTR DWORD_PTR;

#define MAXPNAMELEN 32
#define MMSYSERR_NOERROR 0
#define CALLBACK_NULL 0x00000000l

typedef struct {
    WORD      wMid;
    WORD      wPid;
    MMVERSION vDriverVersion;
    CHAR      szPname[MAXPNAMELEN];
    WORD      wTechnology;
    WORD      wVoices;
    WORD      wNotes;
    WORD      wChannelMask;
    DWORD     dwSupport;
} MIDIOUTCAPSA;

typedef struct midihdr_tag {
    LPSTR       lpData;
    DWORD       dwBufferLength;
    DWORD       dwBytesRecorded;
    DWORD_PTR   dwUser;
    DWORD       dwFlags;
    struct midihdr_tag *lpNext;
    DWORD_PTR   reserved;
    DWORD       dwOffset;
    DWORD_PTR   dwReserved[8];
} MIDIHDR, *PMIDIHDR, *NPMIDIHDR, *LPMIDIHDR;

inline int midiOutGetNumDevs() { return 0; }
inline int midiOutGetDevCapsA(int uDeviceID, MIDIOUTCAPSA* lpMidiOutCaps, UINT cbMidiOutCaps) { return 0; }
inline MMRESULT midiOutClose(HMIDIOUT hmo) { return MMSYSERR_NOERROR; }
inline MMRESULT midiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen) { 
    if (phmo) *((HMIDIOUT*)phmo) = (HMIDIOUT)(uintptr_t)1;
    return MMSYSERR_NOERROR; 
}
inline MMRESULT midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg) { return MMSYSERR_NOERROR; }
inline MMRESULT midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh) { return MMSYSERR_NOERROR; }
inline MMRESULT midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh) { return MMSYSERR_NOERROR; }
inline MMRESULT midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh) { return MMSYSERR_NOERROR; }

// File Stubs
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004  
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5
#define FILE_ATTRIBUTE_NORMAL               0x00000080  
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

#define FILE_ATTRIBUTE_DIRECTORY 0x00000010

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[260];
    WCHAR cAlternateFileName[14];
} WIN32_FIND_DATAW, WIN32_FIND_DATA, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  long long QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

inline HANDLE CreateFile(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, void* lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) { 
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string filename = ResolvePath(converter.to_bytes(lpFileName));

    if (dwCreationDisposition == CREATE_ALWAYS || dwCreationDisposition == CREATE_NEW || dwCreationDisposition == OPEN_ALWAYS) {
        size_t lastSlash = filename.find_last_of('/');
        if (lastSlash != std::string::npos) {
            std::string dir = filename.substr(0, lastSlash);
            if (!dir.empty() && !fs::exists(dir)) {
                try { fs::create_directories(dir); } catch(...) {}
            }
        }
    }

    int flags = 0;
    if ((dwDesiredAccess & GENERIC_READ) && (dwDesiredAccess & GENERIC_WRITE)) flags = O_RDWR;
    else if (dwDesiredAccess & GENERIC_WRITE) flags = O_WRONLY;
    else flags = O_RDONLY;

    if (dwCreationDisposition == CREATE_ALWAYS) flags |= O_CREAT | O_TRUNC;
    else if (dwCreationDisposition == CREATE_NEW) flags |= O_CREAT | O_EXCL;
    else if (dwCreationDisposition == OPEN_ALWAYS) flags |= O_CREAT;
    else if (dwCreationDisposition == TRUNCATE_EXISTING) flags |= O_TRUNC;

    int fd = open(filename.c_str(), flags, 0666);
    if (fd == -1) return INVALID_HANDLE_VALUE;
    return (HANDLE)(intptr_t)fd;
}
inline BOOL CloseHandle(HANDLE hObject) { 
    if (hObject == INVALID_HANDLE_VALUE || hObject == NULL) return FALSE;
    if ((uintptr_t)hObject < 65536) {
        close((int)(intptr_t)hObject);
        return TRUE;
    }
    return TRUE; 
}
inline BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, void* lpOverlapped) { 
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;
    ssize_t bytes = read((int)(intptr_t)hFile, lpBuffer, nNumberOfBytesToRead);
    if (bytes == -1) return FALSE;
    if (lpNumberOfBytesRead) *lpNumberOfBytesRead = (DWORD)bytes;
    return TRUE; 
}
inline BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, void* lpOverlapped) { 
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;
    ssize_t bytes = write((int)(intptr_t)hFile, lpBuffer, nNumberOfBytesToWrite);
    if (bytes == -1) return FALSE;
    if (lpNumberOfBytesWritten) *lpNumberOfBytesWritten = (DWORD)bytes;
    return TRUE; 
}
inline DWORD SetFilePointer(HANDLE hFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod) { 
    if (hFile == INVALID_HANDLE_VALUE) return 0xFFFFFFFF;
    int whence = SEEK_SET;
    if (dwMoveMethod == FILE_CURRENT) whence = SEEK_CUR;
    else if (dwMoveMethod == FILE_END) whence = SEEK_END;
    off_t res = lseek((int)(intptr_t)hFile, lDistanceToMove, whence);
    return (DWORD)res;
}
struct FindData {
    DIR* dir;
    std::string pattern;
    std::string directory;
};

inline BOOL FindNextFile(HANDLE hFindFile, WIN32_FIND_DATAW* lpFindFileData);

inline HANDLE FindFirstFile(LPCWSTR lpFileName, WIN32_FIND_DATAW* lpFindFileData) { 
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string fullPattern = ResolvePath(converter.to_bytes(lpFileName));

    size_t lastSlash = fullPattern.find_last_of('/');
    std::string directory = (lastSlash == std::string::npos) ? "." : fullPattern.substr(0, lastSlash);
    std::string pattern = (lastSlash == std::string::npos) ? fullPattern : fullPattern.substr(lastSlash + 1);

    DIR* dir = opendir(directory.c_str());
    if (!dir) return INVALID_HANDLE_VALUE;

    FindData* fd = new FindData{ dir, pattern, directory };
    
    if (FindNextFile((HANDLE)fd, lpFindFileData)) {
        return (HANDLE)fd;
    }

    closedir(dir);
    delete fd;
    return INVALID_HANDLE_VALUE;
}

inline BOOL FindNextFile(HANDLE hFindFile, WIN32_FIND_DATAW* lpFindFileData) { 
    if (hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL) return FALSE;
    FindData* fd = (FindData*)hFindFile;

    struct dirent* entry;
    while ((entry = readdir(fd->dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        // Simple wildcard match for *.* or *
        bool match = false;
        if (fd->pattern == "*.*" || fd->pattern == "*") match = true;
        else {
            // Very basic matching: check if name contains pattern or matches exactly
            if (name == fd->pattern) match = true;
        }

        if (match) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring wname = converter.from_bytes(name);
            wcsncpy(lpFindFileData->cFileName, wname.c_str(), 260);
            
            // Get file info
            std::string fullPath = fd->directory + "/" + name;
            struct stat st;
            if (stat(fullPath.c_str(), &st) == 0) {
                lpFindFileData->dwFileAttributes = S_ISDIR(st.st_mode) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
                lpFindFileData->nFileSizeLow = (DWORD)st.st_size;
                lpFindFileData->nFileSizeHigh = (DWORD)(st.st_size >> 32);
            } else {
                lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
                lpFindFileData->nFileSizeLow = 0;
                lpFindFileData->nFileSizeHigh = 0;
            }
            return TRUE;
        }
    }
    return FALSE; 
}

inline BOOL FindClose(HANDLE hFindFile) { 
    if (hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL) return FALSE;
    FindData* fd = (FindData*)hFindFile;
    closedir(fd->dir);
    delete fd;
    return TRUE; 
}
inline BOOL DeleteFile(LPCWSTR lpFileName) { 
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string filename = ResolvePath(converter.to_bytes(lpFileName));
    return unlink(filename.c_str()) == 0;
}

inline BOOL GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize) { 
    if (hFile == INVALID_HANDLE_VALUE) return FALSE;
    struct stat st;
    if (fstat((int)(intptr_t)hFile, &st) == 0) {
        lpFileSize->QuadPart = st.st_size;
        return TRUE;
    }
    return FALSE; 
}
inline DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh) { 
    LARGE_INTEGER li;
    if (GetFileSizeEx(hFile, &li)) {
        if (lpFileSizeHigh) *lpFileSizeHigh = li.u.HighPart;
        return li.u.LowPart;
    }
    return 0xFFFFFFFF; 
}

// Resource Stubs
typedef void* HMODULE;
typedef void* HRSRC;
typedef void* HGLOBAL;
#define MAKEINTRESOURCE(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
inline HRSRC FindResource(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType) { return NULL; }
inline DWORD SizeofResource(HMODULE hModule, HRSRC hResInfo) { return 0; }
inline HGLOBAL LoadResource(HMODULE hModule, HRSRC hResInfo) { return NULL; }
inline void* LockResource(HGLOBAL hResData) { return NULL; }

// D3DCompiler Stubs
#define D3DCOMPILE_ENABLE_STRICTNESS 0
#define D3DCOMPILE_DEBUG 0
typedef struct ID3D10Blob : public ID3D11DeviceChild {
    void* GetBufferPointer() { return NULL; }
    size_t GetBufferSize() { return 0; }
} ID3D10Blob;
inline HRESULT D3DX11CompileFromMemory(const char* pSrcData, size_t SrcDataLen, const char* pFileName, const void* pDefines, void* pInclude, const char* pFunctionName, const char* pProfile, UINT Flags1, UINT Flags2, void* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult) {
    *ppShader = new ID3D10Blob();
    return S_OK;
}

// Registry Stubs
#define RRF_RT_REG_BINARY 8
#define ERROR_SUCCESS 0
#define PVOID void*
#define REG_BINARY 3
#define RRF_RT_REG_DWORD 0x00000010
#define RRF_RT_REG_SZ 0x00000002
#define REG_SZ 1
typedef void* HKEY;
#define HKEY_CURRENT_USER ((HKEY)0x80000001)
#define KEY_READ 0x20019
#define KEY_WRITE 0x20006
#define REG_DWORD 4
inline std::map<std::string, std::string>& GetRegistryMap() {
    static std::map<std::string, std::string> reg;
    static bool loaded = false;
    if (!loaded) {
        std::ifstream f("registry.txt");
        std::string line;
        while (std::getline(f, line)) {
            size_t sep = line.find('=');
            if (sep != std::string::npos) reg[line.substr(0, sep)] = line.substr(sep + 1);
        }
        loaded = true;
    }
    return reg;
}

inline void SaveRegistry() {
    std::ofstream f("registry.txt");
    for (auto const& [key, val] : GetRegistryMap()) {
        f << key << "=" << val << "\n";
    }
}

inline LONG RegOpenKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, DWORD samDesired, HKEY *phkResult) { 
    GetRegistryMap();
    *phkResult = (HKEY)0x1234;
    return 0; 
}
inline LONG RegCreateKeyEx(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, DWORD samDesired, void* lpSecurityAttributes, HKEY *phkResult, DWORD *lpdwDisposition) { 
    GetRegistryMap();
    *phkResult = (HKEY)0x1234;
    return 0; 
}
inline LONG RegSetValueEx(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) { 
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string name = converter.to_bytes(lpValueName);
    if (dwType == REG_DWORD) {
        GetRegistryMap()[name] = std::to_string(*(DWORD*)lpData);
    } else if (dwType == REG_SZ) {
        GetRegistryMap()[name] = converter.to_bytes((LPCWSTR)lpData);
    }
    SaveRegistry();
    return 0; 
}
inline LONG RegCloseKey(HKEY hKey) { return 0; }

inline LONG RegGetValue(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) { 
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string name = converter.to_bytes(lpValue);
    auto& reg = GetRegistryMap();
    if (reg.count(name)) {
        std::string val = reg[name];
        if (dwFlags & RRF_RT_REG_DWORD) {
            if (pdwType) *pdwType = REG_DWORD;
            if (pvData) *(DWORD*)pvData = (DWORD)std::stoul(val);
            if (pcbData) *pcbData = 4;
            return 0;
        } else if (dwFlags & RRF_RT_REG_SZ) {
            std::wstring wval = converter.from_bytes(val);
            DWORD len = (DWORD)((wval.length() + 1) * sizeof(WCHAR));
            if (pvData) {
                if (*pcbData >= len) {
                    memcpy(pvData, wval.c_str(), len);
                } else {
                    *pcbData = len;
                    return 234; // MORE_DATA
                }
            }
            if (pcbData) *pcbData = len;
            if (pdwType) *pdwType = REG_SZ;
            return 0;
        }
    }
    return 1; 
}

// DirectInput Stubs
typedef struct IDirectInput8 IDirectInput8;
typedef struct IDirectInputDevice8 IDirectInputDevice8;
typedef struct DIDEVICEOBJECTINSTANCE DIDEVICEOBJECTINSTANCE;
typedef struct DIDEVICEINSTANCE DIDEVICEINSTANCE;
typedef struct DIJOYSTATE2 {
    LONG lX;
    LONG lY;
    LONG lZ;
    LONG lRx;
    LONG lRy;
    LONG lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[128];
    LONG lVX;
    LONG lVY;
    LONG lVZ;
    LONG lVRx;
    LONG lVRy;
    LONG lVRz;
    LONG rglVSlider[2];
    LONG lAX;
    LONG lAY;
    LONG lAZ;
    LONG lARx;
    LONG lARy;
    LONG lARz;
    LONG rglASlider[2];
    LONG lFX;
    LONG lFY;
    LONG lFZ;
    LONG lFRx;
    LONG lFRy;
    LONG lFRz;
    LONG rglFSlider[2];
} DIJOYSTATE2;

#define WM_USER 0x0400
#define WM_CLOSE 0x0010
#define WS_CAPTION 0x00C00000L

#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_PRIOR 0x21
#define VK_NEXT 0x22
#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_PAUSE 0x13
#define VK_CAPITAL 0x14
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_HELP 0x2F

#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B

#define VK_NUMPAD0 0x60
#define VK_NUMPAD1 0x61
#define VK_NUMPAD2 0x62
#define VK_NUMPAD3 0x63
#define VK_NUMPAD4 0x64
#define VK_NUMPAD5 0x65
#define VK_NUMPAD6 0x66
#define VK_NUMPAD7 0x67
#define VK_NUMPAD8 0x68
#define VK_NUMPAD9 0x69
#define VK_MULTIPLY 0x6A
#define VK_ADD 0x6B
#define VK_SUBTRACT 0x6D
#define VK_DECIMAL 0x6E
#define VK_DIVIDE 0x6F

inline BOOL PostThreadMessage(DWORD idThread, UINT Msg, WPARAM wParam, LPARAM lParam) { 
    if (Msg == WM_CLOSE) {
        SDL_Event event;
        event.type = SDL_QUIT;
        event.quit.timestamp = SDL_GetTicks();
        SDL_PushEvent(&event);
    }
    return TRUE; 
}
inline BOOL AdjustWindowRect(RECT* lpRect, DWORD dwStyle, BOOL bMenu) { return TRUE; }
inline BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint) { return TRUE; }
inline BOOL ClientToScreen(HWND hWnd, POINT* lpPoint) { return TRUE; }
inline BOOL SetCursorPos(int X, int Y) { 
    SDL_Window* window = SDL_GL_GetCurrentWindow();
    if (window) SDL_WarpMouseInWindow(window, X, Y);
    return TRUE; 
}
inline int _wtoi(const wchar_t *str) { return wcstol(str, NULL, 10); }
inline BOOL GetKeyboardState(PBYTE lpKeyState) { return FALSE; }
inline int ToAscii(UINT uVirtKey, UINT uScanCode, const BYTE* lpKeyState, LPWORD lpChar, UINT uFlags) { return 0; }

inline BOOL GetClientRect(HWND hWnd, LPRECT lpRect) { return TRUE; }
inline BOOL GetClipCursor(LPRECT lpRect) { return TRUE; }
inline BOOL ClipCursor(const RECT* lpRect) { return TRUE; }
inline HWND GetForegroundWindow() { return NULL; }
inline int ShowCursor(BOOL bShow) { 
    return SDL_ShowCursor(bShow ? SDL_ENABLE : SDL_DISABLE);
}
inline void PostQuitMessage(int nExitCode) {}

inline int GetKeyNameTextA(LONG lParam, LPSTR lpString, int nSize) { return 0; }

inline BOOL GetCursorPos(LPPOINT lpPoint) { return TRUE; }
inline BOOL ScreenToClient(HWND hWnd, LPPOINT lpPoint) { return TRUE; }

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

inline void GetLocalTime(LPSYSTEMTIME lpSystemTime) {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    if (tm) {
        lpSystemTime->wYear = tm->tm_year + 1900;
        lpSystemTime->wMonth = tm->tm_mon + 1;
        lpSystemTime->wDayOfWeek = tm->tm_wday;
        lpSystemTime->wDay = tm->tm_mday;
        lpSystemTime->wHour = tm->tm_hour;
        lpSystemTime->wMinute = tm->tm_min;
        lpSystemTime->wSecond = tm->tm_sec;
        lpSystemTime->wMilliseconds = 0;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
inline void _itoa_s(int value, char *buffer, size_t sizeInCharacters, int radix) {
    if (radix == 10) snprintf(buffer, sizeInCharacters, "%d", value);
    else if (radix == 16) snprintf(buffer, sizeInCharacters, "%x", value);
}
template <size_t size>
inline void _itoa_s(int value, char (&buffer)[size], int radix) {
    _itoa_s(value, buffer, size, radix);
}
inline void _gcvt_s(char *buffer, size_t sizeInBytes, double value, int digits) {
    snprintf(buffer, sizeInBytes, "%.*g", digits, value);
}

inline int sprintf_s(char *buffer, size_t sizeOfBuffer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(buffer, sizeOfBuffer, format, args);
    va_end(args);
    return ret;
}

namespace DirectX {
    using ::XMVECTOR;
    using ::XMMATRIX;
    using ::XMFLOAT2;
    using ::XMFLOAT3;
    using ::XMFLOAT4;
    // Add TriangleTests stub if needed, Location.cpp uses DirectX::TriangleTests::Intersects
    namespace TriangleTests {
        inline bool Intersects(XMVECTOR origin, XMVECTOR direction, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float& dist) { 
            XMVECTOR e1 = XMVectorSubtract(v1, v0);
            XMVECTOR e2 = XMVectorSubtract(v2, v0);
            XMVECTOR p = XMVector3Cross(direction, e2);
            float det = XMVector3Dot(e1, p).x;

            if (det > -0.000001f && det < 0.000001f) return false;
            float invDet = 1.0f / det;

            XMVECTOR t = XMVectorSubtract(origin, v0);
            float u = XMVector3Dot(t, p).x * invDet;
            if (u < 0.0f || u > 1.0f) return false;

            XMVECTOR q = XMVector3Cross(t, e1);
            float v = XMVector3Dot(direction, q).x * invDet;
            if (v < 0.0f || u + v > 1.0f) return false;

            dist = XMVector3Dot(e2, q).x * invDet;
            if (dist > 0.000001f) return true;

            return false;
        }
    }
}

#endif
