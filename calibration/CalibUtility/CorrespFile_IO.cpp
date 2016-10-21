#include "StdAfx.h"
#include "CorrespFile_IO.h"

#include "stdio.h"

bool write_CorrespFile(const char *pszFile, const int nObj, LidMC_Obj *pCorObj)
{
	FILE *fp=NULL;
	int objType = pCorObj->getType();
	LidMC_VP *pVP = (LidMC_VP*)pCorObj;
	LidMC_Plane *pPlane = (LidMC_Plane*)pCorObj;

	fp=fopen(pszFile, "wb");
	if(fp==NULL)
		return false;

	fwrite(CorrespFileFlag, sizeof(char), 32, fp);
	fwrite(&nObj, sizeof(int), 1, fp);
	fwrite(&objType, sizeof(int), 1, fp);

	for(int i=0; i<nObj; i++)
	{
		int nLidPt;
		if(objType == OBJTYPE_VP)
		{
//			LidMC_VP *pVP = (LidMC_VP*)(&pCorObj[i]);

			fwrite(&(pVP[i].tieID), sizeof(int), 1, fp);
			fwrite(&(pVP[i].vx), sizeof(double), 1, fp);
			fwrite(&(pVP[i].vy), sizeof(double), 1, fp);
			fwrite(&(pVP[i].vz), sizeof(double), 1, fp);

			fwrite(&(pVP[i].eX), sizeof(double), 1, fp);
			fwrite(&(pVP[i].eY), sizeof(double), 1, fp);
			fwrite(&(pVP[i].eZ), sizeof(double), 1, fp);

			fwrite(&(pVP[i].VP_type), sizeof(char), 1, fp);
			fwrite(&(pVP[i].reliability), sizeof(char), 1, fp);

			nLidPt = pVP[i].pLidPoints->size();
			fwrite(&nLidPt, sizeof(int), 1, fp);
			fwrite(&(*(pVP[i].pLidPoints))[0], sizeof(LidPt_SurvInfo), nLidPt, fp);
		}
		else if(objType == OBJTYPE_Plane)
		{
//			LidMC_Plane *pPlane = (LidMC_Plane*)(&pCorObj[i]);

			fwrite(&(pPlane[i].tieID), sizeof(int), 1, fp);
			fwrite(&(pPlane[i].s1), sizeof(double), 1, fp);
			fwrite(&(pPlane[i].s2), sizeof(double), 1, fp);
			fwrite(&(pPlane[i].s3), sizeof(double), 1, fp);
			fwrite(&(pPlane[i].s4), sizeof(double), 1, fp);

			nLidPt = pPlane[i].pLidPoints->size();
			fwrite(&nLidPt, sizeof(int), 1, fp);
			fwrite(&(*(pPlane[i].pLidPoints))[0], sizeof(LidPt_SurvInfo), nLidPt, fp);
		}
	}

	fclose(fp);
	fp=NULL;
	return true;
}

bool read_CorrespFile(const char *pszFile, int *objType, int *nObj, LidMC_Obj **pCorObj)
{
	FILE *fp=NULL;
	char pFileFlag[32];
//	int objType;

	fp=fopen(pszFile, "rb");
	if(fp==NULL)
		return false;

	fread(pFileFlag, sizeof(char), 32, fp);
	fread(nObj, sizeof(int), 1, fp);
	fread(objType, sizeof(int), 1, fp);

	LidMC_VP *pVP=NULL;
	LidMC_Plane *pPlane=NULL;

	if(*objType == OBJTYPE_VP)
	{
		pVP = new LidMC_VP[*nObj];
		*pCorObj = (LidMC_Obj*)pVP;
	}
	else if(*objType == OBJTYPE_Plane)
	{
		pPlane = new LidMC_Plane[*nObj];
		*pCorObj = (LidMC_Obj*)pPlane;
	}

	int i, j;
	LidPt_SurvInfo LidPt;
	for(i=0; i<*nObj; i++)
	{
		int nLidPt;
		if(*objType == OBJTYPE_VP)
		{
			//LidMC_VP *pVP = (LidMC_VP*)(&(*pCorObj)[i]);

			fread(&(pVP[i].tieID), sizeof(int), 1, fp);
			fread(&(pVP[i].vx), sizeof(double), 1, fp);
			fread(&(pVP[i].vy), sizeof(double), 1, fp);
			fread(&(pVP[i].vz), sizeof(double), 1, fp);

			fread(&(pVP[i].eX), sizeof(double), 1, fp);
			fread(&(pVP[i].eY), sizeof(double), 1, fp);
			fread(&(pVP[i].eZ), sizeof(double), 1, fp);

			fread(&(pVP[i].VP_type), sizeof(char), 1, fp);
			fread(&(pVP[i].reliability), sizeof(char), 1, fp);

			fread(&nLidPt, sizeof(int), 1, fp);
			pVP[i].pLidPoints = new std::vector<LidPt_SurvInfo>;
			for(j=0; j<nLidPt; j++)
			{
				fread(&LidPt, sizeof(LidPt_SurvInfo), 1, fp);
				pVP[i].pLidPoints->push_back(LidPt);
			}
		}
		else if(*objType == OBJTYPE_Plane)
		{
			//LidMC_Plane *pPlane = (LidMC_Plane*)(&(*pCorObj)[i]);

			fread(&(pPlane[i].tieID), sizeof(int), 1, fp);
			fread(&(pPlane[i].s1), sizeof(double), 1, fp);
			fread(&(pPlane[i].s2), sizeof(double), 1, fp);
			fread(&(pPlane[i].s3), sizeof(double), 1, fp);
			fread(&(pPlane[i].s4), sizeof(double), 1, fp);

			fread(&nLidPt, sizeof(int), 1, fp);
			pPlane[i].pLidPoints = new std::vector<LidPt_SurvInfo>;
			for(j=0; j<nLidPt; j++)
			{
				fread(&LidPt, sizeof(LidPt_SurvInfo), 1, fp);
				pPlane[i].pLidPoints->push_back(LidPt);
			}
		}
	}

	fclose(fp);
	fp=NULL;
	return true;
}