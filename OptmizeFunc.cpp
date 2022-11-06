#include "stdafx.h"
#include "OptmizeFunc.h"
#include <cmath>
#include <cstdlib>
#include "CalcuFunc.h"
#include <string>


//选出合适的山脊，避免前排沟 作n 个选择集  输出acgepoint2dary
//参数：A点 （横洞PL 斜井PL）输出所有可能山脊位置，考虑角度，
//隧道点列，考虑隧道上方岩层厚度 


bool OptmizeFunc::Vallay()
{

	AcGeLineSeg3d acGeLineSegtmp1(AcGePoint3d(0, 100, 0), AcGePoint3d(50, 100, 0));
	AcGeLineSeg3d acGeLineSegtmp2(AcGePoint3d(-50, 50, 0), AcGePoint3d(-50, 120, 0));
	AcGeTol gGetol; gGetol.setEqualPoint(0.001);//设置精度
	AcGePoint3d intPnt;//交点
	bool bRec = acGeLineSegtmp1.intersectWith(acGeLineSegtmp2, intPnt, gGetol);//交点
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
	AcGeLineSeg2d line, * pLine; // 几何曲线的直线段部分
	AcGeCircArc2d arc, * pArc; // 几何曲线的圆弧部分
	AcGeVoidPointerArray geCurves; // 指向组成几何曲线各分段的指针数组
	acutPrintf(_T("\n 断点6"));
	nSegs = pPline->numVerts() - 1;

	// 根据多段线创建对应的分段几何曲线
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
	acutPrintf(_T("\n 断点7"));
	// 处理闭合多段线最后一段是圆弧的情况
	if (pPline->isClosed() && pPline->segType(nSegs) ==
		AcDbPolyline::kArc)
	{
		pPline->getArcSegAt(nSegs, arc);
		pArc = new AcGeCircArc2d(arc);
		pArc->setAngles(arc.startAng(), arc.endAng() -
			(arc.endAng() - arc.startAng()) / 100);
		geCurves.append(pArc);
	}
	acutPrintf(_T("\n 断点8"));
	// 根据分段的几何曲线创建对应的复合曲线
	if (geCurves.length() == 1)
	{
		pGeCurve = (AcGeCurve2d*)geCurves[0];
	}
	else
	{
		pGeCurve = new AcGeCompositeCurve2d(geCurves);
	}
	acutPrintf(_T("\n 断点9"));
	// 释放动态分配的内存
	if (geCurves.length() > 1)
	{
		for (int i = 0; i < geCurves.length(); i++)
		{
			delete geCurves[i];
		}
	}

	return true;
}

bool OptmizeFunc::SlopeNLength(AcDbObjectIdArray& XJPLIdAry, AcGePoint3d Apt)
{
	return false;
}

bool OptmizeFunc::NearRdPl(AcGePoint2d& BPt, AcDbObjectId RdLayId)
{
	ads_name SSname; struct resbuf* PointList;
	AcGePoint2dArray SSPL; AcGePoint2d SSpt; int i, rt;
	AcDbEntity* pEnt;
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
	acutPrintf(_T("\n选择集个数：%d"), length);
	for (i = 0; i < length; i++)
	{
		ads_name ent;
		acedSSName(SSname, i, ent);
		AcDbObjectId objId;
		acdbGetObjectId(objId, ent);

		es = acdbOpenAcDbEntity(pEnt, objId,
			AcDb::kForRead);
		if (es == Acad::eWasOpenForWrite) { continue; }
		if (pEnt->layerId() == RdLayId) { return true; }
	}

	return false;
}

//较后的约束，用于判断目标辅道与隧道夹角，确定辅道是否转向
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

//计算隧道上方岩层厚度 避免前排沟
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

//标记图层dz
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

