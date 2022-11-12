#pragma once


const double pi = 3.14159265358979323846;
const double tan10 = 0.176326980708;
const double ATMaxLen = 1500;//辅道最大长度 单位：米
const double Minthickness = 50;//岩层厚度 单位：米
const double Dist = 150.0;//洞口到主路的最大距离
const double TnlWorkingLen = 5000;//隧道最大施工长度，暂定；

const double SectionDiaGramScaleXMin = 5000;//截面图范围，选择集
const double SectionDiaGramScaleXMax = 9000;
const double SectionDiaGramScaleYMin = -1000;
const double SectionDiaGramScaleYMax = -500.0;
const double chartfloorhight = 10;// 表格层高


//struct sectiondiagraminfo {
//	ACHAR* DiZhi;
//	double Changdu;
//	ACHAR* LiCheng;
//	double Xielv;
//	AcGePoint3d position;
//};
class OptmizeFunc
{
public:
	//前排沟约束，也可以作为大范围搜索的条件之一
	static bool Vallay();
	//大范围找出口，小范围找山脊
	//在出口小范围内先找到高点再确定山脊
	static bool Ridge(AcGePoint2dArray& RidgePtAry, AcGePoint2d& TunAPt
		, AcDbObjectIdArray HD, AcDbObjectIdArray XJ);

	static bool PolyToGeCurve(const AcDbPolyline*& pPline, AcGeCurve2d
		*& pGeCurve);

	static bool SlopeNLength(AcDbObjectIdArray& XJPLIdAry, AcGePoint3d Apt);
	//此功能集成在选择集里check

	static bool NearRdPl(AcGePoint2d& BPt, AcDbObjectId RdLayId);
	//约束3选择集确定B点附近有无道路

	static bool TunmileTxtSSget(AcGePoint2d& Point2d, AcString mileString);

	static bool AngBetweenAtNMt(AcGePoint2d APt, AcGePoint2d BPt
		, AcGePoint2dArray TunAPtAry, int num);//check

	static bool Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt, AcDbObjectId SpErthLay);
	//直线隧道判断有无特殊地质

	static bool Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt
		, AcGePoint2d Cpt, AcDbObjectId SpErthLay);//折线重载函数，三个选择集

	static bool SSGtoAvgH(AcGePoint2d Pt, double& elevation, double r);

	static bool PLLength(AcDbPolyline* pPL, double& TolLen);

	//读取隧道高程
	static bool TnlDataBase(const ACHAR* FileN);

	static bool ChooseExTlDb(AcString& DK1, AcString& DK2, double& Elevation1
		, double& elvation2, int& slope, int& Lenth, AcDbObjectIdArray DiZhiIdAry);

private:
	static bool PartialOpenDatabase(AcDbDatabase* pDb);

	static bool OpenExDbinSpacLay(AcDbDatabase* pDb, const ACHAR* LayId);
};

