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


//const double pi = 3.14159265358979323846;
//const double tan10 = 0.176326980708;

bool TunSamplePtTemp(AcGePoint2dArray& APtAry);// current DWG
bool GetTunAPt(AcGePoint2dArray& APtAry);
//insert Wblock �������ܸ�д�ⲿ���ݿ⡣
//bool TunSamplePtNew(AcGePoint2dArray& APtAry, AcGeDoubleArray& APtParamAry, const ACHAR* FileName);
//ѡȡ��ǰ���ݿ��������Ϣ������Ӧ����ͼ����ȡ����߳�  ���AcGePoint3dArray


bool SSGetFunc(AcDbObjectIdArray& HDPolyLineIdary, AcDbObjectIdArray& XJPLIdAry
	, AcGePoint3d& APt, AcDbObjectId& LayerId1, AcDbObjectId& IsoId2
	, AcDbObjectId RdLayerId, AcDbObjectId RdId1);
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


	static void LJYMyGroupATnl() {
		int i, k;
		//��ǰ���ݿ�
		AcDbLayerTable* pLay;
		acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLay, AcDb::kForRead);
		AcDbObjectId IsolLayId, RdLayId, IsoId2, RdId2;//�ȸ���ID...
		pLay->getAt(_T("8110"), IsolLayId);//�ȸ���ͼ��const ACHAR* IsoLName = _T("")
		pLay->getAt(_T("4330"), RdLayId);//const ACHAR* RdName = _T("")
		pLay->getAt(_T("8120"), IsoId2);
		pLay->getAt(_T("4422"), RdId2);
		//....
		pLay->close();

		// ����A����
		int A_z = 700;
		AcGePoint2dArray TunAPtAry;
		AcDbObjectIdArray HDPLIdAry, XJPLIdAry, RdObjIdAry;
		acutPrintf(_T("\n new acdbobjectidary length:%d"), HDPLIdAry.length());
		TunSamplePtTemp(TunAPtAry);
		//GetTunAPt(TunAPtAry);
		acutPrintf(_T("\n ѡȡ��A�������%d"), TunAPtAry.length());
		AcGePoint3d APoint;
		double minD = 0; int minN = 0;
		AcDbEntity* pHDPLEnt;
		Acad::ErrorStatus es;
		for (i = 0; i < TunAPtAry.length(); i++)
		{
			APoint = AcGePoint3d(TunAPtAry[i].x, TunAPtAry[i].y, A_z);
			SSGetFunc(HDPLIdAry, XJPLIdAry, APoint, IsolLayId, IsoId2, RdLayId, RdId2);
			AcGePoint2d Bpoint;

			//�ϲ�һ���͸������ᶴ
			if (HDPLIdAry.length() == 1)
			{
				es = acdbOpenObject(pHDPLEnt, HDPLIdAry[0], AcDb::kForRead);
				if (es == Acad::eWasOpenForWrite) { continue; }
				AcDbPolyline* HDPL = AcDbPolyline::cast(pHDPLEnt);
				MinDistPt(HDPL, minD, minN, TunAPtAry[i]);
				acutPrintf(_T("\n����Сֵ֮�����Ϊ��minD %f, minN %f, "), minD, minN);
				HDPL->getPointAt(minN, Bpoint);
				CreateEnt::CreatePolyline(TunAPtAry[i], Bpoint, 2);
			}
			else if (HDPLIdAry.length() >= 2)
			{
				//�ڶ���������ѡ�����ŵ���ᶴ
				AcGeDoubleArray minDAry;
				int* minNAry = new int[HDPLIdAry.length() + 1];
				AcGePoint2dArray BpointAry;

				for (k = 0; k < HDPLIdAry.length(); k++)
				{
					es = acdbOpenObject(pHDPLEnt, HDPLIdAry[k], AcDb::kForRead);

					if (es == Acad::eWasOpenForWrite) { continue; }
					AcDbPolyline* HDPL = AcDbPolyline::cast(pHDPLEnt);

					MinDistPt(HDPL, minD, minN, TunAPtAry[i]);
					acutPrintf(_T("\n����Сֵ֮�����Ϊ��minD %f, minN %f, "), minD, minN);

					HDPL->getPointAt(minN, Bpoint);
					minDAry.append(minD);
					minNAry[k] = minN;
					BpointAry.append(Bpoint);
					//test
					CreateEnt::CreatePolyline(TunAPtAry[i], Bpoint, 1);

				}
				//

				delete[] minNAry;
			}
			//�õ㸽��û�кᶴ

			HDPLIdAry.removeAll();
			XJPLIdAry.removeAll();
		}
		pHDPLEnt->close();
		HDPLIdAry.~AcArray();
		XJPLIdAry.~AcArray();
		return;
	}

	static void LJYMyGroupTestAPt()
	{
		ads_name SSN; Acad::ErrorStatus es;
		AcDbEntity* pEPlLen;
		acedSSGet(NULL, NULL, NULL, NULL, SSN);
		Adesk::Int32 length;
		acedSSLength(SSN, &length);
		acutPrintf(_T("\nѡ�񼯸�����%d"), length);
		int i = 0;
		for (i = 0; i < length; i++)
		{
			ads_name ent;
			acedSSName(SSN, i, ent);
			AcDbObjectId objId;
			acdbGetObjectId(objId, ent);
			es = acdbOpenAcDbEntity(pEPlLen, objId,
				AcDb::kForRead);
			if (es == Acad::eWasOpenForWrite) { continue; }
			if (pEPlLen->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue) {
				AcDbPolyline* pPoly = AcDbPolyline::cast(pEPlLen);
				double L;
				OptmizeFunc::PLLength(pPoly, L);
				acutPrintf(_T("\nPolyline %d, length:%f"), i, L);
			}
		}
		pEPlLen->close();
		acedSSFree(SSN);

	}

	static void LJYMyGroupTest02()
	{
		AcDbObjectIdArray WYLvl, CD, ZC;
		OptmizeFunc::ChooseExTlDb(WYLvl, CD, ZC);

		acutPrintf(_T("\n��ǰ���ݿ�TEXT ��MTEXT "));
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
			pIter->getEntity(pEnt, AcDb::kForWrite);

			if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
			{
				AcGePoint3dArray txtbound2pt;
				AcDbText* pTxt = AcDbText::cast(pEnt);
				AcString txtstr;
				pTxt->textString(txtstr);
				pTxt->getBoundingPoints(txtbound2pt);
				acutPrintf(_T("\ntext->textstring() = %s\nAND text->textstring(AcString) = %s"),
					pTxt->textString(), txtstr);
				acutPrintf(_T("\ntext->aligmentpoint().x = %f y = %f"
					"\npmt->position().x = %f y = %f "
					"\ngetboundpoints(point3dary)pointary[0].x = %f pointary[0].y = %f"
					"\npointary[1].x = %f y=%f"),
					pTxt->alignmentPoint().x, pTxt->alignmentPoint().y,
					pTxt->position().x, pTxt->position().y,
					txtbound2pt[0].x, txtbound2pt[0].y,
					txtbound2pt[1].x, txtbound2pt[1].y);
			}
			else if (pEnt->isKindOf(AcDbMText::desc()) == Adesk::kTrue)
			{
				AcDbMText* pMt = AcDbMText::cast(pEnt);
				AcGePoint3dArray Mtxtboundptary;
				AcString contents, contentRTF;
				pMt->contents(contents);
				pMt->contentsRTF(contentRTF);
				pMt->getBoundingPoints(Mtxtboundptary);
				acutPrintf(_T("\nMtext->text() = %s\nAND Mtext->content(AcString)"
					" = %s\npmtext->contentsRTF(acs) = %s"),
					pMt->text(), contents, contentRTF);
				acutPrintf(_T("\npmt->location().x = %f y = %f "
					"\npmt->normal().x = %f y = %f"
					"\ngetboundpoints(point3dary)pointary[0].x = %f pointary[0].y = %f"
					"\npointary[1].x = %f y=%f"),
					pMt->location().x, pMt->location().y,
					pMt->normal().x, pMt->normal().y,
					Mtxtboundptary[0].x, Mtxtboundptary[0].y,
					Mtxtboundptary[1].x, Mtxtboundptary[1].y);
			}
		}
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
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, TestAPt, TestAPtl, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CAuxTunnelProjDemo01App, LJYMyGroup, Test02, Test02l, ACRX_CMD_MODAL, NULL)

