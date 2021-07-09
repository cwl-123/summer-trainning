#include "pch.h"

#include "tchar.h"
#include <dbapserv.h>
#include <aced.h>
#include <dbxrecrd.h>
#include <rxregsvc.h>
#include <dbmain.h>

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


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("InputLine"), _T("InputLine"), ACRX_CMD_MODAL, InputLine);
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

