// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CAuxTunnelProjDemo01App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	//
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command
	//
	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	//
	//
	// Modal Command with pickfirst selection
	// ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET)
	// Application Session Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION)
	// The ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO macros can be applied to any static member 
	// function of the CAuxTunnelProjDemo01App class.
	// The function may or may not take arguments and have to return RTNORM, RTERROR, RTCAN, RTFAIL, RTREJ to AutoCAD, but use
	// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal to return
	// a value to the Lisp interpreter.
	//
	// NOTE: ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid.
	//
	//- ACED_ADSFUNCTION_ENTRY_AUTO(classname, name, regFunc) - this example
	//- ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//- ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) - a Lisp command (prefix C:)
	//- ACED_ADSCOMMAND_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//
	// Lisp Function is similar to ARX Command but it creates a lisp 
	// callable function. Many return types are supported not just string
	// or integer.
	// ACED_ADSFUNCTION_ENTRY_AUTO(CAuxTunnelProjDemo01App, MyLispFunction, false)
#include "StdAfx.h"
#include "resource.h"
#include "OptmizeFunc.h"
#include "CreateEnt.h"
#include "CalcuFunc.h"
#include "acedCmdNF.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("LJY")

bool TunSamplePtTemp(AcGePoint3dArray& APtAry,
	AcDbObjectId TunnelLay, AcDbObjectId mileLay);// current DWG
bool GetTunAPt(AcGePoint2dArray& APtAry);
//insert Wblock 函数不能改写外部数据库。
//bool TunSamplePtNew(AcGePoint2dArray& APtAry, AcGeDoubleArray& APtParamAry, 
// const ACHAR* FileName);
//选取当前数据库中隧道信息并打开相应截面图，获取隧道高程  输出AcGePoint3dArray

bool SSGetFunc(AcDbObjectIdArray& HDPolyLineIdary,
	AcDbObjectIdArray& XJPLIdAry, AcGePoint3d& APt,
	AcDbObjectId& IsoLwhite, AcDbObjectId& IsoLorange,
	AcDbObjectId Isoellow);
double PlLength(AcDbPolyline* pPoly);
bool MinDistPt(AcDbPolyline* pPoly, double& MinDist, int& MinNum, AcGePoint2d& Apt);
bool MaxDistPt(AcDbPolyline* pPoly, double& MaxDist, int& MaxNum, AcGePoint2d& Apt);


