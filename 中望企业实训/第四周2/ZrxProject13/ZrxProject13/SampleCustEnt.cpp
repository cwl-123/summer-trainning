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
	m_radius = 0;
}

SampleCustEnt::SampleCustEnt(AcGePoint3d center, double radius, AcDbObjectId id)
{
	m_center = center;
	m_radius = radius;
	m_srcId = id;
}

// 通过id获取到圆，然后新建自定义实体
SampleCustEnt::SampleCustEnt(const AcDbObjectId& circleId)
{
	m_srcId = circleId;
	AcDbCircle *circle = NULL;
	acdbOpenAcDbObject((AcDbObject *&)circle, m_srcId, AcDb::kForWrite);
	double radius = circle->radius();
	m_center = circle->center();
	m_radius = radius + 100;

	circle->close();
}

SampleCustEnt::~SampleCustEnt() {
}


void SampleCustEnt::setCenter(AcGePoint3d mpcenter) {
	assertReadEnabled();
	m_center = mpcenter;
}

void SampleCustEnt::setRadius(double mpradius) {
	assertReadEnabled();
	m_radius = mpradius;
}

AcGePoint3d SampleCustEnt::center() {
	assertReadEnabled();
	return m_center;
}

//视口无关显示实现
Adesk::Boolean SampleCustEnt::subWorldDraw(AcGiWorldDraw *mode) {
	assertReadEnabled();
	mode->geometry().circle(m_center, m_radius, AcGeVector3d::kZAxis);
	return (AcDbEntity::subWorldDraw(mode));
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
	AcDbHardPointerId id(m_srcId);
	pFiler->writeHardPointerId(id);

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
	AcDbHardPointerId id;
	pFiler->readHardPointerId(&id);
	return (pFiler->filerStatus());
}



Acad::ErrorStatus
SampleCustEnt::subDeepClone(AcDbObject*    pOwner,
	AcDbObject*&   pClonedObject,
	AcDbIdMapping& idMap,
	Adesk::Boolean isPrimary) const
{
	//调用者理应在传进来的时候就设为NULL，但是以防万一，还是要再置空
	//否则万一没复制成功拿出去就是野指针
	pClonedObject = NULL;

	//如果这个实体已经存在于Id Map而且已经被复制过了（即Id Map里已经有与其成对的Id了），
	//就不用再复制了，直接返回
	//思考：什么情况下会发生这种事？
	bool isPrim = false;
	if (isPrimary)//isPrimary表示当前被复制的对象是被主动复制的还是因为所有关系而被复制的
		isPrim = true;
	AcDbIdPair idPair(objectId(), (AcDbObjectId)NULL,
		false, isPrim);
	if (idMap.compute(idPair) && (idPair.value() != NULL))
		return Acad::eOk;

	//创建一个类型相同的对象
	auto *pClone = (SampleCustEnt*)isA()->create();
	if (pClone != NULL)
		pClonedObject = pClone;    //这个就是复制出来的对象，可以先存到返回值里
	else
		return Acad::eOutOfMemory;

	AcDbDeepCloneFiler filer;
	dwgOut(&filer);//通过dwgOut获取本对象的数据
	filer.seek(0L, AcDb::kSeekFromStart);//重置虚拟文件的指针，方便接下来dwgIn
	pClone->dwgIn(&filer);

	//这里要设置owner，分三种情况
	//1. 如果owner是块表记录，那就直接调用appendAcDbEntity加进去
	//2. 如果owner是词典，则使用setAt加到词典里（这个例子没有体现）
	//3. 如果当前对象是因为引用关系而被复制的，则使用addAcDbObject()
	//把它加到owner的数据库里并直接设置它的owner
	bool bOwnerXlated = false;//表示owner的id是否已经转译
	if (isPrimary)
	{
		AcDbBlockTableRecord *pBTR =
			AcDbBlockTableRecord::cast(pOwner);
		if (pBTR != NULL)
		{
			pBTR->appendAcDbEntity(pClone);
			bOwnerXlated = true;
		}
		else
		{//在这个例子里这个分支其实是不可能进来的，这里只是容错
			pOwner->database()->addAcDbObject(pClone);
		}
	}
	else {
		pOwner->database()->addAcDbObject(pClone);
		pClone->setOwnerId(pOwner->objectId());
		bOwnerXlated = true;
	}


	//每一个在deepclone函数里创建的对象都要在加入Id Map前调用setAcDbObjectIdsInFlux()方法
	pClone->setAcDbObjectIdsInFlux();
	pClone->disableUndoRecording(true);//pClone现在已经在数据库里，通过这个方式可以禁用掉它的undo


	//把源对象和复制对象凑成一对，放到Id Map里
	idPair.setValue(pClonedObject->objectId());
	idPair.setIsCloned(Adesk::kTrue);
	idPair.setIsOwnerXlated(bOwnerXlated);
	idMap.assign(idPair);

	//查找源对象的所有关系，调用它们的deepClone方法
	AcDbObjectId id;
	while (filer.getNextOwnedObject(id)) {

		AcDbObject *pSubObject;
		AcDbObject *pClonedSubObject;

		//所有关系的对象id不一定都是有效的，需要做检查
		if (id == NULL)
			continue;

		//注意因为这些对象都是由于所有关系而被复制的，所以deepClone的最后一个参数(isPrimary)需要设为false
		acdbOpenAcDbObject(pSubObject, id, AcDb::kForRead);
		pClonedSubObject = NULL;
		pSubObject->deepClone(pClonedObject,
			pClonedSubObject,
			idMap, Adesk::kFalse);

		//在某特殊情况下(比如某些情况下的预览)，复制实际上只是数据“移动”了
		//pSubObject和pClonedSubObject会指向同一个对象，这个时候就不能close pSubObject了
		if (pSubObject != pClonedSubObject)
			pSubObject->close();

		//最后如果复制失败pClonedSubObject就会是空指针，需要检查
		if (pClonedSubObject != NULL)
			pClonedSubObject->close();
	}

	//注意此时pClonedObject还没有被关闭的，交由调用者来处理
	return Acad::eOk;
}

