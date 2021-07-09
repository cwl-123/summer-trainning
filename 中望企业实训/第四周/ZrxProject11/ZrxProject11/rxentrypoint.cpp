#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>

#include "AsdkObjectToNotify.h"
#include "dbmain.h"
#include <dbapserv.h>

#include"SampleCustEnt.h"
//ע????????????
#ifdef ARXPROJECT1_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(SampleCustEnt)
#endif

void helloworld()
{
	auto pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTable* pBT = nullptr;
	pDb->getBlockTable(pBT, AcDb::kForWrite);//????ݿ?ȡ??
	if (pBT)//???Ƿ???
	{
		AcDbBlockTableRecord* pBTR = nullptr;
		pBT->getAt(ACDB_MODEL_SPACE, pBTR, AcDb::kForWrite);//?ӿ????ģ??ռ?????? 
		pBT->close();//?????Ϲر?
		if (pBTR)
		{
			acutPrintf(_T("\nhello world"));

			ads_name en;
			ads_point apt;
			acedEntSel(_T("\nѡ?һ??ʵ?"), en, apt);

			//??????ݿ??
			AcDbObjectId circleId;
			acdbGetObjectId(circleId, en);
			AcDbCircle *circle = NULL;
			acdbOpenAcDbObject((AcDbObject *&)circle, circleId, AcDb::kForWrite);

			double radius = circle->radius();
			acutPrintf(_T("\n%f"), radius);
			SampleCustEnt *sample_1 = new SampleCustEnt(circle->center(), radius + 100);
			AcDbObjectId sId;
			pBTR->appendZcDbEntity(sId, sample_1);//?ʾ?ͼֽ?
			acutPrintf(_T("\n%f"), sId);
			acutPrintf(_T("\n sample_1 %f, %f"), sample_1->center().x, sample_1->center().y);

			//auto pDb = acdbHostApplicationServices()->workingDatabase();

			AcDbDictionary *pNamedObj = NULL;
			AcDbDictionary *pNameList = NULL;
			pDb->getNamedObjectsDictionary(pNamedObj, AcDb::kForWrite);
			if (pNamedObj->getAt(_T("ASDK_DICT"), (AcDbObject*&)pNameList, AcDb::kForWrite) == Acad::eKeyNotFound)
			{
				acutPrintf(_T("\nin if"));

				pNameList = new AcDbDictionary;
				AcDbObjectId DictId;
				pNamedObj->setAt(_T("ASDK_DICT"), pNameList, DictId);
			}
			pNamedObj->close();

			//??Բ??ӷ?Ӧ?
			AsdkObjectToNotify *pObj = new AsdkObjectToNotify();
			pObj->eLinkage(sId);//??Զ??ʵ???d???????Ӧ???
			AcDbObjectId objId;
			if ((pNameList->getAt(_T("object_to_notify_circle"), objId)) == Acad::eKeyNotFound) {
				//?ѷ?Ӧ??洢???ʵ????޷?????
				pNameList->setAt(_T("object_to_notify_circle"), pObj, objId);
				pObj->close();
			}
			else {
				delete pObj;
			}
			//ע???????÷?Ӧ?
			circle->addPersistentReactor(objId);

			circle->close();

			sample_1->close();
			pNameList->close();

			pBTR->close();
		}
	}
}


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("bbb"), _T("bbb"), ACRX_CMD_MODAL, helloworld);
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
		AsdkObjectToNotify::rxInit();//ע??Զ??ʵ?
		acrxBuildClassHierarchy();//??????????
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

