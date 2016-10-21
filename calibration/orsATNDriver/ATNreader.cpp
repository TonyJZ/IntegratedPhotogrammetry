#include "StdAfx.h"
#include "ATNreader.h"

#define  Range_Scale_Factor   160.0

#ifdef _WIN32

#include "lastools/ifile.h"

#define myFILE IFILE
#define fopen ifopen
#define fread ifread
#define fseek ifseek

#define fclose ifclose

#else

#define myFILE FILE

unsigned   long   get_file_length(  FILE*   file_ptr  ) 
{  
	unsigned   long   PosCur      =   0;  
	unsigned   long   PosBegin    =   0;  
	unsigned   long   PosEnd      =   0;  

	if(  NULL ==  file_ptr   )  
	{  
		return   0;  
	}  

	PosCur   =   ftell(   file_ptr   );  //得到文件指针的当前位置
	fseek(   file_ptr,   0L,   SEEK_SET   );  //将文件指针移到文件开始
	PosBegin   =   ftell(   file_ptr   );  //得到当前文件的位置，也就是文件头的位置
	fseek(   file_ptr,   0L,   SEEK_END   );  //将文件指针移到文件末尾
	PosEnd   =   ftell(   file_ptr   );  //得到文件的末尾位置
	fseek(   file_ptr,   PosCur,   SEEK_SET   );  //将指针移到文件的原来位置

	return   PosEnd   -   PosBegin;          //返回文件大小
}

#endif


CATNReader::CATNReader(bool bForRegister)
{
//	m_ptBuf=0;
	m_fp=0;
//	m_mappingFile.SetMappingSize(128*1024*1024);
	additional_bytes_per_point=0;
	m_npoints=0;
	m_pcount=0;

	m_ObsInfo_offset[0] = m_ObsInfo_offset[1]= m_ObsInfo_offset[2] = 0;
	m_ObsInfo_scale[0] = 1.47e-9;
	m_ObsInfo_scale[1] = 1.47e-9;
	m_ObsInfo_scale[2] = 1e-3;

//	m_attFlag = PA_ELEVATION;
}

CATNReader::~CATNReader()
{
	close();
}


void CATNReader::close()
{
// 	if(m_ptBuf)
// 	{
// 		delete[] m_ptBuf;
// 		m_ptBuf=0;
// 	}
	if(m_fp)
	{
		fclose((myFILE *)m_fp);
		m_fp=0;
	}

//	m_mappingFile.Close();
}

bool CATNReader::open(const char *pszName, bool bShared)
{
	close();
	m_fp = (myFILE *)fopen(pszName, "rb");
	if(m_fp==0)
		return false;
	
// 	m_mappingFile.SetMappingSize(128*1024*1024);
// 	m_mappingFile.Open(pszName, "r");

	return read_header(&m_header);

//	return true;
}

void CATNReader::reopen()
{
//	m_mappingFile.Seek(m_header.offset_to_point_data, SEEK_SET);
	
	fseek((myFILE *)m_fp, m_header.offset_to_point_data, SEEK_SET);

	m_pcount=0;
}

// void* CATNReader::get_header()
// {
// 	return &m_header;
// }

