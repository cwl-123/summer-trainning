#pragma once
#include "pch.h"

#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>
#include "dbmain.h"
#include <dbapserv.h>


class AsdkObjectToNotify : public AcDbObject {
public:
	ACRX_DECLARE_MEMBERS(AsdkObjectToNotify);
	AsdkObjectToNotify() {};
	void eLinkage(AcDbObjectId i) { mId = i; };
	void              modified(const AcDbObject*);
	Acad::ErrorStatus dwgInFields(AcDbDwgFiler*);
	Acad::ErrorStatus dwgOutFields(AcDbDwgFiler*) const;
	void erased(const AcDbObject* dbObj, ZSoft::Boolean bErasing);
private:
	AcDbObjectId mId; //关联的直线id
};

