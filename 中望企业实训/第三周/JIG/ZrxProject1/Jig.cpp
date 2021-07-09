// (C) Copyright 2005-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
#include "pch.h"
#include "Jig.h"
#include "SampleCustEnt.h"
#include "tchar.h"


//-----------------------------------------------------------------------------
Jig::Jig() : AcEdJig(),
mCurrentInputLevel(0), mpEntity(NULL)
{
}

Jig::~Jig() {
}

//-----------------------------------------------------------------------------
AcEdJig::DragStatus Jig::startJig(SampleCustEnt *pEntity) {
	//- Store the new entity pointer
	mpEntity = pEntity;
	//- Setup each input prompt
	AcString inputPrompts[2] = {
	_T("\n Pick point"),_T("aaa")
	};
	//- Setup kwords for each input
	AcString kwords[2] = {
		_T("bbb"),_T("ccc")
	};

	bool appendOk = true;
	AcEdJig::DragStatus status = AcEdJig::kNull;
	//- Loop the number of inputs
	for (mCurrentInputLevel = 0; mCurrentInputLevel < 2; mCurrentInputLevel++) {
		//- Add a new input point to the list of input points
		mInputPoints.append(AcGePoint3d());
		//- Set the input prompt
		setDispPrompt(inputPrompts[mCurrentInputLevel]);
		//- Setup the keywords required
		setKeywordList(kwords[mCurrentInputLevel]);

		bool quit = false;
		//- Lets now do the input
		acutPrintf(_T("\nbefore drag"));

		status = drag();
		acutPrintf(_T("\nafter drag"));

		if (status != kNormal) {
			//- If it's a keyword
			switch (status) {
			case kCancel:
			case kNull:
				quit = true;
				break;

			case kKW1:
			case kKW2:
			case kKW3:
			case kKW4:
			case kKW5:
			case kKW6:
			case kKW7:
			case kKW8:
			case kKW9:
				//- Do something

				break;
			}
		}
		else {
			appendOk = true;
		}

		//- If to finish
		if (quit)
			break;
	}

	//- If the input went well
	if (appendOk)
		//- Append to the database
		append();
	else
		//- Clean up
		delete mpEntity;

	return (status);
}

//-----------------------------------------------------------------------------
//- Input sampler
AcEdJig::DragStatus Jig::sampler() {
	//- Setup the user input controls for each input
	AcEdJig::UserInputControls userInputControls[1] = {
		/*AcEdJig::UserInputControls::*/(AcEdJig::UserInputControls)0
	};
	//- Setup the cursor type for each input
	AcEdJig::CursorType cursorType[1] = {
		/*AcEdJig::CursorType::*/(AcEdJig::CursorType)0
	};
	//- Setup the user input controls for each sample
	setUserInputControls(userInputControls[mCurrentInputLevel]);
	setSpecialCursorType(cursorType[mCurrentInputLevel]);

	AcEdJig::DragStatus status = AcEdJig::kCancel;
	//- Check the current input number to see which input to do
	switch (mCurrentInputLevel + 1) {
	case 1:
		// TODO : get an input here
		status =GetStartPoint () ;
		break;
	case 2:
		status = GetNextPoint();
		break;
	default:
		break;
	}
	return (status);
}

//-----------------------------------------------------------------------------
//- Jigged entity update
Adesk::Boolean Jig::update() {
	//- Check the current input number to see which update to do
	switch (mCurrentInputLevel + 1) {
	case 1://第一次输入获取的是中心点
		mpEntity->setCenter(mInputPoints[mCurrentInputLevel]);
		break;
	case 2://第二次输入获取的是半径
		mpEntity->setRadius(mInputPoints[mCurrentInputLevel].distanceTo(mpEntity->center()));
		break;
	default:
		break;
	}
	return (updateDimData());
}

//-----------------------------------------------------------------------------
//- Jigged entity pointer return
AcDbEntity *Jig::entity() const {
	return ((AcDbEntity *)mpEntity);
}

