cbuffer VPBuffer : register(b0)
{
	matrix viewMatrix;
	matrix orthoMatrix;
	matrix projectionMatrix;
};

cbuffer WorldBuffer : register(b1)
{
	matrix worldMatrix;
};

cbuffer VisibilityBuffer : register(b3)
{
	float4 visibility[4096];
};

cbuffer TexFontBuffer : register(b4)
{
	float4 colour1;
	float4 colour2;
	float4 colour3;
	float4 colour4;
};

cbuffer TranslationBuffer : register(b5)
{
	float4 translation[256];
};

Texture2D shaderTexture : register(t0);
SamplerState AASampleType : register(s0);
SamplerState SampleType : register(s1);

struct OrthoVertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct TexturedVertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float2 objectIndex : TEXCOORD1;
	float4 objectParameters : TEXCOORD2;
};

struct TexturedPixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 objectIndex : TEXCOORD1;
	float4 objectParameters : TEXCOORD2;
};

TexturedPixelInputType OrthoVS(OrthoVertexInputType input)
{
	TexturedPixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, orthoMatrix);
	output.tex = input.tex;
	output.objectIndex.y = -1.0f;
	output.objectParameters = float4(0,0,0,0);
	return output;
}

TexturedPixelInputType TexturedVS(TexturedVertexInputType input)
{
	TexturedPixelInputType output;
	input.position.x += translation[input.objectIndex.x].x;
	input.position.y += translation[input.objectIndex.x].y;
	input.position.z += translation[input.objectIndex.x].z;
	input.position.w = min(visibility[input.objectIndex.x].x, visibility[input.objectIndex.y].y);
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = input.tex;
	output.objectIndex = input.objectIndex;
	output.objectParameters = input.objectParameters;
	return output;
}

float4 TexturedPS(TexturedPixelInputType input) : SV_TARGET
{
	float4 c = shaderTexture.Sample(AASampleType, input.tex);

	if (input.objectParameters.x > 0.0f && c.a < 0.25f)
	{
		discard;
	}

	return c;
}

// Font shader

float4 TexFontPS(TexturedPixelInputType input) : SV_TARGET
{
	float2 tt1 = float2(input.tex.x, input.tex.y);
	float2 tt2 = float2(input.tex.x, input.tex.y - 0.25f);
	float2 tt3 = float2(input.tex.x, input.tex.y - 0.5f);
	float2 tt4 = float2(input.tex.x, input.tex.y - 0.75f);
	float4 textureColor1 = shaderTexture.Sample(SampleType, tt1);
	float4 textureColor2 = shaderTexture.Sample(SampleType, tt2);
	float4 textureColor3 = shaderTexture.Sample(SampleType, tt3);
	float4 textureColor4 = shaderTexture.Sample(SampleType, tt4);

	float4 ret;
	ret.a = saturate(textureColor1.a * colour1.a + textureColor2.a * colour2.a + textureColor3.a * colour3.a + textureColor4.a * colour4.a);
	ret.r = saturate(textureColor1.r * colour1.r + textureColor2.r * colour2.r + textureColor3.r * colour3.r + textureColor4.r * colour4.r);
	ret.g = saturate(textureColor1.g * colour1.g + textureColor2.g * colour2.g + textureColor3.g * colour3.g + textureColor4.g * colour4.g);
	ret.b = saturate(textureColor1.b * colour1.b + textureColor2.b * colour2.b + textureColor3.b * colour3.b + textureColor4.b * colour4.b);

	return ret;
}

float4 TexFontPS_AA(TexturedPixelInputType input) : SV_TARGET
{
	float2 tt1 = float2(input.tex.x, input.tex.y);
	float2 tt2 = float2(input.tex.x, input.tex.y - 0.25f);
	float2 tt3 = float2(input.tex.x, input.tex.y - 0.5f);
	float2 tt4 = float2(input.tex.x, input.tex.y - 0.75f);
	float4 textureColor1 = shaderTexture.Sample(AASampleType, tt1);
	float4 textureColor2 = shaderTexture.Sample(AASampleType, tt2);
	float4 textureColor3 = shaderTexture.Sample(AASampleType, tt3);
	float4 textureColor4 = shaderTexture.Sample(AASampleType, tt4);

	float4 ret;
	ret.a = saturate(textureColor1.a * colour1.a + textureColor2.a * colour2.a + textureColor3.a * colour3.a + textureColor4.a * colour4.a);
	ret.r = saturate(textureColor1.r * colour1.r + textureColor2.r * colour2.r + textureColor3.r * colour3.r + textureColor4.r * colour4.r);
	ret.g = saturate(textureColor1.g * colour1.g + textureColor2.g * colour2.g + textureColor3.g * colour3.g + textureColor4.g * colour4.g);
	ret.b = saturate(textureColor1.b * colour1.b + textureColor2.b * colour2.b + textureColor3.b * colour3.b + textureColor4.b * colour4.b);

	return ret;
}

// Colour shader

struct ColouredVertexInputType
{
	float4 position : POSITION;
	float4 colour : COLOR;
};

struct ColouredPixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

ColouredPixelInputType ColouredVS(ColouredVertexInputType input)
{
	ColouredPixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, orthoMatrix);
	output.colour = input.colour;
	return output;
}

float4 ColouredPS(ColouredPixelInputType input) : SV_TARGET
{
	return input.colour;
}

// Transparent objects shader

