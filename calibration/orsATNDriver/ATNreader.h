//Leica Attune使用的标定点云格式_ATN.LAS
#ifndef _ATN_LAS_File_Reader_H_zj_2010_07_14_
#define _ATN_LAS_File_Reader_H_zj_2010_07_14_

//#include <stdio.h>
//#include "orsBase/orsTypedef.h"

#include "lastools/lasdefinitions.h"
//#include "CalibBasedef.h"
#include "ATN_def.h"
#include "orsPointCloud/orsIPointCloudReader.h"

//#include "MappingFile.h"


class CATNReader : public orsIPointCloudReader, public orsObjectBase
{
public:
	CATNReader(bool bForRegister);
	virtual ~CATNReader();

	virtual bool open( const char *lasFileName, bool bShared = false);

	// 取支持存储的内容
	virtual ors_uint32 get_point_contentMask() const
	{
		return m_ptContent;
	}

	virtual ors_int32 get_number_of_points() {return m_header.number_of_point_records;};

	virtual void close();

	virtual void reopen();	//重新从第一个点记录开始读取

	virtual bool set_readpos(int ptNum = 0);
	virtual bool set_readpos(__int64  offset);

	//	virtual bool read_point() = 0;
	//	virtual bool read_point(float* coordinates);
//	virtual void* get_header();
	virtual void  get_offset(double offset[3])
	{
		offset[0] = m_header.x_offset;
		offset[1] = m_header.y_offset;
		offset[2] = m_header.z_offset;
	}

	virtual void  get_scale_factor(double scale[3])
	{
		scale[0] = m_header.x_scale_factor;
		scale[1] = m_header.y_scale_factor;
		scale[2] = m_header.z_scale_factor;
	}

	virtual __int64 get_number_of_point()
	{
		return m_header.number_of_point_records;
	}

	virtual void  get_boundary(orsPOINT3D *bbmax, orsPOINT3D *bbmin)
	{
		bbmax->X=m_header.max_x;	bbmin->X=m_header.min_x;
		bbmax->Y=m_header.max_y;	bbmin->Y=m_header.min_y;
		bbmax->Z=m_header.max_z;	bbmin->Z=m_header.min_z;
	}
// 
// 	virtual __int32 get_point_record_length()
// 	{
// 		return m_header.point_data_record_length;
// 	}


//	virtual void set_attribution_flag(orsPointAttributionFlag flag) {m_attFlag=flag;};
	virtual bool read_point(double* coordinates) ;
	virtual bool read_rawpoint(int* coordinates );

	virtual unsigned short get_intensity(){return m_rawpoint.intensity;};
	virtual unsigned char get_classification(){return m_rawpoint.classification;};
	virtual double get_gpstime() {return m_rawpoint.gpstime;};
	virtual void get_rgb(unsigned short Rgb[3])
	{
	}

	virtual int get_return_number()
	{
		return m_rawpoint.return_number;
	}

	virtual int get_number_of_returns()
	{
		return m_rawpoint.number_of_returns_of_given_pulse;
	}

	virtual unsigned char get_edge_of_flight_line()
	{
		return m_rawpoint.edge_of_flight_line;
	}

	virtual unsigned char get_scan_direction_flag()
	{
		return m_rawpoint.scan_direction_flag;
	}
	
	virtual char get_scan_angle_rank()
	{
		return m_rawpoint.scan_angle_rank;
	}

	virtual unsigned char get_user_data()
	{
		return m_rawpoint.user_data;
	}

	virtual unsigned short get_point_source_ID()
	{
		return m_rawpoint.point_source_ID;
	}

	virtual bool get_point_observed_info(orsPointObservedInfo* info);


private:
	bool  read_header(LASheader *pHeader);
	bool  read_point();

	void get_coordinates(double* coordinates);

//	inline void  Seek(__int64  offset);

public:
	int m_npoints;
	int m_pcount;

	unsigned short rgb[3];

	bool points_have_gps_time;
	bool points_have_rgb;

private:
	LASheader		m_header;

	ATN_RawPoint	m_rawpoint;

	double  m_ObsInfo_scale[3];
	double	m_ObsInfo_offset[3];

	ors_uint32  m_ptContent;

protected:
//	CMappingFile m_mappingFile;
	void	*m_fp;
	int		additional_bytes_per_point;

public:
	ORS_OBJECT_IMP1( CATNReader, orsIPointCloudReader, _T("atn"), _T("atn") );
};


#endif