class CAuxTunnelProjDemo01App : public AcRxArxApp {

public:
	CAuxTunnelProjDemo01App() : AcRxArxApp() {}
	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);

		// TODO: Add your initialization code here

		return (retCode);
	}
	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);

		// TODO: Unload dependencies here

		return (retCode);
	}
	virtual void RegisterServerComponents() {}


	//主函数内不使用迭代器和选择集
	static void LJYMyGroupATnl() {
		int i, j, k;
		AcGePoint3dArray TunAPtAry;
		AcDbObjectIdArray HDPLIdAry, XJPLIdAry, RdObjIdAry;
		//AcGePoint3d APoint;
		double minD = 0; int minN = 0;
		AcDbEntity* pHDPolyLine;
		AcDbEntity* pXJPolyline;
		Acad::ErrorStatus es;
		AcDbLayerTable* pIsoLayerTable;
		AcDbObjectId TunnelLay, MileLay
			, IsolLayId8110, IsolLayId8310
			, IsolLayId8120, IsolLayId8521
			, RdLayId2110, RdId2111
			, RvLayId4430;
		//等高线ID...//当前数据库
		acdbHostApplicationServices()->
			workingDatabase()->getSymbolTable(pIsoLayerTable, AcDb::kForRead);
		if (pIsoLayerTable->has(_T("隧道-斜井"))
			&& pIsoLayerTable->has(_T("PMXX-hngtx"))
			&& pIsoLayerTable->has(_T("Iso8310"))
			&& pIsoLayerTable->has(_T("Iso8310"))
			&& pIsoLayerTable->has(_T("Iso8120"))
			&& pIsoLayerTable->has(_T("Iso8521"))
			&& pIsoLayerTable->has(_T("Rv4430"))
			&& pIsoLayerTable->has(_T("Rd2110"))
			&& pIsoLayerTable->has(_T("Rd2111")) != Adesk::kTrue) {
			acedAlert(_T("pIsoLayerTable doesn't exist"));
			return;
		}
		pIsoLayerTable->getAt(_T("隧道-斜井"), TunnelLay);
		pIsoLayerTable->getAt(_T("PMXX-hngtx"), MileLay);
		pIsoLayerTable->getAt(_T("Iso8110"), IsolLayId8110);
		//等高线图层
		pIsoLayerTable->getAt(_T("Iso8120"), IsolLayId8120);
		//等高线图层黄
		pIsoLayerTable->getAt(_T("Iso8310"), IsolLayId8310);
		//顶点坐标point及高程（text）
		pIsoLayerTable->getAt(_T("Iso8521"), IsolLayId8521);
		//黄线，作为白色等高线的补充
		pIsoLayerTable->getAt(_T("Rd2110"), RdLayId2110);
		pIsoLayerTable->getAt(_T("Rd2111"), RdId2111);
		pIsoLayerTable->getAt(_T("Rv4430"), RvLayId4430);
		//....
		pIsoLayerTable->close();
		// 计算A点标高

		acutPrintf(_T("\n new acdbobjectidary length:%d"), HDPLIdAry.length());
		TunSamplePtTemp(TunAPtAry, TunnelLay, MileLay);
		//GetTunAPt(TunAPtAry);
		acutPrintf(_T("\n 选取的A点个数：%d"), TunAPtAry.length());

		for (i = 0; i < TunAPtAry.length(); i++)
		{
			//APoint = AcGePoint3d(TunAPtAry[i].x, TunAPtAry[i].y, A_z);
			SSGetFunc(HDPLIdAry, XJPLIdAry, TunAPtAry[i],
				IsolLayId8110, IsolLayId8521, IsolLayId8120);
			AcGePoint2d Bpoint;
			if (HDPLIdAry.length() >= 1)
			{
				//在多个多段线中选择最优点出横洞
				AcGeDoubleArray minDAry;
				int* minNAry = new int[HDPLIdAry.length() + 1];
				AcGePoint2dArray BpointAry;
				AcGePoint2d Apoint2d;
				for (k = 0; k < HDPLIdAry.length(); k++)
				{
					es = acdbOpenObject(pHDPolyLine, HDPLIdAry[k], AcDb::kForRead);
					if (es == Acad::eWasOpenForWrite) { continue; }
					AcDbPolyline* HDPL = AcDbPolyline::cast(pHDPolyLine);
					Apoint2d = AcGePoint2d(TunAPtAry[i].x, TunAPtAry[i].y);
					MinDistPt(HDPL, minD, minN, Apoint2d);
					acutPrintf(_T("\n求最小值之后参数为：minD %f, minN %f, "), minD, minN);

					HDPL->getPointAt(minN, Bpoint);
					minDAry.append(minD);
					minNAry[k] = minN;
					BpointAry.append(Bpoint);
					//test
					CreateEnt::CreatePolyline(Apoint2d, Bpoint, 1);
				}
				delete[] minNAry;
			}
			else if (XJPLIdAry.length() >= 1)
			{
				for (j = 0; j < XJPLIdAry.length(); j++)
				{
					es = acdbOpenObject(pXJPolyline, XJPLIdAry[j], AcDb::kForRead);

				}


			}

			//
			HDPLIdAry.removeAll();
			XJPLIdAry.removeAll();

		}
		//该点附近没有横洞

		HDPLIdAry.~AcArray();
		XJPLIdAry.~AcArray();
		pHDPolyLine->close();
		pXJPolyline->close();
		return;
	}
	static void LJYMyGroupTest()
	{
		//ads_name SSN; Acad::ErrorStatus es;
		//AcDbEntity* pEPlLen;
		//acedSSGet(NULL, NULL, NULL, NULL, SSN);
		//Adesk::Int32 length;
		//acedSSLength(SSN, &length);
		//acutPrintf(_T("\n选择集个数：%d"), length);
		//int i = 0;
		//for (i = 0; i < length; i++)
		//{
		//	ads_name ent;
		//	acedSSName(SSN, i, ent);
		//	AcDbObjectId objId;
		//	acdbGetObjectId(objId, ent);
		//	es = acdbOpenAcDbEntity(pEPlLen, objId,
		//		AcDb::kForRead);
		//	if (es == Acad::eWasOpenForWrite) { continue; }
		//	if (pEPlLen->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue) {
		//		AcDbPolyline* pPoly = AcDbPolyline::cast(pEPlLen);
		//		double L;
		//		OptmizeFunc::PLLength(pPoly, L);
		//		acutPrintf(_T("\nPolyline %d, length:%f"), i, L);
		//	}
		//}
		//pEPlLen->close();
		//acedSSFree(SSN);



	}
	//测试 Mtext Content函数 
	// Mtext 行 AcString 行 Text 分成多个 不行 
	static void LJYMyGroupTest02()
	{
		AcString MTDK309590, TxtDK309, DK309n590
			, MTDK209190, TxtDK209Add190, conts, contentRTF, acstext;
		int findrt, i, retrn;
		AcDbBlockTable* pBlk;
		acdbHostApplicationServices()->workingDatabase()->
			getBlockTable(pBlk, AcDb::kForRead);
		AcDbBlockTableRecord* pBlkRcd;
		pBlk->getAt(ACDB_MODEL_SPACE, pBlkRcd, AcDb::kForRead);
		pBlk->close();
		AcDbBlockTableRecordIterator* pIter;
		pBlkRcd->newIterator(pIter);
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			AcDbEntity* pEnt;
			pIter->getEntity(pEnt, AcDb::kForRead);
			//对比append 和concat（concatenate v 链接 adj. 连锁的）compare collate
			//deleteatindex find left match
			if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
			{
				//AcGePoint3dArray txtbound2pt;
				AcDbText* pTxt = AcDbText::cast(pEnt);
				AcString txtstr;
				pTxt->textString(txtstr);
				//pTxt->getBoundingPoints(txtbound2pt);

				findrt = txtstr.find(_T("DK309"));
				if (findrt != -1)
				{
					TxtDK309 = txtstr;
					//选择集变量
					ads_name SSDK309; struct resbuf* PointL;
					AcGePoint2dArray SSPL; AcGePoint2d SSpt;
					AcGePoint3d DK309Alimtpzt;
					Adesk::Int32 length; Acad::ErrorStatus es;
					DK309Alimtpzt = pTxt->alignmentPoint();
					for (i = 0; i < 10; i++)
					{
						SSpt.x = DK309Alimtpzt.x + cos(i * pi / 5) * 45;
						SSpt.y = DK309Alimtpzt.y + sin(i * pi / 5) * 45;
						SSPL.append(SSpt);
					}
					PointL = acutBuildList(RTPOINT, SSPL[0], RTPOINT, SSPL[1]
						, RTPOINT, SSPL[2], RTPOINT, SSPL[3], RTPOINT, SSPL[4], RTPOINT, SSPL[5]
						, RTPOINT, SSPL[6], RTPOINT, SSPL[7], RTPOINT, SSPL[8], RTPOINT, SSPL[9]
						, RTNONE);
					retrn = acedSSGet(_T("CP"), PointL, NULL, NULL, SSDK309);
					if (retrn != RTNORM)
					{
						acedAlert(_T("\n Selction set error!"));
						return;
					}
					acedSSLength(SSDK309, &length);
					for (i = 0; i < length; i++)
					{
						ads_name ent;
						acedSSName(SSDK309, i, ent);
						AcDbObjectId objId;
						acdbGetObjectId(objId, ent);
						es = acdbOpenAcDbEntity(pEnt, objId,
							AcDb::kForRead);
						if (es == Acad::eWasOpenForWrite) { continue; }
						if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
						{
							AcDbText* DKappend590 = AcDbText::cast(pEnt);
							DKappend590->textString(txtstr);
							if (txtstr.find(_T("590")) != -1)
							{
								DK309n590 = TxtDK309.concat(txtstr); 
								TxtDK209Add190 = TxtDK309.append(txtstr);
								//TxtDK309.append(txtstr); error
								TxtDK309 += txtstr;
								//有一个bug，单独1式 不成立，
								// 12式能同时成立
								// 123式中 唯有3式不成立
								// 故全用Mtext进行数据匹配 不用嵌套太多次循环
								//  数据处理留作另一个request 
								// 不属于隧道智能设计范畴
								//unknown reason
								break;
							}
						}
					}
					acutRelRb(PointL);
					acedSSFree(SSDK309);
					//不能循环套循环
					// 循环套选择集  不太行  变量得是自动临时变量
					// 可以一直留在内存中的静态变量或自动变量
					//for (pIter->start(); !pIter->done(); pIter->step())
					//{
					//	pIter->getEntity(pEnt, AcDb::kForRead);
					//	if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
					//	{
					//		AcDbText* p590Txt = AcDbText::cast(pEnt);
					//		p590Txt->textString(txtstr);
					//		findrt = txtstr.find(_T("+590"));
					//		if (findrt != -1)
					//		{
					//			TxtDK309Add590.append(txtstr);
					//			break;
					//		}
					//	}
					//}
					//pIter->seek(pTxt);
					//pIter->step();

				}

				//acutPrintf(_T("\n text->textstring(AcString) = %s"), txtstr);
				//acutPrintf(_T("\ntext->aligmentpoint().x = %f y = %f"
				//	"\npmt->position().x = %f y = %f "
				//	"\ngetboundpoints(point3dary)pointary[0].x = %f pointary[0].y = %f"
				//	"\npointary[1].x = %f y=%f"),
				//	pTxt->alignmentPoint().x, pTxt->alignmentPoint().y,
				//	pTxt->position().x, pTxt->position().y,
				//	txtbound2pt[0].x, txtbound2pt[0].y,
				//	txtbound2pt[1].x, txtbound2pt[1].y);


			}
			else if (pEnt->isKindOf(AcDbMText::desc()) == Adesk::kTrue)
			{
				AcDbMText* pMt = AcDbMText::cast(pEnt);
				AcGePoint3dArray Mtxtboundptary;

				pMt->contents(conts);
				pMt->contentsRTF(contentRTF);
				pMt->text(acstext);
				pMt->getBoundingPoints(Mtxtboundptary);
				findrt = acstext.find(_T("DK309"));
				if (findrt != -1)
				{
					MTDK309590 = acstext;
				}
				//acutPrintf(_T("\nMtext->text(AcString&) = %s\nAND Mtext->content(AcString)"
				//	" = %s\npmtext->contentsRTF(acs) = %s"),
				//	acstext, contents, contentRTF);
				//acutPrintf(_T("\npmt->location().x = %f y = %f "
				//	"\npmt->normal().x = %f y = %f"
				//	"\ngetboundpoints(point3dary)pointary[0].x = %f pointary[0].y = %f"
				//	"\npointary[1].x = %f y=%f"),
				//	pMt->location().x, pMt->location().y,
				//	pMt->normal().x, pMt->normal().y,
				//	Mtxtboundptary[0].x, Mtxtboundptary[0].y,
				//	Mtxtboundptary[1].x, Mtxtboundptary[1].y);

			}
		}

		acutPrintf(_T("\ncollate = %d,DK309n590 = %s "
			"DK297190 = %s"
		)
			, MTDK309590.collate(DK309n590), DK309n590
			, TxtDK209Add190
		);
		acutPrintf(_T("\nCompare result = %d\nMTDK309590 = %s,And%s \nTXTDK309 = %s")
			, MTDK309590.compare(DK309n590), MTDK309590, conts, TxtDK309);

		delete pIter;
		pBlkRcd->close();
		return;
	}

	static void LJYMyGroupMyPickFirst() {
		ads_name result;
		int iRet = acedSSGet(ACRX_T("_I"), NULL, NULL, NULL, result);
		if (iRet == RTNORM)
		{
			// There are selected entities
			// Put your command using pickfirst set code here
		}
		else
		{
			// There are no selected entities
			// Put your command code here
		}
	}
	static void LJYMyGroupMySessionCmd() {
		// Put your command code here
	}
	static int ads_MyLispFunction() {
		//struct resbuf *args =acedGetArgs () ;

		// Put your command code here

		//acutRelRb (args) ;

		// Return a value to the AutoCAD Lisp Interpreter
		// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal

		return (RTNORM);
	}

};
//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CAuxTunnelProjDemo01App)

ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, ATnl, ATnlL, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
ACED_ADSSYMBOL_ENTRY_AUTO(CAuxTunnelProjDemo01App, MyLispFunction, false)
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, Test, Testl, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, Test02, Test02l, ACRX_CMD_MODAL, NULL)