struct TransparentVertexInputType
{
	float4 position : POSITION;
	float4 colour : COLOR;
	float4 objectIndex : TEXCOORD0;
};

struct TransparentPixelInputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

TransparentPixelInputType TransparentVS(TransparentVertexInputType input)
{
	TransparentPixelInputType output;
	input.position.x += translation[input.objectIndex.x].x;
	input.position.y += translation[input.objectIndex.x].y;
	input.position.z += translation[input.objectIndex.x].z;
	input.position.w = min(visibility[input.objectIndex.x].x, visibility[input.objectIndex.y].y);
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.colour = input.colour;
	return output;
}

float4 TransparentPS(TransparentPixelInputType input) : SV_TARGET
{
	return input.colour;
}

// Multicoloured font shader (for use in the hints module)

struct MultiColouredVertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 colour : COLOR;
};

struct MultiColouredPixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 colour : COLOR;
};

MultiColouredPixelInputType MultiColouredFontVS(MultiColouredVertexInputType input)
{
	MultiColouredPixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, orthoMatrix);
	output.tex = input.tex;
	output.colour = input.colour;
	return output;
}

float4 MultiColouredFontPS(MultiColouredPixelInputType input) : SV_TARGET
{
	float2 tt1 = float2(input.tex.x, input.tex.y);
	float2 tt2 = float2(input.tex.x, input.tex.y - 0.25f);
	float2 tt3 = float2(input.tex.x, input.tex.y - 0.5f);
	float2 tt4 = float2(input.tex.x, input.tex.y - 0.75f);
	float4 textureColor1 = shaderTexture.Sample(SampleType, tt1);
	float4 textureColor2 = shaderTexture.Sample(SampleType, tt2);
	float4 textureColor3 = shaderTexture.Sample(SampleType, tt3);
	float4 textureColor4 = shaderTexture.Sample(SampleType, tt4);

	float4 c1 = float4(0.59, 0.59, 0.59, 1.0);	// Shade
	float4 c2 = input.colour;					// Main
	float4 c3 = float4(0.78, 0.78, 0.78, 1.0);	// Highlight

	float4 ret;
	ret.a = saturate(textureColor1.a * c1.a + textureColor2.a * c2.a + textureColor3.a * c2.a + textureColor4.a * c3.a);
	ret.r = saturate(textureColor1.r * c1.r + textureColor2.r * c2.r + textureColor3.r * c2.r + textureColor4.r * c3.r);
	ret.g = saturate(textureColor1.g * c1.g + textureColor2.g * c2.g + textureColor3.g * c2.g + textureColor4.g * c3.g);
	ret.b = saturate(textureColor1.b * c1.b + textureColor2.b * c2.b + textureColor3.b * c2.b + textureColor4.b * c3.b);

	return ret;
}

float4 MultiColouredFontPSPD(MultiColouredPixelInputType input) : SV_TARGET
{
	float2 tt1 = float2(input.tex.x, input.tex.y);
	float2 tt2 = float2(input.tex.x, input.tex.y - 0.25f);
	float2 tt3 = float2(input.tex.x, input.tex.y - 0.5f);
	float2 tt4 = float2(input.tex.x, input.tex.y - 0.75f);
	float4 textureColor1 = shaderTexture.Sample(SampleType, tt1);
	float4 textureColor2 = shaderTexture.Sample(SampleType, tt2);
	float4 textureColor3 = shaderTexture.Sample(SampleType, tt3);
	float4 textureColor4 = shaderTexture.Sample(SampleType, tt4);

	float4 ret;
	ret.a = saturate(textureColor3.a * input.colour.a);
	ret.r = saturate(textureColor3.r * input.colour.r);
	ret.g = saturate(textureColor3.g * input.colour.g);
	ret.b = saturate(textureColor3.b * input.colour.b);

	return ret;
}

// NV12/YUV shader for e.g. external H.264 video

//https://github.com/balapradeepswork/D3D11NV12Rendering
	
float4 YUVPS(TexturedPixelInputType input) : SV_Target
{
	float y = shaderTexture.Sample(SampleType, float2(input.tex.x, input.tex.y * 0.5)).r;
	float u = shaderTexture.Sample(SampleType, float2(input.tex.x * 0.5, 0.50 + input.tex.y * 0.25)).r;
	float v = shaderTexture.Sample(SampleType, float2(input.tex.x * 0.5, 0.75 + input.tex.y * 0.25)).r;

	// https://spec.oneapi.io/oneipl/0.5/convert/nv12-rgb-conversion.html
	float3 yuv = float3(y - 0.0625f, u - 0.5f, v - 0.5f);

	static const float3 yuvCoef_r = { 1.164f, 0.000f, 1.596f };
	static const float3 yuvCoef_g = { 1.164f, -0.392f, -0.183f };
	static const float3 yuvCoef_b = { 1.164f, 2.017f, 0.000f };

	return float4(saturate(float3(dot(yuv, yuvCoef_r), dot(yuv, yuvCoef_g), dot(yuv, yuvCoef_b))), 1.f);
}

// Basic shader (points & lines)

struct BasicVertexInputType
{
	float4 position : POSITION;
};

struct BasicPixelInputType
{
	float4 position : SV_POSITION;
};

BasicPixelInputType BasicVS(BasicVertexInputType input)
{
	BasicPixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	return output;
}

float4 BasicPS(BasicPixelInputType input) : SV_TARGET
{
	return float4(1.0,1.0,1.0,1.0);
}
