// KeyMatch.cpp: implementation of the CKeyMatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KeyMatch.h"
#include <string.h>
#include <assert.h>
#include "TinClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


int ReadKeyFile(vector<KEYPOINT> *kpList, vector<unsigned char> *desc,  unsigned char **keys, keypt_t **info)
{
	int i, num, len;

	//std::vector<Keypoint *> kps;

	num=kpList->size();

	*keys = new unsigned char[128 * num + 8];

	if (info != NULL) 
		*info = new keypt_t[num];

	unsigned char *p = *keys;
	for (i = 0; i < num; i++) 
	{
		//double x, y, scale, ori;
		
		(*info)[i].x = (*kpList)[i].x;
		(*info)[i].y = (*kpList)[i].y;
		(*info)[i].scale = (*kpList)[i].scale;
		(*info)[i].orient = (*kpList)[i].ori;

		
		for(int j=0; j<128; j++)
		{
			*p=(*desc)[i*128+j];
			p++;
		}
		
	}

	return num; // kps;
}

void DeleteDuplicates(char *pszMatches, char *pszNoDuplicates)
{
	struct match_point
	{
		int id1, id2;
		double x1,y1,x2,y2;
	};

	FILE *fin=NULL, *fout=NULL;
	match_point *pBuf=NULL;
	int bufLen=10000;

	pBuf=new match_point[bufLen];
	
	fin=fopen(pszMatches, "r");
	if (fin == NULL) 
	{
		assert(false);
		return ;
	}

	fout=fopen(pszNoDuplicates, "w");
	if (fout == NULL) 
	{
		assert(false);
		return ;
	}

	CTINClass  *tin=new CTINClass;

	int i1, i2;	//条带号
	int num, refineNum;
	while(!feof(fin))
	{
		fscanf(fin, "%d %d\n", &i1, &i2);
		fscanf(fin, "%d\n", &num);

		if(num>bufLen)
		{
			if(pBuf)	delete[] pBuf;
			bufLen=num;
			pBuf=new match_point[bufLen];
		}

		int id1, id2;
		double x1, y1, x2, y2;
		double xs, ys, zs;
		tin->BeginAddPoints();

		int i;
		for(i=0; i<num; i++)
		{
			fscanf(fin, "%d %lf %lf %d %lf %lf\n", &id1, &x1, &y1, &id2, &x2, &y2);
			pBuf[i].id1=id1;
			pBuf[i].x1=x1;
			pBuf[i].y1=y1;
			pBuf[i].id2=id2;
			pBuf[i].x2=x2;
			pBuf[i].y2=y2;

			tin->AddPoint( x1, y1, i );
		}

		tin->FastConstruct();
		refineNum=tin->GetNumberOfPoints();

		tin->PointTraversalInit();
		triPOINT *pt=NULL;
		
		refineNum=0;
		while(pt=tin->PointTraverse())
		{
			refineNum++;
		}
		fprintf(fout, "%d %d\n", i1, i2);
		fprintf(fout, "%d\n", refineNum);

		tin->PointTraversalInit();
		while(pt=tin->PointTraverse())
		{
			i=int(pt->attr);
			fprintf(fout, "%d %lf %lf %d %lf %lf\n", pBuf[i].id1, pBuf[i].x1, pBuf[i].y1, pBuf[i].id2, pBuf[i].x2, pBuf[i].y2);
		}
		
	}

	if(fin)		fclose(fin);
	if(fout)	fclose(fout);
	if(tin)		delete tin;	tin=NULL;
	if(pBuf)	delete[] pBuf;	pBuf=NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeyMatch::CKeyMatch()
{
	m_bHasPOS=false;
	m_ratio=0.6;
}

CKeyMatch::~CKeyMatch()
{
	
}

bool CKeyMatch::ANNMatch_pairwise(char *pszKeyList, char *pszMatches, orsIProcessMsgBar *process)
{
	char *list_in;
    char *file_out;
//    double ratio;
    int i, j, k;
    
    
    list_in = pszKeyList;
//    ratio = 0.6;			//设置比例因子
    file_out = pszMatches;

    clock_t start = clock();

    unsigned char **keys;
    int *num_keys;
	keypt_t	**info;

    /* Read the list of files */
    std::vector<std::string> key_files;
    
    FILE *f = fopen(list_in, "r");
    if (f == NULL) 
	{
        printf("Error opening file %s for reading\n", list_in);
        return false;
    }

    char buf[512];
    while (fgets(buf, 512, f)) 
	{
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        
        key_files.push_back(std::string(buf));
    }

    fclose(f);

    f = fopen(file_out, "w");
    assert(f != NULL);

    int num_images = (int) key_files.size();

    keys = new unsigned char *[num_images];
	info = new keypt_t *[num_images];
    num_keys = new int[num_images];

	process->InitProgressBar("匹配特征点...", "wait...", num_images);
    /* Read all keys */
    for (i = 0; i < num_images; i++) 
	{
        keys[i] = NULL;
		info[i] = NULL;
        num_keys[i] = ReadKeyFile(key_files[i].c_str(), keys+i, info+i);
    }

    clock_t end = clock();
	printf("[KeyMatchFull] Reading keys took %0.3fs\n", 
           (end - start) / ((double) CLOCKS_PER_SEC));
    
    for (i = 0; i < num_images; i++) 
	{
        if (num_keys[i] == 0)
            continue;

        printf("[KeyMatchFull] Matching to image %d\n", i);

        start = clock();

        /* Create a tree from the keys */
        ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i]);

        for (j = 0; j < i; j++) 
		{
            if (num_keys[j] == 0)
                continue;

            /* Compute likely matches between two sets of keypoints */
            std::vector<KeypointMatch> matches = 
                MatchKeys(num_keys[j], keys[j], tree, m_ratio);
            
            int num_matches = (int) matches.size();

            if (num_matches > 0) 
			{
                /* Write the pair */
                fprintf(f, "%d %d\n", j, i);

                /* Write the number of matches */
                fprintf(f, "%d\n", (int) matches.size());

                for (k = 0; k < num_matches; k++) 
				{
                    fprintf(f, "%d %.6f %.6f %d %.6f %.6f\n", 
                            matches[k].m_idx1, info[j][matches[k].m_idx1].x, info[j][matches[k].m_idx1].y,
							matches[k].m_idx2, info[i][matches[k].m_idx2].x, info[i][matches[k].m_idx2].y);

                }
            }
        }

        end = clock();    
        printf("[KeyMatchFull] Matching took %0.3fs\n", 
               (end - start) / ((double) CLOCKS_PER_SEC));
        fflush(stdout);

        // annDeallocPts(tree->pts);
        delete tree;

		process->SetPos(i+1);
    }
    
    /* Free keypoints */
    for (i = 0; i < num_images; i++) 
	{
        if (keys[i] != NULL)
            delete [] keys[i];
    }
    delete [] keys;
    delete [] num_keys;
    
    fclose(f);
    return 0;
}


