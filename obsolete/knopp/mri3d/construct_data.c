#include "utils.h"
#include "nfft.h"

/**
 * nfft makes an 2d-nfft for every slice
 */
void nfft (char * file, int N, int M, int Z, fftw_complex *mem)
{
  int j,l;                /* some variables */
  double tmp;             /* a placeholder */
  nfft_plan my_plan;      /* plan for the two dimensional nfft  */
  FILE* fp;
    
  /* initialise my_plan */
  nfft_init_2d(&my_plan,N,N,M);

  fp=fopen("nodes.dat","r");
  
  for(j=0;j<my_plan.M;j++)
  {
    fscanf(fp,"%le %le ",&my_plan.x[2*j+0],&my_plan.x[2*j+1]);
  }
  fclose(fp);

  fp=fopen(file,"w");

  for(l=0;l<Z;l++) {
    tmp = (double) l;
  
    for(j=0;j<N*N;j++)
    {
      my_plan.f_hat[j][0] = mem[(l*N*N+N*N*Z/2+j)%(N*N*Z)][0];
      my_plan.f_hat[j][1] = mem[(l*N*N+N*N*Z/2+j)%(N*N*Z)][1];
    }
    
    if(my_plan.nfft_flags & PRE_PSI)
      nfft_precompute_psi(&my_plan);

    nfft_trafo(&my_plan);

    for(j=0;j<my_plan.M;j++)
    {
      fprintf(fp,"%le %le %le %le %le\n",my_plan.x[2*j+0],my_plan.x[2*j+1],tmp/Z-0.5,my_plan.f[j][0],my_plan.f[j][1]);
    }
  }
  fclose(fp);

  nfft_finalize(&my_plan);
}

/**
 * fft makes an 1D-ftt for every node through
 * all layers
 */
void fft(int N,int M,int Z, fftw_complex *mem)
{
  fftw_plan plan;
  plan = fftw_plan_many_dft(1, &Z, N*N,
                                  mem, NULL,
                                  N*N, 1,
                                  mem, NULL,
                                  N*N,1 ,
                                  FFTW_FORWARD, FFTW_ESTIMATE);

  fftw_execute(plan); /* execute the fft */
  fftw_destroy_plan(plan);
}

/**
 * read fills the memory with the file input_data_f.dat as
 * the real part of f and with zeros for the imag part of f
 */
void read(int N,int M,int Z, fftw_complex *mem)
{
  int i;
  FILE* fin;
  fin=fopen("input_f.dat","r");


  for(i=0;i<Z*N*N;i++) {
    fscanf(fin,"%le ",&mem[i][0] );
    mem[i][1] = 0.0;
  }

  fclose(fin);
}

int main(int argc, char **argv)
{ 
  fftw_complex *mem;
  
  if (argc <= 4) {
    printf("usage: ./construct_data FILENAME N M Z\n");
    return 1;
  }

  mem = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * atoi(argv[2]) * atoi(argv[2]) * atoi(argv[4]));
  
  read(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]), mem);

  fft(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]), mem);
 
  nfft(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]), mem);
  
  fftw_free(mem);
  
  return 1;
}
