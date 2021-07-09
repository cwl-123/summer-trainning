#include "pch.h"

#include "base.h"
#include "Jig.h"
#include "DisplayEnt.h"


#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(DisplayEnt)
#endif

ZcDbObjectIdArray arr;

// 获取样本点
AcGePoint3d getSamplePoint(AcDbObjectId pEntId) {
	AcDbCurve* pCrv = nullptr;
	AcGeCurve3d* pGeCrv = nullptr;
	acdbOpenAcDbEntity((AcDbEntity*&)pCrv, pEntId, AcDb::kForRead);
	if (pCrv)
	{
		pCrv->getAcGeCurve(pGeCrv);//从AcDbCurve获取AcGeCurve
		pCrv->close();
	}
	AcGePoint3dArray pts;
	pGeCrv->getSamplePoints(3, pts);//对曲线取样
	delete pGeCrv;//用完要delete掉
	pGeCrv = nullptr;

	return pts[1];
}

// 显示实体
void show(AcDbEntity* pEnt) {
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pBlockTable, AcDb::kForWrite);

	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord, AcDb::kForWrite);
	pBlockTable->close();
	pBlockTableRecord->appendAcDbEntity(pEnt);
	pBlockTableRecord->close();
}

// 选择放大镜并展示
void select() {
	Zcad::ErrorStatus err;

	// 初始化要复制到另一个文档的实体id数组
	arr = ZcDbObjectIdArray();

	// 使用jig创建显示范围
	Jig* jig = new Jig();
	DisplayEnt* displayEnt = new DisplayEnt();
	jig->startJig(displayEnt);

	show(displayEnt);
	AcDbObjectId displayEntId = displayEnt->objectId();

	displayEnt->close();

	// 获取放大倍数
	ads_real num;
	if (acedGetReal(_T("请输入放大倍数: \n"), &num) != RTNORM) { acutPrintf(_T("放大倍数无效 \n")); return; }

	// 选择作为放大镜的实体
	AcDbEntity* pEnt;
	AcDbObjectId pEntId;
	ads_name en;
	ads_point pt;

	if (acedEntSel(_T("请选择一个圆：\n"), en, pt) != RTNORM) { acutPrintf(_T("未选择 \n")); return; }

	acdbGetObjectId(pEntId, en);
	acdbOpenObject(pEnt, pEntId, AcDb::kForRead);

	arr.append(pEntId);

	// 将放大镜转换为圆，并获取中心和半径
	AcDbCircle* circle = (AcDbCircle*)pEnt;
	AcGePoint3d center = circle->center();
	double radius = circle->radius();

	AcDbExtents ext;
	pEnt->getGeomExtents(ext);

	// 设置放大倍数
	AcGeMatrix3d mat;
	mat.setToScaling(num);

	// 要放大显示的实体id数组
	AcDbObjectIdArray objIdArr;

	// 获取实体框选的所有实体
	ads_name ss;
	acedSSGet(_T("C"), asDblArray(ext.minPoint()), asDblArray(ext.maxPoint()), NULL, ss);
	Adesk::Int32 len = 0;
	acedSSLength(ss, &len);

	// 遍历框选的实体
	for (auto i = 0; i < len; i++) {
		// 获取实体id
		ads_name en1;
		acedSSName(ss, i, en1);
		AcDbObjectId objId1;
		auto err = acdbGetObjectId(objId1, en1);

		arr.append(objId1);

		AcDbEntity* pEnt1;
		if (acdbOpenAcDbEntity(pEnt1, objId1, AcDb::kForWrite) == Zcad::ErrorStatus::eOk) {
			// 获取交点
			ZcGePoint3dArray points;
			err = pEnt->intersectWith(pEnt1, ZcDb::Intersect::kOnBothOperands, points);
			acutPrintf(_T("获取交点 err: %d, points length: %d \n"), err, points.length());

			// 打断
			if (pEnt1->isKindOf(AcDbCurve::desc())) {
				acutPrintf(_T("开始打断 \n"));

				AcDbCurve* pCurve;
				AcDbVoidPtrArray curveSegments;

				pCurve = AcDbCurve::cast(pEnt1);

				// 获取参数
				double param;
				double* params = new double[points.length()];
				for (auto y = 0; y < points.length(); y++) {
					pCurve->getParamAtPoint(points[y], param);
					params[y] = param;
				}

				// 排序
				sort(params, params + points.length());

				// 转换为 ZcGeDoubleArray
				ZcGeDoubleArray ZcGeparams;
				for (auto z = 0; z < points.length(); z++) {
					ZcGeparams.append(params[z]);
				}

				// 打断
				pCurve->getSplitCurves(ZcGeparams, curveSegments);
				acutPrintf(_T("打断完成，长度： %d \n"), curveSegments.logicalLength());

				// 如果没打断，说明整个图形都在实体中，复制整个图形
				if (curveSegments.logicalLength() == 0) {
					acutPrintf(_T("复制全部 \n"));
					// 克隆一个新实体来放大显示
					AcDbEntity* pEnt2 = AcDbEntity::cast(pEnt1->clone());

					show(pEnt2);
					AcDbObjectId objId2 = pEnt2->objectId();
					pEnt2->transformBy(mat);
					pEnt2->close();
					// 添加到待显示的objidarr中
					objIdArr.append(objId2);
				}
				else {
					acutPrintf(_T("\n 复制每个"));

					for (auto k = 0; k < curveSegments.logicalLength(); k++) {
						// 把打断出来的曲线转换为实体
						AcDbEntity* pEnt2 = (AcDbEntity*)curveSegments[k];

						show(pEnt2);
						AcDbObjectId objId2 = pEnt2->objectId();
						pEnt2->close();

						// 对实体采样
						AcGePoint3d point = getSamplePoint(objId2);

						// 判断采样点是否在圆内
						double distance = pow((center.x - point.x), 2) + pow((center.y - point.y), 2);
						if (distance <= pow(radius, 2)) {
							// 在圆内，放大并添加到待显示数组中
							acdbOpenAcDbEntity(pEnt2, objId2, AcDb::kForWrite);

							pEnt2->transformBy(mat);
							objIdArr.append(objId2);
							pEnt2->close();
						}
					}
				}
				pEnt1->close();
			}
		}
	}

	// 已放大待显示实体长度
	acutPrintf(_T("\n objIdArr长度 :%d"), objIdArr.length());
	err = acdbOpenAcDbEntity((ZcDbEntity*&)displayEnt, displayEntId, AcDb::kForWrite);
	acutPrintf(_T("\n 打开displayEnt err :%d"), err);

	// 设置实体id和放大实体的基点
	displayEnt->setObjIdArr(objIdArr);
	displayEnt->setOriCenter(AcGePoint3d(center.x * num, center.y * num, 0));

	displayEnt->close();

	arr.append(displayEntId);

	pEnt->close();
	acedSSFree(ss);
}