//key1与key2匹配m_x1, m_y1;  m_x2, m_y2对应1, 2中的点
std::vector<KeypointMatch>  CKeyMatch::ANNMatch_pairwise(vector<KEYPOINT> *key1, vector<unsigned char> *desc1,	
														 vector<KEYPOINT> *key2, vector<unsigned char> *desc2)
{
	char *file_out;
//	double ratio;
	int i, j, k;

	unsigned char **keys;
	int *num_keys;
	keypt_t	**info;

	int num_images = 2;

	keys = new unsigned char *[num_images];
	info = new keypt_t *[num_images];
	num_keys = new int[num_images];

	keys[0] = NULL;
	info[0] = NULL;
	num_keys[0] = ReadKeyFile(key1, desc1, keys, info);

	keys[1] = NULL;
	info[1] = NULL;
	num_keys[1] = ReadKeyFile(key2, desc2, keys+1, info+1);

	ANNkd_tree *tree = CreateSearchTree(num_keys[1], keys[1]);

	/* Compute likely matches between two sets of keypoints */
	std::vector<KeypointMatch> matches = MatchKeys(num_keys[0], keys[0], tree, m_ratio);

	int num_matches = (int) matches.size();

	for (k = 0; k < num_matches; k++) 
	{
		matches[k].m_x1=info[0][matches[k].m_idx1].x;
		matches[k].m_y1=info[0][matches[k].m_idx1].y;
		matches[k].m_x2=info[1][matches[k].m_idx2].x;
		matches[k].m_y2=info[1][matches[k].m_idx2].y;

	}

	/* Free keypoints */
	for (i = 0; i < num_images; i++) 
	{
		if (keys[i] != NULL)
			delete [] keys[i];
	}
	delete [] keys;
	delete [] num_keys;

	return  matches;
}