bool CATNReader::read_header(LASheader *pHeader)
{
//	bool bReturn = true;
	int i;

	fseek((myFILE *)m_fp, 0, SEEK_SET);

//	m_mappingFile.Seek(0, SEEK_SET);
	
	pHeader->clean();
	
	// read the m_header variable after variable (to avoid alignment issues)
	fread(&(pHeader->file_signature), sizeof(char), 4, (myFILE *)m_fp);
	fread(&(pHeader->file_source_id), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->global_encoding), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->project_ID_GUID_data_1), sizeof(unsigned int), 1, (myFILE *)m_fp);
	fread(&(pHeader->project_ID_GUID_data_2), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->project_ID_GUID_data_3), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->project_ID_GUID_data_4), sizeof(char), 8, (myFILE *)m_fp);
	fread(&(pHeader->version_major), sizeof(char), 1, (myFILE *)m_fp);
	fread(&(pHeader->version_minor), sizeof(char), 1, (myFILE *)m_fp);
	fread(&(pHeader->system_identifier), sizeof(char), 32, (myFILE *)m_fp);
	fread(&(pHeader->generating_software), sizeof(char), 32, (myFILE *)m_fp);
	fread(&(pHeader->file_creation_day), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->file_creation_year), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->header_size), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->offset_to_point_data), sizeof(unsigned int), 1, (myFILE *)m_fp);
	fread(&(pHeader->number_of_variable_length_records), sizeof(unsigned int), 1, (myFILE *)m_fp);
	fread(&(pHeader->point_data_format), sizeof(unsigned char), 1, (myFILE *)m_fp);
	fread(&(pHeader->point_data_record_length), sizeof(unsigned short), 1, (myFILE *)m_fp);
	fread(&(pHeader->number_of_point_records), sizeof(unsigned int), 1, (myFILE *)m_fp);
	fread(&(pHeader->number_of_points_by_return), sizeof(unsigned int), 5, (myFILE *)m_fp);
	fread(&(pHeader->x_scale_factor), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->y_scale_factor), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->z_scale_factor), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->x_offset), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->y_offset), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->z_offset), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->max_x), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->min_x), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->max_y), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->min_y), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->max_z), sizeof(double), 1, (myFILE *)m_fp);
	fread(&(pHeader->min_z), sizeof(double), 1, (myFILE *)m_fp);

	
	//////////////////////////////////////////////////
	/////////////////////////////////////////////////
	m_npoints=pHeader->number_of_point_records;
	m_pcount=0;

	// check m_header contents
	
	if (strncmp(pHeader->file_signature, "LASF", 4) != 0)
	{
		fprintf(stderr,"ERROR: wrong m_fp signature '%s'\n", pHeader->file_signature);
		return false;
	}
	if ((pHeader->version_major != 1) || ((pHeader->version_minor != 0) && (pHeader->version_minor != 1) && (pHeader->version_minor != 2)))
	{
		fprintf(stderr,"WARNING: unknown version %d.%d (should be 1.0 or 1.1 or 1.2)\n", pHeader->version_major, pHeader->version_minor);
	}
	if (pHeader->header_size < 227)
	{
		fprintf(stderr,"WARNING: pHeader size is %d but should be at least 227\n", pHeader->header_size);
	}
	if (pHeader->offset_to_point_data < pHeader->header_size)
	{
		fprintf(stderr,"ERROR: offset to point data %d is smaller than pHeader size %d\n", pHeader->offset_to_point_data, pHeader->header_size);
		return false;
	}
	if (pHeader->number_of_point_records <= 0)
	{
		fprintf(stderr,"WARNING: number of point records is %d\n", pHeader->number_of_point_records);
	}

	if (pHeader->point_data_record_length == 60 )
	{
		points_have_gps_time = true;
		points_have_rgb = false;
	}
	else if(pHeader->point_data_record_length == 66 )
	{
		points_have_gps_time = true;
		points_have_rgb = true;
	}
	else
	{
		return false;
	}

	if (pHeader->x_scale_factor == 0 || pHeader->y_scale_factor == 0 || pHeader->z_scale_factor == 0)
	{
		fprintf(stderr,"WARNING: some scale factors are zero %g %g %g. will set them to 0.01.\n", pHeader->x_scale_factor, pHeader->y_scale_factor, pHeader->z_scale_factor);
		if (pHeader->x_scale_factor == 0) pHeader->x_scale_factor = 0.01;
		if (pHeader->y_scale_factor == 0) pHeader->y_scale_factor = 0.01;
		if (pHeader->z_scale_factor == 0) pHeader->z_scale_factor = 0.01;
	}
	if (pHeader->max_x < pHeader->min_x || pHeader->max_y < pHeader->min_y || pHeader->max_z < pHeader->min_z)
	{
		fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", pHeader->min_x, pHeader->min_y, pHeader->min_z, pHeader->max_x, pHeader->max_y, pHeader->max_z);
	}
	
	// load any number of user-defined bytes that might have been added to the pHeader
	pHeader->user_data_in_header_size = pHeader->header_size - 227;
	if (pHeader->user_data_in_header_size)
	{
		pHeader->user_data_in_header = new char[pHeader->user_data_in_header_size];
		
		if (!fread(pHeader->user_data_in_header, sizeof(char), pHeader->user_data_in_header_size, (myFILE *)m_fp))
		{
			fprintf(stderr,"ERROR: reading %d bytes of data into pHeader.user_data_in_header\n", pHeader->user_data_in_header_size);
			return false;
		}
	}
	
	bool skip_all_headers=false;
	if (skip_all_headers)
	{
#ifdef WIN32
		fseek( (myFILE*)m_fp, pHeader->offset_to_point_data, SEEK_SET );
#else
		for(i = pHeader->header_size; i < (int)pHeader->offset_to_point_data; i++) 
			fgetc(m_fp);
#endif
 	}
	else
	{
		int vlrs_size = 0;
		
		// read the variable length records into the pHeader
		
		if (pHeader->number_of_variable_length_records)
		{
			pHeader->vlrs = new LASvlr[pHeader->number_of_variable_length_records];
			
			for (i = 0; i < (int)pHeader->number_of_variable_length_records; i++)
			{
				// make sure there are enough bytes left to read a variable length record before the point block starts
				if (((int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size) < 54)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block after reading %d of %d vlrs. skipping remaining vlrs ...\n", (int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size, i, pHeader->number_of_variable_length_records);
					pHeader->number_of_variable_length_records = i;
					break;
				}
				
				// read variable length records variable after variable (to avoid alignment issues)
				
				if (!fread(&(pHeader->vlrs[i].reserved), sizeof(unsigned short), 1, (myFILE *)m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].reserved\n", i);
					return false;
				}
				if (!fread(pHeader->vlrs[i].user_id, sizeof(char), 16, (myFILE *)m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].user_id\n", i);
					return false;
				}
				if (!fread(&(pHeader->vlrs[i].record_id), sizeof(unsigned short), 1, (myFILE *)m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].record_id\n", i);
					return false;
				}
				if (!fread(&(pHeader->vlrs[i].record_length_after_header), sizeof(unsigned short), 1, (myFILE *)m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].record_length_after_header\n", i);
					return false;
				}
				if (!fread(pHeader->vlrs[i].description, sizeof(char), 32, (myFILE *)m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].description\n", i);
					return false;
				}
				
				// keep track on the number of bytes we have read so far
				
				vlrs_size += 54;
				
				// check variable length record contents
				
				if (pHeader->vlrs[i].reserved != 0xAABB)
				{
					fprintf(stderr,"WARNING: wrong pHeader.vlrs[%d].reserved: %d != 0xAABB\n", i, pHeader->vlrs[i].reserved);
				}
				
				// make sure there are enough bytes left to read the data of the variable length record before the point block starts
				
				if (((int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size) < pHeader->vlrs[i].record_length_after_header)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block when trying to read %d bytes into pHeader.vlrs[%d].data\n", (int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size, pHeader->vlrs[i].record_length_after_header, i);
					pHeader->vlrs[i].record_length_after_header = (int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size;
				}
				
				// load data following the pHeader of the variable length record
				
				if (pHeader->vlrs[i].record_length_after_header)
				{
					pHeader->vlrs[i].data = new char[pHeader->vlrs[i].record_length_after_header];
					
					if (!fread(pHeader->vlrs[i].data, sizeof(char), pHeader->vlrs[i].record_length_after_header, (myFILE *)m_fp))
					{
						fprintf(stderr,"ERROR: reading %d bytes of data into pHeader.vlrs[%d].data\n", pHeader->vlrs[i].record_length_after_header, i);
						return false;
					}
				}
				else
				{
					pHeader->vlrs[i].data = 0;
				}
				
				// keep track on the number of bytes we have read so far
				
				vlrs_size += pHeader->vlrs[i].record_length_after_header;
				
				// special handling for known variable pHeader tags
				
				if (strcmp(pHeader->vlrs[i].user_id, "LASF_Projection") == 0)
				{
					if (pHeader->vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
					{
						if (pHeader->vlr_geo_keys)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
						}
						pHeader->vlr_geo_keys = (LASvlr_geo_keys*)pHeader->vlrs[i].data;
						
						// check variable pHeader geo keys contents
						
						if (pHeader->vlr_geo_keys->key_directory_version != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_directory_version: %d != 1\n",pHeader->vlr_geo_keys->key_directory_version);
						}
						if (pHeader->vlr_geo_keys->key_revision != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_revision: %d != 1\n",pHeader->vlr_geo_keys->key_revision);
						}
						if (pHeader->vlr_geo_keys->minor_revision != 0)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->minor_revision: %d != 0\n",pHeader->vlr_geo_keys->minor_revision);
						}
						pHeader->vlr_geo_key_entries = (LASvlr_key_entry*)&pHeader->vlr_geo_keys[1];
					}
					else if (pHeader->vlrs[i].record_id == 34736) // GeoDoubleParamsTag
					{
						if (pHeader->vlr_geo_double_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParamsTag\n");
						}
						pHeader->vlr_geo_double_params = (double*)pHeader->vlrs[i].data;
					}
					else if (pHeader->vlrs[i].record_id == 34737) // GeoAsciiParamsTag
					{
						if (pHeader->vlr_geo_ascii_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
						}
						pHeader->vlr_geo_ascii_params = (char*)pHeader->vlrs[i].data;
					}
				}
			}
		}
	
		// load any number of user-defined bytes that might have been added after the pHeader
		
		pHeader->user_data_after_header_size = (int)pHeader->offset_to_point_data - vlrs_size - pHeader->header_size;
		if (pHeader->user_data_after_header_size)
		{
			pHeader->user_data_after_header = new char[pHeader->user_data_after_header_size];
			
			if (!fread(pHeader->user_data_after_header, sizeof(char), pHeader->user_data_after_header_size, (myFILE *)m_fp))
			{
				fprintf(stderr,"ERROR: reading %d bytes of data into pHeader.user_data_after_header\n", pHeader->user_data_after_header_size);
				return false;
			}
		}
	}
	
	m_ptContent = ORS_PCM_XYZ | ORS_PCM_INTENSITY | ORS_PCM_CLASS | ORS_PCM_NUM_RETURNS | ORS_PCM_OBSINFO;

	if( points_have_gps_time )
		m_ptContent |= ORS_PCM_GPSTIME;

	if( points_have_rgb )
		m_ptContent |= ORS_PCM_RGB;

	return true;
}

