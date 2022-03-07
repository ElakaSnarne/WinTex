#include "File.h"
#include "LZ.h"
#include "PDMap.h"
#include "Utilities.h"

BOOL CPDMap::Init()
{
	BOOL result = FALSE;

	// RMAP.AP, each entry is compressed
	CFile file;
	if (file.Open(L"RMAP.AP"))
	{
		int length = file.Size();
		LPBYTE data = new BYTE[length];
		if (data != NULL)
		{
			if (file.Read(data, length) == length)
			{
				int count = *(short*)data;
				for (int i = 0; i < (count - 1); i++)
				{
					CMapData* pMD = new CMapData();

					int mapOffset = *(int*)(data + 2 + i * 4);
					int mapLength = *(int*)(data + 6 + i * 4) - mapOffset;
					BinaryData map = CLZ::Decompress(data, mapOffset, mapLength);
					if (map.Data != NULL && map.Length > 0)
					{
						LPBYTE mdata = map.Data;

						int inPtr = 0;
						int numberOfStartupPositions = GetInt(mdata, inPtr, 4);
						int offsetToMapData = GetInt(mdata, inPtr + 4, 4);
						inPtr += 8;

						for (int sp = 0; sp < numberOfStartupPositions; sp++)
						{
							StartupPosition pos;

							int ix = GetInt(mdata, inPtr, 2);
							if ((ix & 0x8000) != 0) ix |= ~0xffff;
							inPtr += 2;
							int iz = GetInt(mdata, inPtr, 2);
							if ((iz & 0x8000) != 0) iz |= ~0xffff;
							inPtr += 2;
							int iy = GetInt(mdata, inPtr, 2);
							if ((iy & 0x8000) != 0) iy |= ~0xffff;
							inPtr += 2;
							int iym = GetInt(mdata, inPtr, 2);
							if ((iym & 0x8000) != 0) iym |= ~0xffff;
							inPtr += 2;
							int a = GetInt(mdata, inPtr, 2);
							inPtr += 2;

							float x = ((float)ix) / 16.0f;
							float z = ((float)iz) / 16.0f;
							float y = ((float)iy) / 16.0f;
							float ym = ((float)iym) / 16.0f;
							float dy = (y - ym) / 6.0f;
							float maxy = ym + (dy * 8.0f);
							float miny = ym + (dy * 1.5f);
							float fa = ((float)a) / 10.0f;

							pos.X = -x;
							pos.Y = -y;
							pos.Z = -z;
							pos.MinY = -miny;
							pos.MaxY = -maxy;
							pos.Elevation = -ym;
							pos.Angle = -fa * XM_PI / 180.0f;

							inPtr += 14;

							pMD->StartupPositions.push_back(pos);
						}

						// FF FF
						inPtr += 2;

						// List of files used (16-bit)
						while (true)
						{
							int fileIndex = GetInt(mdata, inPtr, 2);
							if (fileIndex == 0xffff) break;
							inPtr += 2;
						}

						// FF FF FF FF FF FF FF FF
						inPtr += 8;

						pMD->ScriptFileIndex = GetInt(mdata, inPtr, 2);
						inPtr += 2;
						pMD->ScriptFileEntry = GetInt(mdata, inPtr, 2);
						inPtr += 2;

						while (true)
						{
							int fileIndex = GetInt(mdata, inPtr, 2);
							if (fileIndex == 0xffff) break;
							inPtr += 2;

							int fileEntry = GetInt(mdata, inPtr, 2);
							inPtr += 2;

							FileMap fm;
							fm.File = fileIndex;
							fm.Entry = fileEntry;
							pMD->EnvironmentAudioMap.push_back(fm);
						}

						// FF FF
						inPtr += 2;

						while (true)
						{
							int fileIndex = GetInt(mdata, inPtr, 2);
							if (fileIndex == 0xffff) break;
							inPtr += 2;

							int fileEntry = GetInt(mdata, inPtr, 2);
							inPtr += 2;
							FileMap fm;
							fm.File = fileIndex;
							fm.Entry = fileEntry;
							pMD->AudioMap.push_back(fm);
						}

						// FF FF
						inPtr += 2;

						while (true)
						{
							int fileIndex = GetInt(mdata, inPtr, 2);
							if (fileIndex == 0xffff) break;
							inPtr += 2;

							int fileEntry = GetInt(mdata, inPtr, 2);
							inPtr += 2;
							FileMap fm;
							fm.File = fileIndex;
							fm.Entry = fileEntry;
							pMD->VideoMap.push_back(fm);
						}

						// FF FF FF FF
						inPtr += 4;

						pMD->LocationFileIndex = GetInt(mdata, inPtr, 2);
						inPtr += 4;

						while (true)
						{
							int index = GetInt(mdata, inPtr, 1);
							if (index == 0xff) break;
							inPtr++;
							pMD->AnimationMap.push_back(index);
						}

						// FF
						inPtr++;

						while (true)
						{
							int index = GetInt(mdata, inPtr, 4);
							if (index == -1) break;
							inPtr += 4;
							pMD->ObjectMap.push_back(index);
						}

						// FF FF FF FF
						inPtr += 4;

						// FF 00
						inPtr += 2;

						delete[] map.Data;
					}

					_entries.push_back(pMD);
				}

				result = TRUE;
			}
		}

		file.Close();
	}

	return result;
}
