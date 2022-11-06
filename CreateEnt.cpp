#include "stdafx.h"
#include "CreateEnt.h"

AcDbObjectId CreateEnt::CreateLine(AcGePoint3d ptStart, AcGePoint3d ptEnd)
{
	AcDbLine* pLine = new AcDbLine(ptStart, ptEnd);
	// ��ʵ����ӵ�ͼ�����ݿ�
	AcDbObjectId lineId;
	lineId = CreateEnt::PostToModelSpace(pLine);
	return lineId;
}
//��ǰ���ݿ��ʵ������
AcDbObjectId CreateEnt::PostToModelSpace(AcDbEntity* pEnt)
{
	AcDbBlockTable* pBlockTable;
	acdbHostApplicationServices()->workingDatabase()
		->getSymbolTable(pBlockTable, AcDb::kForRead);
	AcDbBlockTableRecord* pBlockTableRecord;
	pBlockTable->getAt(ACDB_MODEL_SPACE, pBlockTableRecord,
		AcDb::kForWrite);
	AcDbObjectId entId;
	pBlockTableRecord->appendAcDbEntity(entId, pEnt);
	pBlockTable->close();
	pBlockTableRecord->close();
	pEnt->close();
	return entId;
}


//��insert���������½����ݿ�����޸�

AcDbObjectId CreateEnt::CreatePolyline(AcGePoint2dArray points, double width)
{
	int numVertices = points.length();
	AcDbPolyline* pPoly = new AcDbPolyline(numVertices);
	for (int i = 0; i < numVertices; i++)
	{
		pPoly->addVertexAt(i, points.at(i), 0, width, width);
	}
	AcDbObjectId polyId;
	polyId = CreateEnt::PostToModelSpace(pPoly);
	return polyId;
}

AcDbObjectId CreateEnt::CreatePolyline(AcGePoint2d ptStart, AcGePoint2d ptEnd, double width)
{
	AcGePoint2dArray points;
	points.append(ptStart);
	points.append(ptEnd);
	return CreateEnt::CreatePolyline(points, width);
}

AcDbObjectId CreateEnt::Create3dPolyline(AcGePoint3dArray points)
{
	AcDb3dPolyline* pPoly3d = new
		AcDb3dPolyline(AcDb::k3dSimplePoly, points);
	return CreateEnt::PostToModelSpace(pPoly3d);
}
