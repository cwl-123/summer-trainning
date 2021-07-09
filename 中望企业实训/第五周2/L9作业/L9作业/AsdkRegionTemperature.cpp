#include "pch.h"
#include"dbregion.h"
#include "AsdkRegionTemperature.h"

AsdkRegionTemperature::AsdkRegionTemperature(void)
{
}


AsdkRegionTemperature::~AsdkRegionTemperature(void)
{
}


double AsdkRegionTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkRegionTemperature::reflectedEnergy"));

	return -1.0;
}

ZcGePoint3d AsdkRegionTemperature::getCenter(AcDbEntity*) const {
	acutPrintf(_T("\n not implement AsdkRegionTemperature::getCenter"));

	return ZcGePoint3d();
}
