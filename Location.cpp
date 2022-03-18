#include "Location.h"
#include "SQZ.h"
#include "File.h"
#include "Configuration.h"
#include "DXScreen.h"
#include <DirectXCollision.h>
#include "Globals.h"
#include "DXText.h"
#include "GameBase.h"
#include "AnimationController.h"
#include "GameController.h"
#include "Utilities.h"
#include "Math.h"
#include <tuple>
#include "Elevation.h"
#include "LocationDataHeader.h"

BOOL CLocation::_loading = FALSE;

#define CLOSE	1.0f

#define OBJECT_FLAGS_HIDDEN				0x80000000
#define SUBOBJECT_FLAGS_TRANSPARENT		0x00000001
#define SUBOBJECT_FLAGS_TEXTURED		0x00000002
#define SUBOBJECT_FLAGS_SPRITE			0x00000004
//#define SUBOBJECT_FLAGS_???			0x00000008	Vertical?
#define SUBOBJECT_FLAGS_ALPHA			0x00000010
#define SUBOBJECT_FLAGS_BOTTOM			0x00000020
//#define SUBOBJECT_FLAGS_DONT_???		0x00000040
#define SUBOBJECT_FLAGS_TOP				0x00000080
//#define SUBOBJECT_FLAGS_DONT_???		0x00000100
#define SUBOBJECT_FLAGS_OBJECTID		0x00000200
#define SUBOBJECT_FLAGS_SINGLE_COLOUR	0x00000800
//#define SUBOBJECT_FLAGS_SIDE			0x00001000
#define SUBOBJECT_FLAGS_HIDDEN			0x80000000

#define TEXTURE_FLAGS_LARGE				0x200

double DistanceSquared(DPoint p1, DPoint p2)
{
	double x = p1.X - p2.X;
	double z = p1.Z - p2.Z;
	return x * x + z * z;
}

DPoint Project(DPoint& p1, DPoint& p2, DPoint& p3)
{
	double px = p2.X - p1.X, pz = p2.Z - p1.Z, dAB = px * px + pz * pz;
	double u = ((p3.X - p1.X) * px + (p3.Z - p1.Z) * pz) / dAB;

	return DPoint{ p1.X + u * px , 0.0f, p1.Z + u * pz };
}

DPoint Normalize(DPoint v)
{
	double len = sqrt(v.X * v.X + v.Z * v.Z);
	v.X /= len;
	v.Z /= len;
	return v;
}

float CLocation::_x = 0.0f;
float CLocation::_y = 0.0f;
float CLocation::_z = 0.0f;
float CLocation::_angle1 = 0.0f;
float CLocation::_angle2 = 0.0f;
float CLocation::_y_adj = 0.0f;
float CLocation::_y_elevation = 0.0f;
float CLocation::_y_min = 0.0f;
float CLocation::_y_max = 0.0f;

CLocation::CLocation()
{
	_loading = FALSE;

	_pLocObjects = NULL;
	_pLocSubObjects = NULL;

	_currentLocationId = -1;

	_paths = NULL;
	_pathCount = 0;

	HitObject = -1;
	HitSubObject = -1;
	ObjectIndex = -1;

	_vertexBuffer = NULL;
	_indexBuffer = NULL;
	_points = NULL;
	_verticeCount = 0;
	_indexCount = 0;

	_pIndexes = NULL;

	_texturedVertexBuffer = NULL;
	_texturedVerticeCount = 0;
	_visibilityChanged = FALSE;

	_transparentVertexBuffer = NULL;
	_transparentVerticeCount = 0;
	_translationChanged = FALSE;

	_x = 0.0f;
	_y = 0.0f;
	_z = 0.0f;
	_angle1 = 0.0f;
	_angle2 = 0.0f;
	_y_adj = 0.0f;
	_y_elevation = 0.0f;

	_spriteVertexBuffer = NULL;
	_spriteVerticeCount = 0;

	_ppObjects = NULL;
	_objectCount = 0;

	_objectMap = NULL;
	_objectMapCount = 0;

	ZeroMemory(Animations, 50 * sizeof(Animation));

	_locationData = NULL;
}

CLocation::~CLocation()
{
	Clear();
}

BOOL CLocation::Load(int locationFileIndex)
{
	_loading = TRUE;

	std::wstring file = CGameController::GetFileName(locationFileIndex);

	PointingChanged = TRUE;

	// Load location entries
	CFile f;
	if (f.Open(file.c_str()))
	{
		int len = f.Size();
		_locationData = new BYTE[len];
		if (_locationData != NULL)
		{
			f.Read(_locationData, len);
		}

		//if (file == L"ALLEY.AP")
		//{
		//	// Force start drip animation
		//	BinaryData abd = GetLocationData(0);
		//	abd.Data[0xfc] = 1;
		//}
		//else if (file == L"CASTLE.AP")
		//{
		//	// Force start sealed door animation
		//	BinaryData abd = GetLocationData(0);
		//	abd.Data[0x124] = 1;
		//}

		f.Close();
	}

	// Modify points
	ModifyLocationPoints(file);

	// Load path
	LoadPaths();

	// Load 3D data
	BinaryData bd3d2 = GetLocationData(4);
	PBYTE p3d2 = bd3d2.Data;
	_verticeCount = GetInt(p3d2, 0, 4);
	_objectCount = GetInt(p3d2, 12, 4);

	// Quickly check which textures require transparency (flags & 0x10)
	for (int tc = 0; tc < _objectCount; tc++)
	{
		int objectOffset = GetInt(p3d2, 0x30 + tc * 4, 4) + 0x30;
		int subObjects = GetInt(p3d2, objectOffset + 12, 4);
		int nextSubOffset = objectOffset + 40;
		for (int ts = 0; ts < subObjects; ts++)
		{
			int thisSubOffset = nextSubOffset;
			nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

			int points = GetInt(p3d2, thisSubOffset + 4, 4);
			int flags = GetInt(p3d2, thisSubOffset + 8, 4);
			int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);

			if ((flags & SUBOBJECT_FLAGS_ALPHA) != 0 || points == 1)	// Flagged having alpha or is used as sprite
			{
				transparentTextures[tex] = TRUE;
			}
			else
			{
				opaqueTextures[tex] = TRUE;
			}
			processedTextures[tex] = TRUE;
		}
	}

	LoadTextures();

	// Load vertices
	int moff = 0x30 + _objectCount * 4;
	_points = new Point[_verticeCount];
	for (int i = 0; i < _verticeCount; i++)
	{
		_points[i].X = ((float)GetInt(p3d2, moff, 4)) / 65536.0f;
		_points[i].Y = ((float)GetInt(p3d2, moff + 4, 4)) / 65536.0f;
		_points[i].Z = ((float)GetInt(p3d2, moff + 8, 4)) / 65536.0f;
		moff += 12;
	}

	// Prepare the visibility buffer
	for (int i = 0; i < 3000; i++)
	{
		_visibilityBuffer.visibility[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Prepare the translation buffer
	for (int i = 0; i < 256; i++)
	{
		_translationBuffer.translation[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	// Prepare the object mapping list
	_objectMapCount = _mapEntry->ObjectMap.size();
	_objectMap = new ObjectMap[_objectMapCount];
	std::vector<int>::iterator oit = _mapEntry->ObjectMap.begin();
	std::vector<int>::iterator oend = _mapEntry->ObjectMap.end();
	int ix = 0;
	while (oit != oend)
	{
		_objectMap[ix].id = *oit;
		_objectMap[ix].ObjectIndex = ((_objectMap[ix].id & 0x80000800) == 0) ? _objectMap[ix].id : -1;

		if (((*oit) & 0x88000800) == 0)
		{
			_objectMap[ix].VisibilityFloatPointers.push_back(&_visibilityBuffer.visibility[ix].x);
		}

		//Trace(L"Object ");
		//Trace(ix);
		//Trace(L" maps to ");
		//TraceLine(*oit, 16);

		ix++;
		oit++;
	}

	//int animIndex = 0;
	//for (auto map : _mapEntry->AnimationMap)
	//{
	//	Trace(L"Animation ");
	//	Trace(animIndex++);
	//	Trace(L" maps to ");
	//	TraceLine(map);
	//}

	// Improved object map to test proper visibilities
	_subObjectCount = GetInt(p3d2, 8, 4);
	_improvedObjectMap = new ObjectVisibilityMapping[_subObjectCount];

	// Extract 3D data
	int texturedTriangles = 0;
	int transparentTriangles = 0;
	int toalObjectCount = 0;

	int spriteCount = 0;

	Point lb1, lb2;
	lb1.X = 10000.0f;
	lb2.X = -lb1.X;
	lb1.Y = lb1.X;
	lb2.Y = -lb1.X;
	lb1.Z = lb1.X;
	lb2.Z = -lb1.X;

	int six = 0;

	_pLocObjects = new CLocationObject[_objectCount];
	_pLocSubObjects = new CLocationSubObject[GetInt(p3d2, 8, 4)];
	CLocationSubObject* pCurSubObjPtr = _pLocSubObjects;

	_ppObjects = new ModelObject * [_objectCount];
	for (int i = 0; i < _objectCount; i++)
	{
		_pLocObjects[i].pSubObjects = pCurSubObjPtr;

		int objectOffset = GetInt(p3d2, 0x30 + i * 4, 4) + 0x30;
		int type = (int)GetInt(p3d2, objectOffset, 4);

		_ppObjects[i] = new ModelObject();
		ZeroMemory(_ppObjects[i], sizeof(ModelObject));
		_ppObjects[i]->Index = i;

		float ominx = 10000.0f, omaxx = -ominx;
		float ominy = ominx, omaxy = -ominx;
		float ominz = ominx, omaxz = -ominx;

		if ((type & 0x80000000) != 0)
		{
			// Object visibility
			_visibilityBuffer.visibility[i].x = -1.0f;
		}

		int unkownOffset = GetInt(p3d2, objectOffset + 0x1c, 4);

		// Header is 10 longs
		// Pointer to next sub object is first long in object

		int subObjects = GetInt(p3d2, objectOffset + 12, 4);
		toalObjectCount += subObjects;
		int nextSubOffset = objectOffset + 40;

		_ppObjects[i]->SubObjectCount = subObjects;
		_ppObjects[i]->SubObjects = new ModelSubObject[subObjects];
		ZeroMemory(_ppObjects[i]->SubObjects, subObjects * sizeof(ModelSubObject));

		_pLocObjects[i].SubObjectCount = subObjects;

		for (int j = 0; j < subObjects; j++)
		{
			float sminx = 10000.0f, smaxx = -sminx;
			float sminy = sminx, smaxy = -sminx;
			float sminz = sminx, smaxz = -sminx;

			int thisSubOffset = nextSubOffset;
			nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

			if (thisSubOffset != unkownOffset)
			{
				int points = GetInt(p3d2, thisSubOffset + 4, 4);
				int flags = GetInt(p3d2, thisSubOffset + 8, 4);
				int sid = GetInt(p3d2, thisSubOffset + 0xc, 4);
				int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);
				if ((flags & SUBOBJECT_FLAGS_TEXTURED) == 0) tex = -1;

				_improvedObjectMap[six].ObjectIndex = i;
				_improvedObjectMap[six].SubObjectIndex = six;
				_improvedObjectMap[six].SubObjectId = sid;

				pCurSubObjPtr[j].Id = sid;
				pCurSubObjPtr[j].TextureIndex = tex;
				pCurSubObjPtr[j].VertexIndex = -1;
				pCurSubObjPtr[j].VertexCount = 0;

				if ((sid & 0x88000000) != 0)
				{
					int rid = (sid >> 16) & 0xffff;
					for (int z = 0; z < _objectMapCount; z++)
					{
						if (_objectMap[z].id == sid || ((rid & 0x800) != 0 && _objectMap[z].id == rid))
						{
							// TODO: This is not right! Can be set multiple times
							// TODO: May have to do a reverse mapping, setting the object id on the triangle
							//_objectMap[z].SubObjectIndex = six;
							_objectMap[z].SubObjectIndices.push_back(six);

							_objectMap[z].VisibilityFloatPointers.push_back(&_visibilityBuffer.visibility[six].y);
							//break;
						}
					}
				}

				if ((flags & SUBOBJECT_FLAGS_HIDDEN) != 0)
				{
					// Sub-object visibility
					_visibilityBuffer.visibility[six].y = -1.0f;
				}

				// Sub-object transparency indicator
				_visibilityBuffer.visibility[six].z = (flags & SUBOBJECT_FLAGS_ALPHA) ? 1.0f : 0.0f;

				_ppObjects[i]->SubObjects[j].ModelIndex = i;
				_ppObjects[i]->SubObjects[j].SubObjectIndex = six;
				_ppObjects[i]->SubObjects[j].Flags = flags;
				_ppObjects[i]->SubObjects[j].Texture = tex;
				_ppObjects[i]->SubObjects[j].Active = ((flags & SUBOBJECT_FLAGS_HIDDEN) == 0);
				_ppObjects[i]->SubObjects[j].ID = sid;
				_ppObjects[i]->SubObjects[j].PointCount = points;
				_ppObjects[i]->SubObjects[j].Points = new TLPoint[points];
				_ppObjects[i]->SubObjects[j].BoundingBox.X1 = _ppObjects[i]->SubObjects[j].BoundingBox.Y1 = _ppObjects[i]->SubObjects[j].BoundingBox.Z1 = 10000.0f;
				_ppObjects[i]->SubObjects[j].BoundingBox.X2 = _ppObjects[i]->SubObjects[j].BoundingBox.Y2 = _ppObjects[i]->SubObjects[j].BoundingBox.Z2 = -_ppObjects[i]->SubObjects[j].BoundingBox.X1;
				_ppObjects[i]->SubObjects[j].Triangles = (points > 2) ? new Triangle[points - 2] : NULL;

				if (points > 0)
				{
					CTextureGroup* stex = (tex >= 0) ? _allTextures.at(tex) : NULL;

					if (points == 1)
					{
						// 2020-11-27 New sprite loader

						// Find number of sub-sprites
						int subSprites = GetInt(p3d2, thisSubOffset + 0x38, 4);
						// Find sprite main point
						int spoio = thisSubOffset + 0x44 + subSprites * 32;
						int pointIndex = _objectCount + (GetInt(p3d2, thisSubOffset + 0x44 + subSprites * 32, 4) >> 4);
						float cx = _points[pointIndex].X;
						float cy = _points[pointIndex].Y;
						float cz = _points[pointIndex].Z;

						if (subSprites > 0)
						{
							// Find sprite texture dimensions
							float sw = (stex != NULL) ? stex->pTexture->Width() : 1.0f;
							float sh = (stex != NULL) ? stex->pTexture->Height() : 1.0f;
							float ssw = ((float)GetInt(p3d2, thisSubOffset + 0x30, 4)) / 65536.0f;
							float ssh = ((float)GetInt(p3d2, thisSubOffset + 0x34, 4)) / 65536.0f;

							// Find sprite world dimensions
							float sww = ((float)GetInt(p3d2, thisSubOffset + 0x28, 4)) / 65536.0f;
							float swh = ((float)GetInt(p3d2, thisSubOffset + 0x2c, 4)) / 65536.0f;

							for (int ss = 0; ss < subSprites; ss++)
							{
								// Find sub-sprite x & y offsets
								float subSpriteOffsetX = ((float)GetInt(p3d2, thisSubOffset + 0x44 + ss * 32, 4)) / 65536.0f;
								float subSpriteOffsetY = ((float)GetInt(p3d2, thisSubOffset + 0x48 + ss * 32, 4)) / 65536.0f;

								// Find sub-sprite width & height
								float subSpriteTexX1 = ((float)GetInt(p3d2, thisSubOffset + 0x54 + ss * 32, 4)) / 65536.0f;
								float subSpriteTexY1 = ((float)GetInt(p3d2, thisSubOffset + 0x58 + ss * 32, 4)) / 65536.0f;
								float subSpriteTexX2 = 1.0f + (((float)GetInt(p3d2, thisSubOffset + 0x5c + ss * 32, 4)) / 65536.0f);
								float subSpriteTexY2 = 1.0f + (((float)GetInt(p3d2, thisSubOffset + 0x60 + ss * 32, 4)) / 65536.0f);

								float subSpriteWidth = subSpriteTexX2 - subSpriteTexX1;
								float subSpriteHeight = subSpriteTexY2 - subSpriteTexY1;

								// Add sprite
								SpriteInfo si;
								si.TextureIndex = tex;
								si.P.X = cx;
								si.P.Y = cy;
								si.P.Z = cz;
								si.OX = (sww * subSpriteOffsetX) / ssw;
								si.OY = (swh * subSpriteOffsetY) / ssh;
								si.W = (sww * subSpriteWidth) / ssw;
								si.H = (swh * subSpriteHeight) / ssh;
								si.U1 = subSpriteTexX1 / sw;
								si.V1 = subSpriteTexY1 / sh;
								si.U2 = subSpriteTexX2 / sw;
								si.V2 = subSpriteTexY2 / sh;
								si.ObjectIndex = i;
								si.SubObjectIndex = six;
								si.SubObjectId = sid;

								if (stex != NULL) stex->SpriteInfos.push_back(si);

								spriteCount++;

								// Get sprite bounding box
								if ((si.P.X - si.W / 2) < sminx)
								{
									sminx = si.P.X - si.W / 2;
								}
								if ((si.P.X + si.W / 2) > smaxx)
								{
									smaxx = si.P.X + si.W / 2;
								}
								if ((si.P.Y - si.H) < sminy)
								{
									sminy = si.P.Y - si.H;
								}
								if (si.P.Y > smaxy)
								{
									smaxy = si.P.Y;
								}
								if ((si.P.Z - si.W / 2) < sminz)
								{
									sminz = si.P.Z - si.W / 2;
								}
								if ((si.P.Z + si.W / 2) > smaxz)
								{
									smaxz = si.P.Z + si.W / 2;
								}
							}

							// 2020-11-27 End of new sprite loader
						}
						else
						{
							sminx = smaxx = cx;
							sminy = smaxy = cy;
							sminz = smaxz = cz;
						}
					}
					else if (points >= 3)
					{
						Object obj;
						obj.TextureIndex = tex;

						CTexture* pt = (stex != NULL) ? stex->pTexture : NULL;
						float tw = (pt != NULL) ? pt->Width() : 0.0f;
						float th = (pt != NULL) ? pt->Height() : 0.0f;

						// Test new triangulation
						std::vector<TLPoint> vpoints;
						for (int p = 0; p < points; p++)
						{
							TLPoint px = GetPoint(p3d2, thisSubOffset + 0x28, p, points, tw, th, _objectCount, i, six);
							px.SubObjectId = j;

							if ((flags & SUBOBJECT_FLAGS_SINGLE_COLOUR) != 0)
							{
								//float tu = px.U;
								//px.U = px.V;
								//px.V = tu;
								px.U = 0.0f;
								px.V = 0.0f;
							}

							_ppObjects[i]->SubObjects[j].Points[p] = px;

							vpoints.push_back(px);

							// Get bounding box
							if (px.Point->X < sminx) sminx = px.Point->X;
							if (px.Point->X > smaxx) smaxx = px.Point->X;
							if (px.Point->Y < sminy) sminy = px.Point->Y;
							if (px.Point->Y > smaxy) smaxy = px.Point->Y;
							if (px.Point->Z < sminz) sminz = px.Point->Z;
							if (px.Point->Z > smaxz) smaxz = px.Point->Z;
						}

						int startp = 0;
						int tri = points - 2;
						int pointsLeft = points;
						int trix = 0;
						while (pointsLeft > 2)
						{
							int prev1 = startp - 1;
							if (prev1 < 0) prev1 += pointsLeft;
							int prev2 = startp - 2;
							if (prev2 < 0) prev2 += pointsLeft;

							// If this + 2 previous points form a proper triangle, create it and remove previous point
							TLPoint p0 = vpoints.at(startp);
							TLPoint p1 = vpoints.at(prev1);
							TLPoint p2 = vpoints.at(prev2);

							Point v1;
							v1.X = p0.Point->X - p1.Point->X;
							v1.Y = p0.Point->Y - p1.Point->Y;
							v1.Z = p0.Point->Z - p1.Point->Z;

							float len = sqrt(v1.X * v1.X + v1.Y * v1.Y + v1.Z * v1.Z);
							v1.X /= len;
							v1.Y /= len;
							v1.Z /= len;

							Point v2;
							v2.X = p1.Point->X - p2.Point->X;
							v2.Y = p1.Point->Y - p2.Point->Y;
							v2.Z = p1.Point->Z - p2.Point->Z;

							len = sqrt(v2.X * v2.X + v2.Y * v2.Y + v2.Z * v2.Z);
							v2.X /= len;
							v2.Y /= len;
							v2.Z /= len;

							if (v1.X != v2.X || v1.Y != v2.Y || v1.Z != v2.Z)
							{
								// Points form a triangle
								Triangle t1;
								t1.ObjectId = i;
								t1.SubObjectId = sid & 0xffff;
								t1.P1 = p0;
								t1.P2 = p2;
								t1.P3 = p1;

								obj.Triangles.push_back(t1);
								if (stex != NULL)
								{
									if ((flags & SUBOBJECT_FLAGS_TRANSPARENT) == 0)
									{
										// Regular textured object
										stex->Triangles.push_back(t1);
										texturedTriangles++;
										obj.VertexCount += 3;
									}
									else
									{
										// Transparent object
										stex->TransparentTriangles.push_back(t1);
										transparentTriangles++;
									}
								}

								_ppObjects[i]->SubObjects[j].Triangles[trix++] = t1;

								// Remove point at prev1
								vpoints.erase(vpoints.begin() + prev1);
								pointsLeft--;
							}
							else
							{
								// Points do not form a triangle
								//t--;
								if (pointsLeft == 3)
								{
									//break;
								}
							}

							startp++;
							if (startp >= pointsLeft) startp -= pointsLeft;
						}

						_objects.push_back(obj);
					}
					else
					{
						int debug = 0;
					}
				}
			}

			if (sminx < ominx)
			{
				ominx = sminx;
			}
			if (sminy < ominy)
			{
				ominy = sminy;
			}
			if (sminz < ominz)
			{
				ominz = sminz;
			}
			if (smaxx > omaxx)
			{
				omaxx = smaxx;
			}
			if (smaxy > omaxy)
			{
				omaxy = smaxy;
			}
			if (smaxz > omaxz)
			{
				omaxz = smaxz;
			}

			_ppObjects[i]->SubObjects[j].BoundingBox.X1 = sminx;
			_ppObjects[i]->SubObjects[j].BoundingBox.X2 = smaxx;
			_ppObjects[i]->SubObjects[j].BoundingBox.Y1 = sminy;
			_ppObjects[i]->SubObjects[j].BoundingBox.Y2 = smaxy;
			_ppObjects[i]->SubObjects[j].BoundingBox.Z1 = sminz;
			_ppObjects[i]->SubObjects[j].BoundingBox.Z2 = smaxz;

			six++;

			if (nextSubOffset < thisSubOffset) break;
		}

		pCurSubObjPtr += subObjects;

		_ppObjects[i]->BoundingBox.X1 = ominx;
		_ppObjects[i]->BoundingBox.X2 = omaxx;
		_ppObjects[i]->BoundingBox.Y1 = ominy;
		_ppObjects[i]->BoundingBox.Y2 = omaxy;
		_ppObjects[i]->BoundingBox.Z1 = ominz;
		_ppObjects[i]->BoundingBox.Z2 = omaxz;

		if (ominx < lb1.X)
		{
			lb1.X = ominx;
		}
		if (omaxx > lb2.X)
		{
			lb2.X = omaxx;
		}
		if (ominy < lb1.Y)
		{
			lb1.Y = ominy;
		}
		if (omaxy > lb2.Y)
		{
			lb2.Y = omaxy;
		}
		if (ominz < lb1.Z)
		{
			lb1.Z = ominz;
		}
		if (omaxz > lb2.Z)
		{
			lb2.Z = omaxz;
		}
	}

	// Create points vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Point) * _verticeCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = _points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	dx.CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer, "Location Vertices");

	// Create textured triangles
	_texturedVerticeCount = texturedTriangles * 3;
	TEXTURED_VERTEX* pTV = new TEXTURED_VERTEX[_texturedVerticeCount];
	_transparentVerticeCount = transparentTriangles * 3;
	COLOURED_VERTEX* pTTV = new COLOURED_VERTEX[_transparentVerticeCount];
	std::vector<CTextureGroup*>::iterator tit = _allTextures.begin();
	std::vector<CTextureGroup*>::iterator tend = _allTextures.end();
	int tix = 0;
	int ttix = 0;

	while (tit != tend)
	{
		CTextureGroup* pTex = *tit;

		int vertexStart = tix;

		pTex->TransparentVertexStart = ttix;

		std::vector<Triangle>::iterator trit = pTex->Triangles.begin();
		std::vector<Triangle>::iterator trend = pTex->Triangles.end();
		int curobid = -1;
		int cursubobid = -1;
		CLocationSubObject* pSub = NULL;
		while (trit != trend)
		{
			Triangle* pTri = &(*trit);
			if (pTri->ObjectId == 0 && pTri->SubObjectId == 0x004f)
			{
				int debug = 0;
			}

			if (pTri->ObjectId != curobid || pTri->SubObjectId != cursubobid)
			{
				pSub = NULL;

				CLocationObject* pOb = &_pLocObjects[pTri->ObjectId];
				for (int sx = 0; sx < pOb->SubObjectCount; sx++)
				{
					if ((pOb->pSubObjects[sx].Id & 0xffff) == pTri->SubObjectId)
					{
						pSub = &pOb->pSubObjects[sx];
						if (pSub->VertexIndex < 0)
						{
							pSub->VertexIndex = tix;
						}
						break;
					}
				}
			}

			if (pSub != NULL)
			{
				pSub->VertexCount += 3;
			}

			pTV[tix].position.x = pTri->P1.Point->X;
			pTV[tix].position.y = pTri->P1.Point->Y;
			pTV[tix].position.z = pTri->P1.Point->Z;
			pTV[tix].texture.x = pTri->P1.U;
			pTV[tix].texture.y = pTri->P1.V;
			pTV[tix].object.x = (float)pTri->P1.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P1.SubObjectIndex;
			tix++;
			pTV[tix].position.x = pTri->P2.Point->X;
			pTV[tix].position.y = pTri->P2.Point->Y;
			pTV[tix].position.z = pTri->P2.Point->Z;
			pTV[tix].texture.x = pTri->P2.U;
			pTV[tix].texture.y = pTri->P2.V;
			pTV[tix].object.x = (float)pTri->P2.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P2.SubObjectIndex;
			tix++;
			pTV[tix].position.x = pTri->P3.Point->X;
			pTV[tix].position.y = pTri->P3.Point->Y;
			pTV[tix].position.z = pTri->P3.Point->Z;
			pTV[tix].texture.x = pTri->P3.U;
			pTV[tix].texture.y = pTri->P3.V;
			pTV[tix].object.x = (float)pTri->P3.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P3.SubObjectIndex;
			tix++;

			trit++;
		}

		int vertexCount = tix - vertexStart;
		if (vertexCount > 0)
		{
			pTex->Points.Add(vertexStart, vertexCount);
		}

		trit = pTex->TransparentTriangles.begin();
		trend = pTex->TransparentTriangles.end();
		while (trit != trend)
		{
			Triangle* pTri = &(*trit);

			XMFLOAT4 transparentColour = GetTransparentColour(file, pTri->P1.ObjectIndex, pTri->P1.SubObjectId);

			pTTV[ttix].position.x = pTri->P1.Point->X;
			pTTV[ttix].position.y = pTri->P1.Point->Y;
			pTTV[ttix].position.z = pTri->P1.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P1.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P1.SubObjectIndex;
			ttix++;
			pTTV[ttix].position.x = pTri->P2.Point->X;
			pTTV[ttix].position.y = pTri->P2.Point->Y;
			pTTV[ttix].position.z = pTri->P2.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P2.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P2.SubObjectIndex;
			ttix++;
			pTTV[ttix].position.x = pTri->P3.Point->X;
			pTTV[ttix].position.y = pTri->P3.Point->Y;
			pTTV[ttix].position.z = pTri->P3.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P3.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P3.SubObjectIndex;
			ttix++;

			trit++;
		}

		pTex->TransparentVerticeCount = ttix - pTex->TransparentVertexStart;

		tit++;
	}

	// Textured buffer
	D3D11_BUFFER_DESC triBufferDesc;
	triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	triBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * _texturedVerticeCount;
	triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	triBufferDesc.MiscFlags = 0;
	triBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA triData;
	triData.pSysMem = pTV;
	triData.SysMemPitch = 0;
	triData.SysMemSlicePitch = 0;

	dx.CreateBuffer(&triBufferDesc, &triData, &_texturedVertexBuffer, "Location Textured Triangles");

	delete[] pTV;

	// Transparent buffer
	if (_transparentVerticeCount > 0)
	{
		triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		triBufferDesc.ByteWidth = sizeof(COLOURED_VERTEX) * _transparentVerticeCount;
		triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		triBufferDesc.MiscFlags = 0;
		triBufferDesc.StructureByteStride = 0;

		triData.pSysMem = pTTV;
		triData.SysMemPitch = 0;
		triData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&triBufferDesc, &triData, &_transparentVertexBuffer, "Location Transparent Triangles");
	}
	delete[] pTTV;

	// Create sprite buffer

	if (spriteCount > 0)
	{
		_spriteVerticeCount = spriteCount * 3 * 2;
		pTV = new TEXTURED_VERTEX[_spriteVerticeCount];
		tit = _allTextures.begin();
		tend = _allTextures.end();
		tix = 0;
		while (tit != tend)
		{
			CTextureGroup* pTex = *tit;

			pTex->SpriteVertexStart = tix;

			std::vector<SpriteInfo>::iterator sit = pTex->SpriteInfos.begin();
			std::vector<SpriteInfo>::iterator send = pTex->SpriteInfos.end();
			while (sit != send)
			{
				SpriteInfo* spr = &(*sit);

				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX + spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V1;
				tix++;

				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V2;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				tix++;

				sit++;
			}

			pTex->SpriteVerticeCount = tix - pTex->SpriteVertexStart;

			tit++;
		}

		triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		triBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * _spriteVerticeCount;
		triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		triBufferDesc.MiscFlags = 0;
		triBufferDesc.StructureByteStride = 0;

		triData.pSysMem = pTV;
		triData.SysMemPitch = 0;
		triData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&triBufferDesc, &triData, &_spriteVertexBuffer, "Location Sprites");

		delete[] pTV;
	}

	// Load alternative textures
	if (pConfig->AlternativeMedia)
	{
		wchar_t alternateName[1024];
		ZeroMemory(alternateName, sizeof(alternateName));
		wcscat(alternateName, file.c_str());
		wchar_t* pAlter = alternateName + file.size() - 3;
		pAlter[0] = L'\\';
		pAlter++;

		tit = _allTextures.begin();
		tend = _allTextures.end();
		int tix = 0;
		while (tit != tend)
		{
			_itow(tix, pAlter, 10);
			wchar_t* pAlter2 = pAlter + wcslen(pAlter);
			wcscat(pAlter2, L".png");

			// TODO: If file exists, replace texture...
			if (CFile::Exists(alternateName))
			{
				(*tit)->pTexture->Init(alternateName);
			}

			tix++;
			tit++;
		}
	}

	_visibilityChanged = TRUE;
	_translationChanged = TRUE;

	// Start default animations
	BinaryData animbd = GetLocationData(0);
	int animationCount = GetInt(animbd.Data, 0, 4);
	for (int i = 0; i < animationCount && i < 50; i++)
	{
		int offset = GetInt(animbd.Data, 8 + i * 8, 4);
		int trigger = GetInt(animbd.Data, offset + 8, 4);
		if (trigger == 1)
		{
			StartIndexedAnimation(i);
		}
		else
		{
			// Check type, if 16, add to Elevations
			int type = GetInt(animbd.Data, offset, 4);
			if (type == 16)
			{
				Elevations.push_back(animbd.Data + offset);
			}
		}
	}

	_loading = FALSE;

	Animate();// Force auto-starting animations to play before scripts can initiate more anims or show/hide objects

	return TRUE;
}

