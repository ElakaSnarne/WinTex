#include "PDDMap.h"
#include "BinaryData.h"
#include "LZ.h"
#include "Utilities.h"

BOOL CPDDMap::Init()
{
	BinaryData dmap = CLZ::Decompress(L"DMAP.LZ");

	LPBYTE data = dmap.Data;
	if (data != NULL && dmap.Length > 0)
	{
		// The Pandora Directive DMap
		int offset = 0;
		LPBYTE data = dmap.Data;
		while (offset < dmap.Length)
		{
			CMapData* pMD = new CMapData();

			int ptr = GetInt(data, offset, 4);
			if (ptr == 0xffffffff)
			{
				break;
			}
			else if (ptr != 0)
			{
				// File list, FF FF
				while (ptr < dmap.Length && GetInt(data, ptr, 2) != 0xffff)
				{
					ptr += 2;
				}
				ptr += 2;

				// File list, FF FF FF FF FF FF FF FF
				while (ptr < dmap.Length && GetInt(data, ptr, 2) != 0xffff)
				{
					ptr += 2;
				}
				ptr += 8;

				// 00 00 00 00 FF FF FF ?? 00
				// Skip unknown data
				ptr += 9;

				// Script file index & entry
				pMD->ScriptFileIndex = GetInt(data, ptr, 2);
				ptr += 2;
				pMD->ScriptFileEntry = GetInt(data, ptr, 2);
				ptr += 2;

				// Video map, FF FF
				while (ptr < dmap.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					if (fm.File == 0xffff)
					{
						break;
					}

					ptr += 2;
					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->VideoMap.push_back(fm);
				}
				ptr += 2;

				// Audio map, FF FF FF FF
				while (ptr < dmap.Length)
				{
					FileMap fm;
					fm.File = GetInt(data, ptr, 2);
					if (fm.File == 0xffff)
					{
						break;
					}

					ptr += 2;
					fm.Entry = GetInt(data, ptr, 2);
					ptr += 2;

					pMD->AudioMap.push_back(fm);
				}

				// Remainder is end of list marker and presumably image data (not used)
			}

			_entries.push_back(pMD);
			offset += 4;
		}

		delete data;

		return TRUE;
	}

	return FALSE;
}