//ָ��1.����ͼ�ж�������һ�£�2.��ָ��ʱ������ɣ�question����ô��Ƶ������ӹ���ͼ���ط�ֵ������
// ���߷�ֵ�������ͷ�ֵ�������ƶ�
//�������ü�����档ÿһ�ε����õ�һ��������ٱ任��ʼ�������һ�ֵ������õ�����⣬
// ȡ�ص��������Ľ���е�������������н���ѡ��ǰ5��
// 


//1. ������չ���� XDATA ������չ��¼ XRECORD
// ȷ����Ҫ�����������  
//2. �ҳ�A�㣬ÿ��һ����ȷ��һ��A�㣬ÿ5��A��ȷ��һ�����Ž⣬�ȼ���������ȣ�ȷ��������м���A�㣬
// ÿ��5km����һ���������ƶ��Ե��������Ž�
//
bool TunSamplePtTemp(AcGePoint2dArray& APtAry)
{
	//�򿪵�ͼ�������ڵ�ǰͼ������е��ⲿ���ݿ���Ч
	AcDbLayerTable* pMainTunnelLay;
	acdbHostApplicationServices()->workingDatabase()->
		getSymbolTable(pMainTunnelLay, AcDb::kForRead);
	AcDbObjectId MTId, RdId;
	if (pMainTunnelLay->has(_T("PMXX-hngtx"))
		&& pMainTunnelLay->has(_T("���-б��")) == Adesk::kFalse) {
		AfxMessageBox(_T("Layer 'tunnel' doesn't exist."));
		pMainTunnelLay->close();
		return false;
	}
	if (!pMainTunnelLay->has(_T("4330"))) {
		AfxMessageBox(_T("Layer 'Rd' doesn't exist."));
		pMainTunnelLay->close();
		return false;
	}
	pMainTunnelLay->getAt(_T("���-б��"), MTId);
	pMainTunnelLay->getAt(_T("4330"), RdId);
	pMainTunnelLay->close();
	AcDbBlockTable* pBlk;
	acdbHostApplicationServices()->workingDatabase()->
		getSymbolTable(pBlk, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkRcd;
	pBlk->getAt(ACDB_MODEL_SPACE, pBlkRcd, AcDb::kForRead);
	pBlk->close();
	AcDbBlockTableRecordIterator* pIter;
	pBlkRcd->newIterator(pIter);
	double TnlLen = 0;
	int count, descount, Num, WorkLen, Numb, VtxNum, i = 1, j = 1;
	count = 0;
	//AcGePoint2dArray SamplePtArray;
	//unsigned int index = 1; functoin getpointat() obsolete 
	//struct resbuf* pRb;
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		count++;
		descount = count / 10000;
		if (descount > 1)
		{
			i++;
			acutPrintf(_T("\n%d 0000 "), i);
			descount -= 1;
			if (i > 100)
			{
				acutPrintf((_T("iterate error")));
				acedAlert(_T("process terminated."));
				break;
			}
		}
		AcDbEntity* pEnt;
		pIter->getEntity(pEnt, AcDb::kForWrite);
		//pRb = pEnt->xData();
		//pRb = pEnt->xData(_T("�μ������"));
		//�ҳ������
		if (pEnt->layerId() == MTId &&
			pEnt->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue) {
			//&& pRb != NULL){
			AcDbPolyline* pPL = AcDbPolyline::cast(pEnt);
			OptmizeFunc::PLLength(pPL, TnlLen);
			if (TnlLen < 1000000 && TnlLen > TnlWorkingLen) {
				VtxNum = (int)TnlLen + 1;
				Num = (int)(TnlLen / TnlWorkingLen);
				Numb = Num + 1;
				WorkLen = (int)(TnlLen / Numb);
				AcGePoint2dArray TunPtAry;
				//if (pRb != NULL){
				AcGeCurve2d* pMainTunCur;
				OptmizeFunc::PolyToGeCurve((const AcDbPolyline*&)pPL, pMainTunCur);
				Adesk::Boolean rbl;
				AcGePoint2d PtStart, PtEnd;
				//acutPrintf(_T("\n �ϵ�11"));
				rbl = pMainTunCur->hasStartPoint(PtStart);
				rbl = pMainTunCur->hasEndPoint(PtEnd);
				pMainTunCur->getSamplePoints(VtxNum, TunPtAry);
				//acutPrintf(_T("\n �ϵ�12"));
				acutPrintf(_T("\nTnlLen = %d,Num = %d Numb = %d,workLen = %d")
					, VtxNum, Num, Numb, WorkLen);
				for (j = 1; j < Numb; j++) {
					APtAry.append(TunPtAry[WorkLen * j - 1]);
				}
			}
			//free(pRb);
			//��n�ȷֵ�ѡ��A��acgepoint3d;
			//�����ⲿ���ݿ����ֱ�ӻ�polyline3d
			//
			//AcGePoint3dArray
			//for (k = 1; k < (Num + 1); k++)
			//APtAry.append(TunPtAry[(int)TnlLen / 2]);
			//}
		}
		//else if (pEnt->layerId() == MTId &&
		//	pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue) {
		//	AcDbText* ptxt = AcDbText::cast(pEnt);
		//	acutPrintf(_T("\n text:%s"), ptxt->textString());
		//}

		pEnt->close();
	}
	//if (acutRelRb(pRb) != RTNORM) {
	//	acutPrintf(_T("\nresult buffer release error."));
	//	return false;
	//}
	acutPrintf(_T("\n APtAry.Length() = ��%d"), APtAry.length());
	delete pIter;
	pBlkRcd->close();
	return true;
}

