#include "stdafx.h"
#include "CalibUtility/Dxf_IO.h"



CDxfImporter::CDxfImporter()
{
	m_polyVertices = NULL;
}

CDxfImporter::~CDxfImporter()
{
	for(int i=0; i<m_polylines.size(); i++)
	{
		VertexChain *pVer = m_polylines[i];
		delete pVer;
	}
}

void CDxfImporter::clean()
{
	m_points.clear();
	m_lines.clear();
	
	for(int i=0; i<m_polylines.size(); i++)
	{
		VertexChain *pVer = m_polylines[i];
		pVer->clear();
		delete pVer;
	}
	m_polylines.clear();

	m_faces.clear();
	m_polyVertices = NULL;
}

/**
 * Sample implementation of the method which handles layers.
 */
void CDxfImporter::addLayer(const DL_LayerData& data) {
    printf("LAYER: %s flags: %d\n", data.name.c_str(), data.flags);
}

/**
 * Sample implementation of the method which handles point entities.
 */
void CDxfImporter::addPoint(const DL_PointData& data) {
    printf("POINT    (%6.3f, %6.3f, %6.3f)\n",
           data.x, data.y, data.z);

	orsPOINT3D pt;
	pt.X = data.x;
	pt.Y = data.y;
	pt.Z = data.z;

	m_points.push_back(pt);
}

/**
 * Sample implementation of the method which handles line entities.
 */
void CDxfImporter::addLine(const DL_LineData& data) {
    printf("LINE     (%6.3f, %6.3f, %6.3f) (%6.3f, %6.3f, %6.3f)\n",
           data.x1, data.y1, data.z1, data.x2, data.y2, data.z2);

	myLine  line;
	line.sPoint.X = data.x1; line.sPoint.Y = data.y1; line.sPoint.Z = data.z1;
	line.ePoint.X = data.x2; line.ePoint.Y = data.y2; line.ePoint.Z = data.z2;

	m_lines.push_back(line);
}

/**
 * Sample implementation of the method which handles arc entities.
 */
void CDxfImporter::addArc(const DL_ArcData& data) {
    printf("ARC      (%6.3f, %6.3f, %6.3f) %6.3f, %6.3f, %6.3f\n",
           data.cx, data.cy, data.cz,
           data.radius, data.angle1, data.angle2);
}

/**
 * Sample implementation of the method which handles circle entities.
 */
void CDxfImporter::addCircle(const DL_CircleData& data) {
    printf("CIRCLE   (%6.3f, %6.3f, %6.3f) %6.3f\n",
           data.cx, data.cy, data.cz,
           data.radius);
}


/**
 * Sample implementation of the method which handles polyline entities.
 */
void CDxfImporter::addPolyline(const DL_PolylineData& data) {
    printf("POLYLINE \n");
    printf("flags: %d\n", (int)data.flags);

	m_polyVertices = new VertexChain;
	m_polylines.push_back(m_polyVertices);
}


/**
 * Sample implementation of the method which handles vertices.
 */
void CDxfImporter::addVertex(const DL_VertexData& data) {
    printf("VERTEX   (%6.3f, %6.3f, %6.3f) %6.3f\n",
           data.x, data.y, data.z,
           data.bulge);

	if(m_polyVertices)
	{
		orsPOINT3D pt;
		pt.X = data.x;
		pt.Y = data.y;
		pt.Z = data.z;

		m_polyVertices->push_back(pt);
	}
}


void CDxfImporter::add3dFace(const DL_3dFaceData& data) {
    printf("3DFACE\n");
	my3DFace   face;
    for (int i=0; i<4; i++) {
        printf("   corner %d: %6.3f %6.3f %6.3f\n", 
            i, data.x[i], data.y[i], data.z[i]);

		face.faceVertexs[i].id = i;
		face.faceVertexs[i].pt.X = data.x[i];
		face.faceVertexs[i].pt.Y = data.y[i];
		face.faceVertexs[i].pt.Z = data.z[i];
    }
	m_faces.push_back(face);
}