bool CKeyMatch::ANNMatch_pairwise_strip(vector<string> *pkeyNameList, char *pszMatches, orsIProcessMsgBar *process)
{
	char *file_out;
//	double ratio;
	int i, j, k;


//	ratio = 0.6;			//设置比例因子
	file_out = pszMatches;

//	clock_t start = clock();

	unsigned char **keys;
	int *num_keys;
	keypt_t	**info;

	/* Read the list of files */
//	std::vector<std::string> key_files;

// 	char buf[512];
// 	for(i=0; i<pkeyNameList->GetSize(); i++)
// 	{	
// 		sprintf(buf, "%s\n", pkeyNameList->GetAt(i).GetBuffer(128));
// 
// 		key_files.push_back(std::string(buf));
// 	}

	FILE *f=NULL;
	f = fopen(file_out, "w");
	assert(f != NULL);

	int num_images = pkeyNameList->size();

	keys = new unsigned char *[num_images];
	info = new keypt_t *[num_images];
	num_keys = new int[num_images];

	process->InitProgressBar("匹配特征点...", "wait...", num_images);
	/* Read all keys */
	for (i = 0; i < num_images; i++) 
	{
		keys[i] = NULL;
		info[i] = NULL;
		num_keys[i] = ReadKeyFile(pkeyNameList->at(i).c_str(), keys+i, info+i);
	}

	for (i = 1; i < num_images; i++) 
	{
		if (num_keys[i] == 0)
			continue;

		printf("[KeyMatchFull] Matching to image %d\n", i);

		
		/* Create a tree from the keys */
		ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i]);

		for (j = i-1; j < i; j++) 
		{
			if (num_keys[j] == 0)
				continue;

			/* Compute likely matches between two sets of keypoints */
			std::vector<KeypointMatch> matches = 
				MatchKeys(num_keys[j], keys[j], tree, m_ratio);

			int num_matches = (int) matches.size();

			if (num_matches >= 16/*0*/) 
			{
				/* Write the pair */
				fprintf(f, "%d %d\n", j, i);

				/* Write the number of matches */
				fprintf(f, "%d\n", (int) matches.size());

				for (k = 0; k < num_matches; k++) 
				{
					fprintf(f, "%d %.6f %.6f %d %.6f %.6f\n", 
						matches[k].m_idx1, info[j][matches[k].m_idx1].x, info[j][matches[k].m_idx1].y,
						matches[k].m_idx2, info[i][matches[k].m_idx2].x, info[i][matches[k].m_idx2].y);

				}
			}
		}

			// annDeallocPts(tree->pts);
		delete tree;

		process->SetPos(i+1);
	}

	/* Free keypoints */
	for (i = 0; i < num_images; i++) 
	{
		if (keys[i] != NULL)
			delete [] keys[i];
	}
	delete [] keys;
	delete [] num_keys;

	fclose(f);
	return 0;
}

#include "flann\flann.h"