//ǰһ��ʹ�ñ������ͼ�� ʵ������ ��չ����ѡȡ���
// ������ѡ�񼯹�����ѡ�����
//
bool GetTunAPt(AcGePoint2dArray& APtAry)
{
	ads_name ssTunPl, ssText;
	struct resbuf* RbTunPl;
	struct resbuf* RbTxt;
	//
	//filter index
	//acedInitGet(RSG_NONULL|RSG_NOZERO|RSG_NONEG,NULL);
	//acedGetInt(_T("ѡ��һ��������"), &index);
	//switch (index)
	//{
	//case 2:filter2 x
	//ѡ��ͼ����Բ���� pt1 �� pt2 ���㹹�ɵľ����ڵ�Բ
	//struct resbuf* rb = acutBuildList(RTDXF0, _T("CIRCLE"), // ʵ������
	//	-4, _T(">,>,*"),10, pt1, /// ��ϵ�������ͨ���/ Բ��
	//	-4, _T("<,<,*"), 10, pt2, RTNONE); /// ��ϵ�������ͨ���/ Բ��
	// //case 3:filter3  v// ��չ���ݵ�Ӧ�ó����� ���������ִ�Сд
	// //��չ�����е�ASCII�ַ��� ֻ��ѡȡ���ַ�������Road�����������
	// //����ʵ�����Ͳ�������
	// rb = acutBuildList(1000, _T("Road"), RTNONE);
	// rb = acutBuildList(-4, _T("<OR"), RTDXF0, _T("TEXT"), RTDXF0, _T("MTEXT"),
	//	-4, _T("OR>"), RTNONE);  
	// //case 1:  v
	//	rb = acutBuildList(RTDXF0, _T("CIRCLE"),
	//	-4, _T(">="), 40, 30, RTNONE);
	// //case 4:filter4    v // ʵ������// ͼ�� // �������ַ���
	// rb = acutBuildList(RTDXF0, _T("TEXT"),
	//	8, _T("0,ͼ��1"), 1, _T("*cadhelp*"), RTNONE);
	// //default:ѡ��ͼ����λ��0���ϵ�����ֱ��   v// ʵ������// ͼ��
	// rb = acutBuildList(RTDXF0, _T("LINE"), 8, _T("0"), RTNONE);
	//}
	// 	//ʵ�����Ͱ�����������ߺ�text��ͼ�����pmxx-h.. �����-б��
	//����ѡ�񼯣�һ��ѡ���������ߣ�һ��ѡ������������ڶ�λ
	RbTunPl = acutBuildList(-4, _T("<AND"), RTDXF0, _T("PLINE"),
		8, _T("���-б��"), -4, _T("AND>"), RTNONE);
	RbTxt = acutBuildList(RTDXF0, _T("TEXT"),
		8, _T("PMXX-hngtx,���-б��"), 1, _T("DK*"), RTNONE);

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
	acutPrintf(_T("\n���ʵ����:%d\n���ʵ������%d"), length, txtLen);
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
		//acutPrintf(_T("\nʵ������%s,����ͼ��%s"), pEnt->isA()->name(), pEnt->layer());
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
				acutPrintf(_T("\n �ϵ�%d"), j);
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
			acutPrintf(_T("text%d = %s"), i, pTxt->textString());
		}
	}

	acutPrintf(_T("\n APtAry.Length() = ��%d"), APtAry.length());


	pEMTun->close();
	acutRelRb(RbTunPl);
	acutRelRb(RbTxt);
	acedSSFree(ssText);
	acedSSFree(ssTunPl);
	return true;

}