//-----------------------------------------------------------------------------
//- Dynamic dimension data setup
AcDbDimDataPtrArray *Jig::dimData(const double dimScale) {

	/* SAMPLE CODE:
	AcDbAlignedDimension *dim =new AcDbAlignedDimension () ;
	dim->setDatabaseDefaults () ;
	dim->setNormal (AcGeVector3d::kZAxis) ;
	dim->setElevation (0.0) ;
	dim->setHorizontalRotation (0.0) ;
	dim->setXLine1Point (m_originPoint) ;
	dim->setXLine2Point (m_lastPoint) ;
	//- Get the dimPoint, first the midpoint
	AcGePoint3d dimPoint =m_originPoint + ((m_lastPoint - m_originPoint) / 2.0) ;
	//- Then the offset
	dim->setDimLinePoint (dimPoint) ;
	dim->setDimtad (1) ;

	AcDbDimData *dimData = new AcDbDimData (dim) ;
	//AppData *appData =new AppData (1, dimScale) ;
	//dimData.setAppData (appData) ;
	dimData->setDimFocal (true) ;
	dimData->setDimHideIfValueIsZero (true) ;

	//- Check to see if it is required
	if ( getDynDimensionRequired (m_inputNumber) )
		dimData->setDimInvisible (false) ;
	else
		dimData->setDimInvisible (true) ;

	//- Make sure it is editable TODO:
	dimData->setDimEditable (true) ;
	mDimData.append (dimData) ;

	return (&mDimData) ;
	*/
	AcDbAlignedDimension *dim = new AcDbAlignedDimension();
	dim->setDatabaseDefaults();
	dim->setNormal(AcGeVector3d::kZAxis);
	dim->setElevation(0.0);
	dim->setHorizontalRotation(0.0);
	dim->setXLine1Point(mInputPoints[0]);
	dim->setXLine2Point(mInputPoints[1]);
	AcGePoint3d dimPoint = mInputPoints[0] + ((mInputPoints[1] - mInputPoints[0]) / 2.0);
	dim->setDimLinePoint(dimPoint);
	dim->setDimtad(1);
	AcDbDimData *dimData = new AcDbDimData(dim);
	dimData->setDimFocal(true);
	dimData->setDimHideIfValueIsZero(true);
	dimData->setDimInvisible(false);
	dimData->setDimEditable(true);
	mDimData.append(dimData);//把DimData保存起来并返回给CAD

	return (NULL);
}

//-----------------------------------------------------------------------------
//- Dynamic dimension data update
Acad::ErrorStatus Jig::setDimValue(const AcDbDimData *pDimData, const double dimValue) {
	Acad::ErrorStatus es = Acad::eOk;

	/* SAMPLE CODE:
	//- Convert the const pointer to non const
	AcDbDimData *dimDataNC =const_cast<AcDbDimData *>(pDimData) ;
	int inputNumber =-1 ;
	//- Find the dim data being passed so we can determine the input number
	if ( mDimData.find (dimDataNC, inputNumber) ) {
		//- Now get the dimension
		AcDbDimension *pDim =(AcDbDimension *)dimDataNC->dimension () ;
		//- Check it's the type of dimension we want
		AcDbAlignedDimension *pAlnDim =AcDbAlignedDimension::cast (pDim) ;
		//- If ok
		if ( pAlnDim ) {
			//- Extract the dimensions as they are now
			AcGePoint3d dimStart =pAlnDim->xLine1Point () ;
			AcGePoint3d dimEnd =pAlnDim->xLine2Point () ;
			//- Lets get the new point entered by the user
			AcGePoint3d dimEndNew =dimStart + (dimEnd - dimStart).normalize () * dimValue ;
			//- Finally set the end dim point
			pAlnDim->setXLine2Point (dimEndNew) ;
			//- Now update the jig data to reflect the dynamic dimension input
			mInputPoints [mCurrentInputLevel] =dimEndNew ;
		}
	}*/
	AcDbDimData *dimDataNC = const_cast<AcDbDimData *>(pDimData);//把const指针转成普通指针
	int inputNumber = -1;
	if (mDimData.find(dimDataNC, inputNumber)) {
		AcDbDimension *pDim = (AcDbDimension *)dimDataNC->dimension();//从DimData中获取标注对象
		AcDbAlignedDimension *pAlnDim = AcDbAlignedDimension::cast(pDim);
		if (pAlnDim) {
			AcGePoint3d dimStart = pAlnDim->xLine1Point();
			AcGePoint3d dimEnd = pAlnDim->xLine2Point();
			AcGePoint3d dimEndNew = dimStart + (dimEnd - dimStart).normalize() * dimValue;
			pAlnDim->setXLine2Point(dimEndNew);
			//把通过标注值计算出来的点存储起来作为输入的结果
			mInputPoints[mCurrentInputLevel] = dimEndNew;
			update();//手动调用update，也可以封装一个函数同时被update和setDimValue调用
		}
	}
	return (es);
}

