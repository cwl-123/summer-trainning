#pragma once
#include "base.h"

#include "AsdkEntTemperature.h"

class AsdkLineTemperature : public AsdkEntTemperature
{
public:
	AsdkLineTemperature(void);
	~AsdkLineTemperature(void);

	virtual double reflectedEnergy(AcDbEntity* pEnt) const;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const;
};

