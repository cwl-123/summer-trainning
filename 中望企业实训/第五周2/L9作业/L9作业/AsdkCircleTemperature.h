#pragma once
#include "base.h"

#include "AsdkEntTemperature.h"

class AsdkCircleTemperature : public AsdkEntTemperature
{
public:
	AsdkCircleTemperature(void);
	~AsdkCircleTemperature(void);

	virtual double reflectedEnergy(AcDbEntity* pEnt) const;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const;
};


