#include "pch.h"
#include "tchar.h"

#include "AsdkDefaultTemperature.h"

AsdkDefaultTemperature::AsdkDefaultTemperature(void)
{
}


AsdkDefaultTemperature::~AsdkDefaultTemperature(void)
{
}

double AsdkDefaultTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkDefaultTemperature::reflectedEnergy"));

	return -1.0;
}

ZcGePoint3d AsdkDefaultTemperature::getCenter(AcDbEntity* pEnt) const {
	acutPrintf(_T("\n not implement AsdkDefaultTemperature::getCenter"));

	return ZcGePoint3d();
}