bool CKeyMatch::flann_pairwise(char *pszKeyList, char *pszMatches, orsIProcessMsgBar *process)
{
// 	char *list_in;
//     char *file_out;
//     double ratio;
//     int i, j;
//     
//     
//     list_in = pszKeyList;
//     ratio = 0.6;			//设置比例因子
//     file_out = pszMatches;
// 
//     clock_t start = clock();
// 
//     unsigned char **keys;
//     int *num_keys;
// 
//     /* Read the list of files */
//     std::vector<std::string> key_files;
//     
//     FILE *f = fopen(list_in, "r");
//     if (f == NULL) 
// 	{
//         printf("Error opening file %s for reading\n", list_in);
//         return false;
//     }
// 
//     char buf[512];
//     while (fgets(buf, 512, f)) 
// 	{
//         /* Remove trailing newline */
//         if (buf[strlen(buf) - 1] == '\n')
//             buf[strlen(buf) - 1] = 0;
//         
//         key_files.push_back(std::string(buf));
//     }
// 
//     fclose(f);
// 
//     f = fopen(file_out, "w");
//     assert(f != NULL);
// 
//     int num_images = (int) key_files.size();
// 
//     keys = new unsigned char *[num_images];
//     num_keys = new int[num_images];
// 
// 	process->InitProgressBar("匹配特征点...", "wait...", num_images);
//     /* Read all keys */
// 	int max_keys=0;
//     for (i = 0; i < num_images; i++) 
// 	{
//         keys[i] = NULL;
//         num_keys[i] = ReadKeyFile(key_files[i].c_str(), keys+i);
// 		if(max_keys<num_keys[i])
// 			max_keys=num_keys[i];
//     }
// 
// 	float *dataset, *testset;
// 	FLANNParameters p;
// 	int tcount, datarows;
// 	int cols = 128;
// 
// 	
// 	dataset=new float[max_keys*cols];
// 	testset=new float[max_keys*cols];
// 
// 	if(num_keys[0]>num_keys[1])
// 	{
// 		dataset=new float[num_keys[0]*cols];
// 		for(i=0; i<num_keys[0]; i++)
// 		{
// 			for(j=0; j<cols; j++)
// 			{
// 				dataset[i*cols+j]=keys[0][i*cols+j];
// 			}
// 		}
// 		testset=new float[num_keys[1]*cols];
// 		for(i=0; i<num_keys[1]; i++)
// 		{
// 			for(j=0; j<cols; j++)
// 			{
// 				testset[i*cols+j]=keys[1][i*cols+j];
// 			}
// 		}
// 		datarows=num_keys[0];
// 		tcount=num_keys[1];
// 	}
// 	else
// 	{
// 		dataset=new float[num_keys[1]*cols];
// 		for(i=0; i<num_keys[1]; i++)
// 		{
// 			for(j=0; j<cols; j++)
// 			{
// 				dataset[i*cols+j]=keys[1][i*cols+j];
// 			}
// 		}
// 		testset=new float[num_keys[0]*cols];
// 		for(i=0; i<num_keys[0]; i++)
// 		{
// 			for(j=0; j<cols; j++)
// 			{
// 				testset[i*cols+j]=keys[0][i*cols+j];
// 			}
// 		}
// 		datarows=num_keys[1];
// 		tcount=num_keys[0];
// 	}
// 
// 	int nn = 2;
// 	int* result = new int[tcount*nn];
//     float* dists = new float[tcount*nn];
// 	
// 	p.log_level = LOG_INFO;
// 	p.log_destination = NULL;
// 	
//     p.algorithm = KDTREE;
//     p.checks = 32;
//     p.trees = 8;
//     p.branching = 32;
//     p.iterations = 7;
//     p.target_precision = -1;
// 	
// 	float speedup;
// 	
//     for (i = 0; i < num_images; i++) 
// 	{
//         if (num_keys[i] == 0)
//             continue;
// 		
// 		datarows=num_keys[i];
// 		for(int ii=0; i<num_keys[ii]; ii++)
// 		{
// 			for(int jj=0; jj<cols; jj++)
// 			{
// 				dataset[ii*cols+jj]=keys[i][ii*cols+jj];
// 			}
// 		}
// 		FLANN_INDEX index_id = flann_build_index(dataset, datarows, cols, &speedup, &p);
// 
//         for (j = 0; j < i; j++) 
// 		{
//             if (num_keys[j] == 0)
//                 continue;
// 
// 			tcount=num_keys[j];
// 			for(int ii=0; ii<num_keys[j]; ii++)
// 			{
// 				for(int jj=0; jj<cols; jj++)
// 				{
// 					testset[ii*cols+jj]=keys[j][ii*cols+jj];
// 				}
// 			}
// 			flann_find_nearest_neighbors_index(index_id, testset, tcount, result, dists, nn, p.checks, &p);
// 
// 			for(int ii=0; ii<tcount; ii++)
// 			{
// 				if (((double) dists[ii*nn]) < ratio * ((double) dists[ii*nn+1]))
// 				{
// 					matches.push_back(KeypointMatch(ii, result[ii*nn]));
// 				}
// 			}
// 
// 			int num_matches = (int) matches.size();
// 			
//             if (num_matches >= 16/*0*/) 
// 			{
//                 /* Write the pair */
//                 fprintf(f, "%d %d\n", j, i);
// 				
//                 /* Write the number of matches */
//                 fprintf(f, "%d\n", (int) matches.size());
// 				
//                 for (k = 0; k < num_matches; k++) 
// 				{
//                     fprintf(f, "%d %.6f %.6f %d %.6f %.6f\n", 
// 						matches[k].m_idx1, info[j][matches[k].m_idx1].x, info[j][matches[k].m_idx1].y,
// 						matches[k].m_idx2, info[i][matches[k].m_idx2].x, info[i][matches[k].m_idx2].y);
// 					
//                 }
//             }
// 		}
// 
// 		flann_free_index(index_id, &p); 
// 		process->SetPos(i+1);
// 	}
// 	
//     
//     /* Free keypoints */
//     for (i = 0; i < num_images; i++) 
// 	{
//         if (keys[i] != NULL)
//             delete [] keys[i];
//     }
//     delete [] keys;
//     delete [] num_keys;
//     
// 
// 	delete[] dataset;
// 	delete[] testset;
// 	delete[] result;
//     delete[] dists;
// 
//     fclose(f);
    return 0;
}

