#pragma once
#include "base.h"

class AsdkEntTemperature : public AcRxObject
{
public:
	ACRX_DECLARE_MEMBERS(AsdkEntTemperature);

	AsdkEntTemperature(void);
	~AsdkEntTemperature(void);

	virtual double reflectedEnergy(AcDbEntity*) const = 0;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const = 0;
};
