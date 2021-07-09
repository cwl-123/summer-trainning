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