bool OptmizeFunc::ChooseExTlDb(AcDbObjectIdArray& WYLvl, AcDbObjectIdArray& CD, AcDbObjectIdArray& ZC)
{
	//// 提示用户选择图形文件
	//AcDbLayerTable* pLay;
	//acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLay, AcDb::kForRead);
	//AcDbObjectId ChartInfoLayId, DZLayId, EditLID;//等高线ID...
	//pLay->getAt(_T("3"), ChartInfoLayId);// 表格名称x坐标确定选择数据
	//pLay->getAt(_T("地质"), DZLayId);//地质 和 长度 共用一个层名
	//pLay->getAt(_T("编辑"), EditLID);// 总长 图层：编辑  内轨顶面设计高程 设计坡度（‰）
	//当前数据库图层Id不能匹配外部数据库
	//AcDbText* DKText;
	// 第一次循环提取信息锚点
	// position acdbtext左下角坐标 alignmentPoint 文字对齐坐标，如果没有设置则为0点
	//AcGePoint3d postion, alnPoint;
	//AcDbObjectIdArray infoIdAry;

	int n = atoi("123");
	acutPrintf(_T("\nn=%d"), n);
	//AcDbDatabase* pExternalDb(Adesk::kFalse);
	AcDbDatabase* pExternalDb = new AcDbDatabase(Adesk::kFalse);
	struct resbuf* rb;
	rb = acutNewRb(RTSTR);
	if (RTNORM != acedGetFileD(_T("选择图形文件名称"), NULL, _T("dwg"), 0, rb))
	{
		acutRelRb(rb); return false;
	}
	if (Acad::eOk != pExternalDb->readDwgFile(rb->resval.rstring))
	{
		acedAlert(_T("读取DWG文件失败!")); acutRelRb(rb); return false;
	}
	acutRelRb(rb);
	bool bRet = OpenExDbinSpacLay(pExternalDb, _T("编辑"));

	AcDbBlockTable* pBlkTbl01;
	pExternalDb->getSymbolTable(pBlkTbl01, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkTblRcd1;
	pBlkTbl01->getAt(ACDB_MODEL_SPACE, pBlkTblRcd1, AcDb::kForRead);
	pBlkTbl01->close();
	AcDbBlockTableRecordIterator* pBlkTblRcdItr1;
	pBlkTblRcd1->newIterator(pBlkTblRcdItr1);
	AcDbEntity* pEnt;
	ACHAR* abc;
	std::wstring ws, ws1;
	AcString SDText, layerb;
	Acad::ErrorStatus es = eOk;
	double WLlvly = 0, CDy = 0, ZCy = 0;
	for (pBlkTblRcdItr1->start(); !pBlkTblRcdItr1->done(); pBlkTblRcdItr1->step())
	{
		pBlkTblRcdItr1->getEntity(pEnt, AcDb::kForRead);

		//if (pEnt->layerId() == ChartInfoLayId && 
		//if ( pEnt->layer() == _T("编辑") || 
		//	layerb == _T("编辑") || abc == _T("编辑"))
		//用AcString 转换成 ACHAR*识别图层是有效的
		// 由于textstring和text stringconst没有terminalsign，不能作为文字识别
		// 可用textstring（AcString）识别
		//

		if (pEnt->isKindOf(AcDbText::desc()) == Adesk::kTrue)
		{
			AcDbText* pText = AcDbText::cast(pEnt);
			//postion = pText->position();
			//alnPoint = pText->alignmentPoint();
			//const ACHAR* textstr = pText->textString();
			//pText->layer(layerb);
			// //acutPrintf(_T("\nstrlen:%d"), strlen((const char*)pText->textStringConst()));
			//通过x坐标顺序依次读取每段里程数据，
			//textstringconst() 和textstring() 两函数没有terminal sign（\0）
			// 不能放在判断语句里
			//
			SDText.length();
			pText->textString(SDText);
			
			abc = acutAcStringToAChar(SDText, es);
			
			ws = pText->textString();
			ws1 = SDText;
			if (pText->textStringConst() == _T("围岩级别") ||
				pText->textString() == _T("围岩级别") ||
				SDText == _T("围岩级别"))
			{
				acutPrintf(_T("\n  sdtext \ntext %s,layer:%s"), pText->textStringConst(), pText->layer());
			}
			else if (abc == _T("总  长（m）") || ws == _T("总  长（m）") ||
				pText->textString() == (ACHAR*)_T("总  长（m）"))
			{
				acutPrintf(_T("\npostion:x = %f y=%f\nalignment:x = %f,y = %f"),
					pText->position().x, pText->position().y,
					pText->alignmentPoint().x, pText->alignmentPoint().y);
				acutPrintf(_T("\n  ws \ntext %s,layer:%s"), pText->textStringConst(), pText->layer());
			}
			
			else if (pText->textString() == (ACHAR*)_T("内轨顶面设计高程"))
			{
				
			}
			else if (pText->textString() == _T("里  程"));
			{

			}

		}
		else if (pEnt->isKindOf(AcDbMText::desc()) == Adesk::kTrue)
		{
			AcDbMText* pMText = AcDbMText::cast(pEnt);
			pMText->text(SDText);
			if (SDText == _T("围岩级别"))
			{
				acutPrintf(_T("\nMtext:%s,LAYER:%s\nlocation:x=%d,y=%d"),
					pMText->text(), pMText->layer(), pMText->location());
			}
			else if (SDText == _T("长  度（m）"))
			{

			}
			else if (SDText == _T("总  长（m）"))
			{
				acutPrintf(_T("\nMtext:%s,\nlocation:x=%d,y=%d"),
					pMText->text(), pMText->location());
			}
			

		}


	}
	pBlkTblRcd1->close();
	delete pBlkTblRcdItr1;

	bRet = OpenExDbinSpacLay(pExternalDb, _T("地质"));
	AcDbBlockTable* pBlkTbl02;
	pExternalDb->getSymbolTable(pBlkTbl02, AcDb::kForRead);
	AcDbBlockTableRecord* pBlkTblRcd2;
	pBlkTbl02->getAt(ACDB_MODEL_SPACE, pBlkTblRcd2, AcDb::kForRead);
	pBlkTbl02->close();
	AcDbBlockTableRecordIterator* pBlkTblRcdItr2;
	pBlkTblRcd2->newIterator(pBlkTblRcdItr2);
	for (pBlkTblRcdItr2->start(); !pBlkTblRcdItr2->done(); pBlkTblRcdItr2->step())
	{


	}


	pEnt->close();
	pBlkTblRcd2->close();
	delete pBlkTblRcdItr2;
	delete pExternalDb;
	return true;
}









//private:
// 
//partialopenfilter 需要在readDWGfile()后和closeInput()前使用
bool OptmizeFunc::PartialOpenDatabase(AcDbDatabase* pDb)
{
	if (pDb == NULL)
		return false;
	// 指定限制窗口的两个角点
	ads_point pt1, pt2;
	pt1[X] = SectionDiaGramScaleXMin;
	pt1[Y] = SectionDiaGramScaleYMin;
	pt1[Z] = 0.0;
	pt2[X] = SectionDiaGramScaleXMax;
	pt2[Y] = SectionDiaGramScaleYMax;
	pt2[Z] = 0.0;

	// 获得当前视图的方向
	AcGeVector3d normal; // 视图法线方向
	struct resbuf rb;
	acedGetVar(_T("VIEWDIR"), &rb);
	normal[0] = rb.resval.rpoint[0];
	normal[1] = rb.resval.rpoint[1];
	normal[2] = rb.resval.rpoint[2];
	normal.normalize();
	// 将窗口角点从WCS转换到ECS
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // WCS 
	rbTo.restype = RT3DPOINT;
	rbTo.resval.rpoint[0] = normal[0];
	rbTo.resval.rpoint[1] = normal[1];
	rbTo.resval.rpoint[2] = normal[2];
	acedTrans(pt1, &rbFrom, &rbTo, FALSE, pt1);
	acedTrans(pt2, &rbFrom, &rbTo, FALSE, pt2);
	// 创建空间过滤器
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
	// 创建图层过滤器
	AcDbLayerFilter layerFilter;
	layerFilter.add(_T("地质"));
	layerFilter.add(_T("编辑"));
	layerFilter.add(_T("3"));
	// 对图形数据库应用局部加载
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
	// 指定限制窗口的两个角点
	ads_point pt1, pt2;
	pt1[X] = SectionDiaGramScaleXMin;
	pt1[Y] = SectionDiaGramScaleYMin;
	pt1[Z] = 0.0;
	pt2[X] = SectionDiaGramScaleXMax;
	pt2[Y] = SectionDiaGramScaleYMax;
	pt2[Z] = 0.0;

	// 获得当前视图的方向
	AcGeVector3d normal; // 视图法线方向
	struct resbuf rb;
	acedGetVar(_T("VIEWDIR"), &rb);
	normal[0] = rb.resval.rpoint[0];
	normal[1] = rb.resval.rpoint[1];
	normal[2] = rb.resval.rpoint[2];
	normal.normalize();
	// 将窗口角点从WCS转换到ECS
	struct resbuf rbFrom, rbTo;
	rbFrom.restype = RTSHORT;
	rbFrom.resval.rint = 0; // WCS 
	rbTo.restype = RT3DPOINT;
	rbTo.resval.rpoint[0] = normal[0];
	rbTo.resval.rpoint[1] = normal[1];
	rbTo.resval.rpoint[2] = normal[2];
	acedTrans(pt1, &rbFrom, &rbTo, FALSE, pt1);
	acedTrans(pt2, &rbFrom, &rbTo, FALSE, pt2);
	// 创建空间过滤器
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