void CLocation::Clear()
{
	if (_paths != NULL)
	{
		delete[] _paths;
		_paths = NULL;
	}

	_pathCount = 0;

	if (_ppObjects != NULL)
	{
		for (int i = 0; i < _objectCount; i++)
		{
			if (_ppObjects[i]->SubObjects != NULL)
			{
				for (int j = 0; j < _ppObjects[i]->SubObjectCount; j++)
				{
					if (_ppObjects[i]->SubObjects[j].Points != NULL)
					{
						delete[] _ppObjects[i]->SubObjects[j].Points;
						_ppObjects[i]->SubObjects[j].Points = NULL;
					}

					if (_ppObjects[i]->SubObjects[j].Triangles != NULL)
					{
						delete[] _ppObjects[i]->SubObjects[j].Triangles;
						_ppObjects[i]->SubObjects[j].Triangles = NULL;
					}
				}

				delete[] _ppObjects[i]->SubObjects;
				_ppObjects[i]->SubObjects = NULL;
			}

			delete[] _ppObjects[i];
			_ppObjects[i] = NULL;
		}

		delete[] _ppObjects;
		_ppObjects = NULL;
	}

	if (_pLocObjects != NULL)
	{
		delete[] _pLocObjects;
		_pLocObjects = NULL;
	}

	if (_pLocSubObjects != NULL)
	{
		delete[] _pLocSubObjects;
		_pLocSubObjects = NULL;
	}

	// Delete old location data

	if (_locationData != NULL)
	{
		delete[] _locationData;
		_locationData = NULL;
	}

	if (_indexBuffer != NULL)
	{
		_indexBuffer->Release();
		_indexBuffer = NULL;
	}

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	if (_points != NULL)
	{
		delete[] _points;
		_points = NULL;
	}

	_verticeCount = 0;

	// TODO: Loop through list of currently loaded textures and free the resources
	std::vector<CTextureGroup*>::iterator texit = _allTextures.begin();
	std::vector<CTextureGroup*>::iterator texend = _allTextures.end();
	while (texit != texend)
	{
		// Delete texture lists
		CTextureGroup* pTG = *texit;
		CTexture* pTex = pTG->pTexture;

		pTG->SpriteInfos.clear();
		pTG->TextureInfos.clear();
		for (auto it : pTG->Textures)
		{
			delete it;
		}

		delete pTex;
		delete pTG;
		texit++;
	}

	if (_pIndexes != NULL)
	{
		delete[] _pIndexes;
		_pIndexes = NULL;
	}

	if (_texturedVertexBuffer != NULL)
	{
		_texturedVertexBuffer->Release();
		_texturedVertexBuffer = NULL;
	}

	if (_transparentVertexBuffer != NULL)
	{
		_transparentVertexBuffer->Release();
		_transparentVertexBuffer = NULL;
	}

	_allTextures.clear();
	_objects.clear();
	_indexes.clear();

	if (_spriteVertexBuffer != NULL)
	{
		_spriteVertexBuffer->Release();
		_spriteVertexBuffer = NULL;
	}
	_spriteVerticeCount = 0;

	if (_objectMap != NULL)
	{
		delete[] _objectMap;
		_objectMap = NULL;
	}
	_objectMapCount = 0;

	ZeroMemory(Animations, 50 * sizeof(Animation));

	Elevations.clear();
}

