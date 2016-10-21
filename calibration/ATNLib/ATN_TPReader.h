//转到切平面的ATN数据读写
#ifndef _ATN_TP_Reader_H_zj_2010_09_27_
#define _ATN_TP_Reader_H_zj_2010_09_27_

#include "ATNReader.h"

#ifdef ATNLIB_EXPORTS
#define  _ATN_Lib_  __declspec(dllexport)
#else
#define  _ATN_Lib_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif



class _ATN_Lib_ CATN_TPReader : public CATNReader
{
public:
	CATN_TPReader();
	~CATN_TPReader();
	
//	bool  open(char *pszName);
	void  reopen();	//重新从第一个点记录开始读取
//	void  close();
	
//	virtual bool  read_point(ATNPoint *point);
	virtual inline bool  read_point(ATNTP_Point *point);

	inline void  Seek(__int64  offset);
	
	
// public:
// 	LASheader		m_header;
// 	
// 	int m_npoints;
// 	int m_pcount;
	
private:
	ATN_TP_RawPoint	*m_rawpoint;

// 	CMappingFile m_mappingFile;
// 	int		additional_bytes_per_point;
};


class _ATN_Lib_ CATN_TPWriter : public CATNWriter
{
public:
	CATN_TPWriter();
	~CATN_TPWriter();
	
// 	bool  create(char *pszName);
// 	bool  open(char *pszName);
// 	void  close();
	
	virtual inline bool  write_point(ATNTP_Point *point);
	
//	virtual bool  write_block(LidBlock *pBlock);
//	bool  write_header(LASheader *pHeader);
	
// public:
// 	LASheader		m_header;
// 	
// 	
// 	int m_npoints;
// 	int m_pcount;
	
private:
	ATN_TP_RawPoint	m_rawpoint;

// 	bool	m_bCreate;
// 	CMappingFile	m_mappingFile;
// 	int		additional_bytes_per_point;
};




#endif