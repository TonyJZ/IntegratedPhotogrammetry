#include "StdAfx.h"
#include "TrjReader.h"


CTrjReader::CTrjReader()
{
//	m_fp=0;
	m_PosBuf=0;
}

CTrjReader::~CTrjReader()
{
	close();
}

void CTrjReader::close()
{
	if(m_PosBuf)
	{
		delete[] m_PosBuf;
		m_PosBuf=0;
	}

	memset(&m_TrjHeader, 0, sizeof(m_TrjHeader));
}

bool CTrjReader::read_header(char *pszName, TrajHdr *pHeader)
{
	FILE  *fp=NULL;

	fp=fopen(pszName, "rb");
	if(fp==0)
		return false;
	
	fread(pHeader->Recog, sizeof(char), 8, fp);
	fread(&(pHeader->Version), sizeof(int), 1, fp);
	fread(&(pHeader->HdrSize), sizeof(int), 1, fp);
	fread(&(pHeader->PosCnt), sizeof(int), 1, fp);
	fread(&(pHeader->PosSize), sizeof(int), 1, fp);
	fread(pHeader->Desc, sizeof(char), 79, fp);
	fread(&(pHeader->Quality), sizeof(BYTE), 1, fp);
	fread(&(pHeader->BegTime), sizeof(double), 1, fp);
	fread(&(pHeader->EndTime), sizeof(double), 1, fp);
	fread(&(pHeader->OrigNbr), sizeof(int), 1, fp);
	fread(&(pHeader->Number), sizeof(int), 1, fp);
	fread(pHeader->VrtVideo, sizeof(char), 400, fp);
	fread(&(pHeader->VrtBeg), sizeof(double), 1, fp);
	fread(&(pHeader->VrtEnd), sizeof(double), 1, fp);
	fread(pHeader->FwdVideo, sizeof(char), 400, fp);
	fread(&(pHeader->FwdBeg), sizeof(double), 1, fp);
	fread(&(pHeader->FwdEnd), sizeof(double), 1, fp);

	fclose(fp);

	return true;
}

bool CTrjReader::readTrjPos(char *pszName)
{
	FILE  *fp=NULL;
	bool bReturn=true;
	int numread=0;
	
	close();

	fp=fopen(pszName, "rb");
	if(fp==0)
	{
		bReturn=false;
		goto FUNC_END;
	}

	fread(m_TrjHeader.Recog, sizeof(char), 8, fp);
	fread(&(m_TrjHeader.Version), sizeof(int), 1, fp);
	fread(&(m_TrjHeader.HdrSize), sizeof(int), 1, fp);
	fread(&(m_TrjHeader.PosCnt), sizeof(int), 1, fp);
	fread(&(m_TrjHeader.PosSize), sizeof(int), 1, fp);
	fread(m_TrjHeader.Desc, sizeof(char), 79, fp);
	fread(&(m_TrjHeader.Quality), sizeof(BYTE), 1, fp);
	fread(&(m_TrjHeader.BegTime), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.EndTime), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.OrigNbr), sizeof(int), 1, fp);
	fread(&(m_TrjHeader.Number), sizeof(int), 1, fp);
	fread(m_TrjHeader.VrtVideo, sizeof(char), 400, fp);
	fread(&(m_TrjHeader.VrtBeg), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.VrtEnd), sizeof(double), 1, fp);
	fread(m_TrjHeader.FwdVideo, sizeof(char), 400, fp);
	fread(&(m_TrjHeader.FwdBeg), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.FwdEnd), sizeof(double), 1, fp);

	if(m_TrjHeader.PosCnt<=0)
	{
		bReturn=false;
		goto FUNC_END;
	}
	

	if((m_PosBuf=new TrajPos[m_TrjHeader.PosCnt])==NULL)
	{
		bReturn=false;
		goto FUNC_END;
	}

// 	for(int i=0; i<m_TrjHeader.PosCnt; i++)
// 	{
// 		numread=fread(&(m_PosBuf[i]), sizeof(TrajPos), 1, m_fp);
// 		if(numread!=1)
// 		{
// 			int err=feof(m_fp);
// 			return false;
// 		}
// 	}

	numread=fread(m_PosBuf, sizeof(TrajPos), m_TrjHeader.PosCnt, fp);

	if(numread!=m_TrjHeader.PosCnt)
	{
		int err=feof(fp);
		bReturn=false;
		goto FUNC_END;

	}
	
FUNC_END:
	if(fp)	fclose(fp); fp=NULL;
	return bReturn;
}

int ascending_time_stamp(const void *p1, const void  *p2)
{
	TrajPos *elem1, *elem2;
	elem1=(TrajPos *)p1;
	elem2=(TrajPos *)p2;
	
	if(elem1->Time < elem2->Time)
		return -1;
	else 
		if(elem1->Time > elem2->Time)
			return 1;
		else
			return 0;
}

