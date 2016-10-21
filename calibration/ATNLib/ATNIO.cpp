#include "StdAfx.h"
#include "ATNLib/ATNIO.h"

#define  Range_Scale_Factor   160.0

CATNReader::CATNReader()
{
//	m_ptBuf=0;
	m_fp=0;
//	m_mappingFile.SetMappingSize(128*1024*1024);
	additional_bytes_per_point=0;
	m_npoints=0;
	m_pcount=0;
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
		fclose(m_fp);
		m_fp=0;
	}

//	m_mappingFile.Close();
}

bool CATNReader::open(const char *pszName)
{
	close();
	m_fp=fopen(pszName, "rb");
	if(m_fp==0)
		return false;
	
// 	m_mappingFile.SetMappingSize(128*1024*1024);
// 	m_mappingFile.Open(pszName, "r");

	read_header(&m_header);

	return true;
}

void CATNReader::reopen()
{
//	m_mappingFile.Seek(m_header.offset_to_point_data, SEEK_SET);
	
	fseek(m_fp, m_header.offset_to_point_data, SEEK_SET);

	m_pcount=0;
}

bool CATNReader::read_header(LASheader *pHeader)
{
	int i;

	fseek(m_fp, 0, SEEK_SET);

//	m_mappingFile.Seek(0, SEEK_SET);
	
	pHeader->clean();
	
	// read the m_header variable after variable (to avoid alignment issues)
	fread(&(pHeader->file_signature), sizeof(char), 4, m_fp);
	fread(&(pHeader->file_source_id), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->global_encoding), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->project_ID_GUID_data_1), sizeof(unsigned int), 1, m_fp);
	fread(&(pHeader->project_ID_GUID_data_2), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->project_ID_GUID_data_3), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->project_ID_GUID_data_4), sizeof(char), 8, m_fp);
	fread(&(pHeader->version_major), sizeof(char), 1, m_fp);
	fread(&(pHeader->version_minor), sizeof(char), 1, m_fp);
	fread(&(pHeader->system_identifier), sizeof(char), 32, m_fp);
	fread(&(pHeader->generating_software), sizeof(char), 32, m_fp);
	fread(&(pHeader->file_creation_day), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->file_creation_year), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->header_size), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->offset_to_point_data), sizeof(unsigned int), 1, m_fp);
	fread(&(pHeader->number_of_variable_length_records), sizeof(unsigned int), 1, m_fp);
	fread(&(pHeader->point_data_format), sizeof(unsigned char), 1, m_fp);
	fread(&(pHeader->point_data_record_length), sizeof(unsigned short), 1, m_fp);
	fread(&(pHeader->number_of_point_records), sizeof(unsigned int), 1, m_fp);
	fread(&(pHeader->number_of_points_by_return), sizeof(unsigned int), 5, m_fp);
	fread(&(pHeader->x_scale_factor), sizeof(double), 1, m_fp);
	fread(&(pHeader->y_scale_factor), sizeof(double), 1, m_fp);
	fread(&(pHeader->z_scale_factor), sizeof(double), 1, m_fp);
	fread(&(pHeader->x_offset), sizeof(double), 1, m_fp);
	fread(&(pHeader->y_offset), sizeof(double), 1, m_fp);
	fread(&(pHeader->z_offset), sizeof(double), 1, m_fp);
	fread(&(pHeader->max_x), sizeof(double), 1, m_fp);
	fread(&(pHeader->min_x), sizeof(double), 1, m_fp);
	fread(&(pHeader->max_y), sizeof(double), 1, m_fp);
	fread(&(pHeader->min_y), sizeof(double), 1, m_fp);
	fread(&(pHeader->max_z), sizeof(double), 1, m_fp);
	fread(&(pHeader->min_z), sizeof(double), 1, m_fp);

	
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
	if ((pHeader->point_data_format & 127) == 0)
	{
		if (pHeader->point_data_record_length != 20)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", pHeader->point_data_record_length);
			if (pHeader->point_data_record_length > 20)
			{
				additional_bytes_per_point = pHeader->point_data_record_length - 20;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			pHeader->point_data_record_length = 20;
		}
	}
	else if ((pHeader->point_data_format & 127) == 1)
	{
		if (pHeader->point_data_record_length != 28)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 28 for format 1\n", pHeader->point_data_record_length);
			if (pHeader->point_data_record_length > 28)
			{
				additional_bytes_per_point = pHeader->point_data_record_length - 28;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			pHeader->point_data_record_length = 28;
		}
	}
	else if ((pHeader->point_data_format & 127) == 2)
	{
		if (pHeader->point_data_record_length != 26)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 26 for format 2\n", pHeader->point_data_record_length);
			if (pHeader->point_data_record_length > 26)
			{
				additional_bytes_per_point = pHeader->point_data_record_length - 26;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			pHeader->point_data_record_length = 26;
		}
	}
	else if ((pHeader->point_data_format & 127) == 3)
	{
		if (pHeader->point_data_record_length != 34)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 34 for format 3\n", pHeader->point_data_record_length);
			if (pHeader->point_data_record_length > 34)
			{
				additional_bytes_per_point = pHeader->point_data_record_length - 34;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			pHeader->point_data_record_length = 34;
		}
	}
	else
	{
		fprintf(stderr,"WARNING: unknown point data format %d ... assuming format 0\n", pHeader->point_data_format);
		if (pHeader->point_data_record_length != 20)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", pHeader->point_data_record_length);
			if (pHeader->point_data_record_length > 20)
			{
				additional_bytes_per_point = pHeader->point_data_record_length - 20;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
		//	pHeader.point_data_record_length = 20;
		}
	//	pHeader.point_data_format = 0;
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
		
		if (!fread(pHeader->user_data_in_header, sizeof(char), pHeader->user_data_in_header_size, m_fp))
		{
			fprintf(stderr,"ERROR: reading %d bytes of data into pHeader.user_data_in_header\n", pHeader->user_data_in_header_size);
			return false;
		}
	}
	
	bool skip_all_headers=false;
	if (skip_all_headers)
	{
//		m_mappingFile.Seek(pHeader->offset_to_point_data, SEEK_CUR);
		for (i = pHeader->header_size; i < (int)pHeader->offset_to_point_data; i++) 
			fgetc(m_fp);
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
				
				if (!fread(&(pHeader->vlrs[i].reserved), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].reserved\n", i);
					return false;
				}
				if (!fread(pHeader->vlrs[i].user_id, sizeof(char), 16, m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].user_id\n", i);
					return false;
				}
				if (!fread(&(pHeader->vlrs[i].record_id), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].record_id\n", i);
					return false;
				}
				if (!fread(&(pHeader->vlrs[i].record_length_after_header), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading pHeader.vlrs[%d].record_length_after_header\n", i);
					return false;
				}
				if (!fread(pHeader->vlrs[i].description, sizeof(char), 32, m_fp))
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
					
					if (!fread(pHeader->vlrs[i].data, sizeof(char), pHeader->vlrs[i].record_length_after_header, m_fp))
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
			
			if (!fread(pHeader->user_data_after_header, sizeof(char), pHeader->user_data_after_header_size, m_fp))
			{
				fprintf(stderr,"ERROR: reading %d bytes of data into pHeader.user_data_after_header\n", pHeader->user_data_after_header_size);
				return false;
			}
		}
	}
	
	return true;
}

