#pragma once
#include "pch.h"
#include "tchar.h"
#include "acadstrc.h"
#include "dbproxy.h"
#include"rxboiler.h"
#include"dbappgrip.h"
#include"dbidmap.h"
#include"dbcfilrs.h"
#include "SampleCustEnt.h"

//version控制
Adesk::UInt32 SampleCustEnt::kCurrentVersionNumber = 1;

//定义夹点，上下左右中
AcGePoint3d m_left;
AcGePoint3d m_right;
AcGePoint3d m_above;
AcGePoint3d m_bottom;
AcGePoint3d m;

//注释代码，不生效
ACRX_DXF_DEFINE_MEMBERS(
	SampleCustEnt, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, SAMPLECUSTENT,
	ARXPROJECT3APP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)


//-----------------------------------------------------------------------------
SampleCustEnt::SampleCustEnt() : AcDbEntity() {
}

SampleCustEnt::SampleCustEnt(AcGePoint3d center, double radius)
{
	m_center = center;
	m_radius = radius;
}

SampleCustEnt::~SampleCustEnt() {
}


void SampleCustEnt::setCenter(AcGePoint3d mpcenter) {
	m_center = mpcenter;
}

void SampleCustEnt::setRadius(double mpradius) {
	m_radius = mpradius;
}

AcGePoint3d SampleCustEnt::center() {
	return m_center;
}

//视口无关显示实现
//Adesk::Boolean SampleCustEnt::subWorldDraw(AcGiWorldDraw *mode) {
//	assertReadEnabled();
//	mode->geometry().circle(m_center, m_radius, AcGeVector3d::kZAxis);
//	return (AcDbEntity::subWorldDraw(mode));
//}

Adesk::Boolean SampleCustEnt::subWorldDraw(AcGiWorldDraw* pDraw)
{
	AcGiWorldGeometry * pGeom = &pDraw->geometry();
	//pGeom->pushModelTransform(myTransform());
	AcGiClipBoundary cb;
	cb.m_bDrawBoundary = true;
	cb.m_vNormal = AcGeVector3d::kZAxis;
	cb.m_ptPoint = AcGePoint3d::kOrigin;
	// Two points treated as a rectangle, three creates a triangle
	cb.m_aptPoints.append(AcGePoint2d(0, 0));
	cb.m_aptPoints.append(AcGePoint2d(5, 5));
	// We are clipping in our own space
	cb.m_xToClipSpace.setToIdentity();
	//cb.m_xInverseBlockRefXForm = myTransform().inverse();
	// No Z clipping
	cb.m_bClippingBack = cb.m_bClippingFront = false;
	cb.m_dFrontClipZ = cb.m_dBackClipZ = 0.;
	Adesk::Boolean bPopClipBoundary = pGeom->pushClipBoundary(&cb);
	// Draw something
	pGeom->circle(m_center, m_radius, AcGeVector3d::kZAxis);
	pGeom->popModelTransform();
	if (bPopClipBoundary) { pGeom->popClipBoundary(); }
	return true; // world-only
}


//视口相关的显示实现
void SampleCustEnt::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	mode->geometry().circle(m_center, m_radius, AcGeVector3d::kZAxis);
	AcDbEntity::subViewportDraw(mode);
}

//设置显示相关的属性
Adesk::UInt32 SampleCustEnt::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
	// return Adesk::UInt32();
}

/////////////////subTransformBy 通过矩阵变换来实现平移旋转缩放////////////////
Acad::ErrorStatus SampleCustEnt::subTransformBy(const AcGeMatrix3d& xform) {
	assertWriteEnabled();
	m_center = m_center.transformBy(xform);
	return Acad::eOk;
}


/////////////////subGetGripPoints 向CAD发送夹点（位置）////////////////
Acad::ErrorStatus SampleCustEnt::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const {
	//const :说明函数是"只读"函数，不可以修改成员变量的值，否则编译报错
	assertReadEnabled();//授权

	m_left[X] = m_center[X] - m_radius;
	m_left[Y] = m_center[Y];
	m_right[X] = m_center[X] + m_radius;
	m_right[Y] = m_center[Y];
	m_above[X] = m_center[X];
	m_above[Y] = m_center[Y] + m_radius;
	m_bottom[X] = m_center[X];
	m_bottom[Y] = m_center[Y] - m_radius;

	gripPoints.append(m_center);
	gripPoints.append(m_right);
	gripPoints.append(m_left);
	gripPoints.append(m_above);
	gripPoints.append(m_bottom);
	return Acad::eOk;
}

/////////////////subMoveGripPointsAt 拖拽时定义夹点移动行为////////////////
Acad::ErrorStatus SampleCustEnt::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d& offset) {
	assertWriteEnabled();

	switch (indices[0]) {
	case(0):
		m_center += offset;
		break;
	case(1):
		m = m_right + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(2):
		m = m_left + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(3):
		m = m_above + offset;
		m_radius = m_center.distanceTo(m);
		break;
	case(4):
		m = m_bottom + offset;
		m_radius = m_center.distanceTo(m);
		break;
	}
	return Acad::eOk;
}


/////////////////subGetOsnapPoints 夹点捕捉////////////////
Acad::ErrorStatus SampleCustEnt::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d& pickPoint,
	const AcGePoint3d& lastPoint,
	const AcGeMatrix3d& viewXform,
	AcGePoint3dArray& snapPoints,
	AcDbIntArray & geomIds
) const {
	assertReadEnabled();
	AcDbCircle circle;
	circle.setCenter(m_center);
	circle.setRadius(m_radius);
	return circle.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
}


/////////////////dwgOutFields 自定义实体的读取////////////////
Acad::ErrorStatus SampleCustEnt::dwgOutFields(AcDbDwgFiler *pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(SampleCustEnt::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writePoint3d(m_center);
	pFiler->writeDouble(m_radius);

	return (pFiler->filerStatus());
}

/////////////////dwgInFields 自定义实体的保存///////////////
Acad::ErrorStatus SampleCustEnt::dwgInFields(AcDbDwgFiler *pFiler) {
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > SampleCustEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	if (version < SampleCustEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//----- Read params
	pFiler->readPoint3d(&m_center);
	pFiler->readDouble(&m_radius);
	return (pFiler->filerStatus());
}

