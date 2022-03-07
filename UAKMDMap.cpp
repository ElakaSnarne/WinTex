#include "UAKMDMap.h"
#include "BinaryData.h"
#include "LZ.h"
#include "Utilities.h"

BOOL CUAKMDMap::Init()
{
	BinaryData dmap = CLZ::Decompress(L"DMAP.LZ");

	LPBYTE data = dmap.Data;
	if (data != NULL && dmap.Length > 0)
	{
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

				// Load a word
				ptr += 2;	// Unknown functionality

				// Load 2 words
				int w1 = GetInt(data, ptr, 2);	// Palette file index
				ptr += 2;
				int w2 = GetInt(data, ptr, 2);	// Palette file entry
				ptr += 2;
				if (w1 != 0xffff)
				{
					// Some palette info follows, first and last palette changed?
					ptr += 4;
				}
				else
				{
					ptr += 4;
				}

				while (ptr < dmap.Length)
				{
					int a = data[ptr++];
					if (a == 0xff) break;
					// TODO: Load 2 words, store in table (+load files)
					// This is not in use in UaKM (table is only referenced in script function A4, which is not used)

					int a1 = GetInt(data, ptr, 2);
					ptr += 2;
					int a2 = GetInt(data, ptr, 2);
					ptr += 2;
				}

				int f1 = GetInt(data, ptr, 2);	// File index
				ptr += 2;
				int f2 = GetInt(data, ptr, 2);	// File entry
				ptr += 2;
				if (f1 != 0xffff)
				{
					// Load file (never used)
				}

				pMD->ScriptFileIndex = GetInt(data, ptr, 2);
				ptr += 2;
				pMD->ScriptFileEntry = GetInt(data, ptr, 2);
				ptr += 2;

				while (ptr < dmap.Length)
				{
					int fi1 = GetInt(data, ptr, 2);
					ptr += 2;
					if (fi1 == 0xffff) break;

					int fe1 = GetInt(data, ptr, 2);
					ptr += 2;

					FileMap fm;
					fm.File = fi1;
					fm.Entry = fe1;
					pMD->VideoMap.push_back(fm);

					int fi2 = GetInt(data, ptr, 2);
					ptr += 2;
					int fe2 = GetInt(data, ptr, 2);
					ptr += 2;

					FileMap afm;
					afm.File = fi2;
					afm.Entry = fe2;
					pMD->AudioMap.push_back(afm);
				}

				while (ptr < dmap.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					ptr += 2;
					if (fm.File == 0xffff) break;

					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;
					pMD->ImageMap.push_back(fm);
				}
			}

			_entries.push_back(pMD);
		}

		delete dmap.Data;

		return TRUE;
	}

	return FALSE;
}