// ors_uint32 CATNReader::get_point_contentMask()
// {
// 	ors_uint32 mask = ORS_PCM_XYZ | ORS_PCM_INTENSITY | ORS_PCM_CLASS | ORS_PCM_NUM_RETURNS | ORS_PCM_OBSINFO;
// 
// 	if( points_have_gps_time )
// 		mask |= ORS_PCM_GPSTIME;
// 
// 	if( points_have_rgb )
// 		mask |= ORS_PCM_RGB;
// 
// 	return mask;
// }

bool CATNReader::read_point(double* coordinates)
{
// 	if(m_pcount < m_npoints)
// 	{
// 		fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
// 
// 		coordinates[0]=m_rawpoint.x*m_header.x_scale_factor+m_header.x_offset;
// 		coordinates[1]=m_rawpoint.y*m_header.y_scale_factor+m_header.y_offset;
// 		coordinates[2]=m_rawpoint.z*m_header.z_scale_factor+m_header.z_offset;
// 
// 		if(m_attFlag == PA_INTENSITY)
// 			coordinates[2] = m_rawpoint.intensity;
// 		
// 		m_pcount++;
// 		return true;
// 	}
// 
// 	return false;	
	if (read_point())
	{
		get_coordinates(coordinates);
		return true;
	}
	return false;
}

