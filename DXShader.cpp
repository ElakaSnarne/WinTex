#include "DXShader.h"
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <d3dcompiler.h>
#else
#include "Win32Compat.h"
#endif
#include <iostream>
#include <string>

CDXShader::CDXShader(CDirectX* pDX, int resource, LPCSTR vsFunctionName, LPCSTR vsProfileName, LPCSTR psFunctionName, LPCSTR psProfileName, D3D11_INPUT_ELEMENT_DESC* ied, int numDescriptors)
{
	_vs = NULL;
	_ps = NULL;
	_layout = NULL;

#ifdef PLATFORM_LINUX
    std::string vsName = vsFunctionName;
    std::string psName = psFunctionName;
    
    const char* vsSource = NULL;
    const char* psSource = NULL;

    // Vertex Shaders
    const char* vsOrtho = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 World;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "void main() {\n"
        "   gl_Position = Projection * View * World * vec4(position, 1.0);\n"
        "   TexCoord = texCoord;\n"
        "}\n";

    const char* vsTextured = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "layout(location = 1) in vec2 texCoord;\n"
        "layout(location = 2) in vec2 object;\n"
        "layout(location = 3) in vec4 objectParameters;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 World;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "\n"
        "layout(std140) uniform Visibility {\n"
        "    vec4 visibility[4096];\n"
        "};\n"
        "layout(std140) uniform Translation {\n"
        "    vec4 translation[256];\n"
        "};\n"
        "\n"
        "void main() {\n"
        "   int objIdx = int(object.x);\n"
        "   int subObjIdx = int(object.y);\n"
        "\n"
        "   // Object Visibility Check\n"
        "   if (objIdx >= 0 && objIdx < 4096) {\n"
        "       if (visibility[objIdx].x <= 0.0) {\n"
        "           gl_Position = vec4(2.0, 2.0, 2.0, 1.0); return;\n"
        "       }\n"
        "   }\n"
        "\n"
        "   // SubObject Visibility Check\n"
        "   if (subObjIdx >= 0 && subObjIdx < 4096) {\n"
        "       if (visibility[subObjIdx].y <= 0.0) {\n"
        "           gl_Position = vec4(2.0, 2.0, 2.0, 1.0); return;\n"
        "       }\n"
        "   }\n"
        "\n"
        "   // Translation (using Object Index)\n"
        "   vec3 pos = position;\n"
        "   if (objIdx >= 0 && objIdx < 256) {\n"
        "       pos += translation[objIdx].xyz;\n"
        "   }\n"
        "\n"
        "   gl_Position = Projection * View * World * vec4(pos, 1.0);\n"
        "   TexCoord = texCoord;\n"
        "}\n";

    const char* vsColoured = 
        "#version 330 core\n"
        "layout(location = 0) in vec4 position;\n"
        "layout(location = 1) in vec4 colour;\n"
        "out vec4 Color;\n"
        "uniform mat4 World;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "void main() {\n"
        "   gl_Position = Projection * View * World * position;\n"
        "   Color = colour;\n"
        "}\n";

     const char* vsBasic = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 position;\n"
        "uniform mat4 World;\n"
        "uniform mat4 View;\n"
        "uniform mat4 Projection;\n"
        "void main() {\n"
        "   gl_Position = Projection * View * World * vec4(position, 1.0);\n"
        "}\n";

    // Pixel Shaders
    const char* psTextured = 
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D texture1;\n"
        "void main() {\n"
        "   color = texture(texture1, TexCoord);\n"
        "}\n";

    const char* psColoured = 
        "#version 330 core\n"
        "in vec4 Color;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   color = Color;\n"
        "}\n";

    const char* psTexFont = 
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 color;\n"
        "uniform sampler2D texture1;\n"
        "\n"
        "layout(std140) uniform TexFont {\n"
        "    vec4 colour1;\n"
        "    vec4 colour2;\n"
        "    vec4 colour3;\n"
        "    vec4 colour4;\n"
        "};\n"
        "\n"
        "void main() {\n"
        "   vec4 l0 = texture(texture1, vec2(TexCoord.x, TexCoord.y - 0.75));\n"
        "   vec4 l1 = texture(texture1, vec2(TexCoord.x, TexCoord.y - 0.50));\n"
        "   vec4 l2 = texture(texture1, vec2(TexCoord.x, TexCoord.y - 0.25));\n"
        "   vec4 l3 = texture(texture1, vec2(TexCoord.x, TexCoord.y));\n"
        "   color = l0.a * colour1 + l1.a * colour2 + l2.a * colour3 + l3.a * colour4;\n"
        "}\n";
    
    const char* psBasic = 
        "#version 330 core\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   color = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n";

    // Select Source
    if (vsName == "OrthoVS") vsSource = vsOrtho;
    else if (vsName == "TexturedVS") vsSource = vsTextured;
    else if (vsName == "ColouredVS" || vsName == "TransparentVS" || vsName == "MultiColouredFontVS") vsSource = vsColoured;
    else if (vsName == "BasicVS") vsSource = vsBasic;
    else vsSource = vsOrtho; // Fallback

    if (psName == "TexturedPS" || psName == "YUVPS") psSource = psTextured;
    else if (psName == "TexFontPS" || psName == "TexFontPS_AA") psSource = psTexFont;
    else if (psName == "ColouredPS" || psName == "TransparentPS" || psName == "MultiColouredFontPS" || psName == "MultiColouredFontPSPD") psSource = psColoured;
    else if (psName == "BasicPS") psSource = psBasic;
    else psSource = psTextured; // Fallback

    // Compile VS
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << vsName << "\n" << infoLog << std::endl;
    }

    // Compile PS
    GLuint ps = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(ps, 1, &psSource, NULL);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ps, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PIXEL::COMPILATION_FAILED: " << psName << "\n" << infoLog << std::endl;
    }

    // Link Program
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, ps);
    
    // Bind attributes explicitly to match our assumptions
    glBindAttribLocation(prog, 0, "position");
    if (vsSource == vsOrtho) glBindAttribLocation(prog, 1, "texCoord");
    else if (vsSource == vsTextured) {
        glBindAttribLocation(prog, 1, "texCoord");
        glBindAttribLocation(prog, 2, "object");
        glBindAttribLocation(prog, 3, "objectParameters");
    }
    else if (vsSource == vsColoured) glBindAttribLocation(prog, 1, "colour");
    
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(ps);
    
    // Set texture unit default and binding points for UBOs
    glUseProgram(prog);
    GLint texLoc = glGetUniformLocation(prog, "texture1");
    if (texLoc != -1) {
        glUniform1i(texLoc, 0);
    }

    if (vsSource == vsTextured) {
        GLuint visIdx = glGetUniformBlockIndex(prog, "Visibility");
        if (visIdx != GL_INVALID_INDEX) glUniformBlockBinding(prog, visIdx, 3);

        GLuint transIdx = glGetUniformBlockIndex(prog, "Translation");
        if (transIdx != GL_INVALID_INDEX) glUniformBlockBinding(prog, transIdx, 5);
    }

    if (psSource == psTexFont) {
        GLuint texFontIdx = glGetUniformBlockIndex(prog, "TexFont");
        if (texFontIdx != GL_INVALID_INDEX) glUniformBlockBinding(prog, texFontIdx, 4);
    }
    glUseProgram(0);

    // Store program ID in _vs (Vertex Shader object)
    pDX->GetDevice()->CreateVertexShader(NULL, 0, NULL, &_vs);
    _vs->glId = prog;

    // We don't really use separate PS in this simple GL implementation, 
    // we use the linked program stored in _vs.
    pDX->GetDevice()->CreatePixelShader(NULL, 0, NULL, &_ps);
    
    // Create dummy layout
    pDX->GetDevice()->CreateInputLayout(ied, numDescriptors, NULL, 0, &_layout);
