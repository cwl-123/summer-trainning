# Enterprise Development

- 本质是C++的动态链接库
- 添加程序入口
- 添加必要的头文件引用
- 导出zcrxEntryPoint和zcrxGetApiVersion（在sdk里定义）
- 修改工程配置，包括目标拓展名、头文件和库文件的路径、库文件等。
- ZRX不能Debug
- 附加库：SDK lib-64

## 第一次作业：词典

[中望CAD二次开发环境配置及使用_serena_0916的博客-CSDN博客](https://blog.csdn.net/serena_0916/article/details/91609980)

[中望CAD二次开发自定义菜单（C++）_serena_0916的博客-CSDN博客](https://blog.csdn.net/serena_0916/article/details/95330372)

[中望CAD根据用户输入画线_serena_0916的博客-CSDN博客](https://blog.csdn.net/serena_0916/article/details/95322788?ops_request_misc=%7B%22request%5Fid%22%3A%22162251131516780265469837%22%2C%22scm%22%3A%2220140713.130102334.pc%5Fblog.%22%7D&request_id=162251131516780265469837&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_v2~rank_v29-3-95322788.nonecase&utm_term=CAD&spm=1018.2226.3001.4450)

直线

```c
#include <dbapserv.h>

void helloworld()
{
	acutPrintf(_T("\nHello World"));
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForRead);//从数据库获取块表
	if (pBT)//判断是否成功
	{
		acutPrintf(_T("\n获取块表成功"));
		AcDbBlockTableRecord* pBTR = nullptr;
		//ACDB_MODEL_SPACE = "*Model_Space"
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			acutPrintf(_T("\n成功"));
			AcGePoint3d startPt(4.0, 2.0, 0.0);
			AcGePoint3d endPt(10.0, 7.0, 0.0);
			AcDbLine *pLine = new AcDbLine(startPt, endPt);

			AcDbObjectId lineId;
			pBTR->appendAcDbEntity(lineId, pLine);//把直线添加到块表记录里

			pLine->close();//添加后记得关闭，注意不能把pLine给delete掉
			pBTR->close();
		}
	}
}

```

创建圆

```c
	acutPrintf(_T("\nHello World"));
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//从数据库获取块表
	AcDbObjectId btrId;
	if (pBT)//判断是否成功
	{
		auto pBtr = new AcDbBlockTableRecord();
		pBtr->setName(_T("*U"));
		pBT->add(btrId, pBtr);

		auto pCir = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 500);
		acutPrintf(_T("\n创建圆成功"));
		pBtr->appendAcDbEntity(pCir); // 区别在于不是加到模型空间的块表记录
		pCir->close();

		pBtr->close();
		pBT->close();
	}

	AcDbBlockTableRecord* pBTR = nullptr;
	pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);
	pBT->close();
	if (pBTR)
	{
		auto pRef = new AcDbBlockReference(AcGePoint3d::kOrigin, btrId);
		pBTR->appendAcDbEntity(pRef);
		pRef->close();
		pBTR->close();
	}
```

```C
#include "pch.h"

#include "tchar.h"
#include <dbapserv.h>
#include <aced.h>
#include <dbxrecrd.h>
#include <rxregsvc.h>
#include <dbmain.h>

void helloworld()
{
	//从数据库获取块表
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);
	AcDbObjectId btrId;
	AcDbDictionary* pNOD = nullptr;
	acdbOpenAcDbObject((AcDbObject*&)pNOD, pDb->namedObjectsDictionaryId(), AcDb::kForWrite);//获取命名词典的Id并打开

	//判断是否成功，执行业务
	if (pBT)
	{
		auto pBtr = new AcDbBlockTableRecord();
		pBtr->setName(_T("*U"));
		pBT->add(btrId, pBtr);

		auto pDict = new AcDbDictionary();//创建命名词典
		AcDbObjectId dictId;
		pNOD->setAt(_T("MyDict"), pDict, dictId);//设置命名词典的名称和ID

		//for循环画圆
		for (int i = 1; i <= 255; i++) {
			auto pCir = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, i);
			pCir->setColorIndex(i);
			pBtr->appendAcDbEntity(pCir);
			
			char str[10];
			for (int i = 0; i < 10; i++) {
				str[i] = '\0';
			}
			_itoa(i, str, 10);


			//命名词典记录句柄√
			if (pDict) {
				if (!pDict->has(AcString(str)))
				{
					AcDbHandle hd;//拿到了句柄
					pCir->getAcDbHandle(hd);
					ACHAR strHD[16];
					hd.getIntoAsciiBuffer(strHD, 16);
					acutPrintf(_T("\n Handle: %s"), strHD);
					auto pXrec = new AcDbXrecord();//把数据存在xrecord里
					resbuf* pRb = acutBuildList(AcDb::kDxfText, (_T("\n Handle is %s."), strHD), RTNONE);
					pXrec->setFromRbChain(*pRb);
					AcDbObjectId xrecId;
					pDict->setAt(AcString(str), pXrec, xrecId);//把xrecord存到词典里，并生成其id
					pXrec->close();//记得要close
					acutRelRb(pRb);//resbuf链表复制到xrecord后要删除
					pRb = nullptr;
				}
			}

			//拓展词典记录颜色索引
			if (pCir->extensionDictionary().isNull())//判断该对象是否已经有扩展词典
			{
				pCir->createExtensionDictionary();//没有就创建
			}
			//之后就是常规词典操作
			AcDbDictionary* pXDict = nullptr;
			acdbOpenAcDbObject((AcDbObject*&)pXDict, pCir->extensionDictionary(), AcDb::kForWrite);
			if (pXDict)
			{
				auto pXrec1 = new AcDbXrecord();//把数据存在xrecord里
				resbuf* pRb = acutBuildList(AcDb::kDxfText, (_T("\n ColorIndex is %s."),str), RTNONE);
				pXrec1->setFromRbChain(*pRb);
				AcDbObjectId xrecId;
				pXDict->setAt(AcString(str), pXrec1, xrecId);//把xrecord存到词典里，并生成其id
				pXrec1->close();//记得要close
				acutRelRb(pRb);//resbuf链表复制到xrecord后要删除
				pRb = nullptr;
				pXDict->close();
			}
			pCir->close();
		}
		pDict->close();
		acutPrintf(_T("\n创建圆成功"));
		pBtr->close();
	}

	//把对象显示在画板上
	AcDbBlockTableRecord* pBTR = nullptr;
	pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);
	
	if (pBTR)
	{
		auto pRef = new AcDbBlockReference(AcGePoint3d::kOrigin, btrId);
		pBTR->appendAcDbEntity(pRef);
		pRef->close();
		pBTR->close();
	}
	pBT->close();
}
```

## 第二次作业：画线

```C

void InputLine()
{
	ads_point startPt, endPt;
	acedInitGet(RSG_NONULL, NULL);
	int rc = acedGetPoint(NULL, _T("Input an Point:\n"), startPt);
	int rd = acedGetPoint(NULL, _T("Input an Point:\n"), endPt);

	AcGePoint3d s = asPnt3d(startPt);

	AcGePoint3d e = asPnt3d(endPt);
	//	AcGePoint3d endPt(10.0, 7.0, 0.0);
	AcDbLine *pLine = new AcDbLine(s, e);
	AcDbLine objLine;
	pLine->startPoint();
	objLine.startPoint();

	AcDbBlockTable *pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForRead);

	AcDbBlockTableRecord *pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();

	AcDbObjectId lineId;
	pBlockTableRecord->appendAcDbEntity(lineId, pLine);

	pBlockTableRecord->close();
	pLine->close();
}
```



## 第三次作业:自定义实体

```C
			//把对象显示在图纸上：appendZcDbEntity,appendAcDbEntity
			
			AcDbBlockTableRecord* pBTR = nullptr;
			//或者  AcDbBlockTableRecord *pBTR;

			//1.appendAcDbEntity(a)  A Z混用，都是套皮
			auto sample_1 = new SampleCustEnt(original,200.0);
			pBTR->appendAcDbEntity(sample_1);
			
			//2.appendAcDbEntity(a,b)  A Z混用，都是套皮
			SampleCustEnt *sample_1 = new SampleCustEnt(original, 200.0);
			AcDbObjectId myCircleId;
			pBTR->appendZcDbEntity(myCircleId,sample_1);

			sample_1->close();
			pBTR->close();
```

作业要求：

选择一个圆，根据这个圆的半径创建出一个显示同样半径的圆的自定义实体，并实现合理的夹点和捕捉功能。

```C
//SampleCustEnt.h

#pragma once
#include "dbmain.h"

//此自定义实体继承于AcDbEntity，这是最常用的自定义实体父类
class SampleCustEnt : public AcDbEntity {

public:
	ACRX_DECLARE_MEMBERS(SampleCustEnt);//定义了一些运行时类型识别和转换函数，如cast, desc, isA等

protected:
	AcGePoint3d m_center;
	double m_radius;
	static Adesk::UInt32 kCurrentVersionNumber;//记录自定义实体的版本，方便做版本兼容

public:
	SampleCustEnt();//无参构造
	SampleCustEnt(AcGePoint3d center, double radius);//有参构造
	virtual ~SampleCustEnt();

	//void setCenter(AcGePoint3d mpcenter);
	//void setRadius(double mpradius);
	AcGePoint3d center();

protected:
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

};

#ifdef ARXPROJECT3_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif
```



```C
//SampleCustEnt.cpp
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

//
//void SampleCustEnt::setCenter(AcGePoint3d mpcenter) {
//	m_center = mpcenter;
//}
//
//void SampleCustEnt::setRadius(double mpradius) {
//	m_radius = mpradius;
//}

AcGePoint3d SampleCustEnt::center() {
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
}

//设置显示相关的属性
Adesk::UInt32 SampleCustEnt::subSetAttributes(AcGiDrawableTraits * traits)
{
	return Adesk::UInt32();
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
```



```C
//entrypoint.cpp
#include "pch.h"
#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "dbapserv.h"
#include "SampleCustEnt.h"

void Entity()
{
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//从数据库获取块表
	if (pBT)//判断是否成功
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			ads_name en;
			ads_point apt;
			acedEntSel(_T("\n选择一个实体"), en, apt);

			//创建数据库对象
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);
			AcDbCircle *circle = NULL;
			acdbOpenAcDbObject((AcDbObject *&)circle, circleId, AcDb::kForWrite);

			double radius = circle->radius();
			circle->close();
			acutPrintf(_T("\n%f"), radius);

			//acutPrintf(_T("\n Hello!"));
			AcGePoint3d original(0.0, 0.0, 0.0);
			SampleCustEnt *sample_1 = new SampleCustEnt(original, radius);
			AcDbObjectId myCircleId;
			pBTR->appendZcDbEntity(myCircleId, sample_1);//显示在图纸上
			sample_1->close();
			pBTR->close();
		}
	}
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("Entity"), _T("Entity"), ACRX_CMD_MODAL, Entity);
}

void unloadapp()
{
	acedRegCmds->removeGroup(cmd_group_name);
}

extern "C" AcRx::AppRetCode zcrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
	{
		acrxDynamicLinker->unlockApplication(appId);
		acrxDynamicLinker->registerAppMDIAware(appId);
		initapp();
	}
	break;
	case AcRx::kUnloadAppMsg:
	{
		unloadapp();
	}
	break;

	default:
		break;
	}
	return AcRx::kRetOK;
}

#ifdef _WIN64
#pragma comment(linker, "/export:zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:zcrxGetApiVersion,PRIVATE")
#else // WIN32
#pragma comment(linker, "/export:_zcrxEntryPoint,PRIVATE")
#pragma comment(linker, "/export:_zcrxGetApiVersion,PRIVATE")
#endif

//注册运行时类型识别函数
#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif
```

## 第四次作业:JIG与引用复制

给上次作业的自定义实体添加使用jig创建的功能，让用户选择圆心位置和半径，且实现合理的OPM功能

给之前作业的自定义实体添加与指示放大范围的圆的引用关系，并通过代码把它复制到另一个文档里

```C
//entrypoint.cpp

void helloworld()
{

	//acutPrintf(_T("\nHello World!"));
	acDocManager->lockDocument(curDoc());

	auto pDb = acdbHostApplicationServices()->workingDatabase();

	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//从数据库获取块表

	if (pBT)//判断是否成功
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			//acutPrintf(_T("\nhello world"));

			ads_name en;
			ads_point apt;
			acedEntSel(_T("\n选择一个实体"), en, apt);

			//创建数据库对象
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);

			SampleCustEnt *sample_1 = new SampleCustEnt(circleId);
			AcDbObjectId sId;
			pBTR->appendZcDbEntity(sId, sample_1);//显示在图纸上
			sample_1->close();

			pBTR->close();

			AcApDocManager* pDocManager = acDocManager;
			AcApDocumentIterator* iter = acDocManager->newAcApDocumentIterator();
			if (iter != NULL)
			{

				//acutPrintf(_T("\niter is not null"));

				AcApDocument* tmpDoc = NULL;

				ZcDbObjectIdArray arr;

				arr.append(circleId);
				arr.append(sId);

				for (; !iter->done(); iter->step())
				{//用迭代器来遍历文档
					//acutPrintf(_T("\niter for "));

					tmpDoc = iter->document();

					if (tmpDoc != NULL)
					{
						//acutPrintf(_T("\n tmpDoc is not null "));
						acDocManager->lockDocument(tmpDoc);

						// 新建一个数据库来保存
						AcDbDatabase* newpDb = new AcDbDatabase();

						// 获取id
						AcDbBlockTable* newpBT = nullptr;
						newpDb->getBlockTable(newpBT, AcDb::kForWrite);//从数据库获取块表
						AcDbBlockTableRecord* newpBTR = nullptr;
						auto err = newpBT->getAt(ACDB_MODEL_SPACE, newpBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
						newpBT->close();//用完后马上关闭
						AcDbIdMapping idMap;
						idMap.setDestDb(newpDb);
						auto id = newpBTR->id();
						newpBT->close();
						newpBTR->close();

						// 复制
						Acad::ErrorStatus status = pDb->wblockCloneObjects(arr, id, idMap, AcDb::DuplicateRecordCloning::kDrcIgnore);

						//acutPrintf(_T("\n status is: %s"), status);
						AcDbIdMappingIter iterM(idMap);

						for (iterM.start(); !iterM.done(); iterM.next())
						{
							//acutPrintf(_T("\n iterM for loop"));

							AcDbIdPair idPair;
							iterM.getMap(idPair);
							if (!idPair.isCloned()) { 
								//acutPrintf(_T("\n for continue"));
								continue;
							}
							//acutPrintf(_T("\bObjectId is: %Ld", idPair.value().asOldId()));
						}

						//acutPrintf(_T("\n out loop"));

						err = acDocManager->unlockDocument(tmpDoc);

						//acutPrintf(_T("\n out loop, err: %d", err));

						// 保存
						err = newpDb->saveAs(_T("d:/aaa.dwg"));

						//acutPrintf(_T("\n save file, err: %d", err));
						delete newpDb;
						newpDb = NULL;
					}
				}

				delete iter; //迭代器用完要delete
				iter = NULL;
			}
		}
	}
	acDocManager->unlockDocument(curDoc());

}

```

## 第五次作业：Undo与Overrule

1.//entrypoint.cpp给之前作业的自定义实体实现部分undo  2.用overrule实现之前作业的自定义实体的功能

```C
//entrypoint.cpp
void helloworld()
{
	acutPrintf(_T("\n hello"));
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//从数据库获取块表
	if (pBT)//判断是否成功
	{
		acutPrintf(_T("\n pBT is true"));
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
		pBT->close();//用完后马上关闭
		if (pBTR)
		{
			ads_name en;
			ads_point apt;
			acedEntSel(_T("\n选择一个实体"), en, apt);

			//创建数据库对象
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);
			AcDbCircle *circle = NULL;
			acdbOpenAcDbObject((AcDbObject *&)circle, circleId, AcDb::kForWrite);

			double radius = circle->radius();
			circle->close();
			acutPrintf(_T("\n%f"), radius);

			//acutPrintf(_T("\n Hello!"));
			AcGePoint3d original(0.0, 0.0, 0.0);
			AcDbObjectId myCircleId;
			SampleCustEnt *sample_1 = new SampleCustEnt(original, radius, myCircleId);
			pBTR->appendZcDbEntity(sample_1);//显示在图纸上
			sample_1->close();
			pBTR->close();
		}
	}
}
```

```C
//SampleCustEnt.cpp

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

enum PartialUndoCode
{
	kRadius
};

Acad::ErrorStatus SampleCustEnt::setRadius(double rad)
{
	assertWriteEnabled(false);
	AcDbDwgFiler *pFiler = NULL;
	if ((pFiler = undoFiler()) != NULL) {
		undoFiler()->writeAddress(SampleCustEnt::desc());//导出实体标记
		undoFiler()->writeItem((Adesk::Int16)kRadius);//导出属性标记
		undoFiler()->writeDouble(m_radius);
	}
	m_radius = rad;//记得先导出完再设置新的值
	return Acad::eOk;
}

Acad::ErrorStatus SampleCustEnt::applyPartialUndo(AcDbDwgFiler* undoFiler, AcRxClass* classObj)
{
	//这个classObj就是我们导出时第一个导出的实体标记
	if (classObj != SampleCustEnt::desc())
		return AcDbEntity::applyPartialUndo(undoFiler, classObj);
	Adesk::Int16 shortCode;
	undoFiler->readItem(&shortCode);
	PartialUndoCode code = (PartialUndoCode)shortCode;
	double rad = 0;
	switch (code) {//根据属性标记设置不同的属性值
	case kRadius:
		//与dwgInFields一样，如果有多个数据，顺序要与导出时一样
		undoFiler->readDouble(&rad);
		setRadius(rad);//思考：为什么不直接设置m_radius？
		break;
	default:
		assert(Adesk::kFalse);
		break;
	}
	return Acad::eOk;
}

```

