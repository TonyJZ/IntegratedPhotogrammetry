// AlignPrj.cpp: implementation of the CAlignPrj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "Alignment.h"
#include "AlignPrj.h"
#include "lidBase.h"
#include "orsBase\orsTypedef.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlignPrj::CAlignPrj()
{
//	m_bIsOpen=false;
	m_strPrjName.Empty();
}

CAlignPrj::~CAlignPrj()
{
	Close();
}

void CAlignPrj::Close()
{
	m_LidList.RemoveAll();
	m_ImgList.RemoveAll();

	m_strPrjName.Empty();
	
	ReleaseTieList();
//	m_bIsOpen=false;
}

void CAlignPrj::ReleaseTieList()
{
	int i;
	for(i=0; i<m_TieList.GetSize(); i++)
	{
		TieObjChain *pObjChain;
		pObjChain=m_TieList[i];
		delete pObjChain;
		pObjChain=NULL;
	}
	m_TieList.RemoveAll();
}

CArray<Align_LidLine, Align_LidLine>* CAlignPrj::GetLidList()
{
	return &m_LidList;
}

CArray<Align_Image, Align_Image>* CAlignPrj::GetImgList()
{
	return &m_ImgList;
}

CArray<TieObjChain*, TieObjChain*>* CAlignPrj::GetTieObjList()
{
	return &m_TieList;
}

void CAlignPrj::ImportLidLine(Align_LidLine lidline)
{
	m_LidList.Add(lidline);

//	m_bIsOpen=true;
}

bool CAlignPrj::Save()
{
	if(m_strPrjName.IsEmpty())
	{
		SaveAs();
	}

	FILE *fp=NULL;

	fp=fopen(m_strPrjName.GetBuffer(0), "wb");
	if(fp==NULL)
		return false;
	
//	int num=m_LidList.GetSize();
//	int len=0;
// 	fwrite(&num, sizeof(int), 1, fp);
// 
// 	for(int i=0; i<num; i++)
// 	{
// 		fwrite(&(m_LidList[i].LineID), sizeof(int), 1, fp);
// 		fwrite(m_LidList[i].GridName.GetBuffer(256), sizeof(char), 256, fp);
// 		fwrite(m_LidList[i].LasName.GetBuffer(256), sizeof(char), 256, fp);
// 	}

	WriteHeader(fp);
	WriteAlignLidLineInfo(fp);
	WriteAlignImageInfo(fp);
	WriteTieObjects(fp);

	if(fp)	fclose(fp);	fp=NULL;
	return true;
}

bool CAlignPrj::SaveAs()
{
	CFileDialog dlg(FALSE,"apj",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"Align工程文件(*.apj)|*.apj||",NULL);

	if(dlg.DoModal()!=IDOK)
		return false;
	
	m_strPrjName=dlg.GetPathName();

	Save();
	return  true;
}

/*
	int num;	//lidar条带数
	int ID;		//lidar line ID
	char imgName[256];
	char atnName[256];
*/
bool CAlignPrj::Open(char *pszPrjName)
{
//	int i;
	FILE *fp=NULL;
	Align_LidLine lidLine;

	fp=fopen(pszPrjName, "rb");
	if(fp==NULL)
		return false;

	CString strPrjName=pszPrjName;
	int pos=strPrjName.ReverseFind('\\');
	m_strDir=strPrjName.Left(pos);

	Close();
	m_strPrjName=pszPrjName;

	ReadHeader(fp);

	ReadAlignLidLineInfo(fp);
	ReadAlignImageInfo(fp);

	ReadTieObjects(fp);

//	m_bIsOpen=true;
	if(fp)	fclose(fp);	fp=NULL;
	return true;
}

bool CAlignPrj::ReadHeader(FILE *fp)
{
	fseek(fp, 0, SEEK_SET);
	fread(&m_header, sizeof(m_header), 1, fp);

	return true;
}

