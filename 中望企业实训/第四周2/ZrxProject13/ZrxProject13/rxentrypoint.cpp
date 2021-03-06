#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "acadstrc.h"
#include "dbproxy.h"
#include "rxboiler.h"
#include "dbidmap.h"
#include "dbcfilrs.h"
#include "dbmain.h"
#include <dbapserv.h>
#include "acestext.h"

#include "SampleCustEnt.h"

//注册运行时类型识别函数
#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif

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


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("ccc"), _T("ccc"), ACRX_CMD_SESSION, helloworld);
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
		SampleCustEnt::rxInit();
		acrxBuildClassHierarchy();//重生成运行时类树
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

