#include "StdAfx.h"
#include "CircleDetection.h"
#include "math.h"
//#include "LidarGeometry.h"
#include <time.h>
#include "\dps_geoway\include\tindll.h"
#include "\dps_geoway\include\TinToGrid.h"
#include "\dps_geoway\include\ParallaxGrid.h"
#include "\openrs\desktop\include\orsImage\orsIImageWriter.h"
#include "\openrs\desktop\include\orsImage\orsIImageSource.h"
#include "\openrs\desktop\include\orsSRS\orsISpatialReference.h"
#include "\openrs\desktop\include\orsImageGeometry\orsIImageGeometryService.h"

#define _Export_Accumulator_

//	POINT2D *pData		激光点平面坐标  单位(m)
//	double radius		标定板圆形半径  单位(m)
//  POINT2D &center		圆心平面坐标    单位(m)
void DetectCircleCenter_2d(POINT2D *pData, int ptNum, double radius, POINT2D &center)
{
	double mx, my;	//坐标均值
	int i, j, k;
	double r, r2;
	unsigned int *pAMatrix=NULL;	//累加器矩阵
	double theta;	//递增角(弧度)
	int angleNum;
	POINT2D	AMOpt;			//累加矩阵原点坐标
	double  unitL=0.01;		//单位长度1cm
	int  ASize;				//累加器大小
	double dx, dy;
	int irow, icol;
	double drow, dcol;
	double thresh;
	double delta;
	double radius2;
	
	ASize=int(radius/unitL);
	ASize*=2;	//圆对应的外接矩形

	if((pAMatrix=new unsigned int[ASize*ASize])==NULL)
	{
		return;
	}
	memset(pAMatrix, 0, sizeof(unsigned int)*ASize*ASize);

	//统计离散点集中心
	mx=my=0.0;
	for(i=0; i<ptNum; i++)
	{
		mx+=pData[i].x;
		my+=pData[i].y;
	}

	mx/=ptNum;
	my/=ptNum;

// 	for(i=0; i<ptNum; i++)
// 	{
// 		pData[i].x-=mx;
// 		pData[i].y-=my;
// 	}

	AMOpt.x=mx-radius;
	AMOpt.y=my+radius;
	theta=1.0*unitL/radius;
	angleNum=int(2*PI/theta);

	thresh=0.005;	
	radius2=radius*radius;
	for(i=0; i<ASize; i++)
	{
		dy=my+(ASize/2-i)*unitL;

		for(j=0; j<ASize; j++)
		{
			dx=mx+(j-ASize/2)*unitL;
			
			for(r=radius; r>0.005; r-=0.01)
			{
				r2=r*r;
				for(k=0; k<ptNum; k++)
				{
					delta=(pData[k].x-dx)*(pData[k].x-dx)+(pData[k].y-dy)*(pData[k].y-dy)-r2;
					if(delta<thresh)
					{
						pAMatrix[i*ASize+j]++;
					}	
				}
			}
		}
	}

// 	for(i=0; i<ptNum; i++)
// 	{
// 		for(j=0; j<angleNum; j++)
// 		{
// 			dx=radius*cos(j*theta);
// 			dy=radius*sin(j*theta);
// 			
// 			icol=int((pData[i].x+dx-AMOpt.x)/unitL+0.5);
// 			irow=int((AMOpt.y-(pData[i].y+dy))/unitL+0.5);
// 
// 			if(icol<ASize && irow<ASize && icol>=0 && irow>=0)
// 			{
// 				pAMatrix[irow*ASize+icol]++;
// 			}
// 		}
// 	}

	unsigned int maxCount=0;
	int step=300;
	for(i=0; i<ASize; i++)
	{
		for(j=0; j<ASize; j++)
		{
			if(maxCount<pAMatrix[i*ASize+j])
			{
				maxCount=pAMatrix[i*ASize+j];
				irow=i; 
				icol=j;
			}
		}
	}
	
	center.x=AMOpt.x+icol*unitL;
	center.y=AMOpt.y-irow*unitL;


#ifdef _Export_Accumulator_
	//将累加矩阵导出到文本文件中	
	char *pName="G:\\D_experiment\\hough\\Accumulator.txt";
	FILE *fp=NULL;

	fp=fopen(pName, "wt");
	
	fprintf(fp, "##  x y count\n");
	fprintf(fp, "x/y ");
	for(j=0; j<ASize; j++)
	{
		dx=AMOpt.x+j*unitL;
		fprintf(fp, "%.3lf ", dx);
	}
	fprintf(fp, "\n");

	for(i=0; i<ASize; i++)
	{
		dy=AMOpt.y-i*unitL;
		fprintf(fp, "%.3lf ", dy);

		for(j=0; j<ASize; j++)
		{
			dx=AMOpt.x+j*unitL;
			
			fprintf(fp, "%d ", pAMatrix[i*ASize+j]);

			if(pAMatrix[i*ASize+j]>maxCount-step)
			{
				TRACE("%.3f  %.3f  %d\n", dx, dy, pAMatrix[i*ASize+j]);
			}
		}
		fprintf(fp, "\n");

	}
	
	fclose(fp);		fp=NULL;
#endif

//将累加器导出为影像中
//#ifdef	_Export_Accumulator_Image_
//#endif


	double sArea; //面积
	maxCount-=step;	//调整候选区大小
	for(i=0, sArea=0; i<ASize; i++)
	{//统计候选区面积
		for(j=0; j<ASize; j++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				sArea+=1.0;
		}
	}

	sArea/=2;
	
	double aS=0, lineS=0;
//	double l;
	for(i=0; i<ASize; i++)
	{//按行方向进行面积积分
		lineS=0;
		for(j=0; j<ASize; j++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				lineS++;
		}

		if(aS+lineS>sArea)
		{
			drow=(sArea-aS)/lineS+i;
			
			//center.y=AMOpt.y-l*unitL;
			break;
		}
		else
			aS+=lineS;
	}

	aS=0;
	for(j=0; j<ASize; j++)
	{//按列方向进行面积积分
		lineS=0;
		for(i=0; i<ASize; i++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				lineS++;
		}

		if(aS+lineS>sArea)
		{
			dcol=(sArea-aS)/lineS+j;
			
			//center.x=AMOpt.x+l*unitL;
			break;
		}
		else
			aS+=lineS;
	}

	center.x=mx+(dcol-ASize/2)*unitL;
	center.y=my+(ASize/2-drow)*unitL;

	if(pAMatrix)
	{
		delete pAMatrix;
		pAMatrix=NULL;
	}
}

