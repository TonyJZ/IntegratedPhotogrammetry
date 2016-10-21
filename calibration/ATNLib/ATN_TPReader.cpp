#include "StdAfx.h"
#include "ATNLib/ATN_TPReader.h"


CATN_TPReader::CATN_TPReader()
{
	m_rawpoint=NULL;
}

CATN_TPReader::~CATN_TPReader()
{
//	close();
}

bool CATN_TPReader::read_point(ATNTP_Point *point)
{
	if(m_pcount < m_npoints)
	{
		m_rawpoint=(ATN_TP_RawPoint*)m_mappingFile.ReadPtr(sizeof(ATN_TP_RawPoint));
		//		fread(&m_rawpoint, sizeof(ATN_RawPoint), 1, m_fp);
		
		point->x=m_rawpoint->x*m_header.x_scale_factor+m_header.x_offset;
		point->y=m_rawpoint->y*m_header.y_scale_factor+m_header.y_offset;
		point->z=m_rawpoint->z*m_header.z_scale_factor+m_header.z_offset;
		point->intensity=m_rawpoint->intensity;
		point->return_number=m_rawpoint->return_number;
		point->number_of_returns_of_given_pulse=m_rawpoint->number_of_returns_of_given_pulse;
		point->scan_direction_flag=m_rawpoint->scan_direction_flag;
		point->edge_of_flight_line=m_rawpoint->edge_of_flight_line;
		point->classification=m_rawpoint->classification;
		point->scan_angle_rank=m_rawpoint->scan_angle_rank;
		point->user_data=m_rawpoint->user_data;
		point->point_source_ID=m_rawpoint->point_source_ID;
		
		point->gpstime=m_rawpoint->gpstime;
		point->Xs=m_rawpoint->Xs*m_header.x_scale_factor+m_header.x_offset;	
		point->Ys=m_rawpoint->Ys*m_header.y_scale_factor+m_header.y_offset;	
		point->Zs=m_rawpoint->Zs*m_header.z_scale_factor+m_header.z_offset;	
		point->phi=m_rawpoint->phi;		
		point->omega=m_rawpoint->omega;		
		point->kappa=m_rawpoint->kappa;		
		point->scanAngle=m_rawpoint->scanAngle;	//É¨Ãè½Ç
		point->range=m_rawpoint->range*m_header.x_scale_factor;		// (meters)
		
		m_pcount++;
		return true;
	}
	
	return false;	
}

void CATN_TPReader::reopen()
{
	m_mappingFile.Seek(m_header.offset_to_point_data, SEEK_SET);

	m_pcount=0;
}

void CATN_TPReader::Seek(__int64  offset)
{
	m_mappingFile.Seek(offset, SEEK_SET);
}


CATN_TPWriter::CATN_TPWriter()
{

}

CATN_TPWriter::~CATN_TPWriter()
{

}

bool CATN_TPWriter::write_point(ATNTP_Point *point)
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
		
		m_rawpoint.Xs=int((point->Xs-m_header.x_offset)/m_header.x_scale_factor);
		m_rawpoint.Ys=int((point->Ys-m_header.y_offset)/m_header.y_scale_factor);
		m_rawpoint.Zs=int((point->Zs-m_header.z_offset)/m_header.z_scale_factor);
		m_rawpoint.phi=point->phi;
		m_rawpoint.omega=point->omega;
		m_rawpoint.kappa=point->kappa;
		m_rawpoint.scanAngle=point->scanAngle;
		m_rawpoint.range=point->range/m_header.x_scale_factor;
		
				
		m_mappingFile.Write(&m_rawpoint, sizeof(ATN_RawPoint)*1);
		
		m_pcount++;
		return true;
	}
	
	return false;	
}

