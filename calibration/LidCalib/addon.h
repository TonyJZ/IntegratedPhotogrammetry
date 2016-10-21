
//	=================================================================
//	ADDON.H						21-09-2001 Arttu Soininen
//
//	Structure definitions for TerraScan addon modules.
//	=================================================================

//	Internal/UOR point

#ifndef _addon_h_zj_2010_07_19_
#define _addon_h_zj_2010_07_19_


typedef struct {
	long	x ;
	long	y ;
	long	z ;
} Point3d ;

//	Master unit/meter point
#ifndef _Dp3d_Define_
#define _Dp3d_Define_
typedef struct {
	double	x ;
	double	y ;
	double	z ;
} Dp3d ;
#endif
//	RGB color

typedef struct {
	BYTE	red ;
	BYTE	green ;
	BYTE	blue ;
} RgbClr ;

//	Laser point in TerraScan memory
#ifndef _ScanPnt_Define_
#define _ScanPnt_Define_
typedef struct {
	Point3d	Pnt ;		//	Coordinates in UORs, see IpToDp()
	BYTE	Code ;		//	Classification code
	BYTE	Echo ;		//	Echo information
	BYTE	Flag ;		//	Runtime flag (view visibility)
	BYTE	Mark ;		//	Runtime flag
	USHORT	Line ;		//	Flightline number
	USHORT	Intensity ;	//	Intensity value
} ScanPnt ;
#endif
//	Point class

typedef struct {
	int		Class ;			//	Class number 0-255
	char	Desc[32] ;		//	Description
	char	Reserved[28] ;
} PntCls ;

#define CLASS_ANY		(999)

//	Command specification

typedef struct {
	char	MenuText[40] ;	//	Text to appear in pulldown menu
	int		UseFence ;		//	1=may use fence - tscan marks points, 0=no fence usage
	int		UseSelect ;		//	1=may use selected element, 0=no selection usage
	char	Reserved[200];	//	For future expansion
} AddCmd ;

//	Link to TerraScan tables

typedef struct {
	int		LnkSize ;		//	sizeof(AddLnk)
	int		LnkVersion ;	//	Structure version date (now 20010921)

	//	Currently loaded laser points

	int		*PntCnt ;		//	Number of laser points
	int		PntSize ;		//	sizeof(ScanPnt)
	ScanPnt	**PntTbl ;		//	Laser points or NULL
	UINT	**SecTbl ;		//	Integer time stamps or NULL
	RgbClr	**ClrTbl ;		//	Point color values or NULL

	//	Active point classes

	int		ClsCnt ;		//	Number of point classes
	PntCls	*ClsTbl ;		//	Point classes

	//	Design file coordinate system, see IpToDp() and DpToIp()

	int		Units ;			//	Units per meter = subpermast * uorpersub
	double	OrgX ;			//	Global origin
	double	OrgY ;
	double	OrgZ ;
} AddLnk ;

//	State information when command is run

typedef struct {
	int		SetSize ;		//	sizeof(AddSet)
	int		SetVersion ;	//	Structure version date (now 20010921)

	//	Parent window

	void	*MainWin ;		//	'Loaded points' window (HWND)

	//	Fence information

	int		Fenced ;		//	-1=not requested, 0=no fence, 1=points inside fence marked Mark=1

	//	Selected element

	Dp3d	*SelPts ;		//	Stroked first selected element or NULL
	int		SelCnt ;		//	Number of vertices

	//	Unparsed part of command invocation

	char	Unparsed[400] ;	//	Not currently used
} AddSet ;

//	Return bit mask from command

#define ARET_REDRAW		1	/* Points modified */

//	Link variable change notification

#define AFLG_POINTCOUNT	1	/* Point count has changed */
#define AFLG_CLASSES	2	/* Classes have changed */
#define AFLG_DESIGN		4	/* New design file */


#endif