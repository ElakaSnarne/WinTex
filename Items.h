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

protected:
	static ID3D11Buffer* _vertexBuffer;

	static ID3D11Texture2D* _texture;
	static ID3D11ShaderResourceView* _textureRV;

	class Inventory
	{
	public:
		Inventory(int id, int w, int h);
		~Inventory();

		void Render(float x, float y);
		void RenderName(float x, float y, BOOL highlight = FALSE);
		void RenderImage(float x, float y, BOOL highlight = FALSE);

		float NameWidth() { return Text.Width(); }

	protected:
		int Id;
		ID3D11Buffer* ImageBuffer;	// Using global image texture
		CDXText Text;
	};

	static std::unordered_map<int, Inventory*> _items;
};