void CLocation::LoadTextures()
{
	BinaryData bdPal = GetLocationData(2);
	BinaryData bdsqz = GetLocationData(5);
	BinaryData bdtex = CSQZ::Decompress(bdsqz.Data, bdsqz.Length);

	int poff = (bdPal.Length <= 0x300) ? 0 : bdPal.Length - 0x300;
	int palette[256];
	PBYTE pPal = (PBYTE)palette;
	for (int i = 0; i < 256; i++)
	{
		byte r = bdPal.Data[i * 3 + 0 + poff];
		byte g = bdPal.Data[i * 3 + 1 + poff];
		byte b = bdPal.Data[i * 3 + 2 + poff];
		byte a = 255;

		pPal[i * 4 + 0] = b;
		pPal[i * 4 + 1] = g;
		pPal[i * 4 + 2] = r;
		pPal[i * 4 + 3] = a;
	}

	PBYTE tex = bdtex.Data;
	PBYTE texend = tex + bdtex.Length;

	pPal[0] = 0;
	pPal[1] = 0;
	pPal[2] = 0;

	int hqSize = GetInt(tex, 0, 4);
	int lqSize = GetInt(tex, 4, 4);
	int texCount = GetInt(tex, 8, 4);
	int texPtr = 12 + texCount * 12;
	for (int t = 0; t < texCount; t++)
	{
		pPal[3] = (transparentTextures[t] || processedTextures.find(t) == processedTextures.end()) ? 0 : 255;

		// Read 12 bytes (3 longs)
		int w = GetInt(tex, texPtr, 4);
		int h = GetInt(tex, texPtr + 4, 4);
		int subType = GetInt(tex, texPtr + 8, 4);

		CTextureGroup* stex = new CTextureGroup();
		stex->SourcePointer = tex + texPtr;

		texPtr += 12 + 4 * h;   // Skip table
		PBYTE scan = tex + texPtr;

		if ((subType & 2) == 2)
		{
			// Animated texture, skip for now...
		}
		else
		{
			CTexture* pTex = new CTexture(&dx, w, h, scan, palette, (subType & SUBOBJECT_FLAGS_TRANSPARENT) == 1);

			BOOL transparent = transparentTextures[t];
			BOOL opaque = opaqueTextures[t];

			stex->pTexture = pTex;
			stex->Transparent = transparent;
			stex->Rotated = (subType == 1);
		}

		_allTextures.push_back(stex);

		texPtr += w * h;
	}

	BinaryData bdanim = GetLocationData(0);
	LPBYTE pAnim = bdanim.Data;
	int animcount = GetInt(pAnim, 0, 4);
	for (int i = 0; i < animcount; i++)
	{
		int offset = GetInt(pAnim, 8 + i * 8, 4);
		int type = GetInt(pAnim, offset, 4);
		if (type == 14)
		{
			int animated = GetInt(pAnim, offset + 12, 4);
			int baseTexture = GetInt(pAnim, offset + 16, 4);
			CTextureGroup* pBase = _allTextures.at(baseTexture);
			CTextureGroup* pATex = _allTextures.at(animated);
			LPBYTE pBaseImage = pBase->SourcePointer;
			LPBYTE pMod = pATex->SourcePointer;

			int w = GetInt(pBaseImage, 0, 4);
			int h = GetInt(pBaseImage, 4, 4);

			int mw = GetInt(pMod, 0, 4);
			int mh = GetInt(pMod, 4, 4);

			pMod += 12 + 4 * mh;   // Skip table
			pBaseImage += 12 + 4 * h;

			int v1 = GetInt(pMod, 0, 2);
			int width = GetInt(pMod, 2, 2);
			int height = GetInt(pMod, 4, 2);
			int flags = GetInt(pMod, 6, 2);
			int fbc = (flags & TEXTURE_FLAGS_LARGE) ? 4 : 2;

			LPBYTE pEnd = pMod + mw * mh;
			pMod += 8;
			while (pMod < pEnd)
			{
				// Apply modifications
				int destination = 0;
				int bytesInFrame = GetInt(pMod, 0, fbc);
				pMod += fbc;
				while (bytesInFrame > 0)
				{
					int b = *(pMod++);
					bytesInFrame--;
					if ((b & 0x80) != 0)
					{
						// Skip this many bytes
						destination += (b & 0x7f);
					}
					else
					{
						// Copy this many bytes
						CopyMemory(pBaseImage + destination, pMod, b);
						destination += b;
						pMod += b;
						bytesInFrame -= b;
					}
				}

				pPal[3] = pBase->Transparent ? 0 : 255;

				CTexture* pTex = new CTexture(&dx, w, h, pBaseImage, palette, pBase->Rotated);
				pATex->Textures.push_back(pTex);
				pATex->Transparent = pBase->Transparent;
			}

			pATex->AnimatedTextureIndex = 0;
		}
	}

	delete[] tex;
}

void CLocation::Render()
{
	if (_visibilityChanged)
	{
		CConstantBuffers::SetVisibility(dx, _visibilityBuffer);
		_visibilityChanged = FALSE;
	}

	if (_translationChanged)
	{
		CConstantBuffers::SetTranslation(dx, _translationBuffer);
		_translationChanged = FALSE;
	}

	CConstantBuffers::Setup3D(dx);
	dx.EnableZBuffer();

	XMMATRIX rm1 = XMMatrixRotationX(_angle1);
	XMMATRIX rm2 = XMMatrixRotationY(_angle2);
	XMMATRIX tm = XMMatrixTranslation(_x, _y + _y_adj + _y_elevation, _z);

	XMMATRIX wm;
	wm = tm * rm2 * rm1;
	CConstantBuffers::SetWorld(dx, &wm);

	RenderTextured();

	CShaders::SelectOrthoShader();
	dx.DisableZBuffer();
	CConstantBuffers::Setup2D(dx);

	CDXText caption;

#ifdef DEBUG
	char xbuffer[40];
	_gcvt_s(xbuffer, sizeof(xbuffer), -_x, 3);
	char ybuffer[40];
	_gcvt_s(ybuffer, sizeof(ybuffer), -(_y + _y_adj + _y_elevation), 3);
	char zbuffer[40];
	_gcvt_s(zbuffer, sizeof(zbuffer), -_z, 3);
	char a1buffer[40];
	_gcvt_s(a1buffer, sizeof(a1buffer), _angle1, 3);
	char a2buffer[40];
	_gcvt_s(a2buffer, sizeof(a2buffer), _angle2, 3);

	char obuffer[40];
	_itoa_s(HitObject, obuffer, 10);
	char sbuffer[40];
	_itoa_s(HitSubObject, sbuffer, 16);

	char oixbuffer[40];
	_itoa_s(ObjectIndex, oixbuffer, 10);

	float dy = 40.0f;
	caption.SetText("X");
	caption.Render(0.0f, dy);
	caption.SetText("Y");
	caption.Render(0.0f, dy + 20.0f * pConfig->FontScale);
	caption.SetText("Z");
	caption.Render(0.0f, dy + 40.0f * pConfig->FontScale);
	caption.SetText("Pitch");
	caption.Render(0.0f, dy + 60.0f * pConfig->FontScale);
	caption.SetText("Yaw");
	caption.Render(0.0f, dy + 80.0f * pConfig->FontScale);
	caption.SetText("Object index");
	caption.Render(0.0f, dy + 120.0f * pConfig->FontScale);
	caption.SetText("Sub-object id");
	caption.Render(0.0f, dy + 140.0f * pConfig->FontScale);
	caption.SetText("Mapped object");
	caption.Render(0.0f, dy + 160.0f * pConfig->FontScale);

	caption.SetText(xbuffer);
	caption.Render(100.0f * pConfig->FontScale, dy);
	caption.SetText(ybuffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 20.0f * pConfig->FontScale);
	caption.SetText(zbuffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 40.0f * pConfig->FontScale);
	caption.SetText(a1buffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 60.0f * pConfig->FontScale);
	caption.SetText(a2buffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 80.0f * pConfig->FontScale);
	caption.SetText(obuffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 120.0f * pConfig->FontScale);
	caption.SetText(sbuffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 140.0f * pConfig->FontScale);
	caption.SetText(oixbuffer);
	caption.Render(100.0f * pConfig->FontScale, dy + 160.0f * pConfig->FontScale);
#endif

	if (!CAnimationController::HasAnim())
	{
		caption.SetText("Score:");
		caption.Render(0.0f, dx.GetHeight() - caption.Height() - 10.0f);
		char scorebuffer[40];
		_itoa_s(CGameController::GetScore(), scorebuffer, 10);
		caption.SetText(scorebuffer);
		caption.Render(60.0f * pConfig->FontScale, dx.GetHeight() - caption.Height() - 10.0f);
	}
}

void CLocation::RenderTextured()
{
	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_texturedVertexBuffer, &stride, &offset);
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CShaders::SelectTextureShader();

	// Render all textured objects
	std::vector<CTextureGroup*>::iterator tit = _allTextures.begin();
	std::vector<CTextureGroup*>::iterator tend = _allTextures.end();
	while (tit != tend)
	{
		CTextureGroup* pT = *tit;

		if (pT->Points.Next != NULL)
		{
			CTexture* pTex = (pT->RealTexture != NULL) ? pT->RealTexture : pT->pTexture;
			ID3D11ShaderResourceView* pRV = pTex->GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			CPointList* pP = pT->Points.Next;
			while (pP != NULL)
			{
				dx.Draw(pP->Count, pP->First);
				pP = pP->Next;
			}
		}

		tit++;
	}

	// Render sprites
	stride = sizeof(TEXTURED_VERTEX);
	offset = 0;
	dx.SetVertexBuffers(0, 1, &_spriteVertexBuffer, &stride, &offset);
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	tit = _allTextures.begin();
	tend = _allTextures.end();
	while (tit != tend)
	{
		CTextureGroup* pT = *tit;
		if (pT->SpriteVerticeCount > 0)
		{
			CTexture* pTex = (pT->RealTexture != NULL) ? pT->RealTexture : pT->pTexture;
			ID3D11ShaderResourceView* pRV = pTex->GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(pT->SpriteVerticeCount, pT->SpriteVertexStart);
		}

		tit++;
	}

	if (_transparentVerticeCount > 0)
	{
		// Render all transparent objects
		stride = sizeof(COLOURED_VERTEX);
		dx.SetVertexBuffers(0, 1, &_transparentVertexBuffer, &stride, &offset);
		CShaders::SelectTransparentColourShader();
		dx.Draw(_transparentVerticeCount, 0);
	}
}

TLPoint CLocation::GetPoint(PBYTE p3d2, int offset, int index, int points, float tw, float th, int objectCount, int object, int subObject)
{
	TLPoint p;
	p.ObjectIndex = object;
	p.SubObjectIndex = subObject;

	int pix = GetInt(p3d2, offset + 0 + index * 4, 4) >> 4;
	float fu = ((float)GetInt(p3d2, offset + 0 + points * 4 + index * 8, 4)) / 65536.0f;
	float fv = ((float)GetInt(p3d2, offset + 4 + points * 4 + index * 8, 4)) / 65536.0f;

	p.Point = &_points[pix + objectCount];
	p.U = min(max(0.0f, fu / tw), 1.0f);
	p.V = min(max(0.0f, fv / th), 1.0f);

	return p;
}

void CLocation::SetPosition(StartupPosition pos)
{
	_x = pos.X;
	_y = pos.Y;
	_y_adj = -pos.Elevation;
	_y_elevation = pos.Elevation;
	_z = pos.Z;
	_angle1 = 0.0f;
	_angle2 = pos.Angle;
	_y_min = pos.MinY;
	_y_max = pos.MaxY;
}

double Dot(DPoint& p1, DPoint& p2)
{
	return p1.X * p2.X + p1.Z * p2.Z;
}

DPoint Normal(DPoint& p)
{
	return { -p.Z, 0.0, p.X };
}

DPoint Subtract(DPoint& p1, DPoint& p2)
{
	DPoint p{ p1.X - p2.X, p1.Y - p2.Y, p1.Z - p2.Z };
	return p;
}