bool CATNReader::read_point(ATNPoint *point)
{
	if(m_pcount < m_npoints)
	{
//		m_rawpoint=(ATN_RawPoint*)m_mappingFile.ReadPtr(sizeof(ATN_RawPoint));
		fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
		
		point->x=m_rawpoint.x*m_header.x_scale_factor+m_header.x_offset;
		point->y=m_rawpoint.y*m_header.y_scale_factor+m_header.y_offset;
		point->z=m_rawpoint.z*m_header.z_scale_factor+m_header.z_offset;
		point->intensity=m_rawpoint.intensity;
		point->return_number=m_rawpoint.return_number;
		point->number_of_returns_of_given_pulse=m_rawpoint.number_of_returns_of_given_pulse;
		point->scan_direction_flag=m_rawpoint.scan_direction_flag;
		point->edge_of_flight_line=m_rawpoint.edge_of_flight_line;
		point->classification=m_rawpoint.classification;
		point->scan_angle_rank=m_rawpoint.scan_angle_rank;
		point->user_data=m_rawpoint.user_data;
		point->point_source_ID=m_rawpoint.point_source_ID;
		
		point->gpstime=m_rawpoint.gpstime;
		point->latitude=m_rawpoint.latitude*m_header.x_scale_factor+m_header.x_offset;	// (WGS84/radians - ALS50 OPTICAL CENTER)
		point->longitude=m_rawpoint.longitude*m_header.y_scale_factor+m_header.y_offset;	// (WGS84/radians - ALS50 OPTICAL CENTER)
		point->altitude=m_rawpoint.altitude*m_header.z_scale_factor+m_header.z_offset;	// (WGS84/meters - ALS50 OPTICAL CENTER)
		point->roll=m_rawpoint.roll*m_header.x_scale_factor+m_header.x_offset;		// (radians)
		point->pitch=m_rawpoint.pitch*m_header.x_scale_factor+m_header.x_offset;		// (radians)
		point->heading=m_rawpoint.heading*m_header.x_scale_factor+m_header.x_offset;		// (radians)
		point->scanAngle=m_rawpoint.scanAngle*m_header.x_scale_factor+m_header.x_offset;	//É¨Ãè½Ç
		point->range=m_rawpoint.range/Range_Scale_Factor;		// (meters)

		point->scanAngle*=2;


		//»¡¶È×ª½Ç¶È
// 		point->x=point->x*180/PI;
// 		point->y=point->y*180/PI;
// 		point->latitude=point->latitude*180/PI;
// 		point->longitude=point->longitude*180/PI;

// 		point->roll=point->roll*180/PI;
// 		point->pitch=point->pitch*180/PI;
// 		point->heading=point->heading*180/PI;
// 		point->scanAngle=point->scanAngle*180/PI*2;

		m_pcount++;
		return true;
	}
	
	return false;	
}

