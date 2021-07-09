#include "pch.h"
#include "AsdkPolylineTemperature.h"


AsdkPolylineTemperature::AsdkPolylineTemperature(void)
{
}


AsdkPolylineTemperature::~AsdkPolylineTemperature(void)
{
}

double AsdkPolylineTemperature::reflectedEnergy(AcDbEntity* pEnt) const
{
	acutPrintf(_T("\n not implement AsdkPolylineTemperature::reflectedEnergy"));

	return -1.0;
}

// 获取多段线的中心
ZcGePoint3d AsdkPolylineTemperature::getCenter(AcDbEntity* pEnt) const {
	acutPrintf(_T("\n AsdkPolylineTemperature::getCenter"));

	AcDbPolyline* pPolyline = AcDbPolyline::cast(pEnt);
	if (pPolyline == NULL) {
		acutPrintf(_T("\n 请选择一条多段线"));
		return ZcGePoint3d();
	}

	ZcGePoint3d startPoint, endPoint;
	double x = 0, y = 0, z = 0;
	int pointCount = pPolyline->numVerts();

	for (int i = 0; i < pointCount - 1; i++) {
		pPolyline->getPointAt(i, startPoint);
		pPolyline->getPointAt(i, endPoint);


		x += (startPoint.x + endPoint.x) / 2;
		y += (startPoint.y + endPoint.y) / 2;
		z += (startPoint.z + endPoint.z) / 2;

	}
	x /= pointCount - 1;
	y /= pointCount - 1;
	z /= pointCount - 1;

	return ZcGePoint3d(x, y, z);
}