void CATNReader::get_coordinates(double* coordinates)
{
	coordinates[0] = m_rawpoint.x*m_header.x_scale_factor+m_header.x_offset;
	coordinates[1] = m_rawpoint.y*m_header.y_scale_factor+m_header.y_offset;
	coordinates[2] = m_rawpoint.z*m_header.z_scale_factor+m_header.z_offset;
}

bool CATNReader::read_rawpoint(int* coordinates )
{
	if (read_point())
	{
		coordinates[0]=m_rawpoint.x;
		coordinates[1]=m_rawpoint.y;
		coordinates[2]=m_rawpoint.z;
		return true;
	}
	return false;
}

bool CATNReader::read_point()
{
	if(m_pcount < m_npoints)
	{
		//		m_rawpoint=(ATN_RawPoint*)m_mappingFile.ReadPtr(sizeof(ATN_RawPoint));
		if(fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, (myFILE *)m_fp)==false)
			return false;

		m_pcount++;
		return true;
	}

	return false;	
}

// bool CATNReader::read_point(void* pData)
// {
// 	ATNPoint *point = (ATNPoint*)pData;
// 
// 	if(m_pcount < m_npoints)
// 	{
// //		m_rawpoint=(ATN_RawPoint*)m_mappingFile.ReadPtr(sizeof(ATN_RawPoint));
// 		fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
// 		
// 		point->x=m_rawpoint.x*m_header.x_scale_factor+m_header.x_offset;
// 		point->y=m_rawpoint.y*m_header.y_scale_factor+m_header.y_offset;
// 		point->z=m_rawpoint.z*m_header.z_scale_factor+m_header.z_offset;
// 		point->intensity=m_rawpoint.intensity;
// 		point->return_number=m_rawpoint.return_number;
// 		point->number_of_returns_of_given_pulse=m_rawpoint.number_of_returns_of_given_pulse;
// 		point->scan_direction_flag=m_rawpoint.scan_direction_flag;
// 		point->edge_of_flight_line=m_rawpoint.edge_of_flight_line;
// 		point->classification=m_rawpoint.classification;
// 		point->scan_angle_rank=m_rawpoint.scan_angle_rank;
// 		point->user_data=m_rawpoint.user_data;
// 		point->point_source_ID=m_rawpoint.point_source_ID;
// 		
// 		point->gpstime=m_rawpoint.gpstime;
// 		point->latitude=m_rawpoint.latitude*m_header.x_scale_factor+m_header.x_offset;	// (WGS84/radians - ALS50 OPTICAL CENTER)
// 		point->longitude=m_rawpoint.longitude*m_header.y_scale_factor+m_header.y_offset;	// (WGS84/radians - ALS50 OPTICAL CENTER)
// 		point->altitude=m_rawpoint.altitude*m_header.z_scale_factor+m_header.z_offset;	// (WGS84/meters - ALS50 OPTICAL CENTER)
// 		point->roll=m_rawpoint.roll*m_header.x_scale_factor+m_header.x_offset;		// (radians)
// 		point->pitch=m_rawpoint.pitch*m_header.x_scale_factor+m_header.x_offset;		// (radians)
// 		point->heading=m_rawpoint.heading*m_header.x_scale_factor+m_header.x_offset;		// (radians)
// 		point->scanAngle=m_rawpoint.scanAngle*m_header.x_scale_factor+m_header.x_offset;	//扫描角
// 		point->range=m_rawpoint.range/Range_Scale_Factor;		// (meters)
// 
// 		point->scanAngle*=2;
// 
// 
// 		//弧度转角度
// // 		point->x=point->x*180/PI;
// // 		point->y=point->y*180/PI;
// // 		point->latitude=point->latitude*180/PI;
// // 		point->longitude=point->longitude*180/PI;
// 
// // 		point->roll=point->roll*180/PI;
// // 		point->pitch=point->pitch*180/PI;
// // 		point->heading=point->heading*180/PI;
// // 		point->scanAngle=point->scanAngle*180/PI*2;
// 
// 		m_pcount++;
// 		return true;
// 	}
// 	
// 	return false;	
// }

