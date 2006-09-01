#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "nfft3.h"

void simple_test_infft_1d(int N, int M, int iter)
{
  int j,k,l;                            /**< index for nodes, freqencies,iter*/
  nfft_plan p;                          /**< plan for the nfft               */
  infft_plan ip;                        /**< plan for the inverse nfft       */
 
  /** initialise an one dimensional plan */
  nfft_init_1d(&p, N, M);

  /** initialise inverse plan */
  infft_init(&ip,&p);

  /** init pseudo random nodes */
  vrand_shifted_unit_double(p.x,p.M_total);
  
  /** precompute psi, the entries of the matrix B */
  if(p.nfft_flags & PRE_ONE_PSI)
    nfft_precompute_one_psi(&p);

  /** init pseudo random samples and show them */
  vrand_unit_complex(ip.y,p.M_total);
  vpr_complex(ip.y,p.M_total,"Given data, vector y");

  /** initialise some guess f_hat_0 and solve */
  for(k=0;k<p.N_total;k++)
      ip.f_hat_iter[k]=0; 

  vpr_complex(ip.f_hat_iter,p.N_total,"Initial guess, vector f_hat_iter");

  SWAP_complex(ip.f_hat_iter,p.f_hat);
  nfft_trafo(&p);
  vpr_complex(p.f,p.M_total,"Data fit, vector f");
  SWAP_complex(ip.f_hat_iter,p.f_hat);

  infft_before_loop(&ip);
  printf("\n Residual r=%e\n",ip.dot_r_iter);

  for(l=0;l<iter;l++)
    {
      printf("\n********** Iteration l=%d **********\n",l);
      infft_loop_one_step(&ip);
      vpr_complex(ip.f_hat_iter,p.N_total,
		  "Approximate solution, vector f_hat_iter");
      
      SWAP_complex(ip.f_hat_iter,p.f_hat);
      nfft_trafo(&p);
      vpr_complex(p.f,p.M_total,"Data fit, vector f");
      SWAP_complex(ip.f_hat_iter,p.f_hat);

      printf("\n Residual r=%e\n",ip.dot_r_iter);
    }
  
  infft_finalize(&ip);  
  nfft_finalize(&p);  
}

int main()
{
  printf("\n1) Computing a one dimensional inverse nfft\n");
  simple_test_infft_1d(8,4,5);

  return 1;
}
