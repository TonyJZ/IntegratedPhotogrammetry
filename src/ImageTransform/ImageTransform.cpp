// ImageTransform.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "orsBase\orsUtil.h"
#include "orsImage/orsIImageService.h"
#include "orsImage\orsIImageWriter.h"

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

////////////////////////////////////////////////////////////////////
//影像转换: 将像元类型都转换成BYTE型
///////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	//printf("Hello World!\n");
	printf("Image Transform for SiftGPU!\n");
	
	if ((argc-1) < 2 || (argc-1) > 3) 
	{
		printf ("usage: ImageTransform.exe inputimage outputimage \n");
		
		printf ("image: Image file (any common format: JPEG, PNG, TIFF, ..)\n");
		printf ("outputimage: Output image with BYTE type pixel\n");
		printf ("\n");
		
		return -1;
	}

	char* input = argv[1];
	char* output = argv[2];

	ors_string errorinfo;
	g_pPlatform = orsInitialize(errorinfo, true );

	orsIPlatform*  pPlatform = getPlatform();
	
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService,pPlatform->getService(ORS_SERVICE_IMAGE) );
	
	ref_ptr<orsIImageSourceReader> imgReader;
	orsIImageWriter *imgWriter;
	ors_uint32 numBand;
//	orsDataTYPE dataType;
	orsIImageData *imgData;
	orsRect_i rect;
	orsBandSet bandSet;
	int		imgWid, imgHei;
	int i, j;
	BYTE *buf=0;
	double maxPixel, minPixel, span, dValue;
	ors_int32	NumOfPixels;
	ors_byte  resample;
	int offset;
	
	//创建一个orsIImageSourceReader对象
	orsIRegisterService *registerService = getPlatform()->getRegisterService();
	
	imgReader = ORS_CREATE_OBJECT( orsIImageSourceReader, "ors.dataSource.image.reader.gdal");
	if( imgReader == NULL)
	{
		printf("Can't create gdal reader!\n");
		goto FUNC_END;
	}
	imgWriter = ORS_CREATE_OBJECT( orsIImageWriter, "ors.dataSource.imageWriter.gdal");
	if( imgWriter == NULL)
	{
		printf("Can't create gdal writer!\n");
		goto FUNC_END;
	}
	
	if(imgReader->open(input)==false)
	{
		printf("can't open input image!\n");
		goto  FUNC_END;
	}
	


	imgWid=imgReader->getWidth();
	imgHei=imgReader->getHeight();
	
	rect=imgReader->getBoundingRect();
	rect.m_xmax+=1;
	rect.m_ymax+=1;
	
	numBand=imgReader->getNumberOfInputBands();
	for(i = 0; i < numBand; i++)
	{
		bandSet.push_back(i);
	}

	if(imgWriter->Create(output, imgReader.get(), ORS_DT_BYTE, numBand)==false)
	{
		printf("can't create output image!\n");
		goto	FUNC_END;
	}
	
	imgData=imgReader->getImageData(rect, 1.0, bandSet);
	buf=new BYTE[imgWid*imgHei];
	NumOfPixels=imgWid*imgHei;
	for(i=0; i<numBand; i++)
	{
		maxPixel=imgData->getMaxSampleValue(i);
		minPixel=imgData->getMinSampleValue(i);
		span=maxPixel-minPixel;
		for(j=0; j<NumOfPixels; j++)
		{
			dValue=imgData->getSampleValue(j, i);
			resample=((dValue-minPixel)/span)*255;
			offset=j*numBand;
			buf[offset+i]=resample;
		}
		imgWriter->WriteBandRect(i, 0, 0, imgWid, imgHei, buf);
	}

//	imgReader->close();
	imgWriter->Close();
FUNC_END:
	if(buf)
	{
		delete[] buf;
		buf=0;
	}
//	orsUninitialize();
	return 0;
}

