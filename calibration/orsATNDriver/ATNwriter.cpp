#include "StdAfx.h"
#include "ATNwriter.h"

#define  Range_Scale_Factor   160.0

CATNWriter::CATNWriter(bool bForRegister)
{
	m_fp=0;
	additional_bytes_per_point=0;
	npoints=0;
	p_count=0;

	m_ObsInfo_offset[0] = m_ObsInfo_offset[1]= m_ObsInfo_offset[2] = 0;
	m_ObsInfo_scale[0] = 1.47e-9;
	m_ObsInfo_scale[1] = 1.47e-9;
	m_ObsInfo_scale[2] = 1e-3;

	m_bCreate=false;
}

CATNWriter::~CATNWriter()
{
	close();
}

void CATNWriter::close()
{
	if( NULL == m_fp )
		return ;

	if (m_bCreate)
	{
		unsigned int number_of_point_records = (unsigned int)p_count;
		fseek(m_fp, 107, SEEK_SET);
		fwrite(&number_of_point_records, sizeof(unsigned int), 1, m_fp);
		fwrite(&number_of_point_records, sizeof(unsigned int), 1, m_fp);
		fseek(m_fp, 155, SEEK_SET);
		fwrite(&(header.x_offset), sizeof(double), 1, m_fp);
		fwrite(&(header.y_offset), sizeof(double), 1, m_fp);
		fwrite(&(header.z_offset), sizeof(double), 1, m_fp);
		// enquantize and dequantize bounding box before writing it
		if (header.max_x > header.x_offset)
			header.max_x = header.x_offset + ((int)((header.max_x-header.x_offset)/header.x_scale_factor + 0.5)) * header.x_scale_factor;
		else
			header.max_x = header.x_offset + ((int)((header.max_x-header.x_offset)/header.x_scale_factor - 0.5)) * header.x_scale_factor;
		if (header.min_x > header.x_offset)
			header.min_x = header.x_offset + ((int)((header.min_x-header.x_offset)/header.x_scale_factor + 0.5)) * header.x_scale_factor;
		else
			header.min_x = header.x_offset + ((int)((header.min_x-header.x_offset)/header.x_scale_factor - 0.5)) * header.x_scale_factor;
		if (header.max_y > header.y_offset)
			header.max_y = header.y_offset + ((int)((header.max_y-header.y_offset)/header.y_scale_factor + 0.5)) * header.y_scale_factor;
		else
			header.max_y = header.y_offset + ((int)((header.max_y-header.y_offset)/header.y_scale_factor - 0.5)) * header.y_scale_factor;
		if (header.min_y > header.y_offset)
			header.min_y = header.y_offset + ((int)((header.min_y-header.y_offset)/header.y_scale_factor + 0.5)) * header.y_scale_factor;
		else
			header.min_y = header.y_offset + ((int)((header.min_y-header.y_offset)/header.y_scale_factor - 0.5)) * header.y_scale_factor;
		if (header.max_z > header.z_offset)
			header.max_z = header.z_offset + ((int)((header.max_z-header.z_offset)/header.z_scale_factor + 0.5)) * header.z_scale_factor;
		else
			header.max_z = header.z_offset + ((int)((header.max_z-header.z_offset)/header.z_scale_factor - 0.5)) * header.z_scale_factor;
		if (header.min_z > header.z_offset)
			header.min_z = header.z_offset + ((int)((header.min_z-header.z_offset)/header.z_scale_factor + 0.5)) * header.z_scale_factor;
		else
			header.min_z = header.z_offset + ((int)((header.min_z-header.z_offset)/header.z_scale_factor - 0.5)) * header.z_scale_factor;
		fwrite(&(header.max_x), sizeof(double), 1, m_fp);
		fwrite(&(header.min_x), sizeof(double), 1, m_fp);
		fwrite(&(header.max_y), sizeof(double), 1, m_fp);
		fwrite(&(header.min_y), sizeof(double), 1, m_fp);
		fwrite(&(header.max_z), sizeof(double), 1, m_fp);
		fwrite(&(header.min_z), sizeof(double), 1, m_fp);
		fseek(m_fp, 0, SEEK_END);

		npoints = p_count;
	}
	else if (npoints && p_count != npoints) 
		fprintf(stderr,"WARNING: written %d points but expected %d points\n", p_count, npoints);

	p_count = -1;

	fclose( m_fp );
	m_fp = NULL;
}


