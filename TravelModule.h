#pragma once

#include "FullScreenModule.h"
#include "TravelImage.h"
#include "SubLocation.h"

class CTravelModule : public CFullScreenModule
{
public:
	CTravelModule();
	virtual ~CTravelModule();

	virtual void Resize(int width, int height);
	virtual void Dispose();
	virtual void Render() = NULL;

protected:
	virtual void Initialize() = NULL;

	int _travelDataOffset;

	int _selectedLocation;
	int _selectedSubLocation;
	int _selectedSubLocationEntry;

	std::unordered_map<int, CTravelImage*> _images;
	std::list<CSubLocation*> _subLocations;

	ID3D11Buffer* _selectionIndicator;

	// Input related
	virtual void BeginAction();
	virtual void Back();

	const short* _coordinates;
	const short* _hotspots;
	const signed char* _resultTable;
};
