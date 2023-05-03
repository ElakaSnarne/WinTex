#include "PDVidPhoneModule.h"
#include "Utilities.h"
#include "AnimationController.h"

CPDVidPhoneModule::CPDVidPhoneModule() : CModuleBase(ModuleType::VidPhone)
{
	_vertexBuffer = NULL;
}

CPDVidPhoneModule::~CPDVidPhoneModule()
{
}

void CPDVidPhoneModule::Initialize()
{
	DoubleData dd = LoadDoubleEntry(L"GRAPHICS.AP", 5);
	_pBackground = CAnimationController::LoadImage(dd, 640, 366, 1);
	_pBackground->Update();

	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_cursorPosX = static_cast<float>(w) / 2.0f;
	_cursorPosY = static_cast<float>(h) / 2.0f;

	delete[] dd.File1.Data;
	delete[] dd.File2.Data;
}

void CPDVidPhoneModule::Resize(int width, int height)
{
}

void CPDVidPhoneModule::Render()
{
	dx.Clear(0.0f, 0.0f, 0.0f);

	dx.DisableZBuffer();

	_pBackground->Render();

	dx.EnableZBuffer();

	dx.Present(1, 0);
}

void CPDVidPhoneModule::BeginAction()
{

}

void CPDVidPhoneModule::Back()
{
	CModuleController::Pop(this);
}