// 文档间复制
void clone()
{
	Zcad::ErrorStatus err;
	acDocManager->lockDocument(curDoc());

	ZcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcApDocManager* pDocManager = acDocManager;
	AcApDocumentIterator* iter = acDocManager->newAcApDocumentIterator();
	if (iter != NULL)
	{
		AcApDocument* tmpDoc = NULL;

		for (; !iter->done(); iter->step()) //用迭代器来遍历文档
		{
			tmpDoc = iter->document();
			if (tmpDoc != NULL)
			{
				acDocManager->lockDocument(tmpDoc);

				// 新建一个数据库来保存
				AcDbDatabase* newpDb = new AcDbDatabase();

				// 获取id
				AcDbBlockTable* newpBT = nullptr;
				newpDb->getBlockTable(newpBT, AcDb::kForWrite);//从数据库获取块表
				AcDbBlockTableRecord* newpBTR = nullptr;
				newpBT->getAt(ACDB_MODEL_SPACE, newpBTR, AcDb::kForWrite);//从块表获取模型空间的块表记录 
				newpBT->close();//用完后马上关闭
				AcDbIdMapping idMap;
				idMap.setDestDb(newpDb);
				auto id = newpBTR->id();
				newpBTR->close();

				// 复制
				err = pDb->wblockCloneObjects(arr, id, idMap, AcDb::DuplicateRecordCloning::kDrcIgnore);
				acutPrintf(_T("wblockCloneObjects, err: %d \n"), err);

				AcDbIdMappingIter iterM(idMap);

				for (iterM.start(); !iterM.done(); iterM.next())
				{
					AcDbIdPair idPair;
					iterM.getMap(idPair);
					if (!idPair.isCloned()) {
						continue;
					}
				}

				acDocManager->unlockDocument(tmpDoc);

				// 保存
				err = newpDb->saveAs(_T("c:/code/aaa.dwg"));
				acutPrintf(_T("save file, err: %d \n"), err);
				delete newpDb;
				newpDb = NULL;
			}
		}
		delete iter; //迭代器用完要delete
		iter = NULL;
	}
	acDocManager->unlockDocument(curDoc());
}

void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("ss"), _T("ss"), ACRX_CMD_MODAL, select);
	acedRegCmds->addCommand(cmd_group_name, _T("cc"), _T("cc"), ACRX_CMD_SESSION, clone);
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
		DisplayEnt::rxInit();//注册自定义实体
		acrxBuildClassHierarchy();//重生成运行时类树
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