void CATNReader::Seek(__int64  offset)
{
	fseek(m_fp, offset, SEEK_SET);
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
// 		point->scanAngle=m_rawpoint->scanAngle*m_header.x_scale_factor+m_header.x_offset;	//É¨Ãè½Ç
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

CATNWriter::CATNWriter()
{
//	m_fp=0;
	additional_bytes_per_point=0;
	m_npoints=0;
	m_pcount=0;

	m_bCreate=false;
}

CATNWriter::~CATNWriter()
{
	close();
}

void CATNWriter::close()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp=0;
	}
//	m_mappingFile.Close();
// 	if(m_bCreate)
// 		m_mappingFile.FinishWriting();
// 	else
// 		m_mappingFile.Close();
}


bool CATNWriter::create(const char *pszName)
{
	close();
	m_fp=fopen(pszName, "wb");
	if(m_fp==0)
		return false;
	
// 	m_mappingFile.SetMappingSize(128*1024*1024);
// 	m_mappingFile.Create(pszName);

	m_bCreate=true;
	return true;
}

bool CATNWriter::open(const char *pszName)
{
	close();
// 	// 	m_fp=fopen(pszName, "wb");
// 	// 	if(m_fp==0)
// 	// 		return false;
	
// 	m_mappingFile.SetMappingSize(128*1024*1024);
// 	m_mappingFile.Open(pszName, "u");
	m_bCreate=false;
	return true;
}

