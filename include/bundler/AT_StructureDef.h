#ifndef _AT_STRUCTURE_DEFINE_ZJ_20100527_
#define _AT_STRUCTURE_DEFINE_ZJ_20100527_

#include <map>
#include <vector>

// #include "MemoryPool.h"

#define  MaxTiePointNum		1000000L

#define PointID(imgNum, ptNum)		imgNum*1000000L+ptNum 
#define PointID2ImgNum(ptID)	ptID/1000000L
#define PointID2PtNum(ptID)	ptID%1000000L

#ifndef _AT_IMGPOINT_
#define _AT_IMGPOINT_
struct AT_ImgPoint
{
	long	imgID;
	double x, y;	
	int relflag;
};
#endif

struct	AT_ImgPointPair
{
	AT_ImgPoint	imgPt[2];
};

#ifndef _AT_TIEPOINT_
#define _AT_TIEPOINT_
struct AT_TiePoint
{
	double X, Y, Z;
	int  nImgPt;
	AT_ImgPoint *pImgPt;

	int  marker;

	AT_TiePoint::AT_TiePoint()
	{
		pImgPt=0;
	}

	AT_TiePoint::~AT_TiePoint()
	{
		if(pImgPt)	delete[] pImgPt;
	}
};
#endif


struct AT_3DTiePoint
{
	double X, Y, Z;
	int  nImgPt;
	AT_ImgPoint *pImgPt[4];

	int  marker;
};

// struct AT_ImgPointIdx
// {
// 	unsigned int  idx;
// 
// 	AT_ImgPoint data;
// };
typedef std::multimap<long, AT_ImgPointPair> TiePtsMAP;
typedef TiePtsMAP::iterator			tiePtsMapITER;

long getObjPtId( tiePtsMapITER iter )	;



class CATTiePtManager
{
public:
	CATTiePtManager();
	~CATTiePtManager();

public:
	void	Init( int numOfTotalImgs );	//设置影像总数

	void	AddTiePoint( long objPtID, int imgNum1, double x1, double y1,   int imgNum2, double x2, double y2 );

	// 返回某一片
	std::vector<tiePtsMapITER> &GetTiePoints( int imgNum );
	std::vector<tiePtsMapITER> &GetTiePoints( int imgNum1, int imgNum2 );
	std::vector<long> &GetTiePoints( int imgNum1, int imgNum2, int imgNum3, bool bSearchInFromBegin = false );
	

private:
	std::vector<tiePtsMapITER> m_vTiePtsIDBuf2D;
	std::vector<long> m_vTiePtsIDBuf3D;

	unsigned int  *m_pImgCurPtID;


	TiePtsMAP		m_tiePts;
};

/*
void RotateMat_fwk( double f, double w, double k, double *r);
*/

#endif