#else
	HRSRC hShader = FindResource(NULL, MAKEINTRESOURCE(resource), L"SHADER");
	DWORD size = SizeofResource(NULL, hShader);
	HGLOBAL hShaderGlobal = LoadResource(NULL, hShader);
	char* pShader = (char*)LockResource(hShaderGlobal);

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// Create vertex shader
	ID3D10Blob* pVSb;
	D3DX11CompileFromMemory(pShader, size, "SHADER", NULL, NULL, vsFunctionName, vsProfileName, flags, 0, NULL, &pVSb, NULL, NULL);
	pDX->GetDevice()->CreateVertexShader(pVSb->GetBufferPointer(), pVSb->GetBufferSize(), NULL, &_vs);

	// Create input layout
	pDX->GetDevice()->CreateInputLayout(ied, numDescriptors, pVSb->GetBufferPointer(), pVSb->GetBufferSize(), &_layout);
	if (pVSb != NULL)pVSb->Release();
	pVSb = NULL;

	// Create pixel shader
	ID3D10Blob* pPSb;
	D3DX11CompileFromMemory(pShader, size, "SHADER", NULL, NULL, psFunctionName, psProfileName, 0, 0, NULL, &pPSb, NULL, NULL);
	pDX->GetDevice()->CreatePixelShader(pPSb->GetBufferPointer(), pPSb->GetBufferSize(), NULL, &_ps);
	if (pPSb != NULL) pPSb->Release();
	pPSb = NULL;
#endif
}

CDXShader::~CDXShader()
{
	Dispose();
}

void CDXShader::Activate(CDirectX* pDX)
{
#ifdef PLATFORM_LINUX
    glUseProgram(_vs->glId);
#else
	pDX->GetDeviceContext()->VSSetShader(_vs, 0, 0);
	pDX->GetDeviceContext()->PSSetShader(_ps, 0, 0);
	pDX->GetDeviceContext()->IASetInputLayout(_layout);
#endif
}

void CDXShader::Dispose()
{
	if (_vs != NULL)
	{
		_vs->Release();
		_vs = NULL;
	}

	if (_ps != NULL)
	{
		_ps->Release();
		_ps = NULL;
	}

	if (_layout != NULL)
	{
		_layout->Release();
		_layout = NULL;
	}
}
