#pragma once
#include "DXControl.h"

class CDXComboBox : public CDXControl
{
public:
	CDXComboBox();
	~CDXComboBox();

	virtual void Render() = NULL;
};
