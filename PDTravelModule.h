#pragma once

#include "TravelModule.h"
#include "RawFont.h"
#include "PDSubLocation.h"

class CPDTravelModule : public CTravelModule
{
public:
	CPDTravelModule();
	virtual ~CPDTravelModule();

	virtual void Render();

protected:
	virtual void Initialize();

	// Input related
	virtual void BeginAction();

	int _area;

	CRawFont _rawFont;

	std::unordered_map<int, BinaryData> _pdImages;

	void RenderImage(int entryIndex, int x, int y, int w, int h);
	RECT RenderSubImage(int entryIndex, int subEntryIndex, int x, int y);
	void RenderSubImage(int entryIndex, int subEntryIndex, int srcOffsetX, int srcOffsetY,  int dstX, int dstY, int w, int h);
	void RenderLocationImage(int);

	void RenderArea();
	void RenderChandlerAvenue();
	void RenderSanFrancisco();
	void RenderNorthAmerica();
	void RenderRoswell(int level);

	void RenderLocations();

	ULONGLONG _blinkTime;
	CPDSubLocation* _pDestination;
};