#define  _Export_Simulate_Point2D_
#define  _Simulate_Grid_
bool simulate_onecircle(int &ptNum, double radius, POINT2D *pData)
{
	bool	*pSelFlag=NULL;	
	double	unit=0.01;	//1cm划分网格
	int size;
	int i;
//	int angle_q;		//角度量化值
//	int radius_q;		//半径量化值
//	double dr, dtheta;
	double dx, dy;

	size=int(radius/unit);
	size*=2;

	if(ptNum>size*size/4)
	{
		AfxMessageBox("模拟的点数过多，不能保证随机分布!");
		return false;
	}

///////////////////////////模拟圆///////////////////////////////////////
#ifdef _Simulate_OneCircle_

	if((pSelFlag=new bool[size*size])==NULL)
		return false;
	
	memset(pSelFlag, 0, sizeof(bool)*size*size);
	

	angle_q=360;
	radius_q=int(radius/unit);
	i=0;
	srand((unsigned)time(NULL));
	while(i<ptNum)
	{
		
		bool found;
		int idx;
		int irow, icol;
		
		do 
		{
			found = false;
			//dr = rand() % radius_q * radius*unit;
			dr=radius/2;
			dtheta=rand() % angle_q * PI/180.0;

			dx=dr*cos(dtheta);
			dy=dr*sin(dtheta);
			
			icol=int((dx+radius)/unit+0.5);
			irow=int((radius-dy)/unit+0.5);

			if(icol<0)	icol=0;
			if(icol>=size)	icol=size-1;
			if(irow<0)	irow=0;
			if(irow>=size)	irow=size-1;

			ASSERT(icol>=0 && icol<size && irow>=0 && irow<size);
			
			
			idx=irow*size+icol;
			if(pSelFlag[idx]==0)
			{
				found=true;
				pSelFlag[idx]=true;
			}
			
		} while (!found);
		
		pData[i].x=dx;
		pData[i].y=dy;
		i++;
	}

#endif

////////////////////模拟格网/////////////////////////////////////
#ifdef _Simulate_Grid_
	double space;	
	double circleArea;
	double grdSpace;


	circleArea=PI*radius*radius;
	space=sqrt(circleArea/ptNum);
	
	grdSpace=0.1;	
	size=radius/grdSpace;	//0.1m划分网格
	size*=2;

	if((pSelFlag=new bool[size*size])==NULL)
		return false;
	
	memset(pSelFlag, 0, sizeof(bool)*size*size);

	i=0;
	int j=0;
	int row, col;
	int curNum;
	row=int(radius/space+0.5);
	col=int(radius/space+0.5);

	curNum=0;
	for(i=1; i<row; i++)
	{
		for(j=1; j<col; j++)
		{
			dx=j*space;
			dy=i*space;

			if(dx*dx+dy*dy<=radius*radius)
			{
				pData[curNum].x=dx;
				pData[curNum].y=dy;
				curNum++;

				pData[curNum].x=-dx;
				pData[curNum].y=dy;
				curNum++;

				pData[curNum].x=dx;
				pData[curNum].y=-dy;
				curNum++;

				pData[curNum].x=-dx;
				pData[curNum].y=-dy;
				curNum++;
			}
			if(curNum>=ptNum)
				break;
		}
	}


	ptNum=curNum;
#endif


#ifdef _Export_Simulate_Point2D_
	//将模拟的点导出到文件中	
	char *pName="G:\\D_experiment\\hough\\simulate2D.txt";
	FILE *fp=NULL;
	
	fp=fopen(pName, "wt");
	
	fprintf(fp, "##  x y\n");
	for(i=0; i<ptNum; i++)
	{
		fprintf(fp, "%.3lf %.3lf\n", pData[i].x, pData[i].y);
	}
	
	fclose(fp);		fp=NULL;
#endif
	


	if(pSelFlag)
	{
		delete pSelFlag;
		pSelFlag=NULL;
	}

	return	true;
}

