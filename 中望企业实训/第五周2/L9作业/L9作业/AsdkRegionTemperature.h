#pragma once
#include "base.h"

#include "AsdkEntTemperature.h"

class AsdkRegionTemperature : public AsdkEntTemperature
{
public:
	AsdkRegionTemperature(void);
	~AsdkRegionTemperature(void);

	virtual double reflectedEnergy(AcDbEntity* pEnt) const;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const;
};


