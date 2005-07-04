/* 
   inverse - Inverse test for the NFSFT

   Copyright (C) 2005 Jens Keiner

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

#include <termios.h>
#include <grp.h>
#include <pwd.h>
*/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#else
#  error Need config.h
#endif

#ifdef STDC_HEADERS
#  include <stdlib.h>
#  include <stdio.h>
#  include <math.h>
#else
#  error Need ANSI-C headers
#endif

/* Auxilliary headers */
#include <complex.h>
#include <time.h>
#include <fftw3.h>
#include "nfsft.h"
#include "util.h"
#include "../../nfft/utils.h"
#include "api.h"
#include "infsft.h"

#define M_DEFAULT 4

/**
 * The main program.
 *
 * \param argc The number of arguments
 * \param argv An array containing the arguments as C-strings
 *
 * \return Exit code 
 */
int main (int argc, char **argv)
{  
  /** The current bandwidth */
  int M;
  /** Next greater power of two with respect to M */
  int N;
  /** Maximum number of nodes */
  int D;
  /** Loop counter for Legendre index k. */
  int k;
  /** Loop counter for Legendre index n. */
  int n;
  /** Loop counter for nodes. */
  int d;  
  int l;
  
  /** Arrays for complex Fourier coefficients. */
  complex **f_hat;
  /** Copy the original Fourier coefficients. */
  complex **f_hat_orig;
  
  /** Array of angles defining the nodes. */
  double *angles;
  /** Array for angles \f$\theta\f$ of a grid. */
  double *theta;
  /** Array for angles \f$\phi\f$ of a grid. */
  double* phi;
  /** Array for Clenshaw-Curtis weights. */
  double *w;  
  /** Array for Clenshaw-Curtis weights. */
  double *wf;  
  /** Array for function values. */
  complex *f;
  /** Array for function values. */
  complex *f_orig;
  double err_infty, err_1, err_2;
  
  /** Plan for fast spherical fourier transform. */
  nfsft_plan plan;
  infsft_plan iplan;
  
  /** Used to measure computation time. */
  double ctime;
  /** 
    * Used to store the filename of a file containing Gauss-Legendre nodes and 
    * weights.
    */ 
  char filename[100];
  unsigned short seed[3]={1,2,3};
  /** File handle for reading quadrature nodes and weights. */
  FILE *file;
  /** FFTW plan for Clenshaw-Curtis quadrature */
  fftw_plan fplan;
  
  if (argc == 1)
  {
    M = M_DEFAULT;
  }
  else if (argc == 2)
  {
    sscanf(argv[1],"%d",&M);
  }
  else
  {
    fprintf(stderr,"Inverse - Inverse test for NFSFT\n");
    fprintf(stderr,"Usage: inverse M\n");
    return -1;
  }  
  
  N = 1<<ngpt(M);
  D = (2*M+1)*(2*M+2);
  
  /* Initialize random number generator. */
  //srand48(time(NULL));
    
  /* Allocate memory. */
  f_hat = (complex**) malloc((2*M+1)*sizeof(complex*));
  f_hat_orig = (complex**) malloc((2*M+1)*sizeof(complex*));
  for (n = -M; n <= M; n++)
  {
    f_hat[n+M] = (complex*) calloc((N+1),sizeof(complex));
    f_hat_orig[n+M] = (complex*) calloc((N+1),sizeof(complex));
  }  
  
  f = (complex*) malloc(D*sizeof(complex));
  f_orig = (complex*) malloc(D*sizeof(complex));
  angles = (double*) malloc(2*D*sizeof(double));
  theta = (double*) malloc((2*M+1)*sizeof(double));
  phi = (double*) malloc((2*M+2)*sizeof(double));
  w = (double*) malloc((2*M+1)*sizeof(double));
  wf = (double*) malloc(D*sizeof(double));
    
  printf("Precomputing wisdom up to M = %d ...",M);
  fflush(stdout);
  ctime = second();
  nfsft_precompute(M,2000,0U);
  printf(" needed %7.2f secs.\n",second()-ctime);
          
  /* Compute random Fourier coefficients. */
  for (n = -M; n <= M; n++)
  {
    for (k = abs(n); k <= M; k++)
    {
      f_hat[n+M][k] = /*(k <= 0)?1.0:0.0;*/drand48();// + I*drand48();
    }
    /* Save a copy. */
    memcpy(f_hat_orig[n+M],f_hat[n+M],(M+1)*sizeof(complex));
  }
        
  /* Compute Clenshaw-Curtis nodes and weights. */  
  for (k = 0; k < 2*M+2; k++)
  {
    phi[k] = k/((double)2*M+2) - 0.5;
  }
  for (n = 0; n < 2*M+1; n++)
  {
    theta[n] = n/((double)4*M);
  }
  fplan = fftw_plan_r2r_1d(M+1, w, w, FFTW_REDFT00, 0U);
  for (n = 0; n < M+1; n++)
  {
    w[n] = -2.0/(4*n*n-1);
  }  
  fftw_execute(fplan);
  w[0] *= 0.5;
  for (n = 0; n < M+1; n++)
  {
    w[n] *= 1/((double)2*M);
    w[2*M-n] = w[n];
  }  
  fftw_destroy_plan(fplan);

  /* Create grid nodes. */
  d = 0;
  for (n = 0; n < 2*M+1; n++)
  {
    for (k = 0; k < 2*M+2; k++)
    {
      angles[2*d] = phi[k];
      angles[2*d+1] = theta[n];
      wf[d] = w[n]; 
      d++;
    }  
  }
  
  //vpr(wf,D,"wf");
    
  plan = nfsft_init(M, D, f_hat, angles, f, NFSFT_NORMALIZED);
  nfsft_trafo(plan);
  
  /* Save a copy. */
  memcpy(f_orig,f,D*sizeof(complex));

  //vpr_c(f_orig,D,"f_orig:");
  //vpr_c_hat(f_hat_orig,M,"f_hat_orig:");

  /*for (n = -M; n <= M; n++)
  {
    for (k = abs(n); k <= M; k++)
    {
      f_hat[n+M][k] = 0.0;
    }
  }*/

  iplan = infsft_make_plan();
  infsft_init_guru(iplan, plan, NFSFT_CGNR_E | NFSFT_PRECOMPUTE_WEIGHT);
  
  for (d = 0; d < D; d++)
  {
    iplan->given_f[d] = f_orig[d];
    iplan->w[d] = wf[d]/(2*M+2);
  }
  
  for (n = -M; n <= M; n++)
  {
    for (k = abs(n); k <= M; k++)
    {
      iplan->f_hat_iter[n+M][k] = 0.0;
    }
  }
  //vpr_c_hat(iplan->f_hat_iter,M,"f_hat_iter");
  
  //fprintf(stderr,"here\n");
  //fflush(stderr);
  
  /* inverse trafo */  
  infsft_before_loop(iplan);
  fprintf(stderr,"%3d: %e,\n",0,sqrt(iplan->dot_r_iter));
  for(l=0;l<10;l++)
  { 
    infsft_loop_one_step(iplan);
    fprintf(stderr,"%3d: %e,\n",l+1,sqrt(iplan->dot_r_iter));
  }
    
  infsft_finalize(iplan);  
  nfsft_finalize(plan);  
  
  nfsft_forget();
  for (n = -M; n <= M; n++)
  {
    free(f_hat[n+M]);
    free(f_hat_orig[n+M]);
  }  
  free(f_hat);
  free(f_hat_orig);
  
  free(w);
  free(wf);
  free(f_orig);
  free(angles);
  free(theta);
  free(phi);
  free(f);
  return EXIT_SUCCESS;
}