bool CAlignPrj::ReadAlignLidLineInfo(FILE *fp)
{
	int i;
	Align_LidLine lidLine;

	fseek(fp, m_header.offsetToLidLine, SEEK_SET);
	m_LidList.RemoveAll();

	char pName[256];
	for(i=0; i<m_header.lineNum; i++)
	{
		fread(&(lidLine.LineID), sizeof(int), 1, fp);
//		lidLine.LineID++;

		fread(pName, sizeof(char), 256, fp);
		lidLine.GridName=pName;
		fread(pName, sizeof(char), 256, fp);
		lidLine.LasName=pName;
		fread(pName, sizeof(char), 256, fp);
		lidLine.TrjName=pName;

		fread(&(lidLine.averFH), sizeof(double), 1, fp);

// 		lidLine.LineID=1;
// 		lidLine.GridName="I:/Data/anyang2010/1/calibrate/ATN/TP/LDR101108_011429_ATN_TP_i0.5.img";
// 		lidLine.LasName="I:/Data/anyang2010/1/calibrate/ATN/TP/LDR101108_011429_ATN_TP.LAS";
		
		if(lidLine.GridName.IsEmpty()&&lidLine.LasName.IsEmpty())
		{//当lidar条带为空时，只对影像处理
			;
		}
		else if((!_IsFileExist(lidLine.LasName)) || (!_IsFileExist(lidLine.GridName)))
		{//到当前工程文件夹下找
			CString strImgName, strAtnName;
			
			int	pos, length;

			length=lidLine.LasName.GetLength();
			pos=lidLine.LasName.ReverseFind('\\');
			strAtnName=lidLine.LasName.Right(length-pos-1);
			
			length=lidLine.GridName.GetLength();
			pos=lidLine.GridName.ReverseFind('\\');
			strImgName=lidLine.GridName.Right(length-pos-1);

			strAtnName=m_strDir+"\\"+strAtnName;
			strImgName=m_strDir+"\\"+strImgName;

			if((!_IsFileExist(strAtnName)) || (!_IsFileExist(strImgName)))
			{//替换盘符
				int pos1 = m_strDir.Find('\\');
				CString strDisk = m_strDir.Left(pos1);

				length=lidLine.LasName.GetLength();
				pos = lidLine.LasName.Find('\\');
				strAtnName = lidLine.LasName.Right(length-pos-1);
				strAtnName=strDisk+"\\"+strAtnName;

				length=lidLine.GridName.GetLength();
				pos = lidLine.GridName.Find('\\');
				strImgName = lidLine.GridName.Right(length-pos-1);
				strImgName = strDisk+"\\"+strImgName;
			}
			
			lidLine.LasName = strAtnName;
			lidLine.GridName = strImgName;
		}

		m_LidList.Add(lidLine);
	}

	return true;
}

bool CAlignPrj::ReadAlignImageInfo(FILE *fp)
{
	int i;
	Align_Image img;
	
	fseek(fp, m_header.offsetToImage, SEEK_SET);
	m_ImgList.RemoveAll();
	
	char pName[256];
	for(i=0; i<m_header.imgNum; i++)
	{
		fread(&(img.nImgID), sizeof(int), 1, fp);
		fread(pName, sizeof(char), 256, fp);
		img.ImgName=pName;

		if(!_IsFileExist(img.ImgName))
		{//替换盘符
			CString strImgName;
			int	pos, length;

			int pos1 = m_strDir.Find('\\');
			CString strDisk = m_strDir.Left(pos1);

			length=img.ImgName.GetLength();
			pos = img.ImgName.Find('\\');
			strImgName = img.ImgName.Right(length-pos-1);
			strImgName = strDisk+"\\"+strImgName;

			img.ImgName = strImgName;
		}

//		fread(&(img.nStripID), sizeof(int), 1, fp);
//		fread(&(img.nPhoID), sizeof(int), 1, fp);
		fread(&(img.nIsReversal), sizeof(int), 1, fp);
			
		m_ImgList.Add(img);
	}
	
	return true;
}

