//Leica Attune使用的标定点云格式_ATN.LAS
#ifndef _ATN_LAS_File_WRITER_H_zj_2010_07_14_
#define _ATN_LAS_File_WRITER_H_zj_2010_07_14_

//#include <stdio.h>
//#include "orsBase/orsTypedef.h"
//#include "orsBase/orsString.h"
#include "lastools/lasdefinitions.h"
//#include "CalibBasedef.h"
#include "ATN_def.h"
#include "orsPointCloud/orsIPointCloudWriter.h"
//#include "MappingFile.h"

class CATNWriter : public orsIPointCloudWriter, public orsObjectBase
{
public:
	CATNWriter(bool bForRegister);
	virtual ~CATNWriter();
	
	// 取文件格式名
	virtual orsString get_file_format()	const
	{
		return _T("atn.las");
	}

	// 取支持存储的内容
	virtual ors_uint32 getSupportedContents() const
	{
		return ORS_PCM_XYZ | ORS_PCM_INTENSITY | ORS_PCM_CLASS | ORS_PCM_GPSTIME | ORS_PCM_RGB	| ORS_PCM_NUM_RETURNS | ORS_PCM_OBSINFO;
		// ORS_PCM_OBSINFO
	}

	virtual bool open(const char *lasFileName, lasDataFORMAT point_data_format=lasPOINT_ATN,	// unsigned char point_data_format=0, 
		double xScale=0, double yScale=0, double zScale=0);

// 	virtual bool open(const char *lasFileName, lasDataFORMAT point_data_format,  
// 		double scale[3], double offset[3]);

	virtual void close();

	virtual bool set_writepos(ors_int64 offset)
	{

		// 最大可相对定位范围
		fseek( m_fp, offset, SEEK_SET);

		return true;
	};

	virtual bool write_point(double* coordinates) 
	{
		return write_point( coordinates[0], coordinates[1], coordinates[2] );
	}

	virtual bool write_point(double X, double Y, double Z);

	virtual bool write_point(double X, double Y, double Z, unsigned short r, unsigned short g, unsigned short b )
	{
		double coord[3];
		unsigned short rgb[3];
		coord[0]=X; coord[1]=Y; coord[2]=Z;
		rgb[0]=r; rgb[1]=g; rgb[2]=b;
		return write_point( coord, 0, rgb);
	}

	virtual bool write_point(double* coordinates, unsigned short intensity, unsigned short* rgb = NULL );


	virtual bool write_point(double* coordinates, unsigned short intensity,
		unsigned char return_number, unsigned char number_of_returns_of_given_pulse,
		unsigned char scan_direction_flag, unsigned char edge_of_flight_line,
		unsigned char classification, 
		char scan_angle_rank, unsigned char user_data,
		unsigned short point_source_ID, double gps_time = 0, unsigned short* rgb = 0);

	virtual bool write_point(double* coordinates, unsigned short intensity,
		unsigned char return_number, unsigned char number_of_returns_of_given_pulse,
		unsigned char scan_direction_flag, unsigned char edge_of_flight_line,
		unsigned char classification, 
		char scan_angle_rank, unsigned char user_data,
		unsigned short point_source_ID, double gps_time, orsPointObservedInfo *info,
		unsigned short* rgb = 0);


private:
	bool write_point(ATN_RawPoint *point, unsigned short* rgb=0);

	void ModifyHeader( double x, double y, double z );
	//内部坐标转换
	void SetXyz( ATN_RawPoint *point, double x, double y, double z );

	
public:
	LASheader		header;
	
	
	int npoints;
	int p_count;
	
private:
//	ATN_RawPoint	m_rawpoint;
	double  m_ObsInfo_scale[3];
	double	m_ObsInfo_offset[3];

protected:
	bool	m_bCreate;
//	CMappingFile	m_mappingFile;
	FILE		*m_fp;
	int		additional_bytes_per_point;

public:
	ORS_OBJECT_IMP1( CATNWriter, orsIPointCloudWriter, _T("atn"), _T("atn") );
};



#endif