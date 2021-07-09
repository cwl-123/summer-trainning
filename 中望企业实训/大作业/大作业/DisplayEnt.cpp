#pragma once
#include "pch.h"

#include "base.h"
#include "DisplayEnt.h"

//version控制
Adesk::UInt32 DisplayEnt::kCurrentVersionNumber = 1;

//定义夹点，上下左右中
AcGePoint3d d_left;
AcGePoint3d d_right;
AcGePoint3d d_above;
AcGePoint3d d_bottom;
AcGePoint3d d;

//注释代码，不生效
ACRX_DXF_DEFINE_MEMBERS(
	DisplayEnt, AcDbEntity,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, DisplayEnt,
	ARXPROJECT3APP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)


DisplayEnt::DisplayEnt(){}

//-----------------------------------------------------------------------------
DisplayEnt::DisplayEnt(AcGePoint3d center, double radius) : AcDbEntity()
{
	assertWriteEnabled();
	setCenter(center);
	setRadius(radius);
}

DisplayEnt::~DisplayEnt(){}

enum PartialUndoCode
{
	kRadius,
	kCenter,
};

void DisplayEnt::setCenter(AcGePoint3d mpcenter) {
	assertWriteEnabled(false);
	AcDbDwgFiler* pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		acutPrintf(_T("\n set center undo filter"));
		undoFiler()->writeAddress(DisplayEnt::desc()); //导出实体标记
		undoFiler()->writeItem((Adesk::Int16)kCenter); //导出属性标记
		undoFiler()->writePoint3d(center);
	}

	center = mpcenter; //记得先导出完再设置新的值
}

void DisplayEnt::setRadius(double mpradius) {
	assertWriteEnabled(false);
	AcDbDwgFiler *pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		acutPrintf(_T("\n set undo filter"));
		undoFiler()->writeAddress(DisplayEnt::desc()); //导出实体标记
		undoFiler()->writeItem((Adesk::Int16)kRadius); //导出属性标记
		undoFiler()->writeDouble(radius);
	}

	radius = mpradius;//记得先导出完再设置新的值
}

void DisplayEnt::setObjIdArr(AcDbObjectIdArray mpObjIdArr)
{
	assertWriteEnabled();
	objIdArr = mpObjIdArr;
}

void DisplayEnt::setOriCenter(AcGePoint3d center)
{
	assertWriteEnabled();
	oriCenter = center;
}

AcGePoint3d DisplayEnt::getCenter() {
	assertReadEnabled();
	return center;
}

double DisplayEnt::getRadius()
{
	assertReadEnabled();
	return radius;
}

AcDbObjectIdArray DisplayEnt::getObjIdArr()
{
	assertReadEnabled();
	return objIdArr;
}

AcGePoint3d DisplayEnt::getOriCenter()
{
	assertReadEnabled();
	return oriCenter;
}

// 部分undo
Acad::ErrorStatus DisplayEnt::applyPartialUndo(AcDbDwgFiler * undoFiler, AcRxClass * classObj)
{
	acutPrintf(_T("\n [applyPartialUndo]"));

	//这个classObj就是我们导出时第一个导出的实体标记
	if (classObj != DisplayEnt::desc())
		return AcDbEntity::applyPartialUndo(undoFiler, classObj);
	Adesk::Int16 shortCode;
	undoFiler->readItem(&shortCode);
	PartialUndoCode code = (PartialUndoCode)shortCode;
	double rad = 0;
	AcGePoint3d cent;

	switch (code) {//根据属性标记设置不同的属性值
	case kRadius:
		acutPrintf(_T("\n undo set radius"));

		//与dwgInFields一样，如果有多个数据，顺序要与导出时一样
		undoFiler->readDouble(&rad);
		setRadius(rad);
		break;
	case kCenter:
		acutPrintf(_T("\n undo set center"));

		//与dwgInFields一样，如果有多个数据，顺序要与导出时一样
		undoFiler->readPoint3d(&cent);
		setCenter(cent);
		break;
	default:
		assert(Adesk::kFalse);
		break;
	}
	return Acad::eOk;
}

