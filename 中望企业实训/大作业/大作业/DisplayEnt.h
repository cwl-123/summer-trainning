#pragma once
#include "dbmain.h"

// 显示模块，要放大的元素会在这里显示
class DisplayEnt : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(DisplayEnt);//定义了一些运行时类型识别和转换函数，如cast, desc, isA等

	DisplayEnt();
	DisplayEnt(AcGePoint3d mpCenter, double mpRadius);
	~DisplayEnt();

	void setCenter(AcGePoint3d mpCenter);
	void setRadius(double mpRadius);
	void setObjIdArr(AcDbObjectIdArray mpObjIdArr);
	void setOriCenter(AcGePoint3d center);

	AcGePoint3d getCenter();
	double getRadius();
	AcDbObjectIdArray getObjIdArr();
	AcGePoint3d getOriCenter();

	Acad::ErrorStatus applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj);

private:
	AcGePoint3d center;
	double radius;
	AcGePoint3d oriCenter;
	AcDbObjectIdArray objIdArr;

protected:
	Acad::ErrorStatus subWblockClone(
		AcRxObject*    pOwner,
		AcDbObject*&   pClonedObject,
		AcDbIdMapping& idMap,
		Adesk::Boolean isPrimary) const;
	Acad::ErrorStatus subDeepClone(AcDbObject* pOwner,
		AcDbObject*&   pClonedObject,
		AcDbIdMapping& idMap,
		Adesk::Boolean isPrimary) const;

	static Adesk::UInt32 kCurrentVersionNumber;//记录自定义实体的版本，方便做版本兼容

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode);//视口无关的显示
	virtual void subViewportDraw(AcGiViewportDraw* mode);//视口相关的显示
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* traits);//设置显示相关的属性

	//通过矩阵变换来实现平移旋转缩放
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);

	//自定义实体自己定义夹点位置，通过subGetGripPoints向CAD发送位置，
	//在发生夹点拖拽时通过subMoveGripPointsAt定义夹点移动时的行为
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray& indices, const AcGeVector3d& offset);

	//当CAD可以进行对象捕捉的情况下，鼠标靠近自定义实体会触发subGetOsnapPoints函数，
	//由自定义实体来定义如何计算捕捉点
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray& geomIds
	)const;

	//保存文件 undo redo
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler);
};

#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DisplayEnt)
#endif