//NOTE
//指标1.工期图中顶点趋于一致，2.在指定时间内完成，怎么设计迭代，
// 从工期图返回峰值来看，
// 将高峰值辅道往低峰值辅道上移动
//绝对误差：用极差代替。每一次迭代得到一个结果，再变换初始点进行新一轮迭代，
// 得到多个解，取重叠区域最多的解进行迭代，最后再所有解中选出前5。
// 修改反复打开层表
////打开的图层表仅用于当前图层对已有的外部数据库无效
//1. 根据扩展数据 XDATA 或者扩展词典 XDICTIONARY
// 确定需要画辅道的隧道  
//2. 找出A点，每隔一公里确定一个A点，每5个A点确定一个最优解，
// 先计算隧道长度，确定此隧道有几个A点，
// 每隔5km放置一个，左右移动以迭代出最优解
// 扩展数据XDATA找出隧道x
// 	//AcGePoint2dArray SamplePtArray;
	//AcDbLayerTable* pMainTunnelLay;
	//acdbHostApplicationServices()->workingDatabase()->
	//	getSymbolTable(pMainTunnelLay, AcDb::kForRead);
	//AcDbObjectId MTId, RdId;
	//if (pMainTunnelLay->has(_T("PMXX-hngtx"))
	//	&& pMainTunnelLay->has(_T("隧道-斜井")) == Adesk::kFalse) {
	//	AfxMessageBox(_T("Layer 'tunnel' doesn't exist."));
	//	pMainTunnelLay->close();
	//	return false;
	//}
	//if (!pMainTunnelLay->has(_T("4330"))) {
	//	AfxMessageBox(_T("Layer 'Rd' doesn't exist."));
	//	pMainTunnelLay->close();
	//	return false;
	//}
	//pMainTunnelLay->getAt(_T("隧道-斜井"), MTId);
	//pMainTunnelLay->getAt(_T("4330"), RdId);
	//pMainTunnelLay->close();
	// XDATA
	//unsigned int index = 1; functoin getpointat() obsolete 
	//struct resbuf* pRb;
	// //pRb = pEnt->xData();
	//pRb = pEnt->xData(_T("何家梁隧道"));
	// //free(pRb);
	//在n等分点选组A点acgepoint3d;
	//加入外部数据库或者直接画polyline3d X

