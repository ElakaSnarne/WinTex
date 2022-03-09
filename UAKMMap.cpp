#include "UAKMMap.h"
#include "BinaryData.h"
#include "LZ.h"
#include "MapData.h"
#include "Utilities.h"

BOOL CUAKMMap::Init()
{
	BinaryData map = CLZ::Decompress(L"MAP.LZ");

	//CFile file;
	//if (file.Open(L"D:\\UAKM.MAP", CFile::Mode::Write))
	//{
	//	file.Write(map.Data, map.Length);
	//	file.Close();
	//}

	if (map.Data != NULL && map.Length > 0)
	{
		LPBYTE data = map.Data;

		for (int i = 0; i < 64; i++)
		{
			CMapData* pMD = new CMapData();

			int ptr = GetInt(data, i * 4, 4);
			if (ptr > 0)
			{
				for (int j = 0; j < 10; j++)
				{
					int w = GetInt(data, ptr, 2);
					ptr += 2;
					if (w == 0xffff) break;
				}

				// Skip past file table
				for (int j = 0; j < 99; j++)
				{
					int w = GetInt(data, ptr, 2);
					ptr += 2;
					if (w == 0xffff) break;
				}

				ptr += 6;	// Skip unknown word and file index/entry pair

				pMD->ScriptFileIndex = GetInt(data, ptr, 2);
				ptr += 2;
				pMD->ScriptFileEntry = GetInt(data, ptr, 2);
				ptr += 2;

				// Read sound map 1 (ambient?)
				while (ptr < map.Length)
				{
					FileMap s;
					s.File = GetInt(data, ptr, 2);
					ptr += 2;
					if (s.File == 0xffff) break;
					s.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->EnvironmentAudioMap.push_back(s);
				}

				// Read sound map 2
				while (ptr < map.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					ptr += 2;
					if (fm.File == 0xffff) break;
					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->AudioMap.push_back(fm);
				}

				while (ptr < map.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					ptr += 2;
					if (fm.File == 0xffff) break;
					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->VideoMap.push_back(fm);
				}

				while (ptr < map.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					ptr += 2;
					if (fm.File == 0xffff) break;
					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->ImageMap.push_back(fm);
				}

				// Load 2 words, location map file index and entry ?
				pMD->LocationFileIndex = GetInt(data, ptr, 2);
				ptr += 4;	// Skipping second item, don't know what it is for (isn't it always 0?)

				while (ptr < map.Length)
				{
					int u = data[ptr++];
					if (u == 0xff) break;
					pMD->AnimationMap.push_back(u);
				}

				while (ptr < map.Length)
				{
					int u = GetInt(data, ptr, 4);
					ptr += 4;
					if (u == 0xffffffff) break;
					pMD->ObjectMap.push_back(u);
				}

				//while (ptr < map.Length)
				//{
				//	int u = data[ptr++];
				//	if (u == 0xff) break;
				//	pMD->Unknown.push_back(u);
				//}

				// TODO: Read startup positions
				ptr = GetInt(data, 0x100 + i * 4, 4);
				if (ptr > 0 && ptr < map.Length)
				{
					int nextPtr = GetInt(data, 0x104 + i * 4, 4);
					if (nextPtr != 0 && nextPtr < map.Length)
					{
						int entries = (nextPtr - ptr) / 10;
						while (ptr < nextPtr)
						{
							int x = GetInt(data, ptr, 2);
							int z = GetInt(data, ptr + 2, 2);
							int y = GetInt(data, ptr + 4, 2);
							int my = GetInt(data, ptr + 6, 2);
							int a2 = GetInt(data, ptr + 8, 2);

							StartupPosition pos;
							pos.X = 0.0f;
							pos.Y = 0.0f;
							pos.Z = 0.0f;
							pos.Angle = 0.0f;
							if (x != 0xffff && y != 0xffff && z != 0xffff && my != 0xffff && a2 != 0xffff)
							{
								if ((x & 0x8000) != 0) x |= 0xffff0000;
								if ((y & 0x8000) != 0) y |= 0xffff0000;
								if ((z & 0x8000) != 0) z |= 0xffff0000;
								if ((my & 0x8000) != 0) my |= 0xffff0000;

								float fx = ((float)x) / 16.0f;
								float fy = ((float)y) / 16.0f;
								float fz = ((float)z) / 16.0f;
								float fmy = ((float)my) / 16.0f;

								float dy = (fy - fmy) / 6.0f;
								float maxy = fmy + (dy * 8.0f);
								float miny = fmy + (dy * 1.5f);

								float a = ((float)a2) / 10.0f;

								pos.X = -fx;
								pos.Y = -fy;
								pos.Z = -fz;
								pos.MinY = -miny;
								pos.MaxY = -maxy;
								pos.Elevation = -fmy;
								pos.Angle = -a * XM_PI / 180.0f;
							}

							pMD->StartupPositions.push_back(pos);
							ptr += 10;
						}
					}
				}
			}

			_entries.push_back(pMD);
		}

		delete map.Data;

		return TRUE;
	}

	return FALSE;
}