Acad::ErrorStatus SampleCustEnt::subWblockClone(
	AcRxObject*    pOwner,
	AcDbObject*&   pClonedObject,
	AcDbIdMapping& idMap,
	Adesk::Boolean isPrimary) const
{
	//记得先把返回值置空
	pClonedObject = NULL;

	//如果源数据库和目标数据库是同一个，可以省略大部分步骤，直接调用基类的wblockClone就可以了
	AcDbDatabase *pDest, *pOrig;
	idMap.destDb(pDest);
	idMap.origDb(pOrig);
	if (pDest == pOrig)
		return AcDbEntity::wblockClone(pOwner, pClonedObject,
			idMap, isPrimary);

	//如果这是在进行xbind操作，由于xbind只处理模型空间的对象，如果源对象在图纸空间，就直接返回
	AcDbObjectId pspace;
	AcDbBlockTable *pTable;
	database()->getSymbolTable(pTable, AcDb::kForRead);
	pTable->getAt(ACDB_PAPER_SPACE, pspace);
	pTable->close();
	if (idMap.deepCloneContext() == AcDb::kDcXrefBind
		&& ownerId() == pspace)
		return Acad::eOk;

	//如果源对象已经被复制过了，直接返回
	bool isPrim = false;
	if (isPrimary)
		isPrim = true;
	AcDbIdPair idPair(objectId(), (AcDbObjectId)NULL,
		false, isPrim);
	if (idMap.compute(idPair) && (idPair.value() != NULL))
		return Acad::eOk;

	//源对象的owner有可能是数据库，也有可能是一个AcDbObject，
	//如果owner是数据库，表示这个对象可能是由于硬指针关系而被复制的，它的真实的owner会在转译阶段被正确设置
	//此时pOwn的值会是NULL，作为一个标志
	AcDbObject   *pOwn = AcDbObject::cast(pOwner);
	AcDbDatabase *pDb = AcDbDatabase::cast(pOwner);
	if (pDb == NULL)
		pDb = pOwn->database();

	//创建一个同类型的对象
	auto *pClone = (SampleCustEnt*)isA()->create();
	if (pClone != NULL)
		pClonedObject = pClone;
	else
		return Acad::eOutOfMemory;

	//如果owner是个块表，或者我们能知道它到底是什么，就可以直接设置复制对象的owner
	//否则就把源对象的owner设置给复制对象，在转译阶段进行转换，这个（源对象与目标对象owner一样）也会作为转译阶段要用到的一个标志
	//要注意这代表源对象的owner也是需要被复制到目标的数据库里的，否则转译阶段会出错
	AcDbBlockTableRecord *pBTR = NULL;
	if (pOwn != NULL)
		pBTR = AcDbBlockTableRecord::cast(pOwn);
	if (pBTR != NULL && isPrimary) {
		pBTR->appendAcDbEntity(pClone);
	}
	else {
		pDb->addAcDbObject(pClonedObject);
	}

	//通过dwgOut和dwgIn复制数据到复制对象里
	AcDbWblockCloneFiler filer;
	dwgOut(&filer);
	filer.seek(0L, AcDb::kSeekFromStart);
	pClone->dwgIn(&filer);

	//添加源对象与复制对象到Id Map，注意AcDbIdPair构造函数的最后一个参数isOwnerXlated代表复制对象的owner是否需要转译
	//在这个例子里是通过上面的pOwn是否为空来判断的
	idMap.assign(AcDbIdPair(objectId(), pClonedObject->objectId(),
		Adesk::kTrue,
		isPrim, (Adesk::Boolean)(pOwn != NULL)));
	pClonedObject->setOwnerId((pOwn != NULL) ?
		pOwn->objectId() : ownerId());

	//与deepclone一样，要对每个复制对象调用setAcDbObjectIdsInFlux()
	pClone->setAcDbObjectIdsInFlux();

	//递归复制所有的硬所有和硬指针
	AcDbObjectId id;
	while (filer.getNextHardObject(id)) {
		AcDbObject *pSubObject;
		AcDbObject *pClonedSubObject;

		//某些情况下会出现引用的对象为空，需要跳过
		if (id == NULL)
			continue;

		//如果引用对象来自于另一个数据库，不需要复制
		acdbOpenAcDbObject(pSubObject, id, AcDb::kForRead);
		if (pSubObject->database() != database()) {
			pSubObject->close();
			continue;
		}

		//如上面所述，如果这个对象不是引用对象的owner，就把数据库作为owner传进去
		//另外跟deepclone一样，这里isPrimary参数要设为false
		pClonedSubObject = NULL;
		if (pSubObject->ownerId() == objectId()) {
			pSubObject->wblockClone(pClone,
				pClonedSubObject,
				idMap, Adesk::kFalse);
		}
		else {
			pSubObject->wblockClone(
				pClone->database(),
				pClonedSubObject,
				idMap, Adesk::kFalse);
		}
		pSubObject->close();

		//如果复制失败，pClonedSubObject会是空指针，需要检查
		if (pClonedSubObject != NULL)
			pClonedSubObject->close();
	}

	//与deepclone一样，复制对象由调用者来close
	return Acad::eOk;
}
