#include "pch.h"
#include "AsdkLineTemperature.h"

AsdkLineTemperature::AsdkLineTemperature(void)
{
}


AsdkLineTemperature::~AsdkLineTemperature(void)
{
}

double AsdkLineTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkLineTemperature::reflectedEnergy"));

	return -1.0;
}

// 获取直线的中心
ZcGePoint3d AsdkLineTemperature::getCenter(AcDbEntity* pEnt) const {
	acutPrintf(_T("\n AsdkLineTemperature::getCenter"));

	AcDbLine* pLine = AcDbLine::cast(pEnt);
	if (pLine == NULL) {
		acutPrintf(_T("\n请选择一条直线"));
		return ZcGePoint3d();
	}

	ZcGePoint3d start = pLine->startPoint();
	ZcGePoint3d end = pLine->endPoint();

	pLine->close();

	return ZcGePoint3d(
		(start.x + end.x) / 2,
		(start.y + end.y) / 2,
		(start.z + end.z) / 2
	);
}