void CTrjReader::qsort_time_stamp_ascending()
{
	qsort(m_PosBuf, m_TrjHeader.PosCnt, sizeof(TrajPos), ascending_time_stamp);

}

void CTrjReader::interpolation_linear(double t, double &Xt, double &Yt, double &Zt)
{
	int sIdx, eIdx;	//时间分段索引

	findTimeSlice(t, sIdx, eIdx);

	if(eIdx==0)
	{
		Xt=m_PosBuf[eIdx].x;
		Yt=m_PosBuf[eIdx].y;
		Zt=m_PosBuf[eIdx].z;
		return;
	}

	if(sIdx==m_TrjHeader.PosCnt-1)
	{
		Xt=m_PosBuf[sIdx].x;
		Yt=m_PosBuf[sIdx].y;
		Zt=m_PosBuf[sIdx].z;
		return;
	}

	double t1=m_PosBuf[sIdx].Time;
	double t2=m_PosBuf[eIdx].Time;
	double delta_t=t2-t1;
	double Vx, Vy, Vz;

	Vx=(m_PosBuf[eIdx].x-m_PosBuf[sIdx].x)/delta_t;
	Vy=(m_PosBuf[eIdx].y-m_PosBuf[sIdx].y)/delta_t;
	Vz=(m_PosBuf[eIdx].z-m_PosBuf[sIdx].z)/delta_t;

	Xt=m_PosBuf[sIdx].x+Vx*(t-t1);
	Yt=m_PosBuf[sIdx].y+Vy*(t-t1);
	Zt=m_PosBuf[sIdx].z+Vz*(t-t1);

}

void CTrjReader::findTimeSlice(double t, int &sIdx, int &eIdx)
{
	int lLow=0, lHigh=m_TrjHeader.PosCnt-1;
	int lMid=0;

	
	if(t<m_PosBuf[lLow].Time)
	{
		sIdx=-1;
		eIdx=lLow;
		return;
	}
	if(t>m_PosBuf[lHigh].Time)
	{
		sIdx=lHigh;
		eIdx=lHigh+1;
		return;
	}
	
	//折半查找
	while(lLow<lHigh)	
	{
		if(lLow+1==lHigh)
		{
			sIdx=lLow;
			eIdx=lHigh;
			break;
		}

		lMid=(lLow+lHigh)/2;
		
		if(m_PosBuf[lMid].Time<=t)
		{
			lLow=lMid;
			
		}
		else
		{
			lHigh=lMid;
		}

	}

}


//////////////////////////////////////////////////////////////////////////
//CTrjWriter class
CTrjWriter::CTrjWriter()
{
	m_fp=NULL;
}

CTrjWriter::~CTrjWriter()
{
	if(m_fp)
		fclose(m_fp);

	m_fp=NULL;
}

bool CTrjWriter::open(char *pszName)
{
	close();

	m_fp=fopen(pszName, "wb");
	if(m_fp==NULL)
		return false;

	return true;
}

void CTrjWriter::close()
{
	if(m_fp)
		fclose(m_fp);
	m_fp=NULL;
}

void CTrjWriter::write_header(TrajHdr *pHeader)
{
	fwrite(pHeader->Recog, sizeof(char), 8, m_fp);
	fwrite(&(pHeader->Version), sizeof(int), 1, m_fp);
	fwrite(&(pHeader->HdrSize), sizeof(int), 1, m_fp);
	fwrite(&(pHeader->PosCnt), sizeof(int), 1, m_fp);
	fwrite(&(pHeader->PosSize), sizeof(int), 1, m_fp);
	fwrite(pHeader->Desc, sizeof(char), 79, m_fp);
	fwrite(&(pHeader->Quality), sizeof(BYTE), 1, m_fp);
	fwrite(&(pHeader->BegTime), sizeof(double), 1, m_fp);
	fwrite(&(pHeader->EndTime), sizeof(double), 1, m_fp);
	fwrite(&(pHeader->OrigNbr), sizeof(int), 1, m_fp);
	fwrite(&(pHeader->Number), sizeof(int), 1, m_fp);
	fwrite(pHeader->VrtVideo, sizeof(char), 400, m_fp);
	fwrite(&(pHeader->VrtBeg), sizeof(double), 1, m_fp);
	fwrite(&(pHeader->VrtEnd), sizeof(double), 1, m_fp);
	fwrite(pHeader->FwdVideo, sizeof(char), 400, m_fp);
	fwrite(&(pHeader->FwdBeg), sizeof(double), 1, m_fp);
	fwrite(&(pHeader->FwdEnd), sizeof(double), 1, m_fp);
}

bool CTrjWriter::writeTrjPos(TrajPos *pPosBuf, int num)
{
	int numwrite=0;
	
	numwrite=fwrite(pPosBuf, sizeof(TrajPos), num, m_fp);
	
	if(numwrite!=num)
	{
		int err=feof(m_fp);
		return false;
	}

	return true;
}