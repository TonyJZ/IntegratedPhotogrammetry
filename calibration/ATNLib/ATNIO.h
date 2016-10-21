//Leica Attune使用的标定点云格式_ATN.LAS
#ifndef _ATN_LAS_File_Reader_H_zj_2010_07_14_
#define _ATN_LAS_File_Reader_H_zj_2010_07_14_

#include "stdio.h"
#include "\OpenRS\desktop\src\orsPointCloud\lastools\lasdefinitions.h"
//#include "CalibBasedef.h"
#include "ATN_def.h"

//#include "MappingFile.h"



#ifdef ATNLIB_EXPORTS
#define  _ATN_Lib_  __declspec(dllexport)
#else
#define  _ATN_Lib_  __declspec(dllimport)	
#endif


class _ATN_Lib_ CATNReader
{
public:
	CATNReader();
	~CATNReader();

	bool  open(const char *pszName);
	void  reopen();	//重新从第一个点记录开始读取
	void  close();

	virtual inline bool  read_point(ATNPoint *point);
//	virtual bool  read_point(ATNPoint_TP *point);

	bool  read_header(LASheader *pHeader);

	inline void  Seek(__int64  offset);

public:
	int m_npoints;
	int m_pcount;

private:
	LASheader		m_header;

	ATN_RawPoint	m_rawpoint;

protected:
//	CMappingFile m_mappingFile;
	FILE		*m_fp;
	int		additional_bytes_per_point;
};


class _ATN_Lib_ CATNWriter
{
public:
	CATNWriter();
	~CATNWriter();
	
	bool  create(const char *pszName);
	bool  open(const char *pszName);
	void  close();
	
	virtual inline bool write_point(ATNPoint *point);
	bool  write_header(LASheader *pHeader);
	
public:
	LASheader		m_header;
	
	
	int m_npoints;
	int m_pcount;
	
private:
	ATN_RawPoint	m_rawpoint;

protected:
	bool	m_bCreate;
//	CMappingFile	m_mappingFile;
	FILE		*m_fp;
	int		additional_bytes_per_point;
};

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif

#endif