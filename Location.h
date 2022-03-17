#pragma once

#include <Windows.h>
#include "Map.h"
#include <vector>
#include "LZ.h"
#include "Texture.h"
#include "D3D11-NoWarn.h"
#include <unordered_map>
#include "DXText.h"
#include "Mutex.h"
#include "LocationObject.h"
#include "PointList.h"
#include "LocationSprite.h"
#include "LocationStructs.h"
#include "ObjectMap.h"

class CLocation
{
public:
	CLocation();
	~CLocation();

	BOOL Load(int locationFileIndex);

	BOOL Load_Improved(int locationFileIndex);

	void Render();

	static void SetPosition(StartupPosition pos);
	void Move(float mx, float my, float mz, float tmx);
	void DeltaAngles(float angle1, float angle2);

	BOOL PointingChanged;

	int GetPickObject(int& objectId, int& subObjectId);
	void SetObjectVisibility(int objectId, BOOL visible);

	void StartMappedAnimation(int index);
	void StartIndexedAnimation(int index);
	void StartIdAnimation(int index);
	void StopMappedAnimation(int index);
	void StopIndexedAnimation(int index);
	BOOL IsAnimationFinished(int index);
	int GetAnimationFrame(int index);
	int GetIndexedAnimationFrame(int index);
	void Animate();

	double GetPlayerDistanceFromPoint(double x, double z);
	Point GetPlayerPosition();
	SpritePosInfo GetSpriteInfo(int index);

	CMapData* _mapEntry;
	//CDMap::DMapEntry _dmapEntry;

	void UpdateSprites();

	static float _x;
	static float _y;
	static float _z;
	static float _angle1;
	static float _angle2;
	static float _y_adj;
	static float _y_elevation;
	static float _y_min;
	static float _y_max;

	void MoveObject(int direction);

protected:
	int _currentLocationId;

	//CMutex _locationMutex;

	//int AnimationType[100];
	//int AnimationParameter[100];
	//bool AnimationActive[100];
	//PBYTE AnimationFramePtr[100];
	//int AnimationFrame[100];
	//ULONGLONG AnimationTime[100];
	//int AnimationFrameDuration[100];

	void Clear();

	LPBYTE _locationData;

	void LoadPaths();
	void LoadTextures();

	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	Point* _points;
	int _verticeCount;
	int _indexCount;

	int* _pIndexes;
	std::vector<int> _indexes;

	static BOOL _loading;

	CLocationObject* _pLocObjects;
	CLocationSubObject* _pLocSubObjects;

	struct Object
	{
		int TextureIndex;
		std::vector<Triangle> Triangles;
		int VertexStart;
		int VertexCount;
	};

	std::vector<Object> _objects;

	ID3D11Buffer* _texturedVertexBuffer;
	int _texturedVerticeCount;
	ID3D11Buffer* _transparentVertexBuffer;
	int _transparentVerticeCount;

	void RenderTextured();

	TLPoint GetPoint(PBYTE p3d2, int offset, int index, int points, float tw, float th, int objectCount, int object, int subObject);
	TLPoint GetSpritePoint(PBYTE p3d2, int offset, int index, int objectCount, int object, int subObject);

	struct Sprite
	{
		TLPoint P1;
		TLPoint P2;
		TLPoint P3;
		TLPoint P4;
	};

	struct SpriteInfo
	{
		int TextureIndex;
		Point P;
		float OX;
		float OY;
		float W;
		float H;
		float U1;
		float V1;
		float U2;
		float V2;
		int ObjectIndex;
		int SubObjectIndex;
		int SubObjectId;
	};

	struct TextureInfo
	{
		int VertexStart;
		int VerticeCount;
	};

	class CTextureGroup
	{
	public:
		CTextureGroup()
		{
			pTexture = NULL;
			Transparent = FALSE;
			//VertexStart = 0;
			//VerticeCount = 0;
			TransparentVertexStart = 0;
			TransparentVerticeCount = 0;
			Rotated = FALSE;
			AnimatedTextureIndex = -1;
			SourcePointer = NULL;
			RealTexture = NULL;

			SpriteVertexStart = 0;
			SpriteVerticeCount = 0;
		}

		BOOL Rotated;

		CTexture* pTexture;
		std::vector<CTexture*> Textures;
		CTexture* RealTexture;
		int AnimatedTextureIndex;
		LPBYTE SourcePointer;

		BOOL Transparent;
		std::vector<Triangle> Triangles;
		std::vector<Triangle> TransparentTriangles;
		//int VertexStart;
		//int VerticeCount;
		int TransparentVertexStart;
		int TransparentVerticeCount;

		//std::vector<Sprite> Sprites;
		int SpriteVertexStart;
		int SpriteVerticeCount;

		std::vector<SpriteInfo> SpriteInfos;
		std::vector<TextureInfo> TextureInfos;

		CPointList Points;

		void RemovePoints(int first, int count);
		void AddPoints(int first, int count);
	};

	std::vector<CTextureGroup*> _allTextures;

	struct Path
	{
		std::vector<DPoint> Points;
		BOOL enabled;
		BOOL allowLeave;
	};

	Path* _paths;
	int _pathCount;

	ID3D11Buffer* _spriteVertexBuffer;
	int _spriteVerticeCount;

	std::unordered_map<int, BOOL> opaqueTextures;
	std::unordered_map<int, BOOL> transparentTextures;
	std::unordered_map<int, BOOL> processedTextures;

	ModelObject** _ppObjects;
	int _objectCount;
	int _subObjectCount;

	BOOL Intersect(Box& boundingBox, Point& from, Point& direction);

	int HitObject;
	int HitSubObject;
	int ObjectIndex;

	VisibilityBufferType _visibilityBuffer;
	TranslationBufferType _translationBuffer;
	ObjectMap* _objectMap;
	int _objectMapCount;
	BOOL _visibilityChanged;
	BOOL _translationChanged;

	void ModifyLocationPoints(std::wstring file);
	void ModifyLocationPoints(int startix, int endix, float x, float y, float z);

	XMFLOAT4 GetTransparentColour(std::wstring file, int objectId, int subObjectId);

	Animation Animations[50];

	BinaryData GetLocationData(int index);
	std::list<LPBYTE> Elevations;

	ObjectVisibilityMapping* _improvedObjectMap;
	void ChangeVisibility(int id, BOOL visible, BOOL setOnSubObjects, std::wstring header);
};