//隧道层可以确定A点位置，里程位置结合截面图确立A点标高
bool TunSamplePtTemp(AcGePoint3dArray& APtAry,
	AcDbObjectId TunnelLay, AcDbObjectId mileLay)
{
	double  Elevat1, Elevat2, A_z = 0, TnlLen = 0;
	int Num, WorkLen, Numb, VtxNum
		//, descount, count = 0
		, i = 1, j = 1
		, slope, Length;
	Adesk::Boolean rbl;//隧道端点
	AcGePoint2d PtStart, PtEnd;//隧道端点
	AcGePoint3d APt;
	AcString TunStPtMTxtToAcStr, TunEdPtMTxtToAcStr;//隧道里程
	AcGePoint2dArray TunPtAry;
	AcGeCurve2d* pMainTunCur;
	AcDbObjectIdArray DiZhiIdAry;
	AcDbBlockTable* pBlk;
	acdbHostApplicationServices()->workingDatabase()->
		getSymbolTable(pBlk, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkRcd;
	pBlk->getAt(ACDB_MODEL_SPACE, pBlkRcd, AcDb::kForRead);
	pBlk->close();
	AcGePoint3d DK309590inSectionDWG
		, DK297190inSectionDWG, DK297190inDWG, DK309590inDWG
		, DK309590elevation, DK297190Elevation;//起点终点高程位置
	AcDbBlockTableRecordIterator* pIter;
	pBlkRcd->newIterator(pIter);
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		//count++;
		//descount = count / 10000;
		//if (descount > 1)
		//{
		//	i++;
		//	acutPrintf(_T("\n%d 0000 "), i);
		//	descount -= 1;
		//	if (i > 100)
		//	{
		//		acutPrintf((_T("iterate error")));
		//		acedAlert(_T("process terminated."));
		//		return false;
		//	}
		//}
		AcDbEntity* pTunnelPL;
		pIter->getEntity(pTunnelPL, AcDb::kForWrite);
		//找出主隧道
		if ((pTunnelPL->layerId() == TunnelLay) &&
			pTunnelPL->isKindOf(AcDbPolyline::desc())) {
			AcDbPolyline* pPL = AcDbPolyline::cast(pTunnelPL);
			OptmizeFunc::PLLength(pPL, TnlLen);
			if (TnlLen < 1000000 && TnlLen > TnlWorkingLen) {
				VtxNum = (int)TnlLen + 1;
				Num = (int)(TnlLen / TnlWorkingLen);
				Numb = Num + 1;
				WorkLen = (int)(TnlLen / Numb);
				OptmizeFunc::PolyToGeCurve((const AcDbPolyline*&)pPL, pMainTunCur);
				rbl = pMainTunCur->hasStartPoint(PtStart);
				rbl = pMainTunCur->hasEndPoint(PtEnd);
				pMainTunCur->getSamplePoints(VtxNum, TunPtAry);
				acutPrintf(_T("\nTnlLen = %d,Num = %d Numb = %d,workLen = %d")
					, VtxNum, Num, Numb, WorkLen);
				OptmizeFunc::TunmileTxtSSget(PtStart, TunStPtMTxtToAcStr);
				OptmizeFunc::TunmileTxtSSget(PtEnd, TunEdPtMTxtToAcStr);
				OptmizeFunc::ChooseExTlDb(TunStPtMTxtToAcStr, TunEdPtMTxtToAcStr
					, Elevat1, Elevat2, slope, Length, DiZhiIdAry);

				if (true)
				{
					//mAcString 
					//是DK309+590，或DK297+190
					//
					for (j = 1; j < Numb; j++) {
						A_z = Elevat1 + (WorkLen * j) * slope / 1000;
						APt = AcGePoint3d(TunPtAry[WorkLen * j - 1].x,
							TunPtAry[WorkLen * j - 1].y, A_z);
						APtAry.append(APt);
					}
				}
			}
			break;
		}
		pTunnelPL->close();
	}

	acutPrintf(_T("\n APtAry.Length() = ：%d"), APtAry.length());
	delete pIter;
	pBlkRcd->close();
	return true;
}

