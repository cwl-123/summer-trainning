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


void initapp()
{
	acedRegCmds->addCommand(cmd_group_name, _T("helloworld"), _T("helloworld"), ACRX_CMD_MODAL, helloworld);
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