float Cal_CorrelationCoeficient(const BYTE *k1, const BYTE *k2, int dim)
{
	double Sxx,Sxy,Syy,Sx,Sy;
	double mn1 = 1.0/dim;
	double vx, vy;

	Sx = Sy = 0;
	Sxx = Sxy = Syy = 0;
	
	for(int i=0; i<dim; i++) 
	{
		Sx += k1[i];
		Sxx += (double)k1[i]* k1[i];
		Sxy += (double)k1[i]* k2[i];
		Syy += (double)k2[i]* k2[i];
		Sy += k2[i];
	}

	vx = Sxx-mn1*Sx*Sx;
	vy = Syy-mn1*Sy*Sy;

	float coef;
	if( vx < 1.0 ||  vy < 1.0 )
		coef = 0.01f;
	else 
		coef = (Sxy - mn1*Sx*Sy) / sqrt( vx * vy );

	return ( coef );
}

std::vector<KeypointMatch> CKeyMatch::Match_pairwise_NCC(const char *pKeyFile1, const char *pKeyFile2, 
														 double NCC_thresh, int cand)
{
//	std::vector<KeypointMatch> matches;

	unsigned char **keys;
	int *num_keys;
	keypt_t	**info;

	int num_images = 2;

	keys = new unsigned char *[num_images];
	info = new keypt_t *[num_images];
	num_keys = new int[num_images];

	keys[0] = NULL;
	info[0] = NULL;
	num_keys[0] = ReadKeyFile(pKeyFile1, keys, info);

	keys[1] = NULL;
	info[1] = NULL;
	num_keys[1] = ReadKeyFile(pKeyFile2, keys+1, info+1);
	
	//对pKeyFile2建树
	ANNkd_tree *tree = CreateSearchTree(num_keys[1], keys[1]);

	/* Compute likely matches between two sets of keypoints */
//	std::vector<KeypointMatch> matches = MatchKeys(num_keys[0], keys[0], tree, m_ratio);

	int max_pts_visit=200;
	annMaxPtsVisit(max_pts_visit);
	std::vector<KeypointMatch> matches;

	/* Now do the search */
	// clock_t start = clock();
	ANNidx *nn_idx=new ANNidx[cand];
	ANNdist *dist=new ANNdist[cand];
	unsigned char *k1, *k2;
	float	ceof;

	int i;
	for (i = 0; i < num_keys[0]; i++) 
	{
		k1 =	keys[0] + 128 * i;
		tree->annkPriSearch(k1, cand, nn_idx, dist, 0.0);

		for(int j=0; j<cand; j++)
		{
			k2=keys[1] + 128*nn_idx[j];
			
			ceof=Cal_CorrelationCoeficient(k1, k2, 128);
			if (ceof > NCC_thresh)
			{
				matches.push_back(KeypointMatch(i, nn_idx[j]));
			}
		}
		
	}
	
	int num_matches = (int) matches.size();

	for (int k = 0; k < num_matches; k++) 
	{
		matches[k].m_x1=info[0][matches[k].m_idx1].x;
		matches[k].m_y1=info[0][matches[k].m_idx1].y;
		matches[k].m_x2=info[1][matches[k].m_idx2].x;
		matches[k].m_y2=info[1][matches[k].m_idx2].y;

	}

	/* Free keypoints */
	for (i = 0; i < num_images; i++) 
	{
		if (keys[i] != NULL)
			delete [] keys[i];
		if(info[i] != NULL)
			delete [] info[i];
	}
	delete [] keys;
	delete [] info;
	delete [] num_keys;

	delete [] nn_idx;
	delete [] dist;
	return matches;
}