//POINT2D *pData1	大圆环上的点
//int ptNum1		大圆环上的点数
//double radius1	大圆半径
//POINT2D *pData2	小圆上的点
//int ptNum2		小圆上的点数
//double radius2	小圆半径
//POINT2D &center	圆心坐标
void DetectConcentric_2d(POINT2D *pData1, int ptNum1, double radius1, POINT2D *pData2, int ptNum2, double radius2, POINT2D &center)
{
	double mx, my;	//坐标均值
	int i, j, k;
	double r, r2;
	unsigned int *pAMatrix=NULL;	//累加器矩阵
	double theta;	//递增角(弧度)
	int angleNum;
	POINT2D	AMOpt;			//累加矩阵原点坐标
	double  unitL=0.01;		//单位长度1cm
	int  ASize;				//累加器大小
	double dx, dy;
	int irow, icol;
	double drow, dcol;
	double thresh;
	double delta;
//	double radius2;
	
	ASize=int(radius1/unitL);
	ASize*=2;	//圆对应的外接矩形

	if((pAMatrix=new unsigned int[ASize*ASize])==NULL)
	{
		return;
	}
	memset(pAMatrix, 0, sizeof(unsigned int)*ASize*ASize);

	//统计离散点集中心
	mx=my=0.0;
	for(i=0; i<ptNum1; i++)
	{
		mx+=pData1[i].x;
		my+=pData1[i].y;
	}
	for(i=0; i<ptNum2; i++)
	{
		mx+=pData2[i].x;
		my+=pData2[i].y;
	}

	mx/=(ptNum1+ptNum2);
	my/=(ptNum1+ptNum2);

// 	for(i=0; i<ptNum; i++)
// 	{
// 		pData[i].x-=mx;
// 		pData[i].y-=my;
// 	}

	AMOpt.x=mx-radius1;
	AMOpt.y=my+radius1;
	theta=1.0*unitL/radius1;
	angleNum=int(2*PI/theta);

	thresh=0.005;	
//	radius2=radius*radius;

	for(i=0; i<ASize; i++)
	{
		dy=my+(ASize/2-i)*unitL;
		
		for(j=0; j<ASize; j++)
		{
			dx=mx+(j-ASize/2)*unitL;
			
			for(r=radius1; r>radius2+0.005; r-=0.01)
			{
				r2=r*r;
				for(k=0; k<ptNum1; k++)
				{
					delta=(pData1[k].x-dx)*(pData1[k].x-dx)+(pData1[k].y-dy)*(pData1[k].y-dy)-r2;
					if(delta<thresh)
					{
						pAMatrix[i*ASize+j]++;
					}	
				}
			}

			for(r=radius2; r>0.005; r-=0.01)
			{
				r2=r*r;
				for(k=0; k<ptNum2; k++)
				{
					delta=(pData2[k].x-dx)*(pData2[k].x-dx)+(pData2[k].y-dy)*(pData2[k].y-dy)-r2;
					if(delta<thresh)
					{
						pAMatrix[i*ASize+j]++;
					}
				}
			}
		}
	}

// 	for(i=0; i<ptNum; i++)
// 	{
// 		for(j=0; j<angleNum; j++)
// 		{
// 			dx=radius*cos(j*theta);
// 			dy=radius*sin(j*theta);
// 			
// 			icol=int((pData[i].x+dx-AMOpt.x)/unitL+0.5);
// 			irow=int((AMOpt.y-(pData[i].y+dy))/unitL+0.5);
// 
// 			if(icol<ASize && irow<ASize && icol>=0 && irow>=0)
// 			{
// 				pAMatrix[irow*ASize+icol]++;
// 			}
// 		}
// 	}

	unsigned int maxCount=0;
	int step=300;
	for(i=0; i<ASize; i++)
	{
		for(j=0; j<ASize; j++)
		{
			if(maxCount<pAMatrix[i*ASize+j])
			{
				maxCount=pAMatrix[i*ASize+j];
				irow=i; 
				icol=j;
			}
		}
	}
	
	center.x=AMOpt.x+icol*unitL;
	center.y=AMOpt.y-irow*unitL;


#ifdef _Export_Accumulator_
	//将累加矩阵导出到文本文件中	
	char *pName="G:\\D_experiment\\hough\\Accumulator.txt";
	FILE *fp=NULL;

	fp=fopen(pName, "wt");
	
	fprintf(fp, "##  x y count\n");
	fprintf(fp, "x/y ");
	for(j=0; j<ASize; j++)
	{
		dx=AMOpt.x+j*unitL;
		fprintf(fp, "%.3lf ", dx);
	}
	fprintf(fp, "\n");

	for(i=0; i<ASize; i++)
	{
		dy=AMOpt.y-i*unitL;
		fprintf(fp, "%.3lf ", dy);

		for(j=0; j<ASize; j++)
		{
			dx=AMOpt.x+j*unitL;
			
			fprintf(fp, "%d ", pAMatrix[i*ASize+j]);

			if(pAMatrix[i*ASize+j]>maxCount-step)
			{
				TRACE("%.3f  %.3f  %d\n", dx, dy, pAMatrix[i*ASize+j]);
			}
		}
		fprintf(fp, "\n");

	}
	
	fclose(fp);		fp=NULL;
#endif

//将累加器导出为影像中
//#ifdef	_Export_Accumulator_Image_
//#endif


	double sArea; //面积
	maxCount-=step;	//调整候选区大小
	for(i=0, sArea=0; i<ASize; i++)
	{//统计候选区面积
		for(j=0; j<ASize; j++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				sArea+=1.0;
		}
	}

	sArea/=2;
	
	double aS=0, lineS=0;
//	double l;
	for(i=0; i<ASize; i++)
	{//按行方向进行面积积分
		lineS=0;
		for(j=0; j<ASize; j++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				lineS++;
		}

		if(aS+lineS>sArea)
		{
			drow=(sArea-aS)/lineS+i;
			
			//center.y=AMOpt.y-l*unitL;
			break;
		}
		else
			aS+=lineS;
	}

	aS=0;
	for(j=0; j<ASize; j++)
	{//按列方向进行面积积分
		lineS=0;
		for(i=0; i<ASize; i++)
		{
			if(pAMatrix[i*ASize+j]>=maxCount)
				lineS++;
		}

		if(aS+lineS>sArea)
		{
			dcol=(sArea-aS)/lineS+j;
			
			//center.x=AMOpt.x+l*unitL;
			break;
		}
		else
			aS+=lineS;
	}

	center.x=mx+(dcol-ASize/2)*unitL;
	center.y=my+(ASize/2-drow)*unitL;

	if(pAMatrix)
	{
		delete pAMatrix;
		pAMatrix=NULL;
	}
}