//-----------------------------------------------------------------------------
//- Various helper functions
//- Dynamic dimdata update function
Adesk::Boolean Jig::updateDimData() {
	//- Check the dim data store for validity
	if (mDimData.length() <= 0)
		return (true);
	/* SAMPLE CODE :
	//- Extract the individual dimData
	AcDbDimData *dimData =mDimData [m_inputNumber] ;
	//- Now get the dimension
	AcDbDimension *pDim =(AcDbDimension *)dimData->dimension () ;
	//- Check it's the type of dimension we want
	AcDbAlignedDimension *pAlnDim =AcDbAlignedDimension::cast (pDim) ;
	//- If ok
	if ( pAlnDim ) {
		//- Check to see if it is required
		if ( getDynDimensionRequired (m_inputNumber) )
			dimData->setDimInvisible (false) ;
		else
			dimData->setDimInvisible (true) ;
		pAlnDim->setXLine1Point (m_originPoint) ;
		pAlnDim->setXLine2Point (m_lastPoint) ;
		//- Get the dimPoint, first the midpoint
		AcGePoint3d dimPoint =m_originPoint + ((m_lastPoint - m_originPoint) / 2.0) ;
		//- Then the offset
		pAlnDim->setDimLinePoint (dimPoint) ;
	} */
	AcDbDimData *dimData = mDimData[0];
	AcDbDimension *pDim = (AcDbDimension *)dimData->dimension();
	AcDbAlignedDimension *pAlnDim = AcDbAlignedDimension::cast(pDim);
	if (pAlnDim) {
		dimData->setDimInvisible(false);
		pAlnDim->setXLine1Point(mInputPoints[0]);
		pAlnDim->setXLine2Point(mInputPoints[1]);
		AcGePoint3d dimPoint = mInputPoints[0] + ((mInputPoints[1] - mInputPoints[0]) / 2.0);
		pAlnDim->setDimLinePoint(dimPoint);
	}
	return (true);
}

//-----------------------------------------------------------------------------
//- Std input to get a point with no rubber band
AcEdJig::DragStatus Jig::GetStartPoint() {
	AcGePoint3d newPnt;
	//- Get the point 
	AcEdJig::DragStatus status = acquirePoint(newPnt);
	//- If valid input
	if (status == AcEdJig::kNormal) {
		//- If there is no difference
		if (newPnt.isEqualTo(mInputPoints[mCurrentInputLevel]))
			return (AcEdJig::kNoChange);
		//- Otherwise update the point
		mInputPoints[mCurrentInputLevel] = newPnt;
	}
	return (status);
}

//-----------------------------------------------------------------------------
//- Std input to get a point with rubber band from point
AcEdJig::DragStatus Jig::GetNextPoint() {
	AcGePoint3d oldPnt = mInputPoints[mCurrentInputLevel];
	AcGePoint3d newPnt;
	//- Get the point 
	AcEdJig::DragStatus status = acquirePoint(newPnt, oldPnt);
	//- If valid input
	if (status == AcEdJig::kNormal) {
		//- If there is no difference
		if (newPnt.isEqualTo(mInputPoints[mCurrentInputLevel]))
			return (AcEdJig::kNoChange);
		//- Otherwise update the point
		mInputPoints[mCurrentInputLevel] = newPnt;
	}
	return (status);
}