bool CATNWriter::open(const char *lasFileName, lasDataFORMAT point_data_format, 
	double xScale, double yScale, double zScale)
{
	close();
	m_fp = fopen(lasFileName, "wb");
	if(m_fp==0)
		return false;
	
	int i;

	fseek(m_fp, 0, SEEK_SET);

	//	m_mappingFile.Seek(0, SEEK_SET);

	header.clean();

	m_bCreate=true;

	if( point_data_format !=0 )
		header.point_data_format = point_data_format;

	switch( point_data_format )	{
	case lasPOINT_ATN:
		header.point_data_record_length = 60;
		break;
	case lasPOINT_ATN_RGB:
		header.point_data_record_length = 66;
		break;
	default:
		fprintf(stderr,"WARNING: unknown point data format %d ... assuming format 0\n", header.point_data_format);
		header.point_data_format = 0;
		return false;
	}
	//	}
	//	this->header = header;

	// check header contents

	if (strncmp(header.file_signature, "LASF", 4) != 0)
	{
		fprintf(stderr,"ERROR: wrong m_file signature '%s'\n", header.file_signature);
		return false;
	}
	if ((header.version_major != 1) || ((header.version_minor != 0) && (header.version_minor != 1) && (header.version_minor != 2)))
	{
		fprintf(stderr,"WARNING: unknown version %d.%d (should be 1.0 or 1.1 or 1.2)\n", header.version_major, header.version_minor);
	}
	if (header.header_size != 227)
	{
		fprintf(stderr,"WARNING: header size is %d but should be 227\n", header.header_size);
	}
	if (header.offset_to_point_data < header.header_size)
	{
		fprintf(stderr,"ERROR: offset to point data %d is smaller than header size %d\n", header.offset_to_point_data, header.header_size);
		return false;
	}
	if (header.point_data_format == lasPOINT_ATN)
	{
		if (header.point_data_record_length != 60)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", header.point_data_record_length);
		}
	}
	else if (header.point_data_format == lasPOINT_ATN_RGB)
	{
		if (header.point_data_record_length != 66)
		{
			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 28 for format 1\n", header.point_data_record_length);
		}
	}
	else
	{
		fprintf(stderr,"WARNING: unknown point data format %d ... assuming format 0\n", header.point_data_format);
		header.point_data_format = 0;
	}
	if(xScale==0 || yScale==0 || zScale==0)
	{
		if (header.x_scale_factor == 0 || header.y_scale_factor == 0 || header.z_scale_factor == 0)
		{
			fprintf(stderr,"WARNING: some scale factors are zero %g %g %g. those are set to 0.01.\n", header.x_scale_factor, header.y_scale_factor, header.z_scale_factor);
			if (header.x_scale_factor == 0) header.x_scale_factor = 0.01;
			if (header.y_scale_factor == 0) header.y_scale_factor = 0.01;
			if (header.z_scale_factor == 0) header.z_scale_factor = 0.01;
		}
	}
	else
	{
		header.x_scale_factor = xScale;
		header.y_scale_factor = yScale;
		header.z_scale_factor = zScale;
	}
	if (header.max_x < header.min_x || header.max_y < header.min_y || header.max_z < header.min_z)
	{
		fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", header.min_x, header.min_y, header.min_z, header.max_x, header.max_y, header.max_z);
	}

	// create the right point writer in dependance on compression and point data format

// 	switch (header.point_data_format)
// 	{
// 	case 0:
// 		pointWriter = new LASpointWriter0raw(m_file);
// 		break;
// 	case 1:
// 		pointWriter = new LASpointWriter1raw(m_file);
// 		break;
// 	case 2:
// 		pointWriter = new LASpointWriter2raw(m_file);
// 		break;
// 	case 3:
// 		pointWriter = new LASpointWriter3raw(m_file);
// 		break;
// 	}

	// read the header variable after variable (to avoid alignment issues)

