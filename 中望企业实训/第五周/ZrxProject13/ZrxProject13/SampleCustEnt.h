#pragma once
#include "dbmain.h"



//此自定义实体继承于AcDbEntity，这是最常用的自定义实体父类
class __declspec(dllexport)  SampleCustEnt : public AcDbEntity {
	//
public:
	ACRX_DECLARE_MEMBERS(SampleCustEnt);//定义了一些运行时类型识别和转换函数，如cast, desc, isA等

protected:
	AcGePoint3d m_center;
	double m_radius;
	AcDbObjectId m_srcId;
	static Adesk::UInt32 kCurrentVersionNumber;//记录自定义实体的版本，方便做版本兼容

public:
	SampleCustEnt();//无参构造
	SampleCustEnt(AcGePoint3d center, double radius, AcDbObjectId id);//有参构造
	SampleCustEnt(const AcDbObjectId& id);
	virtual ~SampleCustEnt();

	void setCenter(AcGePoint3d mpcenter);
	Acad::ErrorStatus setRadius(double rad);
	AcGePoint3d center();

protected:
	Acad::ErrorStatus subWblockClone(
		AcRxObject*    pOwner,
		AcDbObject*&   pClonedObject,
		AcDbIdMapping& idMap,
		Adesk::Boolean isPrimary) const;
	Acad::ErrorStatus subDeepClone(AcDbObject*    pOwner,
			AcDbObject*&   pClonedObject,
			AcDbIdMapping& idMap,
			Adesk::Boolean isPrimary) const;
	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw *mode);//视口无关的显示
	virtual void subViewportDraw(AcGiViewportDraw* mode);//视口相关的显示
	virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits *traits);//设置显示相关的属性

	//通过矩阵变换来实现平移旋转缩放
	virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform);

	//自定义实体自己定义夹点位置，通过subGetGripPoints向CAD发送位置，
	//在发生夹点拖拽时通过subMoveGripPointsAt定义夹点移动时的行为
	virtual Acad::ErrorStatus subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const;
	virtual Acad::ErrorStatus subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d& offset);

	//当CAD可以进行对象捕捉的情况下，鼠标靠近自定义实体会触发subGetOsnapPoints函数，
	//由自定义实体来定义如何计算捕捉点
	virtual Acad::ErrorStatus subGetOsnapPoints(
		AcDb::OsnapMode osnapMode,
		Adesk::GsMarker gsSelectionMark,
		const AcGePoint3d& pickPoint,
		const AcGePoint3d& lastPoint,
		const AcGeMatrix3d& viewXform,
		AcGePoint3dArray& snapPoints,
		AcDbIntArray & geomIds
	)const;

	//保存文件 undo redo
	virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler *pFiler) const;
	virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler *pFiler);

	Acad::ErrorStatus applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj);

};

#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif
