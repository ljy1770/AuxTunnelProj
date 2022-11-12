#include "stdafx.h"
#include "OptmizeFunc.h"
#include <cmath>
#include <cstdlib>
#include "CalcuFunc.h"
#include <string>


//ѡ�����ʵ�ɽ��������ǰ�Ź� ��n ��ѡ��  ���acgepoint2dary
//������A�� ���ᶴPL б��PL��������п���ɽ��λ�ã����ǽǶȣ�
//������У���������Ϸ��Ҳ��� 


bool OptmizeFunc::Vallay()
{

	AcGeLineSeg3d acGeLineSegtmp1(AcGePoint3d(0, 100, 0), AcGePoint3d(50, 100, 0));
	AcGeLineSeg3d acGeLineSegtmp2(AcGePoint3d(-50, 50, 0), AcGePoint3d(-50, 120, 0));
	AcGeTol gGetol; gGetol.setEqualPoint(0.001);//���þ���
	AcGePoint3d intPnt;//����
	bool bRec = acGeLineSegtmp1.intersectWith(acGeLineSegtmp2, intPnt, gGetol);//����
	return false;
}

bool OptmizeFunc::Ridge(AcGePoint2dArray& RidgePtAry, AcGePoint2d& TunAPt,
	AcDbObjectIdArray HD, AcDbObjectIdArray XJ)
{
	return false;
}

bool OptmizeFunc::PolyToGeCurve(const AcDbPolyline*& pPline, AcGeCurve2d*& pGeCurve)
{
	int nSegs; // number of segment
	AcGeLineSeg2d line, * pLine; // �������ߵ�ֱ�߶β���
	AcGeCircArc2d arc, * pArc; // �������ߵ�Բ������
	AcGeVoidPointerArray geCurves; // ָ����ɼ������߸��ֶε�ָ������
	nSegs = pPline->numVerts() - 1;

	for (int i = 0; i < nSegs; i++)
	{
		if (pPline->segType(i) == AcDbPolyline::kLine)
		{
			pPline->getLineSegAt(i, line);
			pLine = new AcGeLineSeg2d(line);
			geCurves.append(pLine);
		}
		else if (pPline->segType(i) == AcDbPolyline::kArc)
		{
			pPline->getArcSegAt(i, arc);
			pArc = new AcGeCircArc2d(arc);
			geCurves.append(pArc);
		}
	}
	if (pPline->isClosed() && pPline->segType(nSegs) ==
		AcDbPolyline::kArc)
	{
		pPline->getArcSegAt(nSegs, arc);
		pArc = new AcGeCircArc2d(arc);
		pArc->setAngles(arc.startAng(), arc.endAng() -
			(arc.endAng() - arc.startAng()) / 100);
		geCurves.append(pArc);
	}
	if (geCurves.length() == 1)
	{
		pGeCurve = (AcGeCurve2d*)geCurves[0];
	}
	else
	{
		pGeCurve = new AcGeCompositeCurve2d(geCurves);
	}
	if (geCurves.length() > 1)
	{
		for (int i = 0; i < geCurves.length(); i++)
		{
			delete geCurves[i];
		}
	}

	return true;
}
//б��ѡȡ
bool OptmizeFunc::SlopeNLength(AcDbObjectIdArray& XJPLIdAry, AcGePoint3d Apt)
{
	return false;
}



bool OptmizeFunc::NearRdPl(AcGePoint2d& BPt, AcDbObjectId RdLayId)
{
	ads_name SSname; struct resbuf* PointList;
	AcGePoint2dArray SSPL; AcGePoint2d SSpt; int i, rt;
	AcDbEntity* pRoad;
	for (i = 0; i < 4; i++)
	{
		SSpt.x = BPt.x + cos(i * pi) * 700;
		SSpt.y = BPt.y + sin(i * pi) * 700;
	}
	PointList = acutBuildList(RTPOINT, SSPL[0], RTPOINT, SSPL[1],
		RTPOINT, SSPL[2], RTPOINT, SSPL[3], RTNONE);
	rt = acedSSGet(_T("CP"), PointList, NULL, NULL, SSname);
	if (rt != RTNORM)
	{
		acedAlert(_T("\n unknown error!"));
		return false;
	}
	Adesk::Int32 length; Acad::ErrorStatus es;
	acedSSLength(SSname, &length);
	acutPrintf(_T("\nѡ�񼯸�����%d"), length);
	for (i = 0; i < length; i++)
	{
		ads_name ent;
		acedSSName(SSname, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);

		es = acdbOpenAcDbEntity(pRoad, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pRoad->layerId() == RdLayId) { return true; }
	}

	return false;
}

bool OptmizeFunc::TunmileTxtSSget(AcGePoint2d& Point2d, AcString MileAcString)
{
	ads_name SSname; struct resbuf* PointList;
	AcGePoint2dArray SSPL; AcGePoint2d SSpt; int i, rt;
	AcDbEntity* pLiCHengTxt;
	for (i = 0; i < 10; i++)
	{
		SSpt.x = Point2d.x + cos(i * pi / 5) * 45;
		SSpt.y = Point2d.y + sin(i * pi / 5) * 45;
		SSPL.append(SSpt);
	}//���ö˵㸽��45�׷�Χ����̱�ʶ
	PointList = acutBuildList(RTPOINT, SSPL[0], RTPOINT, SSPL[1]
		, RTPOINT, SSPL[2], RTPOINT, SSPL[3], RTPOINT, SSPL[4], RTPOINT, SSPL[5]
		, RTPOINT, SSPL[6], RTPOINT, SSPL[7], RTPOINT, SSPL[8], RTPOINT, SSPL[9]
		, RTNONE);
	rt = acedSSGet(_T("CP"), PointList, NULL, NULL, SSname);
	if (rt != RTNORM)
	{
		acedAlert(_T("\n Selction set error!"));
		pLiCHengTxt->close();
		return false;
	}
	Adesk::Int32 length; Acad::ErrorStatus es;
	
	const ACHAR* DK = _T("DK");
	acedSSLength(SSname, &length);
	acutPrintf(_T("\nѡ�񼯸�����%d"), length);
	for (i = 0; i < length; i++)
	{
		ads_name ent;
		acedSSName(SSname, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		es = acdbOpenAcDbEntity(pLiCHengTxt, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pLiCHengTxt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
		{
			AcDbText* mileTxt = AcDbText::cast(pLiCHengTxt);
			mileTxt->textString(MileAcString);
			if (MileAcString.find(DK) != -1)
			{
				return true;
			}
		}
		else if (pLiCHengTxt->isKindOf(AcDbMText::desc()) == Adesk::kTrue)
		{
			AcDbMText* MileMtext = AcDbMText::cast(pLiCHengTxt);
			MileMtext->text(MileAcString);
			if (MileAcString.find(DK) != -1)
			{
				return true;
			}
		}
	}
	acedAlert(_T("\n cannot find DK*,\nplz move text"));
	pLiCHengTxt->close();
	return false;
}

//�Ϻ��Լ���������ж�Ŀ�긨��������нǣ�ȷ�������Ƿ�ת��
bool OptmizeFunc::AngBetweenAtNMt(AcGePoint2d APt, AcGePoint2d BPt, AcGePoint2dArray TunAPtAry, int num)
{
	double a1, a2, b1, b2, AB, cosA1, cosA2;
	AB = APt.distanceTo(BPt);
	a1 = BPt.distanceTo(TunAPtAry[num + 1]);//a1=bc
	a2 = BPt.distanceTo(TunAPtAry[num - 1]);//a2=bd
	b1 = APt.distanceTo(TunAPtAry[num + 1]);//b1=ac
	b2 = APt.distanceTo(TunAPtAry[num - 1]);//b2=ad
	cosA1 = (AB * AB + b1 * b1 - a1 * a1) / (2 * b1 * AB);
	cosA2 = (AB * AB + b2 * b2 - a2 * a2) / (2 * b2 * AB);
	if (fabs(cosA1) < 0.866 && fabs(cosA2) < 0.866) { return true; }
	else { return false; }
}

bool OptmizeFunc::Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt, AcDbObjectId SpErthLay)
{

	return false;
}

bool OptmizeFunc::Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt, AcGePoint2d Cpt, AcDbObjectId SpErthLay)
{
	return false;
}

//��������Ϸ��Ҳ��� ����ǰ�Ź�
bool OptmizeFunc::SSGtoAvgH(AcGePoint2d Pt, double& elevation, double r)
{
	int i, k, rt, plNum = 0; double SUM = 0;
	AcGePoint2d SSpt;
	ads_name SSName;
	AcGePoint2dArray PLPtAry;
	struct resbuf* PointList;
	for (k = 0; k < 4; k++)
	{
		SSpt.x = Pt.x + cos(k * pi / 2) * r * 1.414;
		SSpt.y = Pt.y + sin(k * pi / 2) * r * 1.414;
		PLPtAry.append(SSpt);
	}
	PointList = acutBuildList(RTPOINT, PLPtAry[0], RTPOINT, PLPtAry[1],
		RTPOINT, PLPtAry[2], RTPOINT, PLPtAry[3], RTNONE);
	AcDbEntity* pEnt;
	Acad::ErrorStatus es;
	rt = acedSSGet(_T("CP"), PointList, NULL, NULL, SSName);
	if (rt != RTNORM)
	{
		acedAlert(_T("\n unknown error!"));
		acutPrintf(_T("\n Selection Set error."));
		return false;
	}
	Adesk::Int32 length;
	acedSSLength(SSName, &length);

	for (i = 0; i < length; i++)
	{
		ads_name ent;
		acedSSName(SSName, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);
		es = acdbOpenAcDbEntity(pEnt, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pEnt->isKindOf(AcDbPolyline::desc()) == Adesk::kTrue)
		{
			plNum++;
			SUM = SUM + pEnt->database()->elevation();
		}
	}
	if (plNum == 0) { return false; }
	elevation = SUM / plNum;

	acutRelRb(PointList);
	acedSSFree(SSName);
	return true;
}

bool OptmizeFunc::PLLength(AcDbPolyline* pPL, double& TolLen)
{
	AcGePoint2d Pt, PtNt;
	AcGeCircArc2d arc, * pArc;
	AcGeLineSeg2d line, * pLine;
	double length = 0.0; int nSegs;
	nSegs = pPL->numVerts();
	/*for (i = 0; i < pPL->numVerts() - 1; i++)
	{
		pPL->getPointAt(i, Pt);
		pPL->getPointAt(i + 1, PtNt);
		length = length + Pt.distanceTo(PtNt);
	}
	TolLen = length;*/
	for (int i = 0; i < nSegs; i++)
	{
		if (pPL->segType(i) == AcDbPolyline::kLine)
		{
			pPL->getLineSegAt(i, line);
			pLine = new AcGeLineSeg2d(line);
			length = length + pLine->length();
		}
		else if (pPL->segType(i) == AcDbPolyline::kArc)
		{
			pPL->getArcSegAt(i, arc);
			pArc = new AcGeCircArc2d(arc);
			length = length + (pArc->radius() *
				(abs(pArc->endAng() - pArc->startAng()))
				);
			//* pi / 180);
		}
	}
	TolLen = length;
	return true;
}

//���ͼ��dz
bool OptmizeFunc::TnlDataBase(const ACHAR* FileN)
{
	AcDbDatabase* pDb = new AcDbDatabase(Adesk::kFalse);
	pDb->readDwgFile(FileN);
	AcDbBlockTable* pBlTbl;
	pDb->getSymbolTable(pBlTbl, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkRec;
	pBlTbl->getAt(ACDB_MODEL_SPACE, pBlkRec, AcDb::kForWrite);
	pBlTbl->close();
	AcDbBlockTableRecordIterator* pBlkTblRcdIter;
	pBlkRec->newIterator(pBlkTblRcdIter);
	int i = 0;


	AcGePoint2d& pt = AcGePoint2d();
	//AcDbExtents *pExt;
	for (pBlkTblRcdIter->start(); !pBlkTblRcdIter->done(); pBlkTblRcdIter->step())
	{

	}
	pBlkRec->close();
	delete pBlkTblRcdIter;
	delete pDb;
	return true;
	return false;
}

	//acstring DK309+590 DK297+190 ��Ӧ�߳� 760.xx 1004.xx 
	//б�� int 20 �ܳ� int 12600,���� objectidArray?
bool OptmizeFunc::ChooseExTlDb(AcString& DK1, AcString& DK2, double& Elevation1
	, double& elvation2, int& slope, int& Lenth, AcDbObjectIdArray DiZhiIdAry)
{
	// NOTE:
	//// ��ʾ�û�ѡ��ͼ���ļ�
	//AcDbLayerTable* pLay;
	//acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLay, AcDb::kForRead);
	//AcDbObjectId ChartInfoLayId, DZLayId, EditLID;//�ȸ���ID...
	//pLay->getAt(_T("3"), ChartInfoLayId);// �������x����ȷ��ѡ������
	//pLay->getAt(_T("����"), DZLayId);//���� �� ���� ����һ������
	//pLay->getAt(_T("�༭"), EditLID);// �ܳ� ͼ�㣺�༭  �ڹ춥����Ƹ߳� ����¶ȣ��룩
	//��ǰ���ݿ�ͼ��Id����ƥ���ⲿ���ݿ�
	//AcDbText* DKText;
	// ��һ��ѭ����ȡ��Ϣê��
	// position acdbtext���½����� alignmentPoint ���ֶ������꣬���û��������Ϊ0��
	//AcGePoint3d postion, alnPoint;
	//AcDbObjectIdArray infoIdAry;
		//if (pEnt->layerId() == ChartInfoLayId  
		// �������ݿ�Ĳ��ȶ��ԣ�������layerID������ȡ�ⲿ���ݿ�
		// ��ѡ��ʽ�У�layerFilter,openpartialDB 
		// ������AcString ת���� ACHAR*ʶ��ͼ������Ч��
		// abc = acutAcStringToAChar(AcString, errorStatus);
		//  pText->layer(layerb);
		// if ( pEnt->layer() == _T("�༭") || 
		//	layerb == _T("�༭"))
		//
		// postion = pText->position();
		// alnPoint = pText->alignmentPoint();
		// ���ֵ��������꺯��û����
		// 
		//textstringconst()����û��terminal sign��\0��
		// ���ܷ����ж������
		// ����Text ��MText
		// �����ຯ���Ƽ�ʹ�ô���AcString& ��������
		//textString()�������Ƽ�
		// this function is deprecated,please use another overload
		// Ҳ�ò��ˣ�for unknown reason�����軻����һ�����غ���
		// ����textstring��AcString��ʶ��
		//
	int n = atoi("123");
	acutPrintf(_T("\nn=%d"), n);
	//AcDbDatabase* pExternalDb(Adesk::kFalse);
	AcDbDatabase* pExternalDb = new AcDbDatabase(Adesk::kFalse);
	struct resbuf* rb;
	Acad::ErrorStatus IterSeekRt;
	rb = acutNewRb(RTSTR);
	if (RTNORM != acedGetFileD(_T("ѡ��ͼ���ļ�����"), NULL, _T("dwg"), 0, rb))
	{
		acutRelRb(rb); return false;
	}
	if (Acad::eOk != pExternalDb->readDwgFile(rb->resval.rstring))
	{
		acedAlert(_T("��ȡDWG�ļ�ʧ��!")); acutRelRb(rb); return false;
	}
	acutRelRb(rb);
	//bool bRet = OpenExDbinSpacLay(pExternalDb, _T("�༭"));
	bool bRet = PartialOpenDatabase(pExternalDb);
	AcDbBlockTable* pBlkTbl01;
	pExternalDb->getSymbolTable(pBlkTbl01, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkTblRcd1;
	pBlkTbl01->getAt(ACDB_MODEL_SPACE, pBlkTblRcd1, AcDb::kForRead);
	pBlkTbl01->close();
	AcDbBlockTableRecordIterator* pBlkTblRcdItr1;
	pBlkTblRcd1->newIterator(pBlkTblRcdItr1);
	AcDbEntity* pSectionEntity;
	//AcString = wString
	AcString SDText, ZongChang, layBianji, PoDuAcString, PoduChangDu;
	Acad::ErrorStatus es = eOk;
	double WeiLan_y = 0, ChangDu_y = 0, ZhongChangy = 0
		, LiCHeng_y = 0, Sheji_y = 0, SheJi_h = 0, DKText_y = 0
		, DKPlusTxt_y = 0, SlopeTxt_y = 0, SlopeLen_y = 0; //����¶Ȱ�ǧ�ֱȼ���
	int64_t PoDu, PoDu_Len, i64t = 0;
	for (pBlkTblRcdItr1->start(); !pBlkTblRcdItr1->done(); pBlkTblRcdItr1->step())
	{

		pBlkTblRcdItr1->getEntity(pSectionEntity, AcDb::kForRead);
		if (pSectionEntity->isKindOf(AcDbText::desc()) == Adesk::kTrue)
		{
			AcDbText* pText = AcDbText::cast(pSectionEntity);
			pText->textString(SDText);
			if (SDText == _T("Χ�Ҽ���"))
			{
				WeiLan_y = pText->alignmentPoint().y;
			}
			else if (SDText == _T("��  ����m��"))
			{
				ZhongChangy = pText->alignmentPoint().y;
				acutPrintf(_T("\npostion:x = %f y=%f\nalignment:x = %f,y = %f"),
					pText->position().x, pText->position().y,
					pText->alignmentPoint().x, pText->alignmentPoint().y);
			}
			else if (SDText == _T("����¶ȣ��룩"))
			{
				AcDbText* pPoDutxt;
				Sheji_y = pText->alignmentPoint().y;
				SheJi_h = pText->height();// y+-3/4h�¶�����y������
				for (pBlkTblRcdItr1->start(); !pBlkTblRcdItr1->done(); pBlkTblRcdItr1->step())
				{
					if ((pSectionEntity->colorIndex() == 6)
						&& pSectionEntity->isKindOf(AcDbText::desc()))
					{
						pPoDutxt = AcDbText::cast(pSectionEntity);
						SlopeTxt_y = pPoDutxt->alignmentPoint().y;
						if ((SlopeTxt_y > (Sheji_y - SheJi_h * 0.75))
							&& (SlopeTxt_y < (Sheji_y + SheJi_h * 0.75)))
							//��ʱpPoDutext���¶Ȼ��߳���
						{
							pPoDutxt->textString(PoDuAcString);
							i64t = PoDuAcString.asDeci64();
							if (i64t > 0 && i64t < 1000)
							{
								PoDu = i64t;
							}
							else if (i64t > 1000)
							{
								PoDu_Len = i64t;
							}
						}
					}
					else if (pSectionEntity->colorIndex() == 1)
					{

					}
				}

				IterSeekRt = pBlkTblRcdItr1->seek(pSectionEntity);
				if (IterSeekRt != Acad::eOk)
				{
					acedAlert(_T("��������λʧ��"));
					pText->close();
					return false;
				}
			}
			else if (SDText == _T("��  ��"))
			{
				LiCHeng_y = pText->alignmentPoint().y;
				AcDbText* pDKtext;
				for (pBlkTblRcdItr1->start(); !pBlkTblRcdItr1->done(); pBlkTblRcdItr1->step())
				{
					pBlkTblRcdItr1->getEntity(pSectionEntity, AcDb::kForRead);
					if ((pSectionEntity->colorIndex() == 6)
						&& pSectionEntity->isKindOf(AcDbText::desc()))
					{
						pDKtext = AcDbText::cast(pSectionEntity);
						DKText_y = pDKtext->alignmentPoint().y;
						if (DKText_y == LiCHeng_y)
						{

						}
					}
				}
				pDKtext->close();
				IterSeekRt = pBlkTblRcdItr1->seek(pText);
				if (IterSeekRt != Acad::eOk)
				{
					acedAlert(_T("��������λʧ��"));
					pText->close();
					return false;
				}

			}
			pText->close();

		}
		else if (pSectionEntity->isKindOf(AcDbMText::desc()) == Adesk::kTrue)
		{
			//AcDbMText* pMText = AcDbMText::cast(pSectionEntity);
			//pMText->text(SDText);
			//if (SDText == _T("Χ�Ҽ���"))
			//{
			//	acutPrintf(_T("\nMtext:%s,LAYER:%s\nlocation:x=%d,y=%d"),
			//		pMText->text(), pMText->layer(), pMText->location());
			//}
			//else if (SDText == _T("��  �ȣ�m��"))
			//{
			//}
			//else if (SDText == _T("��  ����m��"))
			//{
			//	acutPrintf(_T("\nMtext:%s,\nlocation:x=%d,y=%d"),
			//		pMText->text(), pMText->location());
			//}
		}


	}

	pSectionEntity->close();
	pBlkTblRcd1->close();
	delete pBlkTblRcdItr1;
	delete pExternalDb;
	return true;
}









//private:
// 
//partialopenfilter ��Ҫ��readDWGfile()���closeInput()ǰʹ��
bool OptmizeFunc::PartialOpenDatabase(AcDbDatabase* pDb)
{
	if (pDb == NULL)
		return false;
	// ָ�����ƴ��ڵ������ǵ�
	ads_point pt1, pt2;
	pt1[X] = SectionDiaGramScaleXMin;
	pt1[Y] = SectionDiaGramScaleYMin;
	pt1[Z] = 0.0;
	pt2[X] = SectionDiaGramScaleXMax;
	pt2[Y] = SectionDiaGramScaleYMax;
	pt2[Z] = 0.0;

	// ��õ�ǰ��ͼ�ķ���
	AcGeVector3d normal; // ��ͼ���߷���
	struct resbuf rb;
	acedGetVar(_T("VIEWDIR"), &rb);
	normal[0] = rb.resval.rpoint[0];
	normal[1] = rb.resval.rpoint[1];
	normal[2] = rb.resval.rpoint[2];
	normal.normalize();
	// �����ڽǵ��WCSת����ECS
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // WCS 
	rbTo.restype = RT3DPOINT;
	rbTo.resval.rpoint[0] = normal[0];
	rbTo.resval.rpoint[1] = normal[1];
	rbTo.resval.rpoint[2] = normal[2];
	acedTrans(pt1, &rbFrom, &rbTo, FALSE, pt1);
	acedTrans(pt2, &rbFrom, &rbTo, FALSE, pt2);
	// �����ռ������
	AcGePoint2dArray array;
	array.append(AcGePoint2d(pt1[0], pt1[1]));
	array.append(AcGePoint2d(pt2[0], pt2[1]));
	AcDbSpatialFilter spatialFilter;
	spatialFilter.setDefinition(
		array,
		normal,
		pt1[Z],
		ACDB_INFINITE_XCLIP_DEPTH,
		-ACDB_INFINITE_XCLIP_DEPTH,
		Adesk::kTrue);
	// ����ͼ�������
	AcDbLayerFilter layerFilter;
	layerFilter.add(_T("����"));
	layerFilter.add(_T("�༭"));
	layerFilter.add(_T("3"));
	layerFilter.add(_T("GiCAD_PouMian"));

	// ��ͼ�����ݿ�Ӧ�þֲ�����
	Acad::ErrorStatus es;
	es = pDb->applyPartialOpenFilters(&spatialFilter, &layerFilter);

	if ((es == Acad::eOk) && pDb->isPartiallyOpened())
	{
		pDb->closeInput(true);
		return true;
	}
	else { return false; }
}

bool OptmizeFunc::OpenExDbinSpacLay(AcDbDatabase* pDb, const ACHAR* LayId)
{
	if (pDb == NULL)
		return false;
	// ָ�����ƴ��ڵ������ǵ�
	ads_point pt1, pt2;
	pt1[X] = SectionDiaGramScaleXMin;
	pt1[Y] = SectionDiaGramScaleYMin;
	pt1[Z] = 0.0;
	pt2[X] = SectionDiaGramScaleXMax;
	pt2[Y] = SectionDiaGramScaleYMax;
	pt2[Z] = 0.0;

	// ��õ�ǰ��ͼ�ķ���
	AcGeVector3d normal; // ��ͼ���߷���
	struct resbuf rb;
	acedGetVar(_T("VIEWDIR"), &rb);
	normal[0] = rb.resval.rpoint[0];
	normal[1] = rb.resval.rpoint[1];
	normal[2] = rb.resval.rpoint[2];
	normal.normalize();
	// �����ڽǵ��WCSת����ECS
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // WCS 
	rbTo.restype = RT3DPOINT;
	rbTo.resval.rpoint[0] = normal[0];
	rbTo.resval.rpoint[1] = normal[1];
	rbTo.resval.rpoint[2] = normal[2];
	acedTrans(pt1, &rbFrom, &rbTo, FALSE, pt1);
	acedTrans(pt2, &rbFrom, &rbTo, FALSE, pt2);
	// �����ռ������
	AcGePoint2dArray array;
	array.append(AcGePoint2d(pt1[0], pt1[1]));
	array.append(AcGePoint2d(pt2[0], pt2[1]));
	AcDbSpatialFilter spatialFilter;
	spatialFilter.setDefinition(
		array,
		normal,
		pt1[Z],
		ACDB_INFINITE_XCLIP_DEPTH,
		-ACDB_INFINITE_XCLIP_DEPTH,
		Adesk::kTrue);
	AcDbLayerFilter layerFilter;
	layerFilter.add(LayId);
	layerFilter.add(_T("3"));
	Acad::ErrorStatus es;
	es = pDb->applyPartialOpenFilters(&spatialFilter, &layerFilter);
	if (es == Acad::eOk) { return true; }
	else { return false; }
	//if ((es == Acad::eOk) && pDb->isPartiallyOpened())
	//{
	//	pDb->closeInput(true);
	//	return true;
	//}
	//else { return false; }
}