//视口无关显示实现
Adesk::Boolean DisplayEnt::subWorldDraw(AcGiWorldDraw * mode) 
{
	assertReadEnabled();
	// 画自己
	mode->geometry().circle(center, radius, AcGeVector3d::kZAxis);
	
	AcDbObjectIdArray idArr = this->getObjIdArr();
	// 设置偏移量，将中心设置为自定义实体的中心
	mode->geometry().pushModelTransform(AcGeMatrix3d::translation(center - this->getOriCenter()));

	acutPrintf(_T("\n subWorldDraw, length: %d"), idArr.length());

	// 遍历每个实体并显示
	for (int i = 0; i < idArr.length(); i++) {
		AcDbEntity* pEnt = nullptr;

		acdbOpenAcDbEntity(pEnt, idArr[i], AcDb::kForWrite);
		if (pEnt)
		{
			pEnt->setColorIndex(1);
			mode->geometry().draw(pEnt);
			pEnt->close();
		}
	}

	mode->geometry().popModelTransform();
	return (AcDbEntity::subWorldDraw(mode));
}

//视口相关的显示实现
void DisplayEnt::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	mode->geometry().circle(center, radius, AcGeVector3d::kZAxis);
	AcDbEntity::subViewportDraw(mode);
}

//设置显示相关的属性
Adesk::UInt32 DisplayEnt::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbEntity::subSetAttributes(traits));
}

//通过矩阵变换来实现平移旋转缩放
Acad::ErrorStatus DisplayEnt::subTransformBy(const AcGeMatrix3d & xform) {
	assertWriteEnabled();
	setCenter(center.transformBy(xform));
	return Acad::eOk;
}


//向CAD发送夹点（位置）
Acad::ErrorStatus DisplayEnt::subGetGripPoints(AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds) const {
	assertReadEnabled();//授权

	d_left[X] = center[X] - radius;
	d_left[Y] = center[Y];
	d_right[X] = center[X] + radius;
	d_right[Y] = center[Y];
	d_above[X] = center[X];
	d_above[Y] = center[Y] + radius;
	d_bottom[X] = center[X];
	d_bottom[Y] = center[Y] - radius;

	gripPoints.append(center);
	gripPoints.append(d_right);
	gripPoints.append(d_left);
	gripPoints.append(d_above);
	gripPoints.append(d_bottom);
	return Acad::eOk;
}

//拖拽时定义夹点移动行为
Acad::ErrorStatus DisplayEnt::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset) {
	assertWriteEnabled();

	switch (indices[0]) {
	case(0):
		setCenter(center + offset);
		break;
	case(1):
		d = d_right + offset;
		setRadius(center.distanceTo(d));
		break;
	case(2):
		d = d_left + offset;
		setRadius(center.distanceTo(d));
		break;
	case(3):
		d = d_above + offset;
		setRadius(center.distanceTo(d));
		break;
	case(4):
		d = d_bottom + offset;
		setRadius(center.distanceTo(d));
		break;
	}
	return Acad::eOk;
}


//夹点捕捉
Acad::ErrorStatus DisplayEnt::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds
) const {
	assertReadEnabled();
	AcDbCircle circle;
	circle.setCenter(center);
	circle.setRadius(radius);
	return circle.getOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
}


//自定义实体的读取
Acad::ErrorStatus DisplayEnt::dwgOutFields(AcDbDwgFiler * pFiler) const {
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(DisplayEnt::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	pFiler->writePoint3d(center);
	pFiler->writeDouble(radius);
	pFiler->writePoint3d(oriCenter);
	// 导出要显示的实体的id的长度
	pFiler->writeInt32(objIdArr.length());
	// 导出要显示的实体的id
	for (int i = 0; i < objIdArr.length(); i++) {
		pFiler->writeHardPointerId(objIdArr[i]);
	}

	return (pFiler->filerStatus());
}

//自定义实体的保存
Acad::ErrorStatus DisplayEnt::dwgInFields(AcDbDwgFiler * pFiler) {

	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > DisplayEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	if (version < DisplayEnt::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//----- Read params
	pFiler->readPoint3d(&center);
	pFiler->readDouble(&radius);
	pFiler->readPoint3d(&oriCenter);

	int length;
	AcDbObjectIdArray arr;
	ZcDbHardPointerId _id;
	// 获取要显示的实体id数组的长度
	pFiler->readInt32(&length);
	// 设置要放大显示的实体的id
	for (int i = 0; i < length; i++) {
		pFiler->readHardPointerId(&_id);
		arr.append(_id);
	}
	this->setObjIdArr(arr);
	return (pFiler->filerStatus());
}

Acad::ErrorStatus
DisplayEnt::subDeepClone(AcDbObject*    pOwner,
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
	auto *pClone = (DisplayEnt*)isA()->create();
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

Acad::ErrorStatus DisplayEnt::subWblockClone(
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
	auto *pClone = (DisplayEnt*)isA()->create();
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