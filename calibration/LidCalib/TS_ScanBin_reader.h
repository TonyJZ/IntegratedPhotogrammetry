//读取TS定义的bin格式
#ifndef _TerraScan_binary_file_reader_define_zj_2010_07_19_
#define _TerraScan_binary_file_reader_define_zj_2010_07_19_


#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif

#include "addon.h"

typedef unsigned char BYTE ;
typedef unsigned short USHORT ;
typedef unsigned int UINT ;


//	TerraScan binary file point record (ver 20010712, 20010129, 970404)

typedef struct {
	BYTE	Code ;		//	Classification code 0-255
	BYTE	Line ;		//	Flightline number 0-255
	USHORT	EchoInt ;	//	Intensity bits 0-13, echo bits 14-15
	long	X ;			//	Coordinates
	long	Y ;
	long	Z ;
} ScanRow;

//	TerraScan binary file point record ver 20020715

#ifndef _ScanPnt_Define_
#define _ScanPnt_Define_
typedef struct {
	Point3d	Pnt ;		//	Coordinates
	BYTE	Code ;		//	Classification code
	BYTE	Echo ;		//	Echo information
	BYTE	Flag ;		//	Runtime flag (view visibility)
	BYTE	Mark ;		//	Runtime flag
	USHORT	Line ;		//	Flightline number
	USHORT	Intensity ;	//	Intensity value
} ScanPnt ;
#endif


//	TerraScan binary file header
#ifndef _ScanHdr_Define_
#define _ScanHdr_Define_
typedef struct {
	int		HdrSize ;	//	sizeof(ScanHdr)
	int		HdrVersion;	//	Version number 20020715, 20010712, 20010129 or 970404
	int		Tunniste ;	//	Always 970401	
	char	Magic[4];	//	CXYZ
	int		PntCnt ;	//	Number of points stored		
	int		Units ;		//	Units per meter = subpermast * uorpersub
	double	OrgX ;		//	Global origin
	double	OrgY ;
	double	OrgZ ;
	int		Time ;		//	32 bit integer time stamps appended to points
	int		Color ;		//	Color values appended to points
} ScanHdr ;
#endif

//	Undefined time stamp

#define SEC_UNDEFINED	0xFFFFFFFF

int	ScanReadHdr( ScanHdr *Hp, FILE *File);
int	SrvScanHeaderValid( ScanHdr *H);
int	ScanGetHeader( ScanHdr *Hdr, char *Name);
int ScanReadBinary( ScanPnt *Tbl, UINT *Sec, RgbClr *Clr, int Cnt, char *Path);

class _lidCalib_Dll_ CScanBinReader
{
public:
	CScanBinReader();
	~CScanBinReader();
	
	bool open(char *pszFileName, ScanHdr *header);
//	int  read_header(ScanHdr *header);
	int	 read_point(double &x, double &y, double &z);

	
private:
	FILE   *m_fp;
	int m_npoints;
	int m_pcount;
	ScanHdr  m_header;
};

#endif