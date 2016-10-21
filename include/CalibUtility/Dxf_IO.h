#ifndef _DXF_FILE_IO_ZJ_2016_04_07_H__
#define _DXF_FILE_IO_ZJ_2016_04_07_H__

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif


#include "dl_creationadapter.h"
#include <vector>
#include "orsBase/orsTypedef.h"

using namespace std;

struct myLine
{
	orsPOINT3D  sPoint;
	orsPOINT3D  ePoint;
};

struct myFaceVetex
{
	int id;
	orsPOINT3D  pt;
};

struct my3DFace
{
	myFaceVetex    faceVertexs[4];
};

typedef vector<orsPOINT3D>  VertexChain;

class _CalibUtility_Dll_ CDxfImporter : public DL_CreationAdapter
{
public:
	CDxfImporter();
	~CDxfImporter();

	void clean();

	virtual void addLayer(const DL_LayerData& data);
	virtual void addPoint(const DL_PointData& data);
	virtual void addLine(const DL_LineData& data);
	virtual void addArc(const DL_ArcData& data);
	virtual void addCircle(const DL_CircleData& data);
	virtual void addPolyline(const DL_PolylineData& data);
	virtual void addVertex(const DL_VertexData& data);
	virtual void add3dFace(const DL_3dFaceData& data);

public:
	vector<orsPOINT3D>  m_points;
	vector<myLine>		m_lines;
	vector<VertexChain*>	m_polylines;

	vector<my3DFace>	 m_faces;
	
	VertexChain			*m_polyVertices;
};


#endif