//	memcpy(header.file_signature, pHeader->file_signature, sizeof(char)*4);
	if (!fwrite(&(header.file_signature), sizeof(char), 4, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.file_signature\n");
		return false;
	}
//	header.file_source_id=pHeader->file_source_id;
	if (!fwrite(&(header.file_source_id), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.file_source_id\n");
		return false;
	}
//	header.global_encoding=pHeader->global_encoding;
	if (!fwrite(&(header.global_encoding), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.global_encoding\n");
		return false;
	}
//	header.project_ID_GUID_data_1=pHeader->project_ID_GUID_data_1;
	if (!fwrite(&(header.project_ID_GUID_data_1), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_1\n");
		return false;
	}
//	header.project_ID_GUID_data_2=pHeader->project_ID_GUID_data_2;
	if (!fwrite(&(header.project_ID_GUID_data_2), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_2\n");
		return false;
	}
//	header.project_ID_GUID_data_3=pHeader->project_ID_GUID_data_3;
	if (!fwrite(&(header.project_ID_GUID_data_3), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_3\n");
		return false;
	}
//	memcpy(header.project_ID_GUID_data_4, pHeader->project_ID_GUID_data_4, sizeof(char)*8);
	if (!fwrite(&(header.project_ID_GUID_data_4), sizeof(char), 8, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_4\n");
		return false;
	}
//	header.version_major=pHeader->version_major;
	if (!fwrite(&(header.version_major), sizeof(char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.version_major\n");
		return false;
	}
//	header.version_minor=pHeader->version_minor;
	if (!fwrite(&(header.version_minor), sizeof(char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.version_minor\n");
		return false;
	}
//	memcpy(header.system_identifier, pHeader->system_identifier, sizeof(char)*32);
	if (!fwrite(&(header.system_identifier), sizeof(char), 32, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.system_identifier\n");
		return false;
	}
//	memcpy(header.generating_software, pHeader->generating_software, sizeof(char)*32);
	if (!fwrite(&(header.generating_software), sizeof(char), 32, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.generating_software\n");
		return false;
	}
//	header.file_creation_day=pHeader->file_creation_day;
	if (!fwrite(&(header.file_creation_day), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.file_creation_day\n");
		return false;
	}
//	header.file_creation_year=pHeader->file_creation_year;
	if (!fwrite(&(header.file_creation_year), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.file_creation_year\n");
		return false;
	}
//	header.header_size=pHeader->header_size;
	if (!fwrite(&(header.header_size), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.header_size\n");
		return false;
	}
//	header.offset_to_point_data=pHeader->offset_to_point_data;
	if (!fwrite(&(header.offset_to_point_data), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.offset_to_point_data\n");
		return false;
	}
//	header.number_of_variable_length_records=pHeader->number_of_variable_length_records;
	if (!fwrite(&(header.number_of_variable_length_records), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.number_of_variable_length_records\n");
		return false;
	}
//	header.point_data_format=pHeader->point_data_format;
	if (!fwrite(&(header.point_data_format), sizeof(unsigned char), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.point_data_format\n");
		return false;
	}
//	header.point_data_record_length=pHeader->point_data_record_length;
	if (!fwrite(&(header.point_data_record_length), sizeof(unsigned short), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.point_data_record_length\n");
		return false;
	}
//	header.number_of_point_records=pHeader->number_of_point_records;
	if (!fwrite(&(header.number_of_point_records), sizeof(unsigned int), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.number_of_point_records\n");
		return false;
	}
//	memcpy(header.number_of_points_by_return, pHeader->number_of_points_by_return, sizeof(unsigned int)*5);
	if (!fwrite(&(header.number_of_points_by_return), sizeof(unsigned int), 5, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.number_of_points_by_return\n");
		return false;
	}
//	header.x_scale_factor=pHeader->x_scale_factor;
	if (!fwrite(&(header.x_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.x_scale_factor\n");
		return false;
	}
//	header.y_scale_factor=pHeader->y_scale_factor;
	if (!fwrite(&(header.y_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.y_scale_factor\n");
		return false;
	}
//	header.z_scale_factor=pHeader->z_scale_factor;
	if (!fwrite(&(header.z_scale_factor), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.z_scale_factor\n");
		return false;
	}
//	header.x_offset=pHeader->x_offset;
	if (!fwrite(&(header.x_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.x_offset\n");
		return false;
	}
//	header.y_offset=pHeader->y_offset;	
	if (!fwrite(&(header.y_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.y_offset\n");
		return false;
	}
//	header.z_offset=pHeader->z_offset;
	if (!fwrite(&(header.z_offset), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.z_offset\n");
		return false;
	}
//	header.max_x=pHeader->max_x;
	if (!fwrite(&(header.max_x), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.max_x\n");
		return false;
	}
//	header.min_x=pHeader->min_x;
	if (!fwrite(&(header.min_x), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.min_x\n");
		return false;
	}
//	header.max_y=pHeader->max_y;
	if (!fwrite(&(header.max_y), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.max_y\n");
		return false;
	}
//	header.min_y=pHeader->min_y;
	if (!fwrite(&(header.min_y), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.min_y\n");
		return false;
	}
//	header.max_z=pHeader->max_z;
	if (!fwrite(&(header.max_z), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.max_z\n");
		return false;
	}
//	header.min_z=pHeader->min_z;
	if (!fwrite(&(header.min_z), sizeof(double), 1, m_fp))
	{
		fprintf(stderr,"ERROR: reading header.min_z\n");
		return false;
	}

	//////////////////////////////////////////////////
	// load any number of user-defined bytes that might have been added to the header
	header.user_data_in_header_size = header.header_size - 227;
	if (header.user_data_in_header_size)
	{
		if (header.user_data_in_header == 0)
		{
			fprintf(stderr,"ERROR: there should be %d bytes of data in header->user_data_in_header\n", header.user_data_in_header_size);
			return false;
		}

		if (fwrite(header.user_data_in_header, sizeof(char), header.user_data_in_header_size, m_fp) != (unsigned int)header.user_data_in_header_size)
		{
			fprintf(stderr,"ERROR: writing %d bytes of data from header->user_data_in_header\n", header.user_data_in_header_size);
			return false;
		}
	}

	bool skip_all_headers=false;
	if (skip_all_headers)
	{
		for (i = header.header_size; i < (int)header.offset_to_point_data; i++) fputc(' ', m_fp);
		//		m_mappingFile.Seek(pHeader->offset_to_point_data, SEEK_CUR);
	}
	else
	{
		int vlrs_size = 0;

		// read the variable length records into the header

		if (header.number_of_variable_length_records)
		{
			header.vlrs = new LASvlr[header.number_of_variable_length_records];
//			memcpy(header.vlrs, pHeader->vlrs, sizeof(LASvlr)*header.number_of_variable_length_records);

			for (i = 0; i < (int)header.number_of_variable_length_records; i++)
			{
				// make sure there are enough bytes left to read a variable length record before the point block starts
				if (((int)header.offset_to_point_data - vlrs_size - header.header_size) < 54)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block after reading %d of %d vlrs. skipping remaining vlrs ...\n", (int)header.offset_to_point_data - vlrs_size - header.header_size, i, header.number_of_variable_length_records);
					header.number_of_variable_length_records = i;
					break;
				}

				// read variable length records variable after variable (to avoid alignment issues)

				if (!fwrite(&(header.vlrs[i].reserved), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading header.vlrs[%d].reserved\n", i);
					return false;
				}
				if (!fwrite(header.vlrs[i].user_id, sizeof(char), 16, m_fp))
				{
					fprintf(stderr,"ERROR: reading header.vlrs[%d].user_id\n", i);
					return false;
				}
				if (!fwrite(&(header.vlrs[i].record_id), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading header.vlrs[%d].record_id\n", i);
					return false;
				}
				if (!fwrite(&(header.vlrs[i].record_length_after_header), sizeof(unsigned short), 1, m_fp))
				{
					fprintf(stderr,"ERROR: reading header.vlrs[%d].record_length_after_header\n", i);
					return false;
				}
				if (!fwrite(header.vlrs[i].description, sizeof(char), 32, m_fp))
				{
					fprintf(stderr,"ERROR: reading header.vlrs[%d].description\n", i);
					return false;
				}

				// keep track on the number of bytes we have read so far

				vlrs_size += 54;

				// check variable length record contents

				if (header.vlrs[i].reserved != 0xAABB)
				{
					fprintf(stderr,"WARNING: wrong header.vlrs[%d].reserved: %d != 0xAABB\n", i, header.vlrs[i].reserved);
				}

				// make sure there are enough bytes left to read the data of the variable length record before the point block starts

				if (((int)header.offset_to_point_data - vlrs_size - header.header_size) < header.vlrs[i].record_length_after_header)
				{
					fprintf(stderr,"WARNING: only %d bytes until point block when trying to read %d bytes into header.vlrs[%d].data\n", (int)header.offset_to_point_data - vlrs_size - header.header_size, header.vlrs[i].record_length_after_header, i);
					header.vlrs[i].record_length_after_header = (int)header.offset_to_point_data - vlrs_size - header.header_size;
				}

				// load data following the header of the variable length record

				if (header.vlrs[i].record_length_after_header)
				{
					header.vlrs[i].data = new char[header.vlrs[i].record_length_after_header];

//					memcpy(header.vlrs[i].data, pHeader->vlrs[i].data, sizeof(char)*header.vlrs[i].record_length_after_header);
					if (!fwrite(header.vlrs[i].data, sizeof(char), header.vlrs[i].record_length_after_header, m_fp))
					{
						fprintf(stderr,"ERROR: reading %d bytes of data into header.vlrs[%d].data\n", header.vlrs[i].record_length_after_header, i);
						return false;
					}
				}
				else
				{
					header.vlrs[i].data = 0;
				}

				// keep track on the number of bytes we have read so far

				vlrs_size += header.vlrs[i].record_length_after_header;

				// special handling for known variable header tags

				if (strcmp(header.vlrs[i].user_id, "LASF_Projection") == 0)
				{
					if (header.vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
					{
						if (header.vlr_geo_keys)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
						}
						header.vlr_geo_keys = (LASvlr_geo_keys*)header.vlrs[i].data;

						// check variable header geo keys contents

						if (header.vlr_geo_keys->key_directory_version != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_directory_version: %d != 1\n",header.vlr_geo_keys->key_directory_version);
						}
						if (header.vlr_geo_keys->key_revision != 1)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_revision: %d != 1\n",header.vlr_geo_keys->key_revision);
						}
						if (header.vlr_geo_keys->minor_revision != 0)
						{
							fprintf(stderr,"WARNING: wrong vlr_geo_keys->minor_revision: %d != 0\n",header.vlr_geo_keys->minor_revision);
						}
						header.vlr_geo_key_entries = (LASvlr_key_entry*)&header.vlr_geo_keys[1];
					}
					else if (header.vlrs[i].record_id == 34736) // GeoDoubleParamsTag
					{
						if (header.vlr_geo_double_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParamsTag\n");
						}
						header.vlr_geo_double_params = (double*)header.vlrs[i].data;
					}
					else if (header.vlrs[i].record_id == 34737) // GeoAsciiParamsTag
					{
						if (header.vlr_geo_ascii_params)
						{
							fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
						}
						header.vlr_geo_ascii_params = (char*)header.vlrs[i].data;
					}
				}
			}
		}

		// load any number of user-defined bytes that might have been added after the header

		header.user_data_after_header_size = (int)header.offset_to_point_data - vlrs_size - header.header_size;
		if (header.user_data_after_header_size)
		{
			header.user_data_after_header = new char[header.user_data_after_header_size];
//			memcpy(header.user_data_after_header, pHeader->user_data_after_header, sizeof(char)*header.user_data_after_header_size);
			if (!fwrite(header.user_data_after_header, sizeof(char), header.user_data_after_header_size, m_fp))
			{
				fprintf(stderr,"ERROR: reading %d bytes of data into header.user_data_after_header\n", header.user_data_after_header_size);
				return false;
			}
		}
	}
	/////////////////////////////////////////////////
	npoints=header.number_of_point_records;
	p_count=0;

	return true;
}

// bool CATNWriter::open(const char *lasFileName, lasDataFORMAT point_data_format,  
// 	double scale[3], double offset[3])
// {
// 	memcpy(m_ObsInfo_scale, scale, sizeof(double)*3);
// 	memcpy(m_ObsInfo_offset, offset, sizeof(double)*3);
// 
// 	return open(lasFileName, point_data_format);
// }

// bool CATNWriter::write_header(LASheader *pHeader)
// {
// 	int i;
// 
// 	fseek(m_fp, 0, SEEK_SET);
// 
// //	m_mappingFile.Seek(0, SEEK_SET);
// 	
// 	header.clean();
// 	
// 	// read the header variable after variable (to avoid alignment issues)
// 	
// 	memcpy(header.file_signature, pHeader->file_signature, sizeof(char)*4);
// 	if (!fwrite(&(header.file_signature), sizeof(char), 4, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.file_signature\n");
// 		return false;
// 	}
// 	header.file_source_id=pHeader->file_source_id;
// 	if (!fwrite(&(header.file_source_id), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.file_source_id\n");
// 		return false;
// 	}
// 	header.global_encoding=pHeader->global_encoding;
// 	if (!fwrite(&(header.global_encoding), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.global_encoding\n");
// 		return false;
// 	}
// 	header.project_ID_GUID_data_1=pHeader->project_ID_GUID_data_1;
// 	if (!fwrite(&(header.project_ID_GUID_data_1), sizeof(unsigned int), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_1\n");
// 		return false;
// 	}
// 	header.project_ID_GUID_data_2=pHeader->project_ID_GUID_data_2;
// 	if (!fwrite(&(header.project_ID_GUID_data_2), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_2\n");
// 		return false;
// 	}
// 	header.project_ID_GUID_data_3=pHeader->project_ID_GUID_data_3;
// 	if (!fwrite(&(header.project_ID_GUID_data_3), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_3\n");
// 		return false;
// 	}
// 	memcpy(header.project_ID_GUID_data_4, pHeader->project_ID_GUID_data_4, sizeof(char)*8);
// 	if (!fwrite(&(header.project_ID_GUID_data_4), sizeof(char), 8, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_4\n");
// 		return false;
// 	}
// 	header.version_major=pHeader->version_major;
// 	if (!fwrite(&(header.version_major), sizeof(char), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.version_major\n");
// 		return false;
// 	}
// 	header.version_minor=pHeader->version_minor;
// 	if (!fwrite(&(header.version_minor), sizeof(char), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.version_minor\n");
// 		return false;
// 	}
// 	memcpy(header.system_identifier, pHeader->system_identifier, sizeof(char)*32);
// 	if (!fwrite(&(header.system_identifier), sizeof(char), 32, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.system_identifier\n");
// 		return false;
// 	}
// 	memcpy(header.generating_software, pHeader->generating_software, sizeof(char)*32);
// 	if (!fwrite(&(header.generating_software), sizeof(char), 32, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.generating_software\n");
// 		return false;
// 	}
// 	header.file_creation_day=pHeader->file_creation_day;
// 	if (!fwrite(&(header.file_creation_day), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.file_creation_day\n");
// 		return false;
// 	}
// 	header.file_creation_year=pHeader->file_creation_year;
// 	if (!fwrite(&(header.file_creation_year), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.file_creation_year\n");
// 		return false;
// 	}
// 	header.header_size=pHeader->header_size;
// 	if (!fwrite(&(header.header_size), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.header_size\n");
// 		return false;
// 	}
// 	header.offset_to_point_data=pHeader->offset_to_point_data;
// 	if (!fwrite(&(header.offset_to_point_data), sizeof(unsigned int), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.offset_to_point_data\n");
// 		return false;
// 	}
// 	header.number_of_variable_length_records=pHeader->number_of_variable_length_records;
// 	if (!fwrite(&(header.number_of_variable_length_records), sizeof(unsigned int), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.number_of_variable_length_records\n");
// 		return false;
// 	}
// 	header.point_data_format=pHeader->point_data_format;
// 	if (!fwrite(&(header.point_data_format), sizeof(unsigned char), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.point_data_format\n");
// 		return false;
// 	}
// 	header.point_data_record_length=pHeader->point_data_record_length;
// 	if (!fwrite(&(header.point_data_record_length), sizeof(unsigned short), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.point_data_record_length\n");
// 		return false;
// 	}
// 	header.number_of_point_records=pHeader->number_of_point_records;
// 	if (!fwrite(&(header.number_of_point_records), sizeof(unsigned int), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.number_of_point_records\n");
// 		return false;
// 	}
// 	memcpy(header.number_of_points_by_return, pHeader->number_of_points_by_return, sizeof(unsigned int)*5);
// 	if (!fwrite(&(header.number_of_points_by_return), sizeof(unsigned int), 5, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.number_of_points_by_return\n");
// 		return false;
// 	}
// 	header.x_scale_factor=pHeader->x_scale_factor;
// 	if (!fwrite(&(header.x_scale_factor), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.x_scale_factor\n");
// 		return false;
// 	}
// 	header.y_scale_factor=pHeader->y_scale_factor;
// 	if (!fwrite(&(header.y_scale_factor), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.y_scale_factor\n");
// 		return false;
// 	}
// 	header.z_scale_factor=pHeader->z_scale_factor;
// 	if (!fwrite(&(header.z_scale_factor), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.z_scale_factor\n");
// 		return false;
// 	}
// 	header.x_offset=pHeader->x_offset;
// 	if (!fwrite(&(header.x_offset), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.x_offset\n");
// 		return false;
// 	}
// 	header.y_offset=pHeader->y_offset;	
// 	if (!fwrite(&(header.y_offset), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.y_offset\n");
// 		return false;
// 	}
// 	header.z_offset=pHeader->z_offset;
// 	if (!fwrite(&(header.z_offset), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.z_offset\n");
// 		return false;
// 	}
// 	header.max_x=pHeader->max_x;
// 	if (!fwrite(&(header.max_x), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.max_x\n");
// 		return false;
// 	}
// 	header.min_x=pHeader->min_x;
// 	if (!fwrite(&(header.min_x), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.min_x\n");
// 		return false;
// 	}
// 	header.max_y=pHeader->max_y;
// 	if (!fwrite(&(header.max_y), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.max_y\n");
// 		return false;
// 	}
// 	header.min_y=pHeader->min_y;
// 	if (!fwrite(&(header.min_y), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.min_y\n");
// 		return false;
// 	}
// 	header.max_z=pHeader->max_z;
// 	if (!fwrite(&(header.max_z), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.max_z\n");
// 		return false;
// 	}
// 	header.min_z=pHeader->min_z;
// 	if (!fwrite(&(header.min_z), sizeof(double), 1, m_fp))
// 	{
// 		fprintf(stderr,"ERROR: reading header.min_z\n");
// 		return false;
// 	}
// 	
// 	//////////////////////////////////////////////////
// 	/////////////////////////////////////////////////
// 	m_npoints=header.number_of_point_records;
// 	m_pcount=0;
// 
// 	// check header contents
// 	
// 	if (strncmp(header.file_signature, "LASF", 4) != 0)
// 	{
// 		fprintf(stderr,"ERROR: wrong m_fp signature '%s'\n", header.file_signature);
// 		return false;
// 	}
// 	if ((header.version_major != 1) || ((header.version_minor != 0) && (header.version_minor != 1) && (header.version_minor != 2)))
// 	{
// 		fprintf(stderr,"WARNING: unknown version %d.%d (should be 1.0 or 1.1 or 1.2)\n", header.version_major, header.version_minor);
// 	}
// 	if (header.header_size < 227)
// 	{
// 		fprintf(stderr,"WARNING: header size is %d but should be at least 227\n", header.header_size);
// 	}
// 	if (header.offset_to_point_data < header.header_size)
// 	{
// 		fprintf(stderr,"ERROR: offset to point data %d is smaller than header size %d\n", header.offset_to_point_data, header.header_size);
// 		return false;
// 	}
// 	if (header.number_of_point_records <= 0)
// 	{
// 		fprintf(stderr,"WARNING: number of point records is %d\n", header.number_of_point_records);
// 	}
// 	if ((header.point_data_format & 127) == 0)
// 	{
// 		if (header.point_data_record_length != 20)
// 		{
// 			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", header.point_data_record_length);
// 			if (header.point_data_record_length > 20)
// 			{
// 				additional_bytes_per_point = header.point_data_record_length - 20;
// 				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
// 			}
// 			header.point_data_record_length = 20;
// 		}
// 	}
// 	else if ((header.point_data_format & 127) == 1)
// 	{
// 		if (header.point_data_record_length != 28)
// 		{
// 			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 28 for format 1\n", header.point_data_record_length);
// 			if (header.point_data_record_length > 28)
// 			{
// 				additional_bytes_per_point = header.point_data_record_length - 28;
// 				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
// 			}
// 			header.point_data_record_length = 28;
// 		}
// 	}
// 	else if ((header.point_data_format & 127) == 2)
// 	{
// 		if (header.point_data_record_length != 26)
// 		{
// 			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 26 for format 2\n", header.point_data_record_length);
// 			if (header.point_data_record_length > 26)
// 			{
// 				additional_bytes_per_point = header.point_data_record_length - 26;
// 				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
// 			}
// 			header.point_data_record_length = 26;
// 		}
// 	}
// 	else if ((header.point_data_format & 127) == 3)
// 	{
// 		if (header.point_data_record_length != 34)
// 		{
// 			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 34 for format 3\n", header.point_data_record_length);
// 			if (header.point_data_record_length > 34)
// 			{
// 				additional_bytes_per_point = header.point_data_record_length - 34;
// 				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
// 			}
// 			header.point_data_record_length = 34;
// 		}
// 	}
// 	else
// 	{
// 		fprintf(stderr,"WARNING: unknown point data format %d ... assuming format 0\n", header.point_data_format);
// 		if (header.point_data_record_length != 20)
// 		{
// 			fprintf(stderr,"WARNING: wrong point data record length of %d instead of 20 for format 0\n", header.point_data_record_length);
// 			if (header.point_data_record_length > 20)
// 			{
// 				additional_bytes_per_point = header.point_data_record_length - 20;
// 				fprintf(stderr,"WARNING: skipping the %d additional bytes per point\n", additional_bytes_per_point);
// 			}
// 		//	header.point_data_record_length = 20;
// 		}
// 	//	header.point_data_format = 0;
// 	}
// 	if (header.x_scale_factor == 0 || header.y_scale_factor == 0 || header.z_scale_factor == 0)
// 	{
// 		fprintf(stderr,"WARNING: some scale factors are zero %g %g %g. will set them to 0.01.\n", header.x_scale_factor, header.y_scale_factor, header.z_scale_factor);
// 		if (header.x_scale_factor == 0) header.x_scale_factor = 0.01;
// 		if (header.y_scale_factor == 0) header.y_scale_factor = 0.01;
// 		if (header.z_scale_factor == 0) header.z_scale_factor = 0.01;
// 	}
// 	if (header.max_x < header.min_x || header.max_y < header.min_y || header.max_z < header.min_z)
// 	{
// 		fprintf(stderr,"WARNING: invalid bounding box [ %g %g %g / %g %g %g ]\n", header.min_x, header.min_y, header.min_z, header.max_x, header.max_y, header.max_z);
// 	}
// 	
// 	// load any number of user-defined bytes that might have been added to the header
// 	header.user_data_in_header_size = header.header_size - 227;
// 	if (header.user_data_in_header_size)
// 	{
// 		header.user_data_in_header = new char[header.user_data_in_header_size];
// 		memcpy(header.user_data_in_header, pHeader->user_data_in_header, sizeof(char)*header.user_data_in_header_size);
// 		
// 		if (!fwrite(header.user_data_in_header, sizeof(char), header.user_data_in_header_size, m_fp))
// 		{
// 			fprintf(stderr,"ERROR: reading %d bytes of data into header.user_data_in_header\n", header.user_data_in_header_size);
// 			return false;
// 		}
// 	}
// 	
// 	bool skip_all_headers=false;
// 	if (skip_all_headers)
// 	{
// 		for (i = header.header_size; i < (int)header.offset_to_point_data; i++) fputc(' ', m_fp);
// //		m_mappingFile.Seek(pHeader->offset_to_point_data, SEEK_CUR);
// 	}
// 	else
// 	{
// 		int vlrs_size = 0;
// 		
// 		// read the variable length records into the header
// 		
// 		if (header.number_of_variable_length_records)
// 		{
// 			header.vlrs = new LASvlr[header.number_of_variable_length_records];
// 			memcpy(header.vlrs, pHeader->vlrs, sizeof(LASvlr)*header.number_of_variable_length_records);
// 			
// 			for (i = 0; i < (int)header.number_of_variable_length_records; i++)
// 			{
// 				// make sure there are enough bytes left to read a variable length record before the point block starts
// 				if (((int)header.offset_to_point_data - vlrs_size - header.header_size) < 54)
// 				{
// 					fprintf(stderr,"WARNING: only %d bytes until point block after reading %d of %d vlrs. skipping remaining vlrs ...\n", (int)header.offset_to_point_data - vlrs_size - header.header_size, i, header.number_of_variable_length_records);
// 					header.number_of_variable_length_records = i;
// 					break;
// 				}
// 				
// 				// read variable length records variable after variable (to avoid alignment issues)
// 				
// 				if (!fwrite(&(header.vlrs[i].reserved), sizeof(unsigned short), 1, m_fp))
// 				{
// 					fprintf(stderr,"ERROR: reading header.vlrs[%d].reserved\n", i);
// 					return false;
// 				}
// 				if (!fwrite(header.vlrs[i].user_id, sizeof(char), 16, m_fp))
// 				{
// 					fprintf(stderr,"ERROR: reading header.vlrs[%d].user_id\n", i);
// 					return false;
// 				}
// 				if (!fwrite(&(header.vlrs[i].record_id), sizeof(unsigned short), 1, m_fp))
// 				{
// 					fprintf(stderr,"ERROR: reading header.vlrs[%d].record_id\n", i);
// 					return false;
// 				}
// 				if (!fwrite(&(header.vlrs[i].record_length_after_header), sizeof(unsigned short), 1, m_fp))
// 				{
// 					fprintf(stderr,"ERROR: reading header.vlrs[%d].record_length_after_header\n", i);
// 					return false;
// 				}
// 				if (!fwrite(header.vlrs[i].description, sizeof(char), 32, m_fp))
// 				{
// 					fprintf(stderr,"ERROR: reading header.vlrs[%d].description\n", i);
// 					return false;
// 				}
// 				
// 				// keep track on the number of bytes we have read so far
// 				
// 				vlrs_size += 54;
// 				
// 				// check variable length record contents
// 				
// 				if (header.vlrs[i].reserved != 0xAABB)
// 				{
// 					fprintf(stderr,"WARNING: wrong header.vlrs[%d].reserved: %d != 0xAABB\n", i, header.vlrs[i].reserved);
// 				}
// 				
// 				// make sure there are enough bytes left to read the data of the variable length record before the point block starts
// 				
// 				if (((int)header.offset_to_point_data - vlrs_size - header.header_size) < header.vlrs[i].record_length_after_header)
// 				{
// 					fprintf(stderr,"WARNING: only %d bytes until point block when trying to read %d bytes into header.vlrs[%d].data\n", (int)header.offset_to_point_data - vlrs_size - header.header_size, header.vlrs[i].record_length_after_header, i);
// 					header.vlrs[i].record_length_after_header = (int)header.offset_to_point_data - vlrs_size - header.header_size;
// 				}
// 				
// 				// load data following the header of the variable length record
// 				
// 				if (header.vlrs[i].record_length_after_header)
// 				{
// 					header.vlrs[i].data = new char[header.vlrs[i].record_length_after_header];
// 					
// 					memcpy(header.vlrs[i].data, pHeader->vlrs[i].data, sizeof(char)*header.vlrs[i].record_length_after_header);
// 					if (!fwrite(header.vlrs[i].data, sizeof(char), header.vlrs[i].record_length_after_header, m_fp))
// 					{
// 						fprintf(stderr,"ERROR: reading %d bytes of data into header.vlrs[%d].data\n", header.vlrs[i].record_length_after_header, i);
// 						return false;
// 					}
// 				}
// 				else
// 				{
// 					header.vlrs[i].data = 0;
// 				}
// 				
// 				// keep track on the number of bytes we have read so far
// 				
// 				vlrs_size += header.vlrs[i].record_length_after_header;
// 				
// 				// special handling for known variable header tags
// 				
// 				if (strcmp(header.vlrs[i].user_id, "LASF_Projection") == 0)
// 				{
// 					if (header.vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
// 					{
// 						if (header.vlr_geo_keys)
// 						{
// 							fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
// 						}
// 						header.vlr_geo_keys = (LASvlr_geo_keys*)header.vlrs[i].data;
// 						
// 						// check variable header geo keys contents
// 						
// 						if (header.vlr_geo_keys->key_directory_version != 1)
// 						{
// 							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_directory_version: %d != 1\n",header.vlr_geo_keys->key_directory_version);
// 						}
// 						if (header.vlr_geo_keys->key_revision != 1)
// 						{
// 							fprintf(stderr,"WARNING: wrong vlr_geo_keys->key_revision: %d != 1\n",header.vlr_geo_keys->key_revision);
// 						}
// 						if (header.vlr_geo_keys->minor_revision != 0)
// 						{
// 							fprintf(stderr,"WARNING: wrong vlr_geo_keys->minor_revision: %d != 0\n",header.vlr_geo_keys->minor_revision);
// 						}
// 						header.vlr_geo_key_entries = (LASvlr_key_entry*)&header.vlr_geo_keys[1];
// 					}
// 					else if (header.vlrs[i].record_id == 34736) // GeoDoubleParamsTag
// 					{
// 						if (header.vlr_geo_double_params)
// 						{
// 							fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParamsTag\n");
// 						}
// 						header.vlr_geo_double_params = (double*)header.vlrs[i].data;
// 					}
// 					else if (header.vlrs[i].record_id == 34737) // GeoAsciiParamsTag
// 					{
// 						if (header.vlr_geo_ascii_params)
// 						{
// 							fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
// 						}
// 						header.vlr_geo_ascii_params = (char*)header.vlrs[i].data;
// 					}
// 				}
// 			}
// 		}
// 	
// 		// load any number of user-defined bytes that might have been added after the header
// 		
// 		header.user_data_after_header_size = (int)header.offset_to_point_data - vlrs_size - header.header_size;
// 		if (header.user_data_after_header_size)
// 		{
// 			header.user_data_after_header = new char[header.user_data_after_header_size];
// 			memcpy(header.user_data_after_header, pHeader->user_data_after_header, sizeof(char)*header.user_data_after_header_size);
// 			if (!fwrite(header.user_data_after_header, sizeof(char), header.user_data_after_header_size, m_fp))
// 			{
// 				fprintf(stderr,"ERROR: reading %d bytes of data into header.user_data_after_header\n", header.user_data_after_header_size);
// 				return false;
// 			}
// 		}
// 	}
// 	
// 	return true;
// }


bool CATNWriter::write_point(double X, double Y, double Z)
{
	ATN_RawPoint point;

	if (m_bCreate)
		ModifyHeader(X,Y,Z);

	SetXyz( &point, X, Y, Z);

	return write_point( &point, 0 );
}

bool CATNWriter::write_point(double* coordinates, unsigned short intensity, unsigned short* rgb /* = NULL */ )
{
	ATN_RawPoint point;

	if (m_bCreate)
		ModifyHeader(coordinates[0],coordinates[1],coordinates[2]);

	SetXyz( &point, coordinates[0],coordinates[1],coordinates[2]);
	point.intensity = intensity;
		
	return write_point(&point, rgb);
}

bool CATNWriter::write_point(double* coordinates, unsigned short intensity,
	unsigned char return_number, unsigned char number_of_returns_of_given_pulse,
	unsigned char scan_direction_flag, unsigned char edge_of_flight_line,
	unsigned char classification, 
	char scan_angle_rank, unsigned char user_data,
	unsigned short point_source_ID, double gps_time, unsigned short* rgb)
{
	ATN_RawPoint point;

	if (m_bCreate)
		ModifyHeader(coordinates[0],coordinates[1],coordinates[2]);

	SetXyz( &point, coordinates[0],coordinates[1],coordinates[2]);

	point.intensity = intensity;
	point.return_number = return_number;
	point.number_of_returns_of_given_pulse = number_of_returns_of_given_pulse;
	point.scan_direction_flag = scan_direction_flag;
	point.edge_of_flight_line = edge_of_flight_line;
	point.classification = classification;
	point.scan_angle_rank = scan_angle_rank;
	point.user_data = user_data;
	point.point_source_ID = point_source_ID;

	point.gpstime = gps_time;
	
	return write_point(&point, rgb);
}

bool CATNWriter::write_point(double* coordinates, unsigned short intensity,
	unsigned char return_number, unsigned char number_of_returns_of_given_pulse,
	unsigned char scan_direction_flag, unsigned char edge_of_flight_line,
	unsigned char classification, 
	char scan_angle_rank, unsigned char user_data,
	unsigned short point_source_ID, double gps_time, orsPointObservedInfo *info,
	unsigned short* rgb)
{
	ATN_RawPoint point;

	if (m_bCreate)
		ModifyHeader(coordinates[0],coordinates[1],coordinates[2]);

	SetXyz( &point, coordinates[0],coordinates[1],coordinates[2]);

	point.intensity = intensity;
	point.return_number = return_number;
	point.number_of_returns_of_given_pulse = number_of_returns_of_given_pulse;
	point.scan_direction_flag = scan_direction_flag;
	point.edge_of_flight_line = edge_of_flight_line;
	point.classification = classification;
	point.scan_angle_rank = scan_angle_rank;
	point.user_data = user_data;
	point.point_source_ID = point_source_ID;

	point.gpstime = gps_time;

	point.latitude=int((info->pos.lat-header.y_offset)/header.y_scale_factor);
	point.longitude=int((info->pos.lon-header.x_offset)/header.x_scale_factor);
	point.altitude=int((info->pos.h-header.z_offset)/header.z_scale_factor);

	point.roll=int((info->roll-m_ObsInfo_offset[0])/m_ObsInfo_scale[0]);
	point.pitch=int((info->pitch-m_ObsInfo_offset[0])/m_ObsInfo_scale[0]);
	point.heading=int((info->heading-m_ObsInfo_offset[0])/m_ObsInfo_scale[0]);
	point.scanAngle=int((info->scanAngle/2.0-m_ObsInfo_offset[0])/m_ObsInfo_scale[0]);
	point.range=int(info->range*Range_Scale_Factor);

	return write_point(&point, rgb);
}

// bool CATNWriter::write_point(void* pData)
// {
// 	ATNPoint *point = (ATNPoint*)pData;
// 
// 	if(m_pcount < m_npoints)
// 	{
// 		m_rawpoint.x=int((point->x-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.y=int((point->y-header.y_offset)/header.y_scale_factor);
// 		m_rawpoint.z=int((point->z-header.z_offset)/header.z_scale_factor);
// 
// 		m_rawpoint.intensity=point->intensity;
// 		m_rawpoint.return_number=point->return_number;
// 		m_rawpoint.number_of_returns_of_given_pulse=point->number_of_returns_of_given_pulse;
// 		m_rawpoint.scan_direction_flag=point->scan_direction_flag;
// 		m_rawpoint.edge_of_flight_line=point->edge_of_flight_line;
// 		m_rawpoint.classification=point->classification;
// 		m_rawpoint.scan_angle_rank=point->scan_angle_rank;
// 		m_rawpoint.user_data=point->user_data;
// 		m_rawpoint.point_source_ID=point->point_source_ID;
// 		
// 		m_rawpoint.gpstime=point->gpstime;
// 
// 		m_rawpoint.latitude=int((point->latitude-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.longitude=int((point->longitude-header.y_offset)/header.y_scale_factor);
// 		m_rawpoint.altitude=int((point->altitude-header.z_offset)/header.z_scale_factor);
// 		m_rawpoint.roll=int((point->roll-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.pitch=int((point->pitch-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.heading=int((point->heading-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.scanAngle=int((point->scanAngle/2.0-header.x_offset)/header.x_scale_factor);
// 		m_rawpoint.range=int(point->range*Range_Scale_Factor);
// 		
// 		
// //		fwrite(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
// 
// 		fwrite(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
// 
// 		m_pcount++;
// 		return true;
// 	}
// 	
// 	return false;	
// }

bool CATNWriter::write_point(ATN_RawPoint *point, unsigned short* rgb)
{
	p_count++;

	fwrite(point, sizeof(ATN_RawPoint), 1, m_fp);

	if(header.point_data_format == lasPOINT_ATN_RGB)
	{
		fwrite(rgb, sizeof(unsigned short), 3, m_fp);
	}
	
	return true;
}

void CATNWriter::ModifyHeader( double x, double y, double z )
{
	if (p_count == 0) // then no header was provided
	{
		header.x_offset = ((int)x);
		header.y_offset = ((int)y);
		header.z_offset = ((int)z);
		header.min_x = header.max_x = x;
		header.min_y = header.max_y = y;
		header.min_z = header.max_z = z;
	}
	else
	{
		if (x < header.min_x) header.min_x = x;
		else if (x > header.max_x) header.max_x = x;
		if (y < header.min_y) header.min_y = y;
		else if (y > header.max_y) header.max_y = y;
		if (z < header.min_z) header.min_z = z;
		else if (z > header.max_z) header.max_z = z;
	}
}


void CATNWriter::SetXyz( ATN_RawPoint *point, double x, double y, double z )
{
	if (x > header.x_offset)
		point->x = (int)((x-header.x_offset)/header.x_scale_factor + 0.5);
	else
		point->x = (int)((x-header.x_offset)/header.x_scale_factor - 0.5);

	if (y > header.y_offset)
		point->y = (int)((y-header.y_offset)/header.y_scale_factor + 0.5);
	else
		point->y = (int)((y-header.y_offset)/header.y_scale_factor - 0.5);

	if (z > header.z_offset)
		point->z = (int)((z-header.z_offset)/header.z_scale_factor + 0.5);
	else
		point->z = (int)((z-header.z_offset)/header.z_scale_factor - 0.5);
}
