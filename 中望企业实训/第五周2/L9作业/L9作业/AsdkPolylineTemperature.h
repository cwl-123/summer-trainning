#pragma once
#include "base.h"

#include "AsdkEntTemperature.h"

class AsdkPolylineTemperature : public AsdkEntTemperature
{
public:
	AsdkPolylineTemperature(void);
	~AsdkPolylineTemperature(void);

	virtual double reflectedEnergy(AcDbEntity* pEnt) const;
	virtual ZcGePoint3d getCenter(AcDbEntity*) const;
};