//����ѡ�񼯺�������ȡ1.�ᶴHDPolyLineIdary 2.����б��XJPLIdAry,��һ�η�Χѡȡ��
//�Ե�·�����ɵ����͸̳߳���ɸѡ
//ѡ��ѡ���������̣�������ڼ�����Χ�̺߳��ʵȸ���
bool SSGetFunc(AcDbObjectIdArray& HDPolyLineIdary, AcDbObjectIdArray& XJPLIdAry
	, AcGePoint3d& APt, AcDbObjectId& IsoLayerId1, AcDbObjectId& IsoId2
	, AcDbObjectId RdLayerId, AcDbObjectId RdId1)
{
	//variable
	int i, k, rt;//for ѭ��
	AcGePoint2d SSAreaPt;
	AcDbEntity* pEIsoPl;
	Adesk::Int32 TunNum, txtLen, IsoNum;
	Acad::ErrorStatus es;
	ads_name SSIsoPLs, ssTun, ssText;//�ȸ���ѡ��, ���ѡ��1-10��
	AcGePoint2dArray PLPtAry;
	struct resbuf* PointList;
	struct resbuf* RbTunPl;
	struct resbuf* RbTxt;
	int NumofPl1 = 0; int XJNum = 0;
	double maxd = 1; int maxn = 0;

	RbTunPl = acutBuildList(-4, _T("<AND"), RTDXF0, _T("PLINE"),
		8, _T("���-б��"), -4, _T("AND>"), RTNONE);
	RbTxt = acutBuildList(RTDXF0, _T("TEXT"),
		8, _T("PMXX-hngtx"), 1, _T("DK*"), RTNONE);
	if (acedSSGet(_T("X"), NULL, NULL, RbTunPl, ssTun) != RTNORM
		|| acedSSGet(_T("X"), NULL, NULL, RbTxt, ssText) != RTNORM)
	{
		acedAlert(_T("Filter Selection Set FAILED."));
		return false;
	}
	acedSSLength(ssTun, &TunNum);
	acedSSLength(ssText, &txtLen);
	for (k = 0; k < 8; k++)
	{
		//������󳤶�1500m���¶�10�ȣ����߲�260m  cos22.5<1.1
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
	//acutPrintf(_T("\nѡ�񼯸�����%d"), length);

	for (i = 0; i < IsoNum; i++)
	{
		ads_name ent;
		acedSSName(SSIsoPLs, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		es = acdbOpenAcDbEntity(pEIsoPl, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pEIsoPl->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue &&
			(pEIsoPl->layerId() == IsoLayerId1 || pEIsoPl->layerId() == IsoId2))
		{
			//AcGePoint3dArray BPtAry;�ҳ�����B��
			AcDbPolyline* pPoly = AcDbPolyline::cast(pEIsoPl);
			AcGePoint2d Apt2d = AcGePoint2d(APt.x, APt.y);//2άA��
			//ѡȡ�ᶴ
			if (pPoly->elevation() <= APt.z)
			{
				//���ڿ��ܺᶴ
				//double dist; int min;
				//acutPrintf(_T("\n ��%d�������ʵ����%f"), i, pPoly->elevation());
				//MinDistPt(pPoly, dist, min, Apt2d);
				NumofPl1++;
				HDPolyLineIdary.append(pEIsoPl->objectId());
			}
			else if (pPoly->elevation() <= (APt.z + 260))
			{
				//��Сб��С��10�ȣ����ܴ���б��
				MaxDistPt(pPoly, maxd, maxn, Apt2d);
				if (((pPoly->elevation() - APt.z) / maxd) < tan10)
				{
					XJNum++;
					//acutPrintf(_T("\n ��%d�������ʵ����%f"), i, pPoly->elevation());
					XJPLIdAry.append(pEIsoPl->objectId());
				}
			}
		}

	}
	pEIsoPl->close();
	acutRelRb(RbTunPl);
	acutRelRb(RbTxt);
	acedSSFree(ssText);
	acedSSFree(ssTun);
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

//�������ߵ�ָ�������С����͸õ�λ��
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
		else {}//���ֲ���
	}
	MaxNum = Tempi;
	MaxDist = TempDist;
	return true;
}