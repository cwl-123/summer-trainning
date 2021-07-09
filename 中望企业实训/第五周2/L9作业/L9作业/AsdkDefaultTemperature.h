#pragma once
#include "base.h"

#include "AsdkEntTemperature.h"

class AsdkDefaultTemperature : public AsdkEntTemperature
{
public:
	AsdkDefaultTemperature(void);
	~AsdkDefaultTemperature(void);

	virtual double reflectedEnergy(AcDbEntity* pEnt) const;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const;
};