//前一个使用遍历结合图层 实体类型 扩展数据选取隧道
// 这是用选择集过滤器选择隧道
//
bool GetTunAPt(AcGePoint2dArray& APtAry)
{
	ads_name ssTunPl, ssText;
	struct resbuf* RbTunPl;
	struct resbuf* RbTxt;

	RbTunPl = acutBuildList(-4, _T("<AND"), RTDXF0, _T("PLINE"),
		8, _T("隧道-斜井"), -4, _T("AND>"), RTNONE);
	RbTxt = acutBuildList(RTDXF0, _T("TEXT"),
		8, _T("PMXX-hngtx,隧道-斜井"), 1, _T("DK*"), RTNONE);

	if (acedSSGet(_T("X"), NULL, NULL, RbTunPl, ssTunPl) != RTNORM)
		//|| acedSSGet(_T("X"), NULL, NULL, RbTxt, ssText) != RTNORM)
	{
		acutPrintf(_T("\nerror for unknown reason!"));
		return false;
	}

	Acad::ErrorStatus es;
	int i; AcDbEntity* pEMTun;
	Adesk::Int32 length, txtLen;
	acedSSLength(ssTunPl, &length);
	acedSSLength(ssText, &txtLen);
	acutPrintf(_T("\n隧道实体数:%d\n里程实体数：%d"), length, txtLen);
	double TnlLen;
	int Num, WorkLen, Numb, VtxNum, j = 1;
	for (i = 0; i < length; i++)
	{
		ads_name ent;
		acedSSName(ssTunPl, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		es = acdbOpenAcDbEntity(pEMTun, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		//acutPrintf(_T("\n实体类型%s,所在图层%s"), pEnt->isA()->name(), pEnt->layer());
		if (pEMTun->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue)
		{
			AcDbPolyline* pPLine = AcDbPolyline::cast(pEMTun);
			OptmizeFunc::PLLength(pPLine, TnlLen);
			if (TnlLen > 1000000 || TnlLen < TnlWorkingLen) { break; }
			VtxNum = (int)TnlLen + 1;
			Num = (int)(TnlLen / TnlWorkingLen);
			Numb = Num + 1;
			WorkLen = (int)(TnlLen / Numb);
			AcGeCurve2d* pMainTunCur;
			OptmizeFunc::PolyToGeCurve((const AcDbPolyline*&)pPLine, pMainTunCur);
			AcGePoint2dArray TunPtAry;
			Adesk::Boolean rbl;
			AcGePoint2d PtStart, PtEnd;

			rbl = pMainTunCur->hasStartPoint(PtStart);
			rbl = pMainTunCur->hasEndPoint(PtEnd);
			pMainTunCur->getSamplePoints(VtxNum, TunPtAry);
			for (j = 1; j < Numb; j++)
			{
				acutPrintf(_T("\n 断点%d"), j);
				APtAry.append(TunPtAry[WorkLen * j - 1]);
			}
		}
	}
	for (i = 0; i < txtLen; i++)
	{
		ads_name TxtEnt;
		acedSSName(ssText, i, TxtEnt);
		AcDbObjectId TobjId;
		acdbGetObjectId(TobjId, TxtEnt);
		es = acdbOpenAcDbEntity(pEMTun, TobjId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pEMTun->isKindOf(AcDbText::desc()) == Adesk::kTrue)
		{
			AcDbText* pTxt = AcDbText::cast(pEMTun);
			acutPrintf(_T("text%d"), i);
		}
	}

	acutPrintf(_T("\n APtAry.Length() = ：%d"), APtAry.length());


	pEMTun->close();
	acutRelRb(RbTunPl);
	acutRelRb(RbTxt);
	acedSSFree(ssText);
	acedSSFree(ssTunPl);
	return true;

}

//做出选择集后依次提取1.横洞HDPolyLineIdary 2.可能斜井XJPLIdAry,第一次范围选取，
//以道路（主干道）和高程初步筛选
//选择集选出隧道，里程，辅道入口及其周围高程合适等高线
bool SSGetFunc(AcDbObjectIdArray& HDPolyLineIdary,
	AcDbObjectIdArray& XJPLIdAry, AcGePoint3d& APt,
	AcDbObjectId& IsoLwhite, AcDbObjectId& IsoLorange,
	AcDbObjectId Isoellow)
{
	//variable
	int i, k, rt;//for 循环
	AcGePoint2d SSAreaPt;
	AcDbEntity* pEIsoPl;
	Adesk::Int32 IsoNum;
	Acad::ErrorStatus es;
	ads_name SSIsoPLs;//等高线选择集, 隧道选择集1-10个
	AcGePoint2dArray PLPtAry;
	struct resbuf* PointList;
	int NumofPl1 = 0; int XJNum = 0;
	double maxd = 1; int maxn = 0;
	for (k = 0; k < 8; k++)
	{
		//辅道最大长度1500m，坡度10度，最大高差260m  cos22.5<1.1
		SSAreaPt.x = APt.x + cos(k * pi / 4) * 1500 * 1.1;
		SSAreaPt.y = APt.y + sin(k * pi / 4) * 1500 * 1.1;
		PLPtAry.append(SSAreaPt);
	}
	PointList = acutBuildList(RTPOINT, PLPtAry[0], RTPOINT, PLPtAry[1]
		, RTPOINT, PLPtAry[2], RTPOINT, PLPtAry[3], RTPOINT, PLPtAry[4]
		, RTPOINT, PLPtAry[5], RTPOINT, PLPtAry[6], RTPOINT, PLPtAry[7], RTNONE);
	rt = acedSSGet(_T("CP"), PointList, NULL, NULL, SSIsoPLs);
	if (rt != RTNORM)
	{
		acutRelRb(PointList);
		acedAlert(_T("\nssget unknown error!"));
		acutPrintf(_T("\n Selection Set error."));
		return false;
	}
	acedSSLength(SSIsoPLs, &IsoNum);
	//acutPrintf(_T("\n选择集个数：%d"), length);

	for (i = 0; i < IsoNum; i++)
	{
		ads_name ent;
		acedSSName(SSIsoPLs, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		es = acdbOpenAcDbEntity(pEIsoPl, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pEIsoPl->isKindOf(AcDbPolyline::desc())
			&& (pEIsoPl->layerId() == IsoLwhite
				|| pEIsoPl->layerId() == Isoellow
				|| pEIsoPl->layerId() == IsoLorange))
		{
			//AcGePoint3dArray BPtAry;找出所有B点
			AcDbPolyline* pPoly = AcDbPolyline::cast(pEIsoPl);
			AcGePoint2d Apt2d = AcGePoint2d(APt.x, APt.y);//2维A点
			//选取横洞
			if (pPoly->elevation() <= APt.z)
			{
				//存在可能横洞
				//double dist; int min;
				//acutPrintf(_T("\n 第%d个多段线实体标高%f"), i, pPoly->elevation());
				//MinDistPt(pPoly, dist, min, Apt2d);
				NumofPl1++;
				HDPolyLineIdary.append(pEIsoPl->objectId());
			}
			else if (pPoly->elevation() <= (APt.z + 260))
			{
				//最小斜率小于10度，可能存在斜井
				MaxDistPt(pPoly, maxd, maxn, Apt2d);
				if (((pPoly->elevation() - APt.z) / maxd) < tan10)
				{
					XJNum++;
					//acutPrintf(_T("\n 第%d个多段线实体标高%f"), i, pPoly->elevation());
					XJPLIdAry.append(pEIsoPl->objectId());
				}
			}
		}

	}
	pEIsoPl->close();
	acutRelRb(PointList);
	acedSSFree(SSIsoPLs);
	return true;
}


double PlLength(AcDbPolyline* pPoly)
{
	double length = 0;
	AcGePoint2d Pt, PtNext;
	for (int i = 0; i < (int)(pPoly->numVerts() - 1); i++)
	{
		pPoly->getPointAt(i, Pt);
		pPoly->getPointAt(i + 1, PtNext);
		length = length + Pt.distanceTo(PtNext);
	}

	return length;
}

//计算多段线到指定点的最小距离和该点位置
bool MinDistPt(AcDbPolyline* pPoly, double& MinDist, int& MinNum, AcGePoint2d& Apt)
{
	//if (pPoly->isOnlyLines() == Adesk::kTrue) { return false; }
	unsigned int i, Tempi = 0; double TempDist;
	AcGePoint2d PtTemp, PtNext;
	pPoly->getPointAt(0, PtTemp);
	TempDist = PtTemp.distanceTo(Apt);
	//acutPrintf(_T("tempdist:%f"), TempDist);
	for (i = 1; i < (pPoly->numVerts()); i++)
	{
		pPoly->getPointAt(i, PtNext);
		if (TempDist > PtNext.distanceTo(Apt))
		{
			TempDist = PtNext.distanceTo(Apt);
			Tempi = i;
		}
		//acutPrintf(_T("tempdist:%f"), TempDist);
	}
	MinNum = Tempi;
	MinDist = TempDist;
	return true;

}

bool MaxDistPt(AcDbPolyline* pPoly, double& MaxDist, int& MaxNum, AcGePoint2d& Apt)
{
	//if (pPoly->isOnlyLines() == Adesk::kTrue) { return false; }
	int i, Tempi = 0; double TempDist;
	AcGePoint2d PtTemp, PtNext;
	pPoly->getPointAt(0, PtTemp);
	TempDist = PtTemp.distanceTo(Apt);

	for (i = 1; i < (int)(pPoly->numVerts()); i++)
	{
		pPoly->getPointAt(i, PtNext);
		if (TempDist < PtNext.distanceTo(Apt))
		{
			TempDist = PtNext.distanceTo(Apt);
			Tempi = i;
		}
		else {}//保持不变
	}
	MaxNum = Tempi;
	MaxDist = TempDist;
	return true;
}