double LineSegmentsDistance(DPoint& p1, DPoint& p2, DPoint& p3, DPoint& p4, double& t, double& uc)
{
	// From https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/submissions/32487/versions/2/previews/DistBetween2Segment.m/index.html
	DPoint u = Subtract(p2, p1);
	DPoint v = Subtract(p4, p3);
	DPoint w = Subtract(p1, p3);

	double a = Dot(u, u);
	double b = Dot(u, v);
	double c = Dot(v, v);
	double d = Dot(u, w);
	double e = Dot(v, w);
	double D = a * c - b * b;
	double sc, sN, sD = D;
	double tc, tN, tD = D;

	double SMALL_NUM = 0.00000001;

	if (D < SMALL_NUM)
	{
		// the lines are almost parallel
		sN = 0.0;
		sD = 1.0;
		tN = e;
		tD = c;
	}
	else
	{
		// get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0)
		{
			// sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD)
		{
			// sc > 1 => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0)
	{
		// tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge

		if (-d < 0.0)
		{
			sN = 0.0;
		}
		else if (-d > a)
		{
			sN = sD;
		}
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{
		// tc > 1 => the t=1 edge is visible
		tN = tD;

		// recompute sc for this edge
		if ((-d + b) < 0.0)
		{
			sN = 0;
		}
		else if ((-d + b) > a)
		{
			sN = sD;
		}
		else
		{
			sN = (-d + b);
			sD = a;
		}
	}

	sc = (abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
	tc = (abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

	// get the difference of the two closest points

	DPoint dP{ w.X + u.X * sc - v.X * tc, 0.0, w.Z + u.Z * sc - v.Z * tc };

	t = sc;
	uc = tc;

	return sqrt(Dot(dP, dP));    // return the closest distance
}

void CLocation::Move(float mx, float my, float mz, float tmx)
{
	if (mx != 0.0f || my != 0.0f || mz != 0.0f)
	{
		PointingChanged = TRUE;
	}

	if (mx != 0.0f || mz != 0.0f)
	{
		double nx = _x - (mz * sin(_angle2) - mx * cos(_angle2));
		double nz = _z + (mz * cos(_angle2) + mx * sin(_angle2));

		// Check for collision with path objects
		BOOL collision = TRUE;

		DPoint pp1{ -_x, 0.0, -_z };
		DPoint pp2{ -nx, 0.0, -nz };

		// Run up to two times
		for (int pass = 0; pass < 2 && collision; pass++)
		{
			collision = FALSE;

			double closest_t = 10000.0;
			double closest_u = 0.0;
			double closest_dist = 10000.0;
			double closest_dp = 10000.0;
			Line closestLine;
			closestLine.P1.X = 0.0;
			closestLine.P1.Y = 0.0;
			closestLine.P1.Z = 0.0;
			closestLine.P2.X = 0.0;
			closestLine.P2.Y = 0.0;
			closestLine.P2.Z = 0.0;
			int closestPathIndex = -1;
			int closestPathSubIndex = -1;

			for (int i = 0; i < _pathCount; i++)
			{
				if (_paths[i].enabled)
				{
					for (size_t p1 = 0; p1 < _paths[i].Points.size(); p1++)
					{
						int p2 = p1 + 1;
						if (p2 == _paths[i].Points.size()) p2 = 0;

						DPoint pt1 = _paths[i].Points.at(p1);
						DPoint pt2 = _paths[i].Points.at(p2);

						DPoint currentLineNormal = Normalize(Normal(Subtract(pt2, pt1)));
						DPoint currentMovement = Normalize(Subtract(pp2, pp1));
						double current_dp = Dot(currentMovement, currentLineNormal);
						//if (!isnan(current_dp) && current_dp >= 0.0)	// Line should be checked
						{
							//Trace(L"Collision with line {");
							//Trace((float)pt1.X);
							//Trace(L", ");
							//Trace((float)pt1.Z);
							//Trace(L"} x {");
							//Trace((float)pt2.X);
							//Trace(L", ");
							//Trace((float)pt2.Z);
							//Trace(L"}, dot product is ");
							//TraceLine((float)current_dp);
							double t = 0.0, u = 0.0;
							double d = LineSegmentsDistance(pp1, pp2, pt1, pt2, t, u);

							// TODO: Should calculate projected distance (player position to line) and compare this value (may help edge cases)
							// TODO: Should ignore walls where the projected point is more than 0.5 units away from edge points

							if (d < 0.5 && d < closest_dist)// && t <= closest_t)
							{
								//Trace(L"Collision of point {");
								//Trace((float)pp1.X);
								//Trace(L", ");
								//Trace((float)pp1.Z);
								//Trace(L"} x {");
								//Trace((float)pp2.X);
								//Trace(L", ");
								//Trace((float)pp2.Z);
								//Trace(L"} with line {");
								//Trace((float)pt1.X);
								//Trace(L", ");
								//Trace((float)pt1.Z);
								//Trace(L"} x {");
								//Trace((float)pt2.X);
								//Trace(L", ");
								//Trace((float)pt2.Z);
								//Trace(L"}, d = ");
								//Trace((float)d);
								//Trace(L"}, t = ");
								//Trace((float)t);
								//Trace(L"}, u = ");
								//TraceLine((float)u);

								//if (current_dp < closest_dp)
								{
									closestLine.P1 = pt1;
									closestLine.P2 = pt2;
									closest_t = t;
									closest_u = u;
									closest_dist = d;
									closest_dp = current_dp;
									collision = TRUE;

									closestPathIndex = i;
									closestPathSubIndex = p1;
								}
							}
						}
					}
				}
			}

			if (collision)
			{
				//if (closest_t < 1.0)
				//{
				//	nx = _x;
				//	nz = _z;
				//	break;
				//}

				// Move player to point close to line and project

				// Calculate ProjectToX,ProjectToY and NewPointX,NewPointY
				DPoint mp = Project(closestLine.P1, closestLine.P2, pp2);
				DPoint pp = Project(closestLine.P1, closestLine.P2, pp1);

				// Check distance projection/ball to movement/ball, will give extension direction (unless ball is midway between movement and projection, but then movement is away from line)
				double distanceToMovement = sqrt(DistanceSquared(pp1, pp2));
				double playerDistanceToLine = sqrt(DistanceSquared(pp1, pp));
				double movementDistanceToLine = sqrt(DistanceSquared(pp2, mp));

				double distanceDelta = playerDistanceToLine - movementDistanceToLine;
				double percentage = 1.0;

				if (playerDistanceToLine > 0.5 && movementDistanceToLine <= 0.5)
				{
					percentage = (distanceDelta - (0.5001 - movementDistanceToLine)) / distanceDelta;
				}
				else if (playerDistanceToLine <= 0.5 && movementDistanceToLine >= playerDistanceToLine)
				{
					percentage = 1.0;
				}
				else if (playerDistanceToLine < 0.5 && movementDistanceToLine < playerDistanceToLine)
				{
					nx = _x;
					nz = _z;
					break;
				}
				else
				{
					percentage = 0.0;
				}

				DPoint tp{ pp1.X + (pp2.X - pp1.X) * percentage, 0.0, pp1.Z + (pp2.Z - pp1.Z) * percentage };
				DPoint tpp = Project(closestLine.P1, closestLine.P2, tp);
				DPoint tmp = Subtract(mp, tpp);

				pp1.X = tp.X;
				pp1.Z = tp.Z;

				pp2.X = tp.X + tmp.X;
				pp2.Z = tp.Z + tmp.Z;

				nx = (pass == 0) ? -pp2.X : -tp.X;
				nz = (pass == 0) ? -pp2.Z : -tp.Z;

				DPoint ntp{ -nx, 0.0, -nz };
				DPoint npp = Project(closestLine.P1, closestLine.P2, ntp);
				double newDistance = sqrt(DistanceSquared(ntp, npp));

				double newClosestD = 10000.0;
				int newclosestpathindex = -1;
				int newclosestPathSubIndex = -1;
				for (int i = 0; i < _pathCount; i++)
				{
					if (_paths[i].enabled)
					{
						for (size_t p1 = 0; p1 < _paths[i].Points.size(); p1++)
						{
							int p2 = p1 + 1;
							if (p2 == _paths[i].Points.size()) p2 = 0;

							DPoint xpt1 = _paths[i].Points.at(p1);
							DPoint xpt2 = _paths[i].Points.at(p2);

							DPoint xnpp = Project(xpt1, xpt2, ntp);
							double newd = sqrt(DistanceSquared(ntp, xnpp));
							if (newd < newClosestD)
							{
								newClosestD = newd;
								newclosestpathindex = i;
								newclosestPathSubIndex = p1;
							}
						}
					}
				}

				//Trace(L"Moving player to {");
				//Trace((float)pp1.X);
				//Trace(L", ");
				//Trace((float)pp1.Z);
				//Trace(L"} x {");
				//Trace((float)pp2.X);
				//Trace(L", ");
				//Trace((float)pp2.Z);
				//Trace(L"}, new distance ");
				//Trace((float)newDistance);
				//Trace(L", new distance from all ");
				//TraceLine((float)newClosestD);

				//if (newDistance < 0.5 || newClosestD < 0.5)
				//{
				//	nx = _x;
				//	nz = _z;
				//	break;
				//}
			}
			else
			{
				//Trace(L"No collision detected, checking new position distances... ");
				double newClosestD = 10000.0;
				int newclosestpathindex = -1;
				int newclosestPathSubIndex = -1;
				DPoint ntp{ nx, 0.0, nz };
				for (int i = 0; i < _pathCount; i++)
				{
					if (_paths[i].enabled)
					{
						for (size_t p1 = 0; p1 < _paths[i].Points.size(); p1++)
						{
							int p2 = p1 + 1;
							if (p2 == _paths[i].Points.size()) p2 = 0;

							DPoint xpt1 = _paths[i].Points.at(p1);
							DPoint xpt2 = _paths[i].Points.at(p2);

							DPoint xnpp = Project(xpt1, xpt2, ntp);
							double newd = sqrt(DistanceSquared(ntp, xnpp));
							if (newd < newClosestD)
							{
								newClosestD = newd;
								newclosestpathindex = i;
								newclosestPathSubIndex = p1;
							}
						}
					}
				}

				//Trace(L"New closest distance ");
				//TraceLine((float)newClosestD);

				if (newClosestD < 0.5)
				{
					int ddddd = 0;
				}
			}
		}

		_x = (float)nx;
		_z = (float)nz;

		// Check elevation
		int eix = -1;
		for (auto el : Elevations)
		{
			eix++;
			Elevation* pE = (Elevation*)el;
			float x1 = ((float)pE->X1) / 65536.0f;
			float y1 = ((float)pE->Y1) / 65536.0f;
			float z1 = ((float)pE->Z1) / 65536.0f;
			float x2 = ((float)pE->X2) / 65536.0f;
			float y2 = ((float)pE->Y2) / 65536.0f;
			float z2 = ((float)pE->Z2) / 65536.0f;

			float stepHeight = ((float)pE->StepHeight) / 65536.0f;
			float stepLength = ((float)pE->StepLength) / 65536.0f;
			float stepWidth = ((float)pE->StepWidth) / 65536.0f;

			int numberOfSteps = pE->NumberOfSteps;
			float len = ((float)pE->HorizontalLength) / 65536.0f;
			float xv = ((float)pE->DirectionX) / 65536.0f;
			float zv = ((float)pE->DirectionZ) / 65536.0f;

			float halfStepWidthX = abs(zv) * stepWidth / 1;
			float halfStepWidthZ = abs(xv) * stepWidth / 1;

			float x11 = x1 + halfStepWidthX;
			float x12 = x1 - halfStepWidthX;
			float x21 = x2 + halfStepWidthX;
			float x22 = x2 - halfStepWidthX;
			float z11 = z1 + halfStepWidthZ;
			float z12 = z1 - halfStepWidthZ;
			float z21 = z2 + halfStepWidthZ;
			float z22 = z2 - halfStepWidthZ;

			float xt1 = min(min(min(x11, x12), x21), x22), xt2 = max(max(max(x11, x12), x21), x22);
			float zt1 = min(min(min(z11, z12), z21), z22), zt2 = max(max(max(z11, z12), z21), z22);

			if (-nx >= xt1 && -nx <= xt2 && -nz >= zt1 && -nz <= zt2)
			{
				float cy = -_y_elevation;

				float miny = min(y1, y2), maxy = max(y1, y2);
				if ((cy + 3) >= miny && (cy - 3) <= maxy)
				{
					//Trace(L"Inside elevation ");
					//TraceLine(eix);
					//Trace(L"Current elevation: ");
					//TraceLine(cy);
					//Trace(L"Elevation anim: ");
					//Trace(y1);
					//Trace(L", ");
					//TraceLine(y2);
					//Trace(L"Elevation diff: ");
					//Trace(cy + y1);
					//Trace(L", ");
					//TraceLine(cy + y2);

					// TODO: Calculate full step index, then sub step value
					// TODO: Should only move up on ~10% of step length?

					// Find percentage position along elevation
					double p = (1 - abs(zv * (z21 + nz) + xv * (x21 + nx)) / len);
					double sp = 1.0 / numberOfSteps;
					double t = p / sp;
					double t2 = (double)((int)t);
					double t3 = t - t2;
					//Trace(L"t=");
					//TraceLine((float)t);
					//Trace(L"t2=");
					//TraceLine((float)t2);
					//Trace(L"t3=");
					//TraceLine((float)t3);
					if (t3 >= 0.2 && t3 < 0.5)
					{
						t2 += (t3 - 0.2) / 0.3;
					}
					else if (t3 >= 0.5)
					{
						t2 += 1.0;
					}

					//t2 += min(t3, 0.5);
					//if (t3 < 0.5) t2 += t3;// (t3 / 0.5);
					//Trace(L"SP=");
					//TraceLine((float)sp);
					//int t = (int)(p * 1000) % (int)(sp * 1000);
					//Trace(L"T2=");
					//TraceLine(t2);

					double d = -(y1 + stepHeight * t2);

					/*
					// Movement is inside an elevation rectangle
					if (xv == 0.0f)
					{
						// Elevation is in z-direction
						//d = -(y2 + (y1 - y2) * zv * (z21 + nz) / len);
						d = -(y1 + stepHeight * t2);
					}
					else if (zv == 0.0f)
					{
						// Elevation is in x-direction
						//d = -(y2 + (y1 - y2) * xv * (x21 + nx) / len);
						d = -(y1 + stepHeight * t2);
					}
					*/

					_y_elevation = static_cast<float>(d + 1.5f);
				}

				//TraceLine(L"");
			}
		}
	}

	if (my != 0.0f)
	{
		//#ifndef DEBUG
		if (_y >= _y_max && _y <= _y_min)
		{
			// Allow movement up/down to limits
			_y += (my < 0.0f) ? max(my, _y_max - _y) : min(my, _y_min - _y);
		}
		//#else
		//		_y += my;
		//#endif
	}

	UpdateSprites();
}

void CLocation::DeltaAngles(float angle1, float angle2)
{
	if (angle1 != 0.0f || angle2 != 0.0f)
	{
		PointingChanged = TRUE;
	}

	_angle1 -= angle1;
	_angle2 -= angle2;

	while (_angle2 < 0.0)
	{
		_angle2 += twopi;
	}
	while (_angle2 > twopi)
	{
		_angle2 -= twopi;
	}

	if (_angle1 > (XM_PI / 2.0f)) _angle1 = (XM_PI / 2.0f);
	else if (_angle1 < -(XM_PI / 2.0f)) _angle1 = -(XM_PI / 2.0f);
}

void CLocation::LoadPaths()
{
	// Read and parse path data
	BinaryData bdpath = GetLocationData(1);

	PBYTE ppath = bdpath.Data;

	_pathCount = GetInt(ppath, 0x1a, 2);
	_paths = new Path[_pathCount];

	std::vector<DPoint> points;
	int os = 0x80;

	int pathVerticeCount = GetInt(ppath, 0x18, 2);
	for (int v = 0; v < pathVerticeCount; v++)
	{
		float x = ((float)GetInt(ppath, os + 0, 4)) / 65536.0f;
		float y = ((float)GetInt(ppath, os + 4, 4)) / 65536.0f;
		float z = ((float)GetInt(ppath, os + 8, 4)) / 65536.0f;
		os += 12;

		DPoint p{ x, y, z };
		points.push_back(p);
	}

	for (int f = 0; f < _pathCount; f++)
	{
		if (GetInt(ppath, os, 4) == 0x45434146)
		{
			_paths[f].enabled = TRUE;
			_paths[f].allowLeave = FALSE;

			int faceSize = GetInt(ppath, os + 4, 4);
			int vertCount = GetInt(ppath, os + 8, 4);

			for (int v = 0; v < vertCount; v++)
			{
				int pix = GetInt(ppath, os + 0x15 + v * 2, 2);
				DPoint pt = points.at(pix);
				_paths[f].Points.push_back(pt);
			}

			os += faceSize;
		}
		else break;
	}
}

TLPoint CLocation::GetSpritePoint(PBYTE p3d2, int offset, int index, int objectCount, int object, int subObject)
{
	TLPoint p;
	p.ObjectIndex = object;
	p.SubObjectIndex = subObject;

	float sprite_w = ((float)GetInt(p3d2, offset + 0, 4)) / 65536.0f;
	float sprite_h = ((float)GetInt(p3d2, offset + 4, 4)) / 65536.0f;

	int subSprites = GetInt(p3d2, offset + 0x10, 4);
	int pointIndex = GetInt(p3d2, offset + 0x1c + subSprites * 32, 2) >> 4;

	float u1 = ((float)GetInt(p3d2, offset + 0x2c, 4)) / 65536.0f;
	float v1 = ((float)GetInt(p3d2, offset + 0x30, 4)) / 65536.0f;
	float u2 = ((float)GetInt(p3d2, offset + 0x34, 4)) / 65536.0f;
	float v2 = ((float)GetInt(p3d2, offset + 0x38, 4)) / 65536.0f;

	int pix = pointIndex + objectCount;
	float cx = _points[pix].X;
	float cy = _points[pix].Y;
	float cz = _points[pix].Z;

	p.Point = new Point();

	if (index == 0)
	{
		p.Point->X = cx - sprite_w / 2;
		p.Point->Y = cy - sprite_h;
		p.Point->Z = cz;
		p.U = u2;
		p.V = v2;
	}
	else if (index == 1)
	{
		p.Point->X = cx + sprite_w / 2;
		p.Point->Y = cy - sprite_h;
		p.Point->Z = cz;
		p.U = u1;
		p.V = v2;
	}
	else if (index == 2)
	{
		p.Point->X = cx + sprite_w / 2;
		p.Point->Y = cy;
		p.Point->Z = cz;
		p.U = u1;
		p.V = v1;
	}
	else if (index == 3)
	{
		p.Point->X = cx - sprite_w / 2;
		p.Point->Y = cy;
		p.Point->Z = cz;
		p.U = u2;
		p.V = v1;
	}

	return p;
}

void CLocation::UpdateSprites()
{
	if (_spriteVertexBuffer != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		ZeroMemory(&subRes, sizeof(subRes));
		dx.Map(_spriteVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes);

		TEXTURED_VERTEX* pTV = (TEXTURED_VERTEX*)subRes.pData;

		std::vector<CTextureGroup*>::iterator tit = _allTextures.begin();
		std::vector<CTextureGroup*>::iterator tend = _allTextures.end();
		int tix = 0;
		int tex = 0;
		while (tit != tend)
		{
			CTextureGroup* pTex = *tit;

			pTex->SpriteVertexStart = tix;

			std::vector<SpriteInfo>::iterator sit = pTex->SpriteInfos.begin();
			std::vector<SpriteInfo>::iterator send = pTex->SpriteInfos.end();

			while (sit != send)
			{
				SpriteInfo* spr = &(*sit);

				Point sv;
				sv.X = -_x - spr->P.X - _translationBuffer.translation[spr->ObjectIndex].x;
				sv.Z = -_z - spr->P.Z - _translationBuffer.translation[spr->ObjectIndex].z;
				float len = sqrt(sv.X * sv.X + sv.Z * sv.Z);
				if (len == 0.0f) len = 1.0f;
				sv.X /= len;
				sv.Z /= len;

				float sy1 = spr->P.Y - spr->H - spr->OY;
				float sy2 = spr->P.Y - spr->OY;

				float x1 = spr->P.X - (spr->OX + spr->W) * sv.Z;
				float x2 = spr->P.X - spr->OX * sv.Z;
				float z1 = spr->P.Z + (spr->OX + spr->W) * sv.X;
				float z2 = spr->P.Z + spr->OX * sv.X;

				// Set sprite to face player
				pTV[tix].position.x = x1;
				pTV[tix].position.y = sy1;
				pTV[tix].position.z = z1;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;
				pTV[tix].position.x = x2;
				pTV[tix].position.y = sy1;
				pTV[tix].position.z = z2;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V2;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;
				pTV[tix].position.x = x2;
				pTV[tix].position.y = sy2;
				pTV[tix].position.z = z2;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;

				pTV[tix].position.x = x1;
				pTV[tix].position.y = sy1;
				pTV[tix].position.z = z1;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;
				pTV[tix].position.x = x2;
				pTV[tix].position.y = sy2;
				pTV[tix].position.z = z2;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;
				pTV[tix].position.x = x1;
				pTV[tix].position.y = sy2;
				pTV[tix].position.z = z1;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V1;
				pTV[tix].object.x = (float)spr->ObjectIndex;
				pTV[tix].object.y = (float)spr->SubObjectIndex;
				tix++;

				sit++;
			}

			pTex->SpriteVerticeCount = tix - pTex->SpriteVertexStart;

			tit++;

			tex++;
		}

		dx.Unmap(_spriteVertexBuffer, 0);
	}
}

BOOL CLocation::Intersect(Box& boundingBox, Point& from, Point& direction)
{
	// From https://gamedev.stackexchange.com/a/150467
	float t1 = (boundingBox.X1 - from.X) * direction.X;
	float t2 = (boundingBox.X2 - from.X) * direction.X;
	float t3 = (boundingBox.Y1 - from.Y) * direction.Y;
	float t4 = (boundingBox.Y2 - from.Y) * direction.Y;
	float t5 = (boundingBox.Z1 - from.Z) * direction.Z;
	float t6 = (boundingBox.Z2 - from.Z) * direction.Z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	// if tmin > tmax, ray doesn't intersect AABB
	return (tmax >= 0.0f && tmax >= tmin);
}

int CLocation::GetPickObject(int& objectId, int& subObjectId)
{
	if (_loading)
	{
		return -1;
	}

	PointingChanged = FALSE;

	// Camera position is from _x, _y, _z
	// Camera angle (looking direction) is from _angle1 and _angle2
	// Since it's the centre of the screen we're tracing, no view/projection transform is necessary
	// Do bounding-box tests of all objects (then of sub-objects, finally triangle/polygon checks)

	Point ld;
	ld.X = cos(_angle1) * sin(-_angle2);
	ld.Y = sin(_angle1);
	ld.Z = cos(_angle1) * cos(-_angle2);

	Point perp;
	perp.X = sin(-_angle2 - XM_PI / 2);
	perp.Y = 0.0f;
	perp.Z = cos(-_angle2 - XM_PI / 2);

	float len = sqrt(ld.X * ld.X + ld.Y * ld.Y + ld.Z * ld.Z);
	ld.X /= len;
	ld.Y /= len;
	ld.Z /= len;

	Point dir;
	dir.X = 1.0f / ld.X;
	dir.Y = 1.0f / ld.Y;
	dir.Z = 1.0f / ld.Z;

	float distance = 1000000.0f;

	// Not using OcTree for hit testing
	for (int i = 0; i < _objectCount; i++)
	{
		if (_visibilityBuffer.visibility[i].x > 0.0f)	// Object is visible
		{
			Point teye;
			teye.X = -_x - _translationBuffer.translation[i].x;
			teye.Y = -(_y + _y_adj + _y_elevation) - _translationBuffer.translation[i].y;
			teye.Z = -_z - _translationBuffer.translation[i].z;

			if (Intersect(_ppObjects[i]->BoundingBox, teye, dir))
			{
				for (int j = 0; j < _ppObjects[i]->SubObjectCount; j++)
				{
					if (_ppObjects[i]->SubObjects[j].Flags != 0 && _visibilityBuffer.visibility[_ppObjects[i]->SubObjects[j].SubObjectIndex].y > 0.0f)	// Sub-object is visible
					{
						if (Intersect(_ppObjects[i]->SubObjects[j].BoundingBox, teye, dir))
						{
							DirectX::XMVECTOR veye;
							veye.m128_f32[0] = teye.X;
							veye.m128_f32[1] = teye.Y;
							veye.m128_f32[2] = teye.Z;
							veye.m128_f32[3] = 1.0f;
							DirectX::XMVECTOR vdir;
							vdir.m128_f32[0] = ld.X;
							vdir.m128_f32[1] = ld.Y;
							vdir.m128_f32[2] = ld.Z;
							vdir.m128_f32[3] = 1.0f;

							DirectX::XMVECTOR v0;
							v0.m128_f32[3] = 1.0f;
							DirectX::XMVECTOR v1;
							v1.m128_f32[3] = 1.0f;
							DirectX::XMVECTOR v2;
							v2.m128_f32[3] = 1.0f;
							float td = 10000.0f;

							if (_ppObjects[i]->SubObjects[j].PointCount == 1)
							{
								// Sprite, check bounding box for dimensions
								// TODO: Get Y and centre X/Z from point
								float y1 = _ppObjects[i]->SubObjects[j].BoundingBox.Y1;
								float y2 = _ppObjects[i]->SubObjects[j].BoundingBox.Y2;
								float cx = (_ppObjects[i]->SubObjects[j].BoundingBox.X1 + _ppObjects[i]->SubObjects[j].BoundingBox.X2) / 2.0f;
								float cz = (_ppObjects[i]->SubObjects[j].BoundingBox.Z1 + _ppObjects[i]->SubObjects[j].BoundingBox.Z2) / 2.0f;
								float w = _ppObjects[i]->SubObjects[j].BoundingBox.X2 - _ppObjects[i]->SubObjects[j].BoundingBox.X1;

								Point sv;
								sv.X = teye.X - cx;
								sv.Z = teye.Z - cz;
								float len = sqrt(sv.X * sv.X + sv.Z * sv.Z); // Distance from player
								if (len == 0.0f) len = 1.0f;
								sv.X /= len;
								sv.Z /= len;	// Normalized direction vector

								Point pp;
								pp.X = sv.Z;
								pp.Z = -sv.X;

								float ax = pp.X * w / 2;
								float az = pp.Z * w / 2;

								float x1 = cx - ax;
								float x2 = cx + ax;
								float z1 = cz - az;
								float z2 = cz + az;

								DirectX::XMVECTOR v3;
								v3.m128_f32[3] = 1.0f;

								v0.m128_f32[0] = x1;
								v0.m128_f32[1] = y1;
								v0.m128_f32[2] = z1;

								v1.m128_f32[0] = x2;
								v1.m128_f32[1] = y1;
								v1.m128_f32[2] = z2;

								v2.m128_f32[0] = x1;
								v2.m128_f32[1] = y2;
								v2.m128_f32[2] = z1;

								v3.m128_f32[0] = x2;
								v3.m128_f32[1] = y2;
								v3.m128_f32[2] = z2;

								if (DirectX::TriangleTests::Intersects(veye, vdir, v0, v3, v2, td) || DirectX::TriangleTests::Intersects(veye, vdir, v0, v1, v3, td))
								{
									if (td <= distance)
									{
										objectId = i;
										subObjectId = _ppObjects[i]->SubObjects[j].ID;
										distance = td;
									}
								}
							}
							else
							{
								int triCount = _ppObjects[i]->SubObjects[j].PointCount - 2;
								for (int t = 0; t < triCount; t++)
								{
									v0.m128_f32[0] = _ppObjects[i]->SubObjects[j].Triangles[t].P1.Point->X;
									v0.m128_f32[1] = _ppObjects[i]->SubObjects[j].Triangles[t].P1.Point->Y;
									v0.m128_f32[2] = _ppObjects[i]->SubObjects[j].Triangles[t].P1.Point->Z;

									v1.m128_f32[0] = _ppObjects[i]->SubObjects[j].Triangles[t].P2.Point->X;
									v1.m128_f32[1] = _ppObjects[i]->SubObjects[j].Triangles[t].P2.Point->Y;
									v1.m128_f32[2] = _ppObjects[i]->SubObjects[j].Triangles[t].P2.Point->Z;

									v2.m128_f32[0] = _ppObjects[i]->SubObjects[j].Triangles[t].P3.Point->X;
									v2.m128_f32[1] = _ppObjects[i]->SubObjects[j].Triangles[t].P3.Point->Y;
									v2.m128_f32[2] = _ppObjects[i]->SubObjects[j].Triangles[t].P3.Point->Z;

									if (DirectX::TriangleTests::Intersects(veye, vdir, v0, v1, v2, td))
									{
										if (td <= distance)
										{
											objectId = i;
											subObjectId = _ppObjects[i]->SubObjects[j].ID;
											distance = td;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	HitObject = objectId;
	HitSubObject = subObjectId;
	ObjectIndex = -1;
	if (HitObject >= 0)
	{
		std::vector<int>::iterator oit = _mapEntry->ObjectMap.begin();
		std::vector<int>::iterator oend = _mapEntry->ObjectMap.end();
		int ix = 0;
		while (oit != oend)
		{
			int oid = *oit;
			if (((HitSubObject & 0x80000000) != 0 && HitSubObject == oid) || ((HitSubObject & 0x8000000) != 0 && oid == (HitSubObject >> 16)) || ((HitSubObject & 0x88000000) == 0 && HitObject == oid))
			{
				ObjectIndex = ix;
				break;
			}

			oit++;
			ix++;
		}
	}

	return ObjectIndex;
}

void CLocation::StartMappedAnimation(int index)
{
	// Lookup animation map
	int mappedIndex = _mapEntry->AnimationMap.at(index);
	StartIndexedAnimation(mappedIndex);
}

void CLocation::StartIndexedAnimation(int index)
{
	BinaryData bd = GetLocationData(0);
	int animationCount = GetInt(bd.Data, 0, 4);
	if (index >= 0 && index < animationCount && index < 50)
	{
		if (Animations[index].Status == AnimationStatus::NotStarted || Animations[index].Status == AnimationStatus::Completed)
		{
			Animations[index].ObjectId = GetInt(bd.Data, index * 8 + 4, 4);
			PBYTE pA = bd.Data + GetInt(bd.Data, index * 8 + 8, 4);
			PBYTE pAE = (index < (animationCount - 1)) ? bd.Data + GetInt(bd.Data, index * 8 + 16, 4) : bd.Data + bd.Length;

			//Trace(L"Starting animation with index ");
			//Trace(index);
			//Trace(L", base is 0x");
			//TraceLine((int)pA, 16);

			int id = GetInt(pA, 4, 4);
			if (id == index)
			{
				Animations[index].Type = GetInt(pA, 0, 4);

				pA += 12;

				Animations[index].AnimDataPointerInit = pA;
				Animations[index].AnimDataPointerEnd = pAE;
				Animations[index].ConstantFrameDuration = 0;

				if (Animations[index].Type == 1)
				{
					Animations[index].Parameter = GetInt(pA, 0, 4);
					pA += 4;
				}
				else if (Animations[index].Type == 15)
				{
					Animations[index].Parameter = GetInt(pA, 0, 4);
					Animations[index].FrameDuration = Animations[index].ConstantFrameDuration = (DWORD)(GetInt(pA, 4, 4) * TIMER_SCALE);
					pA += 8;
					Animations[index].AnimDataPointerInit = pA;
				}
				else
				{
					Animations[index].Parameter = (Animations[index].Type == 3) ? Animations[index].ObjectId : -1;
				}

				Animations[index].AnimDataPointer = pA;
				Animations[index].FrameCounter = 0;
				Animations[index].FrameDuration = 0;
				Animations[index].FrameTime = 0;
				Animations[index].ParentAnim = -1;
				Animations[index].Status = AnimationStatus::Running;
			}
		}
	}
}

void CLocation::StartIdAnimation(int index)
{
	int mappedId = _mapEntry->AnimationMap.at(index);

	BinaryData bd = GetLocationData(0);
	int animationCount = GetInt(bd.Data, 0, 4);
	for (int index = 0; index < animationCount && index < 50; index++)
	{
		int animId = GetInt(bd.Data, index * 8 + 4, 4);
		if (animId == mappedId)
		{
			if (Animations[index].Status == AnimationStatus::NotStarted || Animations[index].Status == AnimationStatus::Completed)
			{
				Animations[index].ObjectId = GetInt(bd.Data, index * 8 + 4, 4);
				PBYTE pA = bd.Data + GetInt(bd.Data, index * 8 + 8, 4);
				PBYTE pAE = (index < (animationCount - 1)) ? bd.Data + GetInt(bd.Data, index * 8 + 16, 4) : bd.Data + bd.Length;

				//Trace(L"Starting animation with index ");
				//Trace(index);
				//Trace(L", base is 0x");
				//TraceLine((int)pA, 16);

				int id = GetInt(pA, 4, 4);
				if (id == index)
				{
					Animations[index].Type = GetInt(pA, 0, 4);

					pA += 12;

					Animations[index].AnimDataPointerInit = pA;
					Animations[index].AnimDataPointerEnd = pAE;
					Animations[index].ConstantFrameDuration = 0;

					if (Animations[index].Type == 1)
					{
						Animations[index].Parameter = GetInt(pA, 0, 4);
						pA += 4;
					}
					else if (Animations[index].Type == 15)
					{
						Animations[index].Parameter = GetInt(pA, 0, 4);
						Animations[index].FrameDuration = Animations[index].ConstantFrameDuration = (DWORD)(GetInt(pA, 4, 4) * TIMER_SCALE);
						pA += 8;
						Animations[index].AnimDataPointerInit = pA;
					}
					else
					{
						Animations[index].Parameter = (Animations[index].Type == 3) ? Animations[index].ObjectId : -1;
					}

					Animations[index].AnimDataPointer = pA;
					Animations[index].FrameCounter = 0;
					Animations[index].FrameDuration = 0;
					Animations[index].FrameTime = 0;
					Animations[index].ParentAnim = -1;
					Animations[index].Status = AnimationStatus::Running;
				}
			}

			break;
		}
	}
}

void CLocation::StopMappedAnimation(int index)
{
	if (index >= 0 && index < 50)
	{
		// Lookup animation map
		int mappedIndex = _mapEntry->AnimationMap.at(index);
		StopIndexedAnimation(mappedIndex);
	}
}

void CLocation::StopIndexedAnimation(int index)
{
	if (index >= 0 && index < 50)
	{

		Animations[index].Status = AnimationStatus::Completed;
	}
}

BOOL CLocation::IsAnimationFinished(int index)
{
	// Should return the mapped animation status
	int mappedIndex = _mapEntry->AnimationMap.at(index);
	return (mappedIndex >= 0 && mappedIndex < 50 && Animations[mappedIndex].Status == AnimationStatus::Completed);
}

int CLocation::GetAnimationFrame(int index)
{
	int mappedIndex = _mapEntry->AnimationMap.at(index);
	return (mappedIndex >= 0 && mappedIndex < 50) ? Animations[mappedIndex].FrameCounter : -1;
}

int CLocation::GetIndexedAnimationFrame(int index)
{
	return (index >= 0 && index < 50) ? Animations[index].FrameCounter : -1;
}

double CLocation::GetPlayerDistanceFromPoint(double x, double z)
{
	double dx = -_x - x;
	double dz = -_z - z;
	return dx * dx + dz * dz;
}

Point CLocation::GetPlayerPosition()
{
	Point p;
	p.X = -_x;
	p.Y = _y + _y_adj + _y_elevation;
	p.Z = -_z;
	return p;
}

SpritePosInfo CLocation::GetSpriteInfo(int index)
{
	// This function should be used to find the location if the eyebot
	SpritePosInfo info;
	info.Position = Point{ 0.0,0.0,0.0 };
	info.Visible = FALSE;

	if (index >= 0 && index < _objectCount)
	{
		// Look up from main data
		BinaryData bd = GetLocationData(4);

		// Find object
		LPBYTE pObj = bd.Data + GetInt(bd.Data, 0x30 + index * 4, 4) + 0x30;

		int subObjects = GetInt(pObj, 12, 4);
		if (subObjects == 1)
		{
			LPBYTE pSub = pObj + 40;
			int flags = GetInt(pSub, 8, 4);

			// Confirm it's a sprite
			if ((flags & SUBOBJECT_FLAGS_SPRITE) != 0)
			{
				// Get point
				int points = GetInt(pSub, 4, 4);
				if (points == 1)
				{
					int subSprites = GetInt(pSub, 0x38, 4);
					if (subSprites == 1)
					{
						int pointIndex = GetInt(pSub, 0x44 + subSprites * 32, 2) >> 4;
						info.Position = _points[pointIndex + _objectCount];

						// Offset by animation
						info.Position.X += _translationBuffer.translation[index].x;
						info.Position.Y += _translationBuffer.translation[index].y;
						info.Position.Z += _translationBuffer.translation[index].z;

						// Get visibility
						info.Visible = (_visibilityBuffer.visibility[index].x == 1.0f);
					}
				}
			}
		}
	}

	return info;
}

void CLocation::ModifyLocationPoints(std::wstring file)
{
	// Modify points in global list
	if (file == L"ALLEY.AP")
	{
		ModifyLocationPoints(238, 241, 0.0f, 0.1f, 0.0f);			// Plank
		ModifyLocationPoints(270, 273, 0.0f, 0.15f, 0.0f);			// Plank
		ModifyLocationPoints(1395, 1398, 0.0f, 0.1f, 0.0f);			// Shoeprint
	}
	else if (file == L"ARBOR.AP")
	{
		ModifyLocationPoints(1023, 1028, -0.1f, 0.0f, 0.0f);		// Closet door
	}
	else if (file == L"AV.AP")
	{
		ModifyLocationPoints(131, 134, 0.0f, 0.0f, 0.01f);			// Cabinet
		ModifyLocationPoints(203, 210, 0.0f, 0.0f, -0.01f);			// Pictures
	}
	else if (file == L"BEDROOM.AP")
	{
		ModifyLocationPoints(58, 61, 0.0f, 0.01f, 0.0f);			// Panties
		ModifyLocationPoints(3652, 3655, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(3837, 3840, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(4025, 4028, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(4210, 4213, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(4402, 4405, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(4591, 4594, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(4781, 4784, 0.0f, 0.0f, 0.01f);		// Drawer lock
		ModifyLocationPoints(62, 69, 0.0f, 0.0f, -0.01f);			// Air vents
	}
	else if (file == L"CASTLE.AP")
	{
		ModifyLocationPoints(87, 90, -0.01f, 0.0f, 0.0f);			// Bungee cord
	}
	else if (file == L"COLOFF.AP")
	{
		ModifyLocationPoints(20, 23, 0.0f, 0.0f, -0.01f);			// Window shades
		ModifyLocationPoints(28, 31, 0.0f, 0.0f, -0.01f);			// Window shades
		ModifyLocationPoints(36, 39, 0.0f, 0.0f, -0.01f);			// Window shades
		ModifyLocationPoints(251, 254, 0.0f, 0.0f, -0.15f);			// Painting
		ModifyLocationPoints(468, 571, 0.0f, 0.0f, -0.2f);			// Safe
		ModifyLocationPoints(2921, 2924, 0.01f, 0.0f, 0.0f);		// Computer screen
		ModifyLocationPoints(2943, 2950, 0.0f, 0.01f, 0.0f);		// Magazines
		ModifyLocationPoints(2951, 2954, 0.0f, 0.02f, 0.0f);		// Magazines
		ModifyLocationPoints(2955, 2958, 0.0f, -0.1f, 0.0f);		// Receipt
	}
	else if (file == L"COUNTESS.AP")
	{
		ModifyLocationPoints(50, 53, 0.0f, 0.01f, 0.0f);			// Trash
		ModifyLocationPoints(58, 65, 0.0f, 0.01f, 0.0f);			// Trash
		ModifyLocationPoints(70, 73, 0.0f, 0.015f, 0.0f);			// Trash
		ModifyLocationPoints(82, 85, 0.0f, -0.02f, 0.0f);			// Trash
		ModifyLocationPoints(86, 89, 0.0f, -0.01f, 0.0f);			// Trash
		ModifyLocationPoints(591, 598, 0.0f, 0.01f, 0.0f);			// Trash
		ModifyLocationPoints(599, 667, 0.0f, 0.4f, 0.0f);			// Table
		ModifyLocationPoints(668, 707, 0.0f, 0.1f, 0.0f);			// Trashcan
	}
	else if (file == L"DUBOIS.AP")
	{
		ModifyLocationPoints(120, 131, 0.0f, 0.0f, 0.01f);			// Screens
		ModifyLocationPoints(132, 139, -0.01f, 0.0f, 0.0f);			// Screens
		ModifyLocationPoints(140, 147, 0.0f, 0.0f, -0.01f);			// Screens
		ModifyLocationPoints(148, 155, 0.01f, 0.0f, 0.0f);			// Screens
		ModifyLocationPoints(156, 159, 0.0f, 0.0f, 0.01f);			// Panel
		ModifyLocationPoints(160, 171, 0.01f, 0.0f, 0.0f);			// Screens
		ModifyLocationPoints(172, 187, 0.0f, 0.0f, -0.01f);			// Screens
		ModifyLocationPoints(192, 195, -0.005f, 0.0f, 0.005f);		// Poster
		ModifyLocationPoints(260, 299, 0.0f, 0.0f, 0.3f);			// Trashcan
		ModifyLocationPoints(475, 478, 0.0f, 0.01f, 0.0f);			// Note
		ModifyLocationPoints(517, 520, 0.0f, 0.01f, 0.0f);			// Note
		ModifyLocationPoints(589, 592, 0.0f, 0.01f, 0.0f);			// Note
	}
	else if (file == L"GRSHALL.AP")
	{
		ModifyLocationPoints(613, 616, 0.0f, 0.0f, 0.01f);			// Conference room door sign
		ModifyLocationPoints(625, 628, 0.0f, 0.0f, 0.01f);			// Marcus Tucker door sign
		ModifyLocationPoints(637, 640, 0.0f, 0.0f, -0.01f);			// Supervisors door sign
		ModifyLocationPoints(649, 652, 0.0f, 0.0f, -0.01f);			// R&D door sign
		ModifyLocationPoints(524, 531, 0.0f, 0.0f, 0.01f);			// Marcus Tucker's door
	}
	else if (file == L"HALL.AP")
	{
		ModifyLocationPoints(154, 157, -1.0f, 0.0f, 0.0f);			// Jacuzzi door
		ModifyLocationPoints(158, 161, 1.0f, 0.0f, 0.0f);			// Piano room door
		ModifyLocationPoints(162, 167, 0.0f, 0.0f, 1.0f);			// Bedroom door
		ModifyLocationPoints(260, 263, 0.0f, 0.0f, -0.01f);			// Sal's list
		ModifyLocationPoints(380, 383, 0.0f, 0.01f, 0.0f);			// Gold foil
		ModifyLocationPoints(1782, 1785, 0.0f, 0.4f, 0.0f);			// Painting

		ModifyLocationPoints(174, 175, 0.0f, 0.0f, -0.01f);			// Air vents
		ModifyLocationPoints(178, 183, 0.0f, 0.0f, -0.01f);			// Air vents
		ModifyLocationPoints(185, 186, -0.01f, 0.0f, 0.0f);			// Air vents
		ModifyLocationPoints(189, 190, -0.01f, 0.0f, 0.0f);			// Air vents
		ModifyLocationPoints(193, 194, -0.01f, 0.0f, 0.0f);			// Air vents
		ModifyLocationPoints(197, 198, -0.01f, 0.0f, 0.0f);			// Air vents
		ModifyLocationPoints(202, 203, 0.0f, 0.0f, 0.01f);			// Air vents
		ModifyLocationPoints(206, 219, 0.0f, 0.0f, 0.01f);			// Air vents
		ModifyLocationPoints(228, 239, 0.01f, 0.0f, 0.0f);			// Air vents
	}
	else if (file == L"JACUZZI.AP")
	{
		ModifyLocationPoints(139, 142, -4.0f, 0.0f, 0.0f);			// Hall door
		ModifyLocationPoints(101, 104, 0.0f, -0.49f, 0.0f);			// Bikini top
		ModifyLocationPoints(420, 423, 0.0f, -0.45f, 0.0f);			// Towel on floor
		ModifyLocationPoints(118, 119, 0.0f, 0.0f, -0.01f);			// Towel on wall
		ModifyLocationPoints(121, 122, 0.0f, 0.0f, -0.01f);			// Towel on wall
	}
	else if (file == L"LIBRARY.AP")
	{
		ModifyLocationPoints(2495, 2498, -0.1f, 0.0f, 0.0f);		// Picture
		ModifyLocationPoints(2499, 2502, -0.1f, 0.0f, 0.0f);		// Picture
		ModifyLocationPoints(104, 107, 0.0f, 0.0f, -0.1f);			// Panel
	}
	else if (file == L"LIBHALL.AP")
	{
	}
	else if (file == L"MAINSTRT.AP")
	{
		ModifyLocationPoints(2933, 2988, 0.0f, 0.6f, 0.0f);			// Trash
		ModifyLocationPoints(2792, 2847, 0.0f, 0.6f, 0.0f);			// Trash
		ModifyLocationPoints(2933, 2936, 0.0f, 0.01f, 0.0f);		// Trash, extra lift
		ModifyLocationPoints(2957, 2960, 0.0f, 0.01f, 0.0f);		// Trash, extra lift
		ModifyLocationPoints(2796, 2799, 0.0f, 0.01f, 0.0f);		// Trash, extra lift
		ModifyLocationPoints(2804, 2811, 0.0f, 0.01f, 0.0f);		// Trash, extra lift
		ModifyLocationPoints(2840, 2843, 0.0f, 0.01f, 0.0f);		// Trash, extra lift
		ModifyLocationPoints(1708, 1717, 0.0f, 0.01f, 0.0f);		// Pavement, electronics shop
		ModifyLocationPoints(2304, 2305, -0.01f, 0.0f, 0.0f);		// Hotel wall, part 1
		ModifyLocationPoints(2315, 2315, -0.01f, 0.0f, 0.0f);		// Hotel wall, part 2
		ModifyLocationPoints(2321, 2321, -0.01f, 0.0f, 0.0f);		// Hotel wall, part 3
		ModifyLocationPoints(281, 281, 0.0f, 0.01f, 0.0f);			// Pavement, brew & stew, part 1
		ModifyLocationPoints(318, 318, 0.0f, 0.01f, 0.0f);			// Pavement, brew & stew, part 1
		ModifyLocationPoints(320, 320, 0.0f, 0.01f, 0.0f);			// Pavement, brew & stew, part 1
		ModifyLocationPoints(2160, 2162, 0.0f, 0.1f, 0.0f);			// Trashcans by stairs
		ModifyLocationPoints(2163, 2165, 0.0f, 0.25f, 0.0f);		// Trashcans on pavement
		ModifyLocationPoints(645, 738, 0.0f, 0.1f, 0.0f);			// Trashcan, brew & stew
		ModifyLocationPoints(1367, 1370, 0.0f, 0.0f, 0.01f);		// Electronics shop door
		ModifyLocationPoints(2667, 2670, 0.0f, 0.0f, 1.5f);			// Electronics shop background
	}
	else if (file == L"MOONHALL.AP")
	{
		ModifyLocationPoints(1033, 1036, -0.01f, 0.0f, 0.0f);		// Door sign
		ModifyLocationPoints(1045, 1048, 0.0f, 0.0f, -0.01f);		// Door sign
		ModifyLocationPoints(1057, 1060, 0.0f, 0.0f, 0.01f);		// Door sign
		ModifyLocationPoints(1069, 1072, -0.01f, 0.0f, 0.0f);		// Door sign
		ModifyLocationPoints(1097, 1100, 0.01f, 0.0f, 0.0f);		// Door sign

		ModifyLocationPoints(655, 655, 0.0f, 0.01f, 0.0f);			// Wall section
		ModifyLocationPoints(662, 662, 0.0f, 0.01f, 0.0f);			// Wall section
		ModifyLocationPoints(636, 636, 0.0f, 0.01f, 0.0f);			// Wall section
		ModifyLocationPoints(675, 675, 0.0f, 0.01f, 0.0f);			// Wall section
		ModifyLocationPoints(786, 786, 0.0f, 0.01f, 0.0f);			// Wall section
		ModifyLocationPoints(789, 789, 0.0f, 0.01f, 0.0f);			// Wall section
	}
	else if (file == L"OBSERVE.AP")
	{
		ModifyLocationPoints(106, 113, -0.01f, 0.0f, 0.0f);			// Door
		ModifyLocationPoints(881, 884, -0.02f, 0.0f, 0.0f);			// Door background
	}
	else if (file == L"PIANORM.AP")
	{
	}
	else if (file == L"RADIO.AP")
	{
		ModifyLocationPoints(741, 746, 0.0f, 0.0f, -0.01f);			// Door
	}
	else if (file == L"RUSTY.AP")
	{
		ModifyLocationPoints(506, 506, 0.0f, 0.0f, -0.01f);			// Door to back room
		ModifyLocationPoints(508, 510, 0.0f, 0.0f, -0.01f);			// Door to back room
	}
	else if (file == L"SAFEROOM.AP")
	{
		ModifyLocationPoints(104, 104, 0.0f, 0.0f, -0.01f);			// Door
		ModifyLocationPoints(106, 106, 0.0f, 0.0f, -0.01f);			// Door
		ModifyLocationPoints(109, 109, 0.0f, 0.0f, -0.01f);			// Door
		ModifyLocationPoints(111, 111, 0.0f, 0.0f, -0.01f);			// Door
		ModifyLocationPoints(418, 457, 0.0f, 0.1f, 0.0f);			// Trashcan
		ModifyLocationPoints(476, 476, 0.0f, 0.15f, 0.0f);			// Cup with pencils
	}
	else if (file == L"SECRET.AP")
	{
		ModifyLocationPoints(81, 84, 0.0f, 0.0f, -0.01f);			// Panel
	}
	else if (file == L"STUDY.AP")
	{
		ModifyLocationPoints(369, 372, 0.02f, 0.0f, 0.0f);			// Panel
		ModifyLocationPoints(608, 611, -0.005f, 0.0f, 0.0f);		// Key panel
		ModifyLocationPoints(506, 509, 0.0f, 0.0f, -0.01f);			// Painting
		ModifyLocationPoints(61, 64, 0.01f, 0.0f, 0.0f);			// Dressing screen
		ModifyLocationPoints(157, 160, 0.0f, 0.0f, 0.01f);			// Terrarium door
		ModifyLocationPoints(1112, 1115, -0.005f, 0.0f, 0.0f);		// Key panel, safe open
		ModifyLocationPoints(2510, 2510, 0.0f, 0.0f, 0.022f);		// Bathroom door
		ModifyLocationPoints(2512, 2512, 0.0f, 0.0f, 0.022f);		// Bathroom door
		ModifyLocationPoints(2545, 2545, 0.0f, 0.0f, 0.022f);		// Bathroom door
		ModifyLocationPoints(2548, 2548, 0.0f, 0.0f, 0.022f);		// Bathroom door
	}
	else if (file == L"TEXOFF.AP")
	{
		ModifyLocationPoints(68, 71, 0.0f, 0.0f, 0.01f);			// Light switch
		ModifyLocationPoints(3410, 3413, 0.0f, 0.0f, 0.01f);		// Dance school sign
		ModifyLocationPoints(2369, 2464, 0.0f, 0.235f, 0.0f);		// Hutch
		ModifyLocationPoints(3886, 3889, 0.0f, 0.01f, 0.0f);		// Letters (3882-3885 OR 3886-3889)
		ModifyLocationPoints(776, 779, 0.0f, -0.33f, 0.0f);			// Pen
		ModifyLocationPoints(957, 960, 0.0f, -0.33f, 0.0f);			// Pen
	}
	else if (file == L"WAREHOUS.AP")
	{
	}
}

void CLocation::ModifyLocationPoints(int startix, int endix, float x, float y, float z)
{
	BinaryData bd = GetLocationData(4);

	PBYTE pData = bd.Data;
	int verticeCount = GetInt(pData, 0, 4);
	int objectCount = GetInt(pData, 12, 4);
	int modCount = endix - startix;

	startix += objectCount;

	PBYTE pVertices = pData + 0x30 + objectCount * 4 + startix * 12;

	for (int i = 0; i <= modCount; i++)
	{
		float nx = x + ((float)GetInt(pVertices, 0, 4)) / 65536.0f;
		float ny = y + ((float)GetInt(pVertices, 4, 4)) / 65536.0f;
		float nz = z + ((float)GetInt(pVertices, 8, 4)) / 65536.0f;

		SetInt(pVertices, 0, (int)(nx * 65536.0f), 4);
		SetInt(pVertices, 4, (int)(ny * 65536.0f), 4);
		SetInt(pVertices, 8, (int)(nz * 65536.0f), 4);

		pVertices += 12;
	}
}

XMFLOAT4 CLocation::GetTransparentColour(std::wstring file, int objectId, int subObjectId)
{
	if (file == L"ALLEY.AP")
	{
		// Glass shard
		return XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f);
	}
	else if (file == L"CASTLE.AP")
	{
		// Field
		return XMFLOAT4(0.0f, 0.0f, 0.6f, 0.4f);
	}
	else if (file == L"JACUZZI.AP")
	{
		// Water surface
		return XMFLOAT4(0.5f, 0.5f, 0.8f, 0.4f);
	}
	else if (file == L"LIBRARY.AP")
	{
		// Window
		return XMFLOAT4(1.0f, 1.0f, 1.0f, 0.1f);
	}
	else if (file == L"RADIO.AP")
	{
		if (objectId == 22 && subObjectId == 16)
		{
			// Paper in fax machine
			return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// Blue light special box field
		return XMFLOAT4(0.3f, 0.3f, 0.7f, 0.4f);
	}
	else if (file == L"SCHANZEE.AP")
	{
		// Window/door
		return XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	}
	else if (file == L"SECRET.AP")
	{
		// Display
		return XMFLOAT4(1.0f, 1.0f, 1.0f, 0.1f);
	}
	else if (file == L"STUDY.AP")
	{
		// Unknown
		return XMFLOAT4(1.0f, 1.0f, 1.0f, 0.08f);
	}

	return XMFLOAT4(0.3f, 0.3f, 0.7f, 0.4f);
}

BinaryData CLocation::GetLocationData(int index)
{
	BinaryData bd;

	int offset = GetInt(_locationData, 2 + index * 4, 4);
	int nextOffset = GetInt(_locationData, 6 + index * 4, 4);
	bd.Data = _locationData + offset;
	bd.Length = nextOffset - offset;

	return bd;
}

void CLocation::CTextureGroup::RemovePoints(int first, int count)
{
	Points.Remove(first, count);
}

void CLocation::CTextureGroup::AddPoints(int first, int count)
{
	Points.Add(first, count);
}

void CLocation::MoveObject(int direction)
{
	if (HitObject != -1)
	{
		_translationBuffer.translation[HitObject].y += 0.1f * direction;
		_translationChanged = TRUE;
	}
}

BOOL CLocation::Load_Improved(int locationFileIndex)
{
	_loading = TRUE;

	std::wstring file = CGameController::GetFileName(locationFileIndex);

	PointingChanged = TRUE;

	// Load location entries
	CFile f;
	if (f.Open(file.c_str()))
	{
		int len = f.Size();
		_locationData = new BYTE[len];
		if (_locationData != NULL)
		{
			f.Read(_locationData, len);
		}

		f.Close();
	}

	// Modify points
	ModifyLocationPoints(file);

	// Load path
	LoadPaths();

	// Load 3D data
	BinaryData bd3d2 = GetLocationData(4);
	PBYTE p3d2 = bd3d2.Data;
	_verticeCount = GetInt(p3d2, 0, 4);
	_objectCount = GetInt(p3d2, 12, 4);

	// Quickly check which textures require transparency (flags & 0x10)
	for (int tc = 0; tc < _objectCount; tc++)
	{
		int objectOffset = GetInt(p3d2, 0x30 + tc * 4, 4) + 0x30;
		int subObjects = GetInt(p3d2, objectOffset + 12, 4);
		int nextSubOffset = objectOffset + 40;
		for (int ts = 0; ts < subObjects; ts++)
		{
			int thisSubOffset = nextSubOffset;
			nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

			int points = GetInt(p3d2, thisSubOffset + 4, 4);
			int flags = GetInt(p3d2, thisSubOffset + 8, 4);
			int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);

			if ((flags & SUBOBJECT_FLAGS_ALPHA) != 0 || points == 1)	// Flagged having alpha or is used as sprite
			{
				transparentTextures[tex] = TRUE;
			}
			else
			{
				opaqueTextures[tex] = TRUE;
			}
			processedTextures[tex] = TRUE;
		}
	}

	LoadTextures();

	LocationDataHeader* pHeader = (LocationDataHeader*)p3d2;
	LPBYTE base = p3d2 + 0x30;

	// Load vertices
	_points = new Point[_verticeCount];
	int verticeOffset = 0;
	for (int i = 0; i < _verticeCount; i++)
	{
		_points[i].X = ((float)GetInt(base, verticeOffset, 4)) / 65536.0f;
		_points[i].Y = ((float)GetInt(base, verticeOffset + 4, 4)) / 65536.0f;
		_points[i].Z = ((float)GetInt(base, verticeOffset + 8, 4)) / 65536.0f;
		verticeOffset += 12;
	}

	// Prepare the visibility buffer
	for (int i = 0; i < 3000; i++)
	{
		_visibilityBuffer.visibility[i] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Prepare the translation buffer
	for (int i = 0; i < 256; i++)
	{
		_translationBuffer.translation[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	/*
	// Prepare the object mapping list
	_objectMapCount = _mapEntry->ObjectMap.size();
	_objectMap = new ObjectMap[_objectMapCount];
	std::vector<int>::iterator oit = _mapEntry->ObjectMap.begin();
	std::vector<int>::iterator oend = _mapEntry->ObjectMap.end();
	int ix = 0;
	while (oit != oend)
	{
		_objectMap[ix].id = *oit;
		_objectMap[ix].ObjectIndex = ((_objectMap[ix].id & 0x80000800) == 0) ? _objectMap[ix].id : -1;

		ix++;
		oit++;
	}

	// Extract 3D data
	int texturedTriangles = 0;
	int transparentTriangles = 0;
	int toalObjectCount = 0;

	int spriteCount = 0;

	Point lb1, lb2;
	lb1.X = 10000.0f;
	lb2.X = -lb1.X;
	lb1.Y = lb1.X;
	lb2.Y = -lb1.X;
	lb1.Z = lb1.X;
	lb2.Z = -lb1.X;

	int six = 0;

	_pLocObjects = new CLocationObject[_objectCount];
	_pLocSubObjects = new CLocationSubObject[GetInt(p3d2, 8, 4)];
	CLocationSubObject* pCurSubObjPtr = _pLocSubObjects;

	_ppObjects = new ModelObject * [_objectCount];
	for (int i = 0; i < _objectCount; i++)
	{
		_pLocObjects[i].pSubObjects = pCurSubObjPtr;

		int objectOffset = GetInt(p3d2, 0x30 + i * 4, 4) + 0x30;
		int type = (int)GetInt(p3d2, objectOffset, 4);

		_ppObjects[i] = new ModelObject();
		ZeroMemory(_ppObjects[i], sizeof(ModelObject));
		_ppObjects[i]->Index = i;

		float ominx = 10000.0f, omaxx = -ominx;
		float ominy = ominx, omaxy = -ominx;
		float ominz = ominx, omaxz = -ominx;

		if ((type & 0x80000000) != 0)
		{
			// Object visibility
			_visibilityBuffer.visibility[i].x = -1.0f;
		}

		int unkownOffset = GetInt(p3d2, objectOffset + 0x1c, 4);

		// Header is 10 longs
		// Pointer to next sub object is first long in object

		int subObjects = GetInt(p3d2, objectOffset + 12, 4);
		toalObjectCount += subObjects;
		int nextSubOffset = objectOffset + 40;

		_ppObjects[i]->SubObjectCount = subObjects;
		_ppObjects[i]->SubObjects = new ModelSubObject[subObjects];
		ZeroMemory(_ppObjects[i]->SubObjects, subObjects * sizeof(ModelSubObject));

		_pLocObjects[i].SubObjectCount = subObjects;

		for (int j = 0; j < subObjects; j++)
		{
			float sminx = 10000.0f, smaxx = -sminx;
			float sminy = sminx, smaxy = -sminx;
			float sminz = sminx, smaxz = -sminx;

			int thisSubOffset = nextSubOffset;
			nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

			if (thisSubOffset != unkownOffset)
			{
				int points = GetInt(p3d2, thisSubOffset + 4, 4);
				int flags = GetInt(p3d2, thisSubOffset + 8, 4);
				int sid = GetInt(p3d2, thisSubOffset + 0xc, 4);
				int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);
				if ((flags & SUBOBJECT_FLAGS_TEXTURED) == 0) tex = -1;

				pCurSubObjPtr[j].Id = sid;
				pCurSubObjPtr[j].TextureIndex = tex;
				pCurSubObjPtr[j].VertexIndex = -1;
				pCurSubObjPtr[j].VertexCount = 0;

				if ((sid & 0x88000000) != 0)
				{
					int rid = (sid >> 16) & 0xffff;
					for (int z = 0; z < _objectMapCount; z++)
					{
						if (_objectMap[z].id == sid || ((rid & 0x800) != 0 && _objectMap[z].id == rid))
						{
							// TODO: This is not right! Can be set multiple times
							// TODO: May have to do a reverse mapping, setting the object id on the triangle
							//_objectMap[z].SubObjectIndex = six;
							_objectMap[z].SubObjectIndices.push_back(six);
						}
					}
				}

				if ((flags & SUBOBJECT_FLAGS_HIDDEN) != 0)
				{
					// Sub-object visibility
					_visibilityBuffer.visibility[six].y = -1.0f;
				}

				// Sub-object transparency indicator
				_visibilityBuffer.visibility[six].z = (flags & SUBOBJECT_FLAGS_ALPHA) ? 1.0f : 0.0f;

				_ppObjects[i]->SubObjects[j].ModelIndex = i;
				_ppObjects[i]->SubObjects[j].SubObjectIndex = six;
				_ppObjects[i]->SubObjects[j].Flags = flags;
				_ppObjects[i]->SubObjects[j].Texture = tex;
				_ppObjects[i]->SubObjects[j].Active = ((flags & SUBOBJECT_FLAGS_HIDDEN) == 0);
				_ppObjects[i]->SubObjects[j].ID = sid;
				_ppObjects[i]->SubObjects[j].PointCount = points;
				_ppObjects[i]->SubObjects[j].Points = new TLPoint[points];
				_ppObjects[i]->SubObjects[j].BoundingBox.X1 = _ppObjects[i]->SubObjects[j].BoundingBox.Y1 = _ppObjects[i]->SubObjects[j].BoundingBox.Z1 = 10000.0f;
				_ppObjects[i]->SubObjects[j].BoundingBox.X2 = _ppObjects[i]->SubObjects[j].BoundingBox.Y2 = _ppObjects[i]->SubObjects[j].BoundingBox.Z2 = -_ppObjects[i]->SubObjects[j].BoundingBox.X1;
				_ppObjects[i]->SubObjects[j].Triangles = (points > 2) ? new Triangle[points - 2] : NULL;

				if (points > 0)
				{
					CTextureGroup* stex = (tex >= 0) ? _allTextures.at(tex) : NULL;

					if (points == 1)
					{
						// 2020-11-27 New sprite loader

						// Find number of sub-sprites
						int subSprites = GetInt(p3d2, thisSubOffset + 0x38, 4);
						// Find sprite main point
						int spoio = thisSubOffset + 0x44 + subSprites * 32;
						int pointIndex = _objectCount + (GetInt(p3d2, thisSubOffset + 0x44 + subSprites * 32, 4) >> 4);
						float cx = _points[pointIndex].X;
						float cy = _points[pointIndex].Y;
						float cz = _points[pointIndex].Z;

						if (subSprites > 0)
						{
							// Find sprite texture dimensions
							float sw = (stex != NULL) ? stex->pTexture->Width() : 1.0f;
							float sh = (stex != NULL) ? stex->pTexture->Height() : 1.0f;
							float ssw = ((float)GetInt(p3d2, thisSubOffset + 0x30, 4)) / 65536.0f;
							float ssh = ((float)GetInt(p3d2, thisSubOffset + 0x34, 4)) / 65536.0f;

							// Find sprite world dimensions
							float sww = ((float)GetInt(p3d2, thisSubOffset + 0x28, 4)) / 65536.0f;
							float swh = ((float)GetInt(p3d2, thisSubOffset + 0x2c, 4)) / 65536.0f;

							for (int ss = 0; ss < subSprites; ss++)
							{
								// Find sub-sprite x & y offsets
								float subSpriteOffsetX = ((float)GetInt(p3d2, thisSubOffset + 0x44 + ss * 32, 4)) / 65536.0f;
								float subSpriteOffsetY = ((float)GetInt(p3d2, thisSubOffset + 0x48 + ss * 32, 4)) / 65536.0f;

								// Find sub-sprite width & height
								float subSpriteTexX1 = ((float)GetInt(p3d2, thisSubOffset + 0x54 + ss * 32, 4)) / 65536.0f;
								float subSpriteTexY1 = ((float)GetInt(p3d2, thisSubOffset + 0x58 + ss * 32, 4)) / 65536.0f;
								float subSpriteTexX2 = 1.0f + (((float)GetInt(p3d2, thisSubOffset + 0x5c + ss * 32, 4)) / 65536.0f);
								float subSpriteTexY2 = 1.0f + (((float)GetInt(p3d2, thisSubOffset + 0x60 + ss * 32, 4)) / 65536.0f);

								float subSpriteWidth = subSpriteTexX2 - subSpriteTexX1;
								float subSpriteHeight = subSpriteTexY2 - subSpriteTexY1;

								// Add sprite
								SpriteInfo si;
								si.TextureIndex = tex;
								si.P.X = cx;
								si.P.Y = cy;
								si.P.Z = cz;
								si.OX = (sww * subSpriteOffsetX) / ssw;
								si.OY = (swh * subSpriteOffsetY) / ssh;
								si.W = (sww * subSpriteWidth) / ssw;
								si.H = (swh * subSpriteHeight) / ssh;
								si.U1 = subSpriteTexX1 / sw;
								si.V1 = subSpriteTexY1 / sh;
								si.U2 = subSpriteTexX2 / sw;
								si.V2 = subSpriteTexY2 / sh;
								si.ObjectIndex = i;
								si.SubObjectIndex = six;
								si.SubObjectId = sid;

								if (stex != NULL) stex->SpriteInfos.push_back(si);

								spriteCount++;

								// Get sprite bounding box
								if ((si.P.X - si.W / 2) < sminx)
								{
									sminx = si.P.X - si.W / 2;
								}
								if ((si.P.X + si.W / 2) > smaxx)
								{
									smaxx = si.P.X + si.W / 2;
								}
								if ((si.P.Y - si.H) < sminy)
								{
									sminy = si.P.Y - si.H;
								}
								if (si.P.Y > smaxy)
								{
									smaxy = si.P.Y;
								}
								if ((si.P.Z - si.W / 2) < sminz)
								{
									sminz = si.P.Z - si.W / 2;
								}
								if ((si.P.Z + si.W / 2) > smaxz)
								{
									smaxz = si.P.Z + si.W / 2;
								}
							}

							// 2020-11-27 End of new sprite loader
						}
						else
						{
							sminx = smaxx = cx;
							sminy = smaxy = cy;
							sminz = smaxz = cz;
						}
					}
					else if (points >= 3)
					{
						Object obj;
						obj.TextureIndex = tex;

						CTexture* pt = (stex != NULL) ? stex->pTexture : NULL;
						float tw = (pt != NULL) ? pt->Width() : 0.0f;
						float th = (pt != NULL) ? pt->Height() : 0.0f;

						// Test new triangulation
						std::vector<TLPoint> vpoints;
						for (int p = 0; p < points; p++)
						{
							TLPoint px = GetPoint(p3d2, thisSubOffset + 0x28, p, points, tw, th, _objectCount, i, six);
							px.SubObjectId = j;

							if ((flags & SUBOBJECT_FLAGS_ROTATEDTEXTURE) != 0)
							{
								float tu = px.U;
								px.U = px.V;
								px.V = tu;
							}

							_ppObjects[i]->SubObjects[j].Points[p] = px;

							vpoints.push_back(px);

							// Get bounding box
							if (px.Point->X < sminx) sminx = px.Point->X;
							if (px.Point->X > smaxx) smaxx = px.Point->X;
							if (px.Point->Y < sminy) sminy = px.Point->Y;
							if (px.Point->Y > smaxy) smaxy = px.Point->Y;
							if (px.Point->Z < sminz) sminz = px.Point->Z;
							if (px.Point->Z > smaxz) smaxz = px.Point->Z;
						}

						int startp = 0;
						int tri = points - 2;
						int pointsLeft = points;
						int trix = 0;
						while (pointsLeft > 2)
						{
							int prev1 = startp - 1;
							if (prev1 < 0) prev1 += pointsLeft;
							int prev2 = startp - 2;
							if (prev2 < 0) prev2 += pointsLeft;

							// If this + 2 previous points form a proper triangle, create it and remove previous point
							TLPoint p0 = vpoints.at(startp);
							TLPoint p1 = vpoints.at(prev1);
							TLPoint p2 = vpoints.at(prev2);

							Point v1;
							v1.X = p0.Point->X - p1.Point->X;
							v1.Y = p0.Point->Y - p1.Point->Y;
							v1.Z = p0.Point->Z - p1.Point->Z;

							float len = sqrt(v1.X * v1.X + v1.Y * v1.Y + v1.Z * v1.Z);
							v1.X /= len;
							v1.Y /= len;
							v1.Z /= len;

							Point v2;
							v2.X = p1.Point->X - p2.Point->X;
							v2.Y = p1.Point->Y - p2.Point->Y;
							v2.Z = p1.Point->Z - p2.Point->Z;

							len = sqrt(v2.X * v2.X + v2.Y * v2.Y + v2.Z * v2.Z);
							v2.X /= len;
							v2.Y /= len;
							v2.Z /= len;

							if (v1.X != v2.X || v1.Y != v2.Y || v1.Z != v2.Z)
							{
								// Points form a triangle
								Triangle t1;
								t1.ObjectId = i;
								t1.SubObjectId = sid & 0xffff;
								t1.P1 = p0;
								t1.P2 = p2;
								t1.P3 = p1;

								obj.Triangles.push_back(t1);
								if (stex != NULL)
								{
									if ((flags & SUBOBJECT_FLAGS_TRANSPARENT) == 0)
									{
										// Regular textured object
										stex->Triangles.push_back(t1);
										texturedTriangles++;
										obj.VertexCount += 3;
									}
									else
									{
										// Transparent object
										stex->TransparentTriangles.push_back(t1);
										transparentTriangles++;
									}
								}

								_ppObjects[i]->SubObjects[j].Triangles[trix++] = t1;

								// Remove point at prev1
								vpoints.erase(vpoints.begin() + prev1);
								pointsLeft--;
							}
							else
							{
								// Points do not form a triangle
								//t--;
								if (pointsLeft == 3)
								{
									//break;
								}
							}

							startp++;
							if (startp >= pointsLeft) startp -= pointsLeft;
						}

						_objects.push_back(obj);
					}
					else
					{
						int debug = 0;
					}
				}
			}

			if (sminx < ominx)
			{
				ominx = sminx;
			}
			if (sminy < ominy)
			{
				ominy = sminy;
			}
			if (sminz < ominz)
			{
				ominz = sminz;
			}
			if (smaxx > omaxx)
			{
				omaxx = smaxx;
			}
			if (smaxy > omaxy)
			{
				omaxy = smaxy;
			}
			if (smaxz > omaxz)
			{
				omaxz = smaxz;
			}

			_ppObjects[i]->SubObjects[j].BoundingBox.X1 = sminx;
			_ppObjects[i]->SubObjects[j].BoundingBox.X2 = smaxx;
			_ppObjects[i]->SubObjects[j].BoundingBox.Y1 = sminy;
			_ppObjects[i]->SubObjects[j].BoundingBox.Y2 = smaxy;
			_ppObjects[i]->SubObjects[j].BoundingBox.Z1 = sminz;
			_ppObjects[i]->SubObjects[j].BoundingBox.Z2 = smaxz;

			six++;

			if (nextSubOffset < thisSubOffset) break;
		}

		pCurSubObjPtr += subObjects;

		_ppObjects[i]->BoundingBox.X1 = ominx;
		_ppObjects[i]->BoundingBox.X2 = omaxx;
		_ppObjects[i]->BoundingBox.Y1 = ominy;
		_ppObjects[i]->BoundingBox.Y2 = omaxy;
		_ppObjects[i]->BoundingBox.Z1 = ominz;
		_ppObjects[i]->BoundingBox.Z2 = omaxz;

		if (ominx < lb1.X)
		{
			lb1.X = ominx;
		}
		if (omaxx > lb2.X)
		{
			lb2.X = omaxx;
		}
		if (ominy < lb1.Y)
		{
			lb1.Y = ominy;
		}
		if (omaxy > lb2.Y)
		{
			lb2.Y = omaxy;
		}
		if (ominz < lb1.Z)
		{
			lb1.Z = ominz;
		}
		if (omaxz > lb2.Z)
		{
			lb2.Z = omaxz;
		}
	}

	// Create points vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(Point) * _verticeCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = _points;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	dx.CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer, "Location Vertices");

	// Create textured triangles
	_texturedVerticeCount = texturedTriangles * 3;
	TEXTURED_VERTEX* pTV = new TEXTURED_VERTEX[_texturedVerticeCount];
	_transparentVerticeCount = transparentTriangles * 3;
	COLOURED_VERTEX* pTTV = new COLOURED_VERTEX[_transparentVerticeCount];
	std::vector<CTextureGroup*>::iterator tit = _allTextures.begin();
	std::vector<CTextureGroup*>::iterator tend = _allTextures.end();
	int tix = 0;
	int ttix = 0;

	while (tit != tend)
	{
		CTextureGroup* pTex = *tit;

		int vertexStart = tix;

		pTex->TransparentVertexStart = ttix;

		std::vector<Triangle>::iterator trit = pTex->Triangles.begin();
		std::vector<Triangle>::iterator trend = pTex->Triangles.end();
		int curobid = -1;
		int cursubobid = -1;
		CLocationSubObject* pSub = NULL;
		while (trit != trend)
		{
			Triangle* pTri = &(*trit);
			if (pTri->ObjectId == 0 && pTri->SubObjectId == 0x004f)
			{
				int debug = 0;
			}

			if (pTri->ObjectId != curobid || pTri->SubObjectId != cursubobid)
			{
				pSub = NULL;

				CLocationObject* pOb = &_pLocObjects[pTri->ObjectId];
				for (int sx = 0; sx < pOb->SubObjectCount; sx++)
				{
					if ((pOb->pSubObjects[sx].Id & 0xffff) == pTri->SubObjectId)
					{
						pSub = &pOb->pSubObjects[sx];
						if (pSub->VertexIndex < 0)
						{
							pSub->VertexIndex = tix;
						}
						break;
					}
				}
			}

			if (pSub != NULL)
			{
				pSub->VertexCount += 3;
			}

			pTV[tix].position.x = pTri->P1.Point->X;
			pTV[tix].position.y = pTri->P1.Point->Y;
			pTV[tix].position.z = pTri->P1.Point->Z;
			pTV[tix].texture.x = pTri->P1.U;
			pTV[tix].texture.y = pTri->P1.V;
			pTV[tix].object.x = (float)pTri->P1.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P1.SubObjectIndex;
			tix++;
			pTV[tix].position.x = pTri->P2.Point->X;
			pTV[tix].position.y = pTri->P2.Point->Y;
			pTV[tix].position.z = pTri->P2.Point->Z;
			pTV[tix].texture.x = pTri->P2.U;
			pTV[tix].texture.y = pTri->P2.V;
			pTV[tix].object.x = (float)pTri->P2.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P2.SubObjectIndex;
			tix++;
			pTV[tix].position.x = pTri->P3.Point->X;
			pTV[tix].position.y = pTri->P3.Point->Y;
			pTV[tix].position.z = pTri->P3.Point->Z;
			pTV[tix].texture.x = pTri->P3.U;
			pTV[tix].texture.y = pTri->P3.V;
			pTV[tix].object.x = (float)pTri->P3.ObjectIndex;
			pTV[tix].object.y = (float)pTri->P3.SubObjectIndex;
			tix++;

			trit++;
		}

		int vertexCount = tix - vertexStart;
		if (vertexCount > 0)
		{
			pTex->Points.Add(vertexStart, vertexCount);
		}

		trit = pTex->TransparentTriangles.begin();
		trend = pTex->TransparentTriangles.end();
		while (trit != trend)
		{
			Triangle* pTri = &(*trit);

			XMFLOAT4 transparentColour = GetTransparentColour(file, pTri->P1.ObjectIndex, pTri->P1.SubObjectId);

			pTTV[ttix].position.x = pTri->P1.Point->X;
			pTTV[ttix].position.y = pTri->P1.Point->Y;
			pTTV[ttix].position.z = pTri->P1.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P1.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P1.SubObjectIndex;
			ttix++;
			pTTV[ttix].position.x = pTri->P2.Point->X;
			pTTV[ttix].position.y = pTri->P2.Point->Y;
			pTTV[ttix].position.z = pTri->P2.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P2.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P2.SubObjectIndex;
			ttix++;
			pTTV[ttix].position.x = pTri->P3.Point->X;
			pTTV[ttix].position.y = pTri->P3.Point->Y;
			pTTV[ttix].position.z = pTri->P3.Point->Z;
			pTTV[ttix].position.w = 1.0f;
			pTTV[ttix].colour = transparentColour;
			pTTV[ttix].object.x = (float)pTri->P3.ObjectIndex;
			pTTV[ttix].object.y = (float)pTri->P3.SubObjectIndex;
			ttix++;

			trit++;
		}

		pTex->TransparentVerticeCount = ttix - pTex->TransparentVertexStart;

		tit++;
	}

	// Textured buffer
	D3D11_BUFFER_DESC triBufferDesc;
	triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	triBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * _texturedVerticeCount;
	triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	triBufferDesc.MiscFlags = 0;
	triBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA triData;
	triData.pSysMem = pTV;
	triData.SysMemPitch = 0;
	triData.SysMemSlicePitch = 0;

	dx.CreateBuffer(&triBufferDesc, &triData, &_texturedVertexBuffer, "Location Textured Triangles");

	delete[] pTV;

	// Transparent buffer
	if (_transparentVerticeCount > 0)
	{
		triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		triBufferDesc.ByteWidth = sizeof(COLOURED_VERTEX) * _transparentVerticeCount;
		triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		triBufferDesc.MiscFlags = 0;
		triBufferDesc.StructureByteStride = 0;

		triData.pSysMem = pTTV;
		triData.SysMemPitch = 0;
		triData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&triBufferDesc, &triData, &_transparentVertexBuffer, "Location Transparent Triangles");
	}
	delete[] pTTV;

	// Create sprite buffer

	if (spriteCount > 0)
	{
		_spriteVerticeCount = spriteCount * 3 * 2;
		pTV = new TEXTURED_VERTEX[_spriteVerticeCount];
		tit = _allTextures.begin();
		tend = _allTextures.end();
		tix = 0;
		while (tit != tend)
		{
			CTextureGroup* pTex = *tit;

			pTex->SpriteVertexStart = tix;

			std::vector<SpriteInfo>::iterator sit = pTex->SpriteInfos.begin();
			std::vector<SpriteInfo>::iterator send = pTex->SpriteInfos.end();
			while (sit != send)
			{
				SpriteInfo* spr = &(*sit);

				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX + spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V1;
				tix++;

				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V1;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U1;
				pTV[tix].texture.y = spr->V2;
				tix++;
				pTV[tix].position.x = spr->P.X + spr->OX - spr->W / 2;
				pTV[tix].position.y = spr->P.Y + spr->OY + spr->H;
				pTV[tix].position.z = spr->P.Z;
				pTV[tix].texture.x = spr->U2;
				pTV[tix].texture.y = spr->V2;
				tix++;

				sit++;
			}

			pTex->SpriteVerticeCount = tix - pTex->SpriteVertexStart;

			tit++;
		}

		triBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		triBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * _spriteVerticeCount;
		triBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		triBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		triBufferDesc.MiscFlags = 0;
		triBufferDesc.StructureByteStride = 0;

		triData.pSysMem = pTV;
		triData.SysMemPitch = 0;
		triData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&triBufferDesc, &triData, &_spriteVertexBuffer, "Location Sprites");

		delete[] pTV;
	}

	// Load alternative textures
	if (pConfig->AlternativeMedia)
	{
		wchar_t alternateName[1024];
		ZeroMemory(alternateName, sizeof(alternateName));
		wcscat(alternateName, file.c_str());
		wchar_t* pAlter = alternateName + file.size() - 3;
		pAlter[0] = L'\\';
		pAlter++;

		tit = _allTextures.begin();
		tend = _allTextures.end();
		int tix = 0;
		while (tit != tend)
		{
			_itow(tix, pAlter, 10);
			wchar_t* pAlter2 = pAlter + wcslen(pAlter);
			wcscat(pAlter2, L".png");

			// TODO: If file exists, replace texture...
			if (CFile::Exists(alternateName))
			{
				(*tit)->pTexture->Init(alternateName);
			}

			tix++;
			tit++;
		}
	}

	_visibilityChanged = TRUE;
	_translationChanged = TRUE;

	// Start default animations
	BinaryData animbd = GetLocationData(0);
	int animationCount = GetInt(animbd.Data, 0, 4);
	for (int i = 0; i < animationCount && i < 50; i++)
	{
		int offset = GetInt(animbd.Data, 8 + i * 8, 4);
		int trigger = GetInt(animbd.Data, offset + 8, 4);
		if (trigger == 1)
		{
			StartIndexedAnimation(i);
		}
		else
		{
			// Check type, if 16, add to Elevations
			int type = GetInt(animbd.Data, offset, 4);
			if (type == 16)
			{
				Elevations.push_back(animbd.Data + offset);
			}
		}
	}

	_loading = FALSE;

	return TRUE;
	*/

	return FALSE;
}

void CLocation::SetObjectVisibility(int objectId, BOOL visible)
{
	/*
	if (_objectMap[objectId].ObjectIndex >= 0)
	{
		_visibilityBuffer.visibility[_objectMap[objectId].ObjectIndex].x = visible ? 1.0f : -1.0f;
	}
	//else if (_objectMap[objectId].SubObjectIndex >= 0)
	//{
	//	_visibility.visibility[_objectMap[objectId].SubObjectIndex].y = visible ? 1.0f : -1.0f;
	//}
	else if (_objectMap[objectId].SubObjectIndices.size() > 0)
	{
		for (auto it : _objectMap[objectId].SubObjectIndices)
		{
			_visibilityBuffer.visibility[it].y = visible ? 1.0f : -1.0f;
		}
	}
	*/

	//_visibilityBuffer.visibility[objectId].x = visible ? 1.0f : -1.0f;

	/*
	if (objectId >= 0 && objectId < _objectMapCount)
	{
		for (auto pVisibilityFloat : _objectMap[objectId].VisibilityFloatPointers)
		{
			*pVisibilityFloat = visible ? 1.0f : -1.0f;
		}

		_visibilityChanged = TRUE;
	}
	*/
	int id = _objectMap[objectId].id;
	if (id < 0x800)
	{
		ChangeVisibility(id, visible, FALSE, L"Script ");
	}
	else
	{
		// Hide master object(s) containing the item
		//Trace(L"Setting visibility of object ");
		//Trace(id, 16);
		//Trace(L" to ");
		//if (visible)
		//{
		//	TraceLine(L"VISIBLE");
		//}
		//else
		//{
		//	TraceLine(L"HIDDEN");
		//}

		for (int i = 0; i < _subObjectCount; i++)
		{
			if (((_improvedObjectMap[i].SubObjectId >> 16) & 0xffff) == id)
			{
				_visibilityBuffer.visibility[_improvedObjectMap[i].ObjectIndex].x = visible ? 1.0f : -1.0f;
			}
		}
	}
}

void CLocation::ChangeVisibility(int id, BOOL visible, BOOL setOnSubObjects, std::wstring header)
{
	//Trace((LPWSTR)header.c_str());
	//Trace(L"Setting visibility of object ");
	//Trace(id, 16);
	//Trace(L" to ");
	//if (visible)
	//{
	//	TraceLine(L"VISIBLE");
	//}
	//else
	//{
	//	TraceLine(L"HIDDEN");
	//}

	BOOL found = FALSE;
	if ((id & 0x80000000) != 0)
	{
		int objectId = (id >> 16) & 0x7fff;
		int subObjectId = id & 0xffff;
		for (int i = 0; i < _subObjectCount; i++)
		{
			if (_improvedObjectMap[i].ObjectIndex == objectId && (_improvedObjectMap[i].SubObjectId & 0xffff) == subObjectId)
			{
				_visibilityBuffer.visibility[_improvedObjectMap[i].SubObjectIndex].y = visible ? 1.0f : -1.0f;
				found = TRUE;
			}
		}
	}
	else if ((id & 0x800) != 0)
	{
		for (int i = 0; i < _subObjectCount; i++)
		{
			if (((_improvedObjectMap[i].SubObjectId >> 16) & 0xffff) == id)
			{
				_visibilityBuffer.visibility[_improvedObjectMap[i].SubObjectIndex].y = visible ? 1.0f : -1.0f;
				found = TRUE;
			}
		}
	}
	else
	{
		_visibilityBuffer.visibility[id].x = visible ? 1.0f : -1.0f;

		if (setOnSubObjects && FALSE)
		{
			for (int i = 0; i < _subObjectCount; i++)
			{
				if (_improvedObjectMap[i].ObjectIndex == id)
				{
					if ((_improvedObjectMap[i].SubObjectId & 0x80000000) == 0)
					{
						//Trace(L"Setting object ");
						//Trace(id, 16);
						//Trace(L", sub-object ");
						//Trace(_improvedObjectMap[i].SubObjectId, 16);
						//Trace(L" visibility to ");
						//if (visible)
						//{
						//	TraceLine(L"VISIBLE");
						//}
						//else
						//{
						//	TraceLine(L"HIDDEN");
						//}
						_visibilityBuffer.visibility[_improvedObjectMap[i].SubObjectIndex].y = visible ? 1.0f : -1.0f;
					}
					//else
					//{
					//	Trace(L"Visibility of object ");
					//	Trace(id, 16);
					//	Trace(L", sub-object ");
					//	Trace(_improvedObjectMap[i].SubObjectId, 16);
					//	Trace(L" remains ");
					//	if (min(_visibilityBuffer.visibility[_improvedObjectMap[i].ObjectIndex].x, _visibilityBuffer.visibility[_improvedObjectMap[i].SubObjectIndex].y) > 0.0f)
					//	{
					//		TraceLine(L"VISIBLE");
					//	}
					//	else
					//	{
					//		TraceLine(L"HIDDEN");
					//	}
					//}
				}
			}
		}

		found = TRUE;
	}

	_visibilityChanged = TRUE;
}

void CLocation::Animate()
{
	if (!_loading)
	{
		ULONGLONG now = GetTickCount64();
		for (int i = 0; i < 50; i++)
		{
			if (Animations[i].Status == AnimationStatus::Running)
			{
				ULONGLONG frameTimeDiff = now - Animations[i].FrameTime;
				if (frameTimeDiff >= Animations[i].FrameDuration)
				{
					Animations[i].FrameTime = now;
					PBYTE pA = Animations[i].AnimDataPointer;

					BOOL frameEnd = FALSE;
					while (!frameEnd)
					{
						int p1 = GetInt(pA, 0, 4);
						pA += 4;
						if (p1 != -1)
						{
							switch (Animations[i].Type)
							{
							case 1:
							{
								// Texture switch
								int objectId = Animations[i].ObjectId;
								int subObjectId = Animations[i].Parameter;
								int newTexture = p1;
								// TODO: Change texture
								// TODO: Get object, then get sub-object, find texture
								// TODO: Find object in memory, find sub-object, find texture, set real texture

								// Take 2, using list of objects and sub-objects to find texture and vertices/indexes
								CLocationObject* lo = &_pLocObjects[objectId];
								for (int soi = 0; soi < lo->SubObjectCount; soi++)
								{
									CLocationSubObject* pSub = &lo->pSubObjects[soi];
									if ((pSub->Id & 0xffff) == subObjectId)
									{
										CTextureGroup* pTG = _allTextures.at(pSub->TextureIndex);
										CTextureGroup* pNewT = _allTextures.at(newTexture);
										if (pTG->SpriteVerticeCount > 0)
										{
											pTG->RealTexture = pNewT->pTexture;
										}
										else
										{
											// TODO: Test with ALL animations of type 1 (or type 4 subtype 1)
											pTG->RemovePoints(pSub->VertexIndex, pSub->VertexCount);
											pNewT->AddPoints(pSub->VertexIndex, pSub->VertexCount);
											pSub->TextureIndex = newTexture;
										}

										break;
									}
								}

								Animations[i].FrameDuration = (DWORD)(GetInt(pA, 0, 4) * TIMER_SCALE);
								pA += 4;
								frameEnd = TRUE;
								break;
							}
							case 2:
							{
								// Object translation
								float x = ((float)p1) / 65536.0f;
								float y = ((float)GetInt(pA, 0, 4)) / 65536.0f;
								float z = ((float)GetInt(pA, 4, 4)) / 65536.0f;
								Animations[i].FrameDuration = (DWORD)(GetInt(pA, 8, 4) * TIMER_SCALE);
								pA += 12;
								_translationBuffer.translation[Animations[i].ObjectId].x += x;
								_translationBuffer.translation[Animations[i].ObjectId].y += y;
								_translationBuffer.translation[Animations[i].ObjectId].z += z;
								_translationChanged = TRUE;
								PointingChanged = TRUE;
								frameEnd = TRUE;
								break;
							}
							case 3:
							{
								// Object visibility
								// Get and hide object from parameter (must be set to -1 on initalization)
								int objectToHide = Animations[i].Parameter;
								int objectToShow = p1;
								if (objectToShow >= 0)
								{
									Animations[i].FrameDuration = (DWORD)(GetInt(pA, 0, 4) * TIMER_SCALE);
									pA += 4;

									if (objectToHide >= 0)
									{
										ChangeVisibility(objectToHide, FALSE, FALSE, L"Animation ");
									}
									ChangeVisibility(objectToShow, TRUE, FALSE, L"Animation ");
									/*
									if (objectToHide >= 0)
									{
										Trace(L"Setting visibility of object ");
										Trace(objectToHide);
										Trace(L" to HIDDEN");

										TraceLine(objectToHide);
										_visibilityBuffer.visibility[objectToHide].x = -1.0f;

									}
									Trace(L"Setting visibility of object ");
									Trace(objectToShow);
									Trace(L" to VISIBLE");

									TraceLine(objectToShow);
									_visibilityBuffer.visibility[objectToShow].x = 1.0f;
									_visibilityChanged = TRUE;
									*/

									// Store object in parameter
									Animations[i].Parameter = objectToShow;
									PointingChanged = TRUE;
								}

								frameEnd = TRUE;
								break;
							}
							case 4:
							{
								// All functions
								switch (p1)
								{
								case 1:
								{
									// Texture switch
									int objectId = GetInt(pA, 0, 4);
									int subObjectId = GetInt(pA, 4, 4);
									int newTexture = GetInt(pA, 8, 4);
									pA += 12;
									BinaryData bd3d2 = GetLocationData(4);
									PBYTE p3d2 = bd3d2.Data;
									int objectOffset = GetInt(p3d2, 0x30 + objectId * 4, 4) + 0x30;

									int subObjects = GetInt(p3d2, objectOffset + 12, 4);
									int nextSubOffset = objectOffset + 40;
									for (int ts = 0; ts < subObjects; ts++)
									{
										int thisSubOffset = nextSubOffset;
										nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

										int sid = GetInt(p3d2, thisSubOffset + 0xc, 4);
										if ((sid & 0xffff) == subObjectId)
										{
											int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);
											CTextureGroup* pTG = _allTextures.at(tex);
											if (newTexture != tex)
											{
												CTextureGroup* pNewT = _allTextures.at(newTexture);
												pTG->RealTexture = pNewT->pTexture;
											}
											else
											{
												pTG->RealTexture = NULL;
											}
										}
									}

									break;
								}
								case 2:
								{
									// Object translation
									int p1 = GetInt(pA, 0, 4);
									int p2 = GetInt(pA, 4, 4);
									int p3 = GetInt(pA, 8, 4);
									int p4 = GetInt(pA, 12, 4);
									int p5 = GetInt(pA, 16, 4);
									pA += 20;

									float x = ((float)p2) / 65536.0f;
									float y = ((float)p3) / 65536.0f;
									float z = ((float)p4) / 65536.0f;

									Animations[i].FrameDuration = (DWORD)(p5 * TIMER_SCALE);
									_translationBuffer.translation[p1].x += x;
									_translationBuffer.translation[p1].y += y;
									_translationBuffer.translation[p1].z += z;
									_translationChanged = TRUE;
									PointingChanged = TRUE;
									frameEnd = TRUE;
									break;
								}
								case 3:
								{
									// Object visibility
									// Get and hide object from parameter (must be set to -1 on initalization)
									int objectId = GetInt(pA, 0, 4);
									int visibility = GetInt(pA, 4, 4);
									pA += 8;

									ChangeVisibility(objectId, visibility, FALSE, L"Animation 4.3 ");
									/*
									Trace(L"Setting visibility of object ");
									Trace(objectId, 16);
									Trace(L" to ");
									if (visibility)
									{
										TraceLine(L"VISIBLE");
									}
									else
									{
										TraceLine(L"HIDDEN");
									}

									_visibilityBuffer.visibility[objectId].x = (visibility == 0) ? -1.0f : 1.0f;
									_visibilityChanged = TRUE;
									*/

									PointingChanged = TRUE;
									break;
								}
								case 5:
								{
									// Object + subobject visibility
									int objectId = GetInt(pA, 0, 4);
									int subObjectId = GetInt(pA, 4, 4);
									int visibility = GetInt(pA, 8, 4);
									pA += 12;

									ChangeVisibility(0x80000000 | (objectId << 16) | subObjectId, visibility, FALSE, L"Animation type 4.5 ");

									//for (int j = 0; j < _ppObjects[objectId]->SubObjectCount; j++)
									//{
									//	if ((_ppObjects[objectId]->SubObjects[j].ID & 0xffff) == subObjectId)
									//	{
											//Trace(L"Setting visibility of object ");
											//Trace(objectId);
											//Trace(L", sub-object ");
											//Trace(subObjectId);
											//Trace(L" to ");
											//if (visibility)
											//{
											//	TraceLine(L"VISIBLE");
											//}
											//else
											//{
											//	TraceLine(L"HIDDEN");
											//}

											//_visibilityBuffer.visibility[_ppObjects[objectId]->SubObjects[j].SubObjectIndex].y = (visibility != 0) ? 1.0f : -1.0f;
											//_visibilityChanged = TRUE;
									//		break;
									//	}
									//}

									PointingChanged = TRUE;
									break;
								}
								case 6:
								{
									// Start indexed animation
									int animix = GetInt(pA, 0, 4);
									pA += 4;
									StartIndexedAnimation(animix);
									break;
								}
								case 7:
								{
									// Start indexed animation and wait for it to complete
									int animix = GetInt(pA, 0, 4);
									pA += 4;
									StartIndexedAnimation(animix);
									Animations[animix].ParentAnim = i;
									Animations[i].Status = AnimationStatus::OnHold;
									frameEnd = TRUE;
									break;
								}
								case 8:
								{
									// Enable or disable path
									int pix = GetInt(pA, 0, 4);
									if (pix >= 0 && pix < _pathCount)
									{
										_paths[pix].enabled = GetInt(pA, 4, 4);
									}
									//_paths[pix].allowLeave = false;	TODO: Check if player is currently inside path, if yes, allow leave
									pA += 8;
									break;
								}
								}
								break;
							}
							case 14:
							{
								// Animated texture
								int  animation = p1;
								int texture = GetInt(pA, 0, 4);
								int duration = GetInt(pA, 4, 4);
								pA -= 4;
								CTextureGroup* pTG = _allTextures.at(animation);
								if (pTG->AnimatedTextureIndex >= pTG->Textures.size() - 1)
								{
									pTG->AnimatedTextureIndex = 0;
								}
								_allTextures.at(texture)->RealTexture = pTG->Textures.at(pTG->AnimatedTextureIndex++);
								Animations[i].FrameDuration = (DWORD)(duration * TIMER_SCALE);
								frameEnd = TRUE;
								break;
							}
							case 15:
							{
								// Textures
								int objectId = Animations[i].ObjectId;
								int subObjectId = Animations[i].Parameter;
								int newTexture = p1;
								// TODO: Change texture
								// TODO: Get object, then get sub-object, find texture
								// TODO: Find object in memory, find sub-object, find texture, set real texture

								BinaryData bd3d2 = GetLocationData(4);
								PBYTE p3d2 = bd3d2.Data;
								int objectOffset = GetInt(p3d2, 0x30 + objectId * 4, 4) + 0x30;

								int subObjects = GetInt(p3d2, objectOffset + 12, 4);
								int nextSubOffset = objectOffset + 40;
								for (int ts = 0; ts < subObjects; ts++)
								{
									int thisSubOffset = nextSubOffset;
									nextSubOffset = GetInt(p3d2, nextSubOffset, 4) + 0x30;

									int sid = GetInt(p3d2, thisSubOffset + 0xc, 4);
									if ((sid & 0xffff) == subObjectId)
									{
										int tex = GetInt(p3d2, thisSubOffset + 0x24, 4);
										CTextureGroup* pTG = _allTextures.at(tex);
										CTextureGroup* pNewT = _allTextures.at(newTexture);
										pTG->RealTexture = pNewT->pTexture;
									}
								}

								Animations[i].FrameDuration = Animations[i].ConstantFrameDuration;
								if (pA >= Animations[i].AnimDataPointerEnd)
								{
									pA = Animations[i].AnimDataPointerInit;
								}

								frameEnd = TRUE;
								break;
							}
							case 16:
							{
								// Elevation, not animated
								break;
							}
							}
						}
						else
						{
							// Post frame command (could be multiple)
							int cmd = GetInt(pA, 0, 4);
							pA += 4;
							switch (cmd)
							{
							case 0:
							{
								// Go back, x times
								int p2 = GetInt(pA, 0, 4);
								int p3 = GetInt(pA, 4, 4);
								int p4 = GetInt(pA, 8, 4);
								if (p3 == -1)
								{
									// Initialization
									p3 = p2;
								}

								SetInt(pA, 4, --p3, 4);

								pA += 12;
								if (p3 > 0)
								{
									pA -= (p4 + 4);
								}
								else
								{
									// Reset counter
									SetInt(pA, -8, -1, 4);
								}

								break;
							}
							case 1:
							{
								// Go back, always
								pA -= GetInt(pA, 0, 4);
								break;
							}
							case 2:
							{
								// End animation
								Animations[i].Status = AnimationStatus::Completed;
								if (Animations[i].ParentAnim >= 0)
								{
									// Resume parent animation
									Animations[Animations[i].ParentAnim].Status = AnimationStatus::Running;
								}
								frameEnd = TRUE;
								break;
							}
							case 3:
							{
								// Set frame duration (and end frame)
								Animations[i].FrameDuration = (DWORD)(GetInt(pA, 0, 4) * TIMER_SCALE);
								pA += 4;
								frameEnd = TRUE;
								break;
							}
							}
						}
					}

					Animations[i].AnimDataPointer = pA;
					Animations[i].FrameCounter++;
				}
			}
		}
	}
}

/*
Another attempt to solve the visibiliy issues
Make a simple list of a struct containing all sub-objects with info about master object, sub-object id and sub-object index
*/