bool CAlignPrj::ReadTieObjects(FILE *fp)
{
	int i, j;
	TieObjChain *pChain=NULL;
	TieObject *pObj=NULL;
	POINT2D *pt2d=NULL, point2d;
	POINT3D *pt3d=NULL, point3d;
	int ptNum;
	
	fseek(fp, m_header.offsetToTieObj, SEEK_SET);
	ReleaseTieList();

	for(i=0; i<m_header.tieObjNum; i++)
	{
		pChain=new TieObjChain;

//		int tieID;
//		fread(&tieID, sizeof(int), 1, fp);
		fread(&(pChain->TieID), sizeof(int), 1, fp);
		fread(&(pChain->type), sizeof(TieObj_Type), 1, fp);
		fread(&(pChain->objNum), sizeof(int), 1, fp);
		pChain->bDel=false;
		
		pChain->pChain=new CArray<TieObject*, TieObject*>;
//		TRACE("%d\n", pChain->type);
		if(pChain->type==TO_POINT)
		{
//			TRACE("%d\n", pChain->objNum);
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=new TiePoint;
				pObj->objType=TO_POINT;
				pObj->tieID=pChain->TieID;
				fread(&(pObj->sourceID), sizeof(int), 1, fp);
				fread(&(pObj->sourceType), sizeof(Source_Type), 1, fp);
				fread(&(((TiePoint*)pObj)->tpType), sizeof(char), 1, fp);
				fread(&point2d, sizeof(point2d), 1, fp);
				((TiePoint*)pObj)->pt2D=point2d;
				fread(&point3d, sizeof(point3d), 1, fp);
				((TiePoint*)pObj)->pt3D=point3d;

				pChain->pChain->Add(pObj);
			}
		}
		else if(pChain->type==TO_LINE)
		{
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=new TieLine;
				pObj->objType=TO_LINE;
				pObj->tieID=pChain->TieID;
				fread(&(pObj->sourceID), sizeof(int), 1, fp);
				fread(&(pObj->sourceType), sizeof(Source_Type), 1, fp);

				fread(&ptNum, sizeof(int), 1, fp);
				pt2d=new POINT2D[ptNum];
				fread(pt2d, sizeof(POINT2D), ptNum, fp);
				pt3d=new POINT3D[ptNum];
				fread(pt3d, sizeof(POINT3D), ptNum, fp);
				((TieLine*)pObj)->ptNum=ptNum;
				((TieLine*)pObj)->pt2D=pt2d;
				((TieLine*)pObj)->pt3D=pt3d;

				pChain->pChain->Add(pObj);
			}
		}
		else if(pChain->type==TO_PATCH)
		{
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=new TiePatch;
				pObj->objType=TO_PATCH;
				pObj->tieID=pChain->TieID;
				fread(&(pObj->sourceID), sizeof(int), 1, fp);
				fread(&(pObj->sourceType), sizeof(Source_Type), 1, fp);
				
				fread(&ptNum, sizeof(int), 1, fp);
				pt2d=new POINT2D[ptNum];
				fread(pt2d, sizeof(POINT2D), ptNum, fp);
				pt3d=new POINT3D[ptNum];
				fread(pt3d, sizeof(POINT3D), ptNum, fp);
				((TiePatch*)pObj)->ptNum=ptNum;
				((TiePatch*)pObj)->pt2D=pt2d;
				((TiePatch*)pObj)->pt3D=pt3d;
				
				pChain->pChain->Add(pObj);
			}
		}
		
		fread(&(pChain->bGCP), sizeof(bool), 1, fp);
		if(pChain->bGCP)
		{
			fread(&(pChain->gcp), sizeof(POINT3D), 1, fp);
		}

		m_TieList.Add(pChain);
	}

	return true;
}

bool CAlignPrj::WriteHeader(FILE *fp)
{
	m_header.file_signature[0]='A';
	m_header.file_signature[1]='L';
	m_header.file_signature[2]='N';
	m_header.file_signature[3]='F';

	m_header.lineNum=m_LidList.GetSize();
	m_header.imgNum=m_ImgList.GetSize();

	int totalNum=m_TieList.GetSize();
	m_header.tieObjNum=0;
	for(int i=0; i<totalNum; i++)
	{//只保存未被删除的对象链
		if(m_TieList[i]->bDel)
			continue;

		m_header.tieObjNum++;
	}
//	m_header.tieObjNum=m_TieList.GetSize();

	m_header.offsetToLidLine=sizeof(m_header);
	m_header.offsetToImage=m_header.offsetToLidLine+(sizeof(int)+sizeof(char)*256*3+sizeof(double))*m_header.lineNum;
	m_header.offsetToTieObj=m_header.offsetToImage+(sizeof(int)*4+sizeof(char)*256)*m_header.imgNum;

	m_header.reserved=0;

	fseek(fp, 0, SEEK_SET);

	fwrite(&m_header, sizeof(m_header), 1, fp);
	return true;
}

bool CAlignPrj::WriteAlignLidLineInfo(FILE *fp)
{
	int i;
	Align_LidLine lidLine;
	
	fseek(fp, m_header.offsetToLidLine, SEEK_SET);
		
	char pName[256];
	for(i=0; i<m_header.lineNum; i++)
	{
		lidLine=m_LidList.GetAt(i);

		fwrite(&(lidLine.LineID), sizeof(int), 1, fp);
		
		memset(pName, 0, sizeof(char)*256);
		strncpy(pName, lidLine.GridName.GetBuffer(0), 256);
		fwrite(pName, sizeof(char), 256, fp);

		memset(pName, 0, sizeof(char)*256);
		strncpy(pName, lidLine.LasName.GetBuffer(0), 256);
		fwrite(pName, sizeof(char), 256, fp);

		memset(pName, 0, sizeof(char)*256);
		strncpy(pName, lidLine.TrjName.GetBuffer(0), 256);
		fwrite(pName, sizeof(char), 256, fp);

		fwrite(&(lidLine.averFH), sizeof(double), 1, fp);
	}
	
	return true;
}

bool CAlignPrj::WriteAlignImageInfo(FILE *fp)
{
	int i;
	Align_Image img;
	
	fseek(fp, m_header.offsetToImage, SEEK_SET);
		
	char pName[256];
	for(i=0; i<m_header.imgNum; i++)
	{
		img=m_ImgList.GetAt(i);

		fwrite(&(img.nImgID), sizeof(int), 1, fp);
		
		memset(pName, 0, sizeof(char)*256);

//		pName = img.ImgName;
		strncpy(pName, (LPCTSTR)img.ImgName, 256);
		fwrite(pName, sizeof(char), 256, fp);

//		fwrite(&(img.nStripID), sizeof(int), 1, fp);
//		fwrite(&(img.nPhoID), sizeof(int), 1, fp);
		fwrite(&(img.nIsReversal), sizeof(int), 1, fp);
	}
	
	return true;
}

