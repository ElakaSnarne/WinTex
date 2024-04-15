#pragma once
#include "DXBase.h"
#include "Texture.h"
#include <unordered_map>
#include "DXText.h"

class CItems : public CDXBase
{
public:
	CItems();
	virtual ~CItems();

	static BOOL Init();
	static void Dispose();

	static void Render();
	static void RenderItem(int id, float x, float y);
	static void RenderItemName(int id, float x, float y, BOOL highlight = FALSE);
	static void RenderItemImage(int id, float x, float y, BOOL highlight = FALSE);

	static int GetWidestName();
	static int GetItemNameWidth(int id);

	static void SetTextColours(int colour1, int colour2, int colour3, int colour4)
	{
		Colour1 = colour1;
		Colour2 = colour2;
		Colour3 = colour3;
		Colour4 = colour4;
	}

	static void SetItemName(int id, std::wstring text);

	static void ResetText();

protected:
	static ID3D11Buffer* _vertexBuffer;

	static ID3D11Texture2D* _texture;
	static ID3D11ShaderResourceView* _textureRV;

	static int Colour1;
	static int Colour2;
	static int Colour3;
	static int Colour4;

	class Inventory
	{
	public:
		Inventory(int id, int w, int h);
		~Inventory();

		void Render(float x, float y);
		void RenderName(float x, float y, int colour1, int colour2, int colour3, int colour4, BOOL highlight = FALSE);
		void RenderImage(float x, float y, BOOL highlight = FALSE);

		float NameWidth() { return Text.Width(); }
		void SetItemName(std::wstring text) { Text.SetTextUnmodified(text.c_str()); }
		void ResetText();

	protected:
		int Id;
		ID3D11Buffer* ImageBuffer;	// Using global image texture
		CDXText Text;
	};

	static std::unordered_map<int, Inventory*> _items;
};
