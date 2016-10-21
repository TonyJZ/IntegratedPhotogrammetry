#include "StdAfx.h"
#include "CalibUtility/cp_IO.h"

#include "orsLidarGeometry/LidGeo_def.h"


bool write_CPfile(const char *pFileName, LidCalib_ModelType type, void *CalibParam, int num)
{
	FILE *fp=NULL;

	fp=fopen(pFileName, "wt");
	if(fp==NULL)
		return false;

	if(type == LCMT_Rigorous)
	{

	}
	else if(type == LCMT_Yuan)
	{
		orsLidYuanParam *pParam = (orsLidYuanParam*)CalibParam;

		fprintf(fp, "%s\n", CPFLAG_YUAN);
		
		//Yuan model只有一组定标参数
		fprintf(fp, "%d\n", 1);
		fprintf(fp, "%e %e %e %e %e\n", pParam->ax, pParam->ay, pParam->az, pParam->kappa, pParam->omega);
	}
	else if(type == LCMT_Jing)
	{
		orsLidJingParam *pParam = (orsLidJingParam*)CalibParam;

		fprintf(fp, "%s\n", CPFLAG_JING);

		//Yuan model只有一组定标参数
		fprintf(fp, "%d\n", 1);
		fprintf(fp, "%e %e %e\n", pParam->phi, pParam->omega, pParam->kappa);

	}
	else if(type == LCMT_Ressl)
	{

	}
	else if(type == LCMT_TrajAided)
	{

	}

	fclose(fp);
	fp = NULL;

	return true;
}


bool read_CPfile(const char *pFileName, LidCalib_ModelType &type, void **CalibParam, int &num)
{
	FILE *fp=NULL;
	char fileFlag[64];

	fp=fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	fscanf(fp, "%s", fileFlag);
	if(strcmp(fileFlag,CPFLAG_RIGOROUS )==0)
	{
		type = LCMT_Rigorous;
	}
	else if(strcmp(fileFlag, CPFLAG_YUAN)==0)
	{
		type = LCMT_Yuan;
		fscanf(fp, "%d\n", &num);
		
		if(num != 1)
		{
			return false;
		}

		orsLidYuanParam *pParam = new orsLidYuanParam;
		*CalibParam = (void*)pParam;

		fscanf(fp, "%lf %lf %lf %lf %lf\n", &(pParam->ax), &(pParam->ay), &(pParam->az), &(pParam->kappa), &(pParam->omega));
	}
	else if(strcmp(fileFlag, CPFLAG_JING)==0)
	{
		type = LCMT_Jing;
		fscanf(fp, "%d\n", &num);

		if(num != 1)
		{
			return false;
		}

		orsLidJingParam *pParam = new orsLidJingParam;
		*CalibParam = (void*)pParam;

		fscanf(fp, "%lf %lf %lf\n", &(pParam->phi), &(pParam->omega), &(pParam->kappa));

	}
	else if(strcmp(fileFlag, CPFLAG_RESSL)==0)
	{
		type = LCMT_Ressl;
	}
	else if(strcmp(fileFlag, CPFLAG_TRAJAIDED)==0)
	{
		type = LCMT_TrajAided;
	}
	else
	{
		fclose(fp);	fp=NULL;
		return false;
	}
	
		
	
	fclose(fp);
	fp = NULL;

	return true;
}