bool CAlignPrj::WriteTieObjects(FILE *fp)
{
	int i, j;
	TieObjChain *pChain=NULL;
	TieObject *pObj=NULL;
	POINT2D *pt2d=NULL, point2d;
	POINT3D *pt3d=NULL, point3d;
	int ptNum, tieID;
	
	fseek(fp, m_header.offsetToTieObj, SEEK_SET);

	tieID=1;
	for(i=0; i<m_TieList.GetSize(); i++)
	{
		pChain=m_TieList.GetAt(i);
		if(pChain->bDel)  //跳过删除节点
			continue;

		pChain->objNum=pChain->pChain->GetSize();
		pChain->TieID=tieID;
		fwrite(&(pChain->TieID), sizeof(int), 1, fp);
		fwrite(&(pChain->type), sizeof(TieObj_Type), 1, fp);
		fwrite(&(pChain->objNum), sizeof(int), 1, fp);

		tieID++;
		
		if(pChain->type==TO_POINT)
		{
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);
								
				fwrite(&(pObj->sourceID), sizeof(int), 1, fp);
				fwrite(&(pObj->sourceType), sizeof(Source_Type), 1, fp);
				fwrite(&(((TiePoint*)pObj)->tpType), sizeof(char), 1, fp);
				point2d=((TiePoint*)pObj)->pt2D;
				fwrite(&point2d, sizeof(POINT2D), 1, fp);
				point3d=((TiePoint*)pObj)->pt3D;
				fwrite(&point3d, sizeof(POINT3D), 1, fp);
			}
		}
		else if(pChain->type==TO_LINE)
		{
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);

				fwrite(&(pObj->sourceID), sizeof(int), 1, fp);
				fwrite(&(pObj->sourceType), sizeof(Source_Type), 1, fp);
				
				ptNum=((TieLine*)pObj)->ptNum;
				pt2d=((TieLine*)pObj)->pt2D;
				pt3d=((TieLine*)pObj)->pt3D;
				fwrite(&ptNum, sizeof(int), 1, fp);
				fwrite(pt2d, sizeof(POINT2D), ptNum, fp);
				fwrite(pt3d, sizeof(POINT3D), ptNum, fp);
			}
		}
		else if(pChain->type==TO_PATCH)
		{
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);
				
				fwrite(&(pObj->sourceID), sizeof(int), 1, fp);
				fwrite(&(pObj->sourceType), sizeof(Source_Type), 1, fp);
				
				ptNum=((TiePatch*)pObj)->ptNum;
				pt2d=((TiePatch*)pObj)->pt2D;
				pt3d=((TiePatch*)pObj)->pt3D;
				fwrite(&ptNum, sizeof(int), 1, fp);
				fwrite(pt2d, sizeof(POINT2D), ptNum, fp);
				fwrite(pt3d, sizeof(POINT3D), ptNum, fp);
			}
		}

		fwrite(&(pChain->bGCP), sizeof(bool), 1, fp);
		if(pChain->bGCP)
		{
			fwrite(&(pChain->gcp), sizeof(POINT3D), 1, fp);
		}
	}
	
	return true;
}

CString CAlignPrj::GetLiDARName(int lidID)
{
//	ASSERT(lidID>0);
	return m_LidList.GetAt(lidID-1).GridName;
}

CString CAlignPrj::GetImgName(int imgID)
{
	for(int i=0; i<m_ImgList.GetSize(); i++)
	{
		//int id = (m_ImgList.GetAt(i).nImgID - sFlag) 
		if(m_ImgList.GetAt(i).nImgID == imgID)
			return m_ImgList.GetAt(i).ImgName;
	}
	return "";
//	return m_ImgList.GetAt(imgID).ImgName;
}

bool CAlignPrj::loadImageTimeStamp(const char* pszFileName)
{
	FILE *fp=NULL;
	char imgName[256];
	int MAX_STRING=256;
	
	fp = fopen(pszFileName, "rt");
	if(fp==NULL)
		return false;

	double t;
	while(!feof(fp)) 
	{
		fscanf( fp, "%s %lf\n", imgName, &t);

		for(int i=0; i<m_ImgList.GetSize(); i++)
		{
			CString pureName = _ExtractFileFullName(m_ImgList[i].ImgName);

			if(pureName.Compare(imgName)==0)
			{
				m_ImgList[i].timestamp = t;
				break;
			}
		}
	}

	return true;
}

