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
						ReadStartupPositions(pMD, data, ptr, entries, 10);
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