std::vector<KeypointMatch> CKeyMatch::Match_pairwise_ratio(const char *pKeyFile1, const char *pKeyFile2, double ratio_thresh)
{
	unsigned char **keys;
	int *num_keys;
	keypt_t	**info;

	int num_images = 2;

	keys = new unsigned char *[num_images];
	info = new keypt_t *[num_images];
	num_keys = new int[num_images];

	keys[0] = NULL;
	info[0] = NULL;
	num_keys[0] = ReadKeyFile(pKeyFile1, keys, info);

	keys[1] = NULL;
	info[1] = NULL;
	num_keys[1] = ReadKeyFile(pKeyFile2, keys+1, info+1);

	//对pKeyFile2建树
	ANNkd_tree *tree = CreateSearchTree(num_keys[1], keys[1]);

	/* Compute likely matches between two sets of keypoints */
	//	std::vector<KeypointMatch> matches = MatchKeys(num_keys[0], keys[0], tree, m_ratio);

	int max_pts_visit=200;
	annMaxPtsVisit(max_pts_visit);
	std::vector<KeypointMatch> matches;

	/* Now do the search */
	// clock_t start = clock();
	ANNidx nn_idx[2];
	ANNdist dist[2];
	unsigned char *k1, *k2;
	float	ceof;

	int i;
	for (i = 0; i < num_keys[0]; i++) 
	{
		k1 =	keys[0] + 128 * i;
		tree->annkPriSearch(k1, 2, nn_idx, dist, 0.0);

		if (((double) dist[0]) < ratio_thresh * ratio_thresh * ((double) dist[1]))
		{
			matches.push_back(KeypointMatch(i, nn_idx[0]));
		}

	}

	int num_matches = (int) matches.size();

	for (int k = 0; k < num_matches; k++) 
	{
		matches[k].m_x1=info[0][matches[k].m_idx1].x;
		matches[k].m_y1=info[0][matches[k].m_idx1].y;
		matches[k].m_x2=info[1][matches[k].m_idx2].x;
		matches[k].m_y2=info[1][matches[k].m_idx2].y;

	}

	/* Free keypoints */
	for (i = 0; i < num_images; i++) 
	{
		if (keys[i] != NULL)
			delete [] keys[i];
		if(info[i] != NULL)
			delete [] info[i];
	}
	delete [] keys;
	delete [] info;
	delete [] num_keys;

	return matches;
}