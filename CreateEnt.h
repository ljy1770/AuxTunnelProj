#pragma once
class CreateEnt
{
public:
	static AcDbObjectId CreateLine(AcGePoint3d ptStart, AcGePoint3d ptEnd);

	static AcDbObjectId PostToModelSpace(AcDbEntity* pEnt);

	//static AcDbObjectId PostToNewDataBase(AcDbEntity* pEnt, AcDbDatabase*& NewDB);

	static AcDbObjectId CreatePolyline(AcGePoint2dArray points, double width);

	static AcDbObjectId CreatePolyline(AcGePoint2d ptStart, AcGePoint2d ptEnd, double width);

	static AcDbObjectId Create3dPolyline(AcGePoint3dArray points);
};

