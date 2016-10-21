#ifndef _IPH_MATRIX_H_ZJ_20110225_
#define _IPH_MATRIX_H_ZJ_20110225_ 

int Gauss(double *A,double *b,int n);

//æÿ’Û«ÛƒÊ◊”≥Ã–Ú
int invers_matrix(double *m1,int n);
//æÿ’Ûœ‡≥À
void Matrixmul(double a[],double b[],int m,int n,int k,double c[]);


static int  findlm(double *A,int i,int n,double *maxx);
static void exlij(double *A,double *b,int li,int lj,int n);
static void eliminate(double *A,double *b,int i,int n);
/******+****************** gauss.c *************************************
function :      solves linear equations with gaussian elimination

A x=b
*************************************************************************/
int Gauss(double *A,double *b,int n)
{
	int i,j,lm;
	double maxx;

	/*	first step:  eliminatio of row main element */

	for (i=0; i<n-1; i++)
	{
		lm=findlm(A,i,n,&maxx);
		if ( maxx <0.001 )	return(0);
		if( lm )
			exlij(A,b,i,lm,n);

		eliminate(A,b,i,n);
		A += n+1;	b++;
	}

	/*	second step:  backward solution */

	*b /= *A;
	for(i=1; i<n; i++)
	{
		A -= n+1;	b--;
		for (j=1; j<i+1; j++)
			*b -= A[j] * b[j];
		*b /= *A;
	}
	return(1);
}


void eliminate(double *A,double *b,int ii,int n)
{
	int i,j;
	double fac,b0,*line,*row;

	line = A + n;
	b0 = *b++;
	for( i=ii+1; i<n; i++)
	{
		row = A+1;
		fac = *line++ / *A;
		for( j = ii + 1; j<n; j++)
			*line++ -= *row++ * fac;
		line += ii;
		*b++ -= b0 * fac;
	}
}


/************************** findlm.c *************************************
function :	find line main element

input	 :      *A,ic,n;

output	 :	A,b
*************************************************************************/
int findlm(double *A,int ic,int n,double *maxx)
{
	int i,im=ic;

	*maxx = fabs(*A);
	if( *maxx > 1 ) return 0;
	for ( i=ic+1; i<n; i++)
	{
		if( fabs(*A) > *maxx ) {
			*maxx = fabs(*A);
			im=i;
		}
		A += n;
	}

	if (im == ic )	return(0);
	else		return(im);
}


/*************************** exlij.c *********************************
function :	exchange line i ang j of matrix A
input	 :	*A-----matrix
li
lj
n  -----number of rows
**********************************************************************/
void exlij(double *A,double *b,int li,int lj,int n)
{
	int i,j;
	double t,*l;

	i = lj-li;
	l = A + i * n;
	for ( j = li; j<n; j++)
		t = *A,	*A++ = *l, *l++ = t;
	t = *b,	*b = b[i];	b[i] = t;
}

//æÿ’Û«ÛƒÊ◊”≥Ã–Ú
int invers_matrix(double *m1,int n)
{ 
	int *is,*js;
	int i,j,k,l,u,v;
	double temp,max_v;
	is=(int *)malloc(n*sizeof(int));
	js=(int *)malloc(n*sizeof(int));
	if(is==NULL||js==NULL)
	{
		printf("out of memory!\n");
		return(0);
	}

	for(k=0;k<n;k++)
	{
		max_v=0.0;
		for(i=k;i<n;i++)
			for(j=k;j<n;j++)
			{
				temp=fabs(m1[i*n+j]);
				if(temp>max_v)
				{
					max_v=temp; is[k]=i; js[k]=j;
				}
			}
			if(max_v==0.0)
			{
				free(is); free(js);
				printf("invers is not availble!\n");
				return(0);
			}

			if(is[k]!=k)
				for(j=0;j<n;j++)
				{
					u=k*n+j; v=is[k]*n+j;
					temp=m1[u]; m1[u]=m1[v]; m1[v]=temp;
				}
				if(js[k]!=k)
					for(i=0;i<n;i++)
					{
						u=i*n+k; v=i*n+js[k];
						temp=m1[u]; m1[u]=m1[v]; m1[v]=temp;
					}

					l=k*n+k;
					m1[l]=1.0/m1[l];
					for(j=0;j<n;j++)
						if(j!=k)
						{
							u=k*n+j;
							m1[u]*=m1[l];
						}

						for(i=0;i<n;i++)
							if(i!=k)
								for(j=0;j<n;j++)
									if(j!=k)
									{
										u=i*n+j;
										m1[u]-=m1[i*n+k]*m1[k*n+j];
									}

									for(i=0;i<n;i++)
										if(i!=k)
										{	
											u=i*n+k;
											m1[u]*=-m1[l];
										}
	}
	for(k=n-1;k>=0;k--)
	{
		if(js[k]!=k)
			for(j=0;j<n;j++)
			{
				u=k*n+j; v=js[k]*n+j;
				temp=m1[u]; m1[u]=m1[v]; m1[v]=temp;
			}
			if(is[k]!=k)
				for(i=0;i<n;i++)
				{
					u=i*n+k; v=i*n+is[k];
					temp=m1[u]; m1[u]=m1[v]; m1[v]=temp;
				}
	}
	free(is); free(js);
	return(1);
}


//æÿ’Ûœ‡≥À
void Matrixmul(double a[],double b[],int m,int n,int k,double c[])
{ 
	int i,j,l,u;
	for (i=0; i<=m-1; i++)
	{
		for (j=0; j<=k-1; j++)
		{
			u=i*k+j; c[u]=0.0;
			for (l=0; l<=n-1; l++)
				c[u]=c[u]+a[i*n+l]*b[l*k+j];
		}
	}
	return;
}



#endif