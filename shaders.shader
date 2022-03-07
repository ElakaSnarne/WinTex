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

/*
cbuffer FontBuffer : register(b2)
{
	float4 mcolour1;
	float4 mcolour2;
	float4 mcolour3;
	float4 mcolour4;
	float4 mcolour5;
	float4 mcolour6;
};
*/

cbuffer VisibilityBuffer : register(b3)
{
	float4 visibility[3000];
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
};

struct TexturedPixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 objectIndex : TEXCOORD1;
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
	return output;
}

float4 TexturedPS(TexturedPixelInputType input) : SV_TARGET
{
	float4 c = shaderTexture.Sample(AASampleType, input.tex);

	if (c.a < 0.25f)
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
	ret.a = saturate(textureColor1.a*colour1.a + textureColor2.a*colour2.a + textureColor3.a*colour3.a + textureColor4.a*colour4.a);
	ret.r = saturate(textureColor1.r*colour1.r + textureColor2.r*colour2.r + textureColor3.r*colour3.r + textureColor4.r*colour4.r);
	ret.g = saturate(textureColor1.g*colour1.g + textureColor2.g*colour2.g + textureColor3.g*colour3.g + textureColor4.g*colour4.g);
	ret.b = saturate(textureColor1.b*colour1.b + textureColor2.b*colour2.b + textureColor3.b*colour3.b + textureColor4.b*colour4.b);

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
	float4 c2 = input.colour;						// Main
	float4 c3 = float4(0.78, 0.78, 0.78, 1.0);	// Highlight

	float4 ret;
	ret.a = saturate(textureColor1.a* c1.a + textureColor2.a * c2.a + textureColor3.a * c2.a + textureColor4.a * c3.a);
	ret.r = saturate(textureColor1.r* c1.r + textureColor2.r * c2.r + textureColor3.r * c2.r + textureColor4.r * c3.r);
	ret.g = saturate(textureColor1.g* c1.g + textureColor2.g * c2.g + textureColor3.g * c2.g + textureColor4.g * c3.g);
	ret.b = saturate(textureColor1.b* c1.b + textureColor2.b * c2.b + textureColor3.b * c2.b + textureColor4.b * c3.b);

	return ret;
}
