#pragma once


const double pi = 3.14159265358979323846;
const double tan10 = 0.176326980708;
const double ATMaxLen = 1500;//������󳤶� ��λ����
const double Minthickness = 50;//�Ҳ��� ��λ����
const double Dist = 150.0;//���ڵ���·��������
const double TnlWorkingLen = 5000;//������ʩ�����ȣ��ݶ���

const double SectionDiaGramScaleXMin = 5000;//����ͼ��Χ��ѡ��
const double SectionDiaGramScaleXMax = 9000;
const double SectionDiaGramScaleYMin = -1000;
const double SectionDiaGramScaleYMax = -500.0;
const double chartfloorhight = 10;// �����


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
	//ǰ�Ź�Լ����Ҳ������Ϊ��Χ����������֮һ
	static bool Vallay();
	//��Χ�ҳ��ڣ�С��Χ��ɽ��
	//�ڳ���С��Χ�����ҵ��ߵ���ȷ��ɽ��
	static bool Ridge(AcGePoint2dArray& RidgePtAry, AcGePoint2d& TunAPt
		, AcDbObjectIdArray HD, AcDbObjectIdArray XJ);

	static bool PolyToGeCurve(const AcDbPolyline*& pPline, AcGeCurve2d
		*& pGeCurve);

	static bool SlopeNLength(AcDbObjectIdArray& XJPLIdAry, AcGePoint3d Apt);
	//�˹��ܼ�����ѡ����check

	static bool NearRdPl(AcGePoint2d& BPt, AcDbObjectId RdLayId);
	//Լ��3ѡ��ȷ��B�㸽�����޵�·

	static bool TunmileTxtSSget(AcGePoint2d& Point2d, AcString mileString);

	static bool AngBetweenAtNMt(AcGePoint2d APt, AcGePoint2d BPt
		, AcGePoint2dArray TunAPtAry, int num);//check

	static bool Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt, AcDbObjectId SpErthLay);
	//ֱ������ж������������

	static bool Spacialearth(AcGePoint2d Apt, AcGePoint2d Bpt
		, AcGePoint2d Cpt, AcDbObjectId SpErthLay);//�������غ���������ѡ��

	static bool SSGtoAvgH(AcGePoint2d Pt, double& elevation, double r);

	static bool PLLength(AcDbPolyline* pPL, double& TolLen);

	//��ȡ����߳�
	static bool TnlDataBase(const ACHAR* FileN);

	static bool ChooseExTlDb(AcString& DK1, AcString& DK2, double& Elevation1
		, double& elvation2, int& slope, int& Lenth, AcDbObjectIdArray DiZhiIdAry);

private:
	static bool PartialOpenDatabase(AcDbDatabase* pDb);

	static bool OpenExDbinSpacLay(AcDbDatabase* pDb, const ACHAR* LayId);
};

