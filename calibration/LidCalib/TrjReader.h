//TerraScan中使用的航迹线文件格式.trj
#ifndef  _Trj_File_Reader_H_zj_2010_07_13_ 
#define	 _Trj_File_Reader_H_zj_2010_07_13_


#include "trj_def.h"

#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif


class _lidCalib_Dll_ CTrjReader
{
public:
	CTrjReader();
	~CTrjReader();

//	bool  open(char *pszName);
	bool  read_header(char *pszName, TrajHdr *pHeader);

	bool  readTrjPos(char *pszName);

	//将航迹数据按时间升序排列
	void  qsort_time_stamp_ascending();
	//采用折半查找，寻找时间段。
	void  findTimeSlice(double t, int &sIdx, int &eIdx);

	//线性内插
	void  interpolation_linear(double t, double &Xt, double &Yt, double &Zt);

public:
	TrajHdr		m_TrjHeader;
	TrajPos		*m_PosBuf;

private:
//	FILE  *m_fp;
	  

private:
	void  close();
	void  read_header(FILE *fp, TrajHdr *pHeader);

};

class _lidCalib_Dll_ CTrjWriter
{
public:
	CTrjWriter();
	~CTrjWriter();

	bool open(char *pszName);
	void close();
	
	void write_header(TrajHdr *pHeader);
	bool writeTrjPos(TrajPos *pPosBuf, int num);

private:
	FILE *m_fp;
};


// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif

#endif