bool CATNReader::set_readpos(__int64  offset)
{
	m_pcount = (offset-m_header.offset_to_point_data)/ATN_RawPoint_Length;

	return fseek((myFILE *)m_fp, offset, SEEK_SET);
}

bool CATNReader::set_readpos(int ptNum)
{
	if( ptNum < 0 )
		return false;

#ifdef WIN32
	// 最大可相对定位范围

	fseek( (myFILE *)m_fp, m_header.offset_to_point_data + (__int64)ATN_RawPoint_Length*ptNum, SEEK_SET);

#else
	fseek( (myFILE *)m_fp, m_header.offset_to_point_data + ATN_RawPoint_Length*ptNum, SEEK_SET);

#endif

	m_pcount = ptNum;

	//gps_time = 0.0;

	return true;
}

// bool CATNReader::read_point(ATNPoint_TP *point)
// {
// 	if(m_pcount < m_npoints)
// 	{
// 		m_rawpoint=(ATN_RawPoint*)m_mappingFile.ReadPtr(sizeof(ATN_RawPoint));
// 		//		fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
// 		
// 		point->x=m_rawpoint->x*m_header.x_scale_factor+m_header.x_offset;
// 		point->y=m_rawpoint->y*m_header.y_scale_factor+m_header.y_offset;
// 		point->z=m_rawpoint->z*m_header.z_scale_factor+m_header.z_offset;
// 		point->intensity=m_rawpoint->intensity;
// 		point->return_number=m_rawpoint->return_number;
// 		point->number_of_returns_of_given_pulse=m_rawpoint->number_of_returns_of_given_pulse;
// 		point->scan_direction_flag=m_rawpoint->scan_direction_flag;
// 		point->edge_of_flight_line=m_rawpoint->edge_of_flight_line;
// 		point->classification=m_rawpoint->classification;
// 		point->scan_angle_rank=m_rawpoint->scan_angle_rank;
// 		point->user_data=m_rawpoint->user_data;
// 		point->point_source_ID=m_rawpoint->point_source_ID;
// 		
// 		point->gpstime=m_rawpoint->gpstime;
// 		point->Xs=m_rawpoint->latitude*m_header.x_scale_factor+m_header.x_offset;	
// 		point->Ys=m_rawpoint->longitude*m_header.y_scale_factor+m_header.y_offset;	
// 		point->Zs=m_rawpoint->altitude*m_header.z_scale_factor+m_header.z_offset;	
// 		point->phi=m_rawpoint->roll*m_header.x_scale_factor+m_header.x_offset;		
// 		point->omega=m_rawpoint->pitch*m_header.x_scale_factor+m_header.x_offset;		
// 		point->kappa=m_rawpoint->heading*m_header.x_scale_factor+m_header.x_offset;		
// 		point->scanAngle=m_rawpoint->scanAngle*m_header.x_scale_factor+m_header.x_offset;	//扫描角
// 		point->range=m_rawpoint->range/Range_Scale_Factor;		// (meters)
// 		
// 		point->scanAngle*=2;
// 	
// 		m_pcount++;
// 		return true;
// 	}
// 	
// 	return false;	
// }

bool CATNReader::get_point_observed_info(orsPointObservedInfo* info)
{
	info->pos.lat=m_rawpoint.latitude*m_header.y_scale_factor+m_header.y_offset;	
	info->pos.lon=m_rawpoint.longitude*m_header.x_scale_factor+m_header.x_offset;	
	info->pos.h=m_rawpoint.altitude*m_header.z_scale_factor+m_header.z_offset;	//线元素可能会在投影坐标系下

	info->roll=m_rawpoint.roll*m_ObsInfo_scale[0]+m_ObsInfo_offset[0];		
	info->pitch=m_rawpoint.pitch*m_ObsInfo_scale[0]+m_ObsInfo_offset[0];		
	info->heading=m_rawpoint.heading*m_ObsInfo_scale[0]+m_ObsInfo_offset[0];		
	info->scanAngle=m_rawpoint.scanAngle*m_ObsInfo_scale[0]+m_ObsInfo_offset[0];	//扫描角
	info->range=m_rawpoint.range/Range_Scale_Factor;		// (meters)

	info->scanAngle*=2;

	return true;
}