bool CATNWriter::write_header(LASheader *pHeader)
{
	int i;

	fseek(m_fp, 0, SEEK_SET);

//	m_mappingFile.Seek(0, SEEK_SET);
	
	m_header.clean();
	
	// read the m_header variable after variable (to avoid alignment issues)
	
	memcpy(m_header.file_signature, pHeader->file_signature, sizeof(char)*4);
	if (!fwrite(&(m_header.file_signature), sizeof(char), 4, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.file_signature\n");
		return false;
	}
	m_header.file_source_id=pHeader->file_source_id;
	if (!fwrite(&(m_header.file_source_id), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.file_source_id\n");
		return false;
	}
	m_header.global_encoding=pHeader->global_encoding;
	if (!fwrite(&(m_header.global_encoding), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.global_encoding\n");
		return false;
	}
	m_header.project_ID_GUID_data_1=pHeader->project_ID_GUID_data_1;
	if (!fwrite(&(m_header.project_ID_GUID_data_1), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.project_ID_GUID_data_1\n");
		return false;
	}
	m_header.project_ID_GUID_data_2=pHeader->project_ID_GUID_data_2;
	if (!fwrite(&(m_header.project_ID_GUID_data_2), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.project_ID_GUID_data_2\n");
		return false;
	}
	m_header.project_ID_GUID_data_3=pHeader->project_ID_GUID_data_3;
	if (!fwrite(&(m_header.project_ID_GUID_data_3), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.project_ID_GUID_data_3\n");
		return false;
	}
	memcpy(m_header.project_ID_GUID_data_4, pHeader->project_ID_GUID_data_4, sizeof(char)*8);
	if (!fwrite(&(m_header.project_ID_GUID_data_4), sizeof(char), 8, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.project_ID_GUID_data_4\n");
		return false;
	}
	m_header.version_major=pHeader->version_major;
	if (!fwrite(&(m_header.version_major), sizeof(char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.version_major\n");
		return false;
	}
	m_header.version_minor=pHeader->version_minor;
	if (!fwrite(&(m_header.version_minor), sizeof(char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.version_minor\n");
		return false;
	}
	memcpy(m_header.system_identifier, pHeader->system_identifier, sizeof(char)*32);
	if (!fwrite(&(m_header.system_identifier), sizeof(char), 32, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.system_identifier\n");
		return false;
	}
	memcpy(m_header.generating_software, pHeader->generating_software, sizeof(char)*32);
	if (!fwrite(&(m_header.generating_software), sizeof(char), 32, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.generating_software\n");
		return false;
	}
	m_header.file_creation_day=pHeader->file_creation_day;
	if (!fwrite(&(m_header.file_creation_day), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.file_creation_day\n");
		return false;
	}
	m_header.file_creation_year=pHeader->file_creation_year;
	if (!fwrite(&(m_header.file_creation_year), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.file_creation_year\n");
		return false;
	}
	m_header.header_size=pHeader->header_size;
	if (!fwrite(&(m_header.header_size), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.header_size\n");
		return false;
	}
	m_header.offset_to_point_data=pHeader->offset_to_point_data;
	if (!fwrite(&(m_header.offset_to_point_data), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.offset_to_point_data\n");
		return false;
	}
	m_header.number_of_variable_length_records=pHeader->number_of_variable_length_records;
	if (!fwrite(&(m_header.number_of_variable_length_records), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.number_of_variable_length_records\n");
		return false;
	}
	m_header.point_data_format=pHeader->point_data_format;
	if (!fwrite(&(m_header.point_data_format), sizeof(unsigned char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.point_data_format\n");
		return false;
	}
	m_header.point_data_record_length=pHeader->point_data_record_length;
	if (!fwrite(&(m_header.point_data_record_length), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.point_data_record_length\n");
		return false;
	}
	m_header.number_of_point_records=pHeader->number_of_point_records;
	if (!fwrite(&(m_header.number_of_point_records), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.number_of_point_records\n");
		return false;
	}
	memcpy(m_header.number_of_points_by_return, pHeader->number_of_points_by_return, sizeof(unsigned int)*5);
	if (!fwrite(&(m_header.number_of_points_by_return), sizeof(unsigned int), 5, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.number_of_points_by_return\n");
		return false;
	}
	m_header.x_scale_factor=pHeader->x_scale_factor;
	if (!fwrite(&(m_header.x_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.x_scale_factor\n");
		return false;
	}
	m_header.y_scale_factor=pHeader->y_scale_factor;
	if (!fwrite(&(m_header.y_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.y_scale_factor\n");
		return false;
	}
	m_header.z_scale_factor=pHeader->z_scale_factor;
	if (!fwrite(&(m_header.z_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.z_scale_factor\n");
		return false;
	}
	m_header.x_offset=pHeader->x_offset;
	if (!fwrite(&(m_header.x_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.x_offset\n");
		return false;
	}
	m_header.y_offset=pHeader->y_offset;	
	if (!fwrite(&(m_header.y_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.y_offset\n");
		return false;
	}
	m_header.z_offset=pHeader->z_offset;
	if (!fwrite(&(m_header.z_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.z_offset\n");
		return false;
	}
	m_header.max_x=pHeader->max_x;
	if (!fwrite(&(m_header.max_x), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.max_x\n");
		return false;
	}
	m_header.min_x=pHeader->min_x;
	if (!fwrite(&(m_header.min_x), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.min_x\n");
		return false;
	}
	m_header.max_y=pHeader->max_y;
	if (!fwrite(&(m_header.max_y), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.max_y\n");
		return false;
	}
	m_header.min_y=pHeader->min_y;
	if (!fwrite(&(m_header.min_y), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.min_y\n");
		return false;
	}
	m_header.max_z=pHeader->max_z;
	if (!fwrite(&(m_header.max_z), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.max_z\n");
		return false;
	}
	m_header.min_z=pHeader->min_z;
	if (!fwrite(&(m_header.min_z), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading m_header.min_z\n");
		return false;
	}
	
	//////////////////////////////////////////////////
	/////////////////////////////////////////////////
	m_npoints=m_header.number_of_point_records;
	m_pcount=0;

	// check m_header contents
	
	if (strncmp(m_header.file_signature, "LASF", 4) != 0)
	{
		fprintf(stderr,"ERROR: wrong m_fp signature '%s'\n", m_header.file_signature);
		return false;
	}
	if ((m_header.version_major != 1) || ((m_header.version_minor != 0) && (m_header.version_minor != 1) && (m_header.version_minor != 2)))
	{
		fprintf(stderr,"WARNING: unknown version %d.%d (should be 1.0 or 1.1 or 1.2)\n", m_header.version_major, m_header.version_minor);
	}
	if (m_header.header_size < 227)
	{
		fprintf(stderr,"WARNING: m_header size is %d but should be at least 227\n", m_header.header_size);
	}
	if (m_header.offset_to_point_data < m_header.header_size)
	{
		fprintf(stderr,"ERROR: offset to point data %d is smaller than m_header size %d\n", m_header.offset_to_point_data, m_header.header_size);
		return false;
	}
	if (m_header.number_of_point_records <= 0)
	{
		fprintf(stderr,"WARNING: number of point records is %d\n", m_header.number_of_point_records);
	}
	if ((m_header.point_data_format & 127) == 0)
	{
		if (m_header.point_data_record_length != 20)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", m_header.point_data_record_length);
			if (m_header.point_data_record_length > 20)
			{
				additional_bytes_per_point = m_header.point_data_record_length - 20;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			m_header.point_data_record_length = 20;
		}
	}
	else if ((m_header.point_data_format & 127) == 1)
	{
		if (m_header.point_data_record_length != 28)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 28 for format 1\n", m_header.point_data_record_length);
			if (m_header.point_data_record_length > 28)
			{
				additional_bytes_per_point = m_header.point_data_record_length - 28;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			m_header.point_data_record_length = 28;
		}
	}
	else if ((m_header.point_data_format & 127) == 2)
	{
		if (m_header.point_data_record_length != 26)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 26 for format 2\n", m_header.point_data_record_length);
			if (m_header.point_data_record_length > 26)
			{
				additional_bytes_per_point = m_header.point_data_record_length - 26;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			m_header.point_data_record_length = 26;
		}
	}
	else if ((m_header.point_data_format & 127) == 3)
	{
		if (m_header.point_data_record_length != 34)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 34 for format 3\n", m_header.point_data_record_length);
			if (m_header.point_data_record_length > 34)
			{
				additional_bytes_per_point = m_header.point_data_record_length - 34;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
			m_header.point_data_record_length = 34;
		}
	}
	else
	{
		fprintf(stderr,"WARNING: unknown point data format %d ... assuming format 0\n", m_header.point_data_format);
		if (m_header.point_data_record_length != 20)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", m_header.point_data_record_length);
			if (m_header.point_data_record_length > 20)
			{
				additional_bytes_per_point = m_header.point_data_record_length - 20;
				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
			}
		//	m_header.point_data_record_length = 20;
		}
	//	m_header.point_data_format = 0;
	}
	if (m_header.x_scale_factor == 0 || m_header.y_scale_factor == 0 || m_header.z_scale_factor == 0)
	{
		fprintf(stderr,"WARNING: some scale factors are zero %g %g %g. will set them to 0.01.\n", m_header.x_scale_factor, m_header.y_scale_factor, m_header.z_scale_factor);
		if (m_header.x_scale_factor == 0) m_header.x_scale_factor = 0.01;
		if (m_header.y_scale_factor == 0) m_header.y_scale_factor = 0.01;
		if (m_header.z_scale_factor == 0) m_header.z_scale_factor = 0.01;
	}
	if (m_header.max_x < m_header.min_x || m_header.max_y < m_header.min_y || m_header.max_z < m_header.min_z)
	{
		fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", m_header.min_x, m_header.min_y, m_header.min_z, m_header.max_x, m_header.max_y, m_header.max_z);
	}
	
	// load any number of user-defined bytes that might have been added to the m_header
	m_header.user_data_in_header_size = m_header.header_size - 227;
	if (m_header.user_data_in_header_size)
	{
		m_header.user_data_in_header = new char[m_header.user_data_in_header_size];
		memcpy(m_header.user_data_in_header, pHeader->user_data_in_header, sizeof(char)*m_header.user_data_in_header_size);
		
		if (!fwrite(m_header.user_data_in_header, sizeof(char), m_header.user_data_in_header_size, m_fp))
		{
			fprintf(stderr,"ERROR: reading %d bytes of data into m_header.user_data_in_header\n", m_header.user_data_in_header_size);
			return false;
		}
	}
	
	bool skip_all_headers=false;
	if (skip_all_headers)
	{
		for (i = m_header.header_size; i < (int)m_header.offset_to_point_data; i++) fputc(' ', m_fp);
//		m_mappingFile.Seek(pHeader->offset_to_point_data, SEEK_CUR);
	}
	else
	{
		int vlrs_size = 0;
		
		// read the variable length records into the m_header
		
		if (m_header.number_of_variable_length_records)
		{
			m_header.vlrs = new LASvlr[m_header.number_of_variable_length_records];
			memcpy(m_header.vlrs, pHeader->vlrs, sizeof(LASvlr)*m_header.number_of_variable_length_records);
			
			for (i = 0; i < (int)m_header.number_of_variable_length_records; i++)
			{
				// make sure there are enough bytes left to read a variable length record before the point block starts
				if (((int)m_header.offset_to_point_data - vlrs_size - m_header.header_size) < 54)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block after reading %d of %d vlrs. skipping remaining vlrs ...\n", (int)m_header.offset_to_point_data - vlrs_size - m_header.header_size, i, m_header.number_of_variable_length_records);
					m_header.number_of_variable_length_records = i;
					break;
				}
				
				// read variable length records variable after variable (to avoid alignment issues)
				
				if (!fwrite(&(m_header.vlrs[i].reserved), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading m_header.vlrs[%d].reserved\n", i);
					return false;
				}
				if (!fwrite(m_header.vlrs[i].user_id, sizeof(char), 16, m_fp))
				{
					fprintf(stderr,"ERROR: reading m_header.vlrs[%d].user_id\n", i);
					return false;
				}
				if (!fwrite(&(m_header.vlrs[i].record_id), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading m_header.vlrs[%d].record_id\n", i);
					return false;
				}
				if (!fwrite(&(m_header.vlrs[i].record_length_after_header), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading m_header.vlrs[%d].record_length_after_header\n", i);
					return false;
				}
				if (!fwrite(m_header.vlrs[i].description, sizeof(char), 32, m_fp))
				{
					fprintf(stderr,"ERROR: reading m_header.vlrs[%d].description\n", i);
					return false;
				}
				
				// keep track on the number of bytes we have read so far
				
				vlrs_size += 54;
				
				// check variable length record contents
				
				if (m_header.vlrs[i].reserved != 0xAABB)
				{
					fprintf(stderr,"WARNING: wrong m_header.vlrs[%d].reserved: %d != 0xAABB\n", i, m_header.vlrs[i].reserved);
				}
				
				// make sure there are enough bytes left to read the data of the variable length record before the point block starts
				
				if (((int)m_header.offset_to_point_data - vlrs_size - m_header.header_size) < m_header.vlrs[i].record_length_after_header)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block when trying to read %d bytes into m_header.vlrs[%d].data\n", (int)m_header.offset_to_point_data - vlrs_size - m_header.header_size, m_header.vlrs[i].record_length_after_header, i);
					m_header.vlrs[i].record_length_after_header = (int)m_header.offset_to_point_data - vlrs_size - m_header.header_size;
				}
				
				// load data following the m_header of the variable length record
				
				if (m_header.vlrs[i].record_length_after_header)
				{
					m_header.vlrs[i].data = new char[m_header.vlrs[i].record_length_after_header];
					
					memcpy(m_header.vlrs[i].data, pHeader->vlrs[i].data, sizeof(char)*m_header.vlrs[i].record_length_after_header);
					if (!fwrite(m_header.vlrs[i].data, sizeof(char), m_header.vlrs[i].record_length_after_header, m_fp))
					{
						fprintf(stderr,"ERROR: reading %d bytes of data into m_header.vlrs[%d].data\n", m_header.vlrs[i].record_length_after_header, i);
						return false;
					}
				}
				else
				{
					m_header.vlrs[i].data = 0;
				}
				
				// keep track on the number of bytes we have read so far
				
				vlrs_size += m_header.vlrs[i].record_length_after_header;
				
				// special handling for known variable m_header tags
				
				if (strcmp(m_header.vlrs[i].user_id, "LASF_Projection") == 0)
				{
					if (m_header.vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
					{
						if (m_header.vlr_geo_keys)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
						}
						m_header.vlr_geo_keys = (LASvlr_geo_keys*)m_header.vlrs[i].data;
						
						// check variable m_header geo keys contents
						
						if (m_header.vlr_geo_keys->key_directory_version != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_directory_version: %d != 1\n",m_header.vlr_geo_keys->key_directory_version);
						}
						if (m_header.vlr_geo_keys->key_revision != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_revision: %d != 1\n",m_header.vlr_geo_keys->key_revision);
						}
						if (m_header.vlr_geo_keys->minor_revision != 0)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->minor_revision: %d != 0\n",m_header.vlr_geo_keys->minor_revision);
						}
						m_header.vlr_geo_key_entries = (LASvlr_key_entry*)&m_header.vlr_geo_keys[1];
					}
					else if (m_header.vlrs[i].record_id == 34736) // GeoDoubleParamsTag
					{
						if (m_header.vlr_geo_double_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParamsTag\n");
						}
						m_header.vlr_geo_double_params = (double*)m_header.vlrs[i].data;
					}
					else if (m_header.vlrs[i].record_id == 34737) // GeoAsciiParamsTag
					{
						if (m_header.vlr_geo_ascii_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
						}
						m_header.vlr_geo_ascii_params = (char*)m_header.vlrs[i].data;
					}
				}
			}
		}
	
		// load any number of user-defined bytes that might have been added after the m_header
		
		m_header.user_data_after_header_size = (int)m_header.offset_to_point_data - vlrs_size - m_header.header_size;
		if (m_header.user_data_after_header_size)
		{
			m_header.user_data_after_header = new char[m_header.user_data_after_header_size];
			memcpy(m_header.user_data_after_header, pHeader->user_data_after_header, sizeof(char)*m_header.user_data_after_header_size);
			if (!fwrite(m_header.user_data_after_header, sizeof(char), m_header.user_data_after_header_size, m_fp))
			{
				fprintf(stderr,"ERROR: reading %d bytes of data into m_header.user_data_after_header\n", m_header.user_data_after_header_size);
				return false;
			}
		}
	}
	
	return true;
}

bool CATNWriter::write_point(ATNPoint *point)
{
	if(m_pcount < m_npoints)
	{
		m_rawpoint.x=int((point->x-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.y=int((point->y-m_header.y_offset)/m_header.y_scale_factor);
		m_rawpoint.z=int((point->z-m_header.z_offset)/m_header.z_scale_factor);

		m_rawpoint.intensity=point->intensity;
		m_rawpoint.return_number=point->return_number;
		m_rawpoint.number_of_returns_of_given_pulse=point->number_of_returns_of_given_pulse;
		m_rawpoint.scan_direction_flag=point->scan_direction_flag;
		m_rawpoint.edge_of_flight_line=point->edge_of_flight_line;
		m_rawpoint.classification=point->classification;
		m_rawpoint.scan_angle_rank=point->scan_angle_rank;
		m_rawpoint.user_data=point->user_data;
		m_rawpoint.point_source_ID=point->point_source_ID;
		
		m_rawpoint.gpstime=point->gpstime;

		m_rawpoint.latitude=int((point->latitude-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.longitude=int((point->longitude-m_header.y_offset)/m_header.y_scale_factor);
		m_rawpoint.altitude=int((point->altitude-m_header.z_offset)/m_header.z_scale_factor);
		m_rawpoint.roll=int((point->roll-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.pitch=int((point->pitch-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.heading=int((point->heading-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.scanAngle=int((point->scanAngle/2.0-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.range=int(point->range*Range_Scale_Factor);
		
		
//		fwrite(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);

		fwrite(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);

		m_pcount++;
		return true;
	}
	
	return false;	
}
