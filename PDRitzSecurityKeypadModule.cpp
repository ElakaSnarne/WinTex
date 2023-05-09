#include "PDRitzSecurityKeypadModule.h"
#include "Utilities.h"

CPDRitzSecurityKeypadModule::CPDRitzSecurityKeypadModule() : CFullScreenModule(ModuleType::CodePanel)
{
}

CPDRitzSecurityKeypadModule::~CPDRitzSecurityKeypadModule()
{
}

void CPDRitzSecurityKeypadModule::Render()
{
}

void CPDRitzSecurityKeypadModule::Initialize()
{
	DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", 57);
	if (dd.File1.Data != NULL)
	{
		_data = dd.File1.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}

		//CreateTexturedRectangle(0.0f, 0.0f, -16.0f, 16.0f, &_iconVertexBuffer, "FullScreenIconVertexBuffer");

		//_animation = dd.File2.Data;
		//_animationLength = dd.File2.Length;

		//_iconTexture.Init(_files[14], 0, 0, &_palette[0], 0, "FullScreenIconTexture");
	}

	/*
	SPECIAL.AP, entries 57-59
	*/
}
