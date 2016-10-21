// POS2aop.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "orsBase/orsUtil.h"
#include "orsBase/orsString.h"
#include "orsBase/orsIPlatform.h"
#include "orsGuiBase/orsIProcessMsgBar.h"
#include <vector>
#include "math.h"
#include "Geometry.h"
#include "matrix.h"

void usage(bool wait=false)
{
	printf("POS2aop V1.0: IDP @Liesmars \n");
	printf("usage:\n");
	printf("POS2aop -i pos.txt -o output_directory \n");
	printf("------------------------------------\n");
	if (wait)
	{
		printf("<press ENTER>\n");
		getc(stdin);
	}
	exit(1);
}

static void byebye(bool wait=false)
{
	if (wait)
	{
		fprintf(stderr,"<press ENTER>\n");
		getc(stdin);
	}
	exit(1);
}

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

typedef struct
{
	orsString  imgName;				//image name without folder name
	double Xs, Ys, Zs;
	double phi, omega, kappa;		//radian
	double R[9];
}exor_element;

/* allow a comment starting with '#' to end-of-line.  
 * Skip white space including any comments. */
void skip_comments(FILE *fp)
{
	int ch;

	fscanf(fp," ");      /* Skip white space. */
	while ((ch = fgetc(fp)) == '#') {
		while ((ch = fgetc(fp)) != '\n'  &&  ch != EOF)
			;
		fscanf(fp," ");
	}
	ungetc(ch, fp);      /* Replace last character read. */
}

int main(int argc, char *argv[])
{
	int i, j, k;
	double step=1;
	char* pOutputDir=NULL;
	char* pInput=NULL;
	
// 	double killTh=100;
// 	int attType=0;
// 	int gridType=0;
// 	bool merge=false;
// 	char* pLasList=NULL;

	if (strcmp(argv[1],"-h") == 0)
	{
		usage();
	}

	for (i = 1; i < argc; i++)
	{
		if(strcmp(argv[i],"-i") == 0)
		{
			i++;
			pInput=argv[i];
		}
		else if(strcmp(argv[i],"-o") == 0)
		{
			i++;
			pOutputDir=argv[i];
		}
		//the following param can be used to define the format of POS file
		/*
		else if (strcmp(argv[i],"-step") == 0)
				{
					i++;
					step = atof(argv[i]);
				}
				else if(strcmp(argv[i],"-kill") == 0)
				{
					i++;
					killTh=atof(argv[i]);
				}
				else if(strcmp(argv[i],"-height") == 0)
				{
					attType=0;
				}
				else if(strcmp(argv[i],"-intensity") == 0)
				{
					attType=1;
				}
				else if(strcmp(argv[i],"-DSM") == 0)
				{
					gridType=0;
				}
				else if(strcmp(argv[i],"-DEM") == 0)
				{
					gridType=1;
				}
				else if(strcmp(argv[i],"-merge") == 0)
				{
					merge=1;
				}*/
		
	}


	printf("Starting OpenRS Platform ... ");
	ors_string errorinfo;
	g_pPlatform = orsInitialize(errorinfo/*, true*/);

	if(g_pPlatform)
		printf("Succeed!\n");
	else
	{
		printf("Fail!\n");
		return 0;
	}

	ref_ptr<orsIProcessMsg> processMsg;
	processMsg = ORS_CREATE_OBJECT( orsIProcessMsg, ORS_PROCESSMSG_CMD );

	FILE *fin=NULL, *fout=NULL;
	std::vector<exor_element>	exList;
	exor_element    element;
	double xs, ys, zs, phi, omega, kappa, focal;
	double	deg2rad=PI/180;
	char pLine[256];
	double R0[9], R1[9];
	double R0_tran[9];

	fin=fopen(pInput, "rt");
	if(fin==NULL)
	{
		getPlatform()->logPrint( ORS_LOG_ERROR, "can't open %s", pInput );
		goto FLAG;
	}

//	RotateMat_fwk(0,0,PI/2, R0);
//	matrix_transpose(3, 3, R0, R0_tran);

	skip_comments(fin);
	int num;
//	fscanf(fin, "%d\n", &num);

	int gpstime;
	while(feof(fin)==0)
	{
		fscanf(fin, "%d  %lf %lf %lf %lf %lf %lf %d\n", /*pLine*/&gpstime,  &omega,&phi, &kappa, &xs, &ys, &zs, &focal /*, &gpstime*/);	//dpgrid空三结果格式
		
		element.Xs=xs;
		element.Ys=ys;
		element.Zs=zs;
		itoa(gpstime, pLine, 10);
		element.imgName=pLine;

//		RotateMat_wfk(omega*deg2rad, phi*deg2rad, kappa*deg2rad, element.R);
		
//		R2fwk(element.R, &(element.phi), &(element.omega), &(element.kappa));

		element.phi=phi*deg2rad;
		element.omega=omega*deg2rad;
		element.kappa=kappa*deg2rad;

		RotateMat_fwk(element.phi, element.omega, element.kappa, element.R);

//		RotateMat_wfk(element.omega, element.phi, element.kappa, element.R);

		exList.push_back(element);
	}

	fclose(fin);	
	fin=NULL;

	for(i=0; i<exList.size(); i++)
	{
		processMsg->process( (float)i/exList.size() );

		element=exList[i];
		orsString imgName=element.imgName;

		int pos=imgName.reverseFind('.');
		if(pos>0)
			imgName=imgName.left(pos);

		orsString aopName=pOutputDir;
		aopName+="\\";
		aopName+=imgName;
		aopName+=".aop.txt";	

		fout=fopen(aopName, "wt");
		if(fout==0)
			continue;

		fprintf(fout,  "[Xs,Ys,Zs]\n");
		fprintf(fout,  "%13.3lf %13.3lf %10.3lf\n\n", element.Xs, element.Ys, element.Zs );

		fprintf(fout,  "[Rotation Angle]\n");
		fprintf(fout,  "%13.9lf %13.9lf %13.9lf\n\n", element.phi, element.omega, element.kappa );
		fprintf(fout,  "[Rotation Matrix]\n"  );
		for( k=0; k<3; k++) 
		{
			for( j=0; j<3; j++)
			{
				fprintf(fout,  "%10.7lf ", element.R[k*3+j] );
			}
			fprintf(fout, "\n");
		}

		fprintf(fout,  "\n[Elevation Range]\n"  );
		fprintf(fout,  "%d %d %d\n", 0, 50, 100);

		fclose(fout);
	}

	fprintf(stderr,"==========>  ok!  <===========\n");

FLAG:
	byebye(argc==1);
	orsUninitialize();
	return 0;
}

