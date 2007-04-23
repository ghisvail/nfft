/* $Id$
 *
 * Copyright (c) 2007 Jens Keiner, Stefan Kunis, Daniel Potts
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*****************************************************************************/
/* options.h                                                                 */
/* options for the direct and fast computation of the NDFT                   */
/*                                                                           */
/* authors: D. Potts                                                         */
/*          S. Kunis 2002-2006                                               */
/*****************************************************************************/

/** window function is KAISER_BESSEL, SINC_POWER, B_SPLINE, or GAUSSIAN
 */
#if !defined(SINC_POWER) && !defined(B_SPLINE) && !defined(GAUSSIAN)
#define KAISER_BESSEL
#endif

/** timing
 */
/*#define MEASURE_TIME*/
                                        /* measure time for the deconvolution
                                           and convolution/evaluation step   */
/*#define MEASURE_TIME_FFTW*/
                                        /* the same for the fftw step        */


/** Timing, method works since the inaccurate timer is updated mostly in the
 *  measured function. For small times not every call of the measured function
 *  will also produce a 'unit' time step.
 *  Measuring the fftw might cause a wrong output vector due to the repeated
 *  ffts.
 */
#ifdef MEASURE_TIME
 int MEASURE_TIME_r;
 double MEASURE_TIME_tt;

#define TIC(a)                                                                \
  ths->MEASURE_TIME_t[(a)]=0;                                                 \
  MEASURE_TIME_r=0;                                                           \
  while(ths->MEASURE_TIME_t[(a)]<0.01)                                        \
    {                                                                         \
      MEASURE_TIME_r++;                                                       \
      MEASURE_TIME_tt=nfft_second();                                               \

/* THE MEASURED FUNCTION IS CALLED REPEATEDLY */

#define TOC(a)                                                                \
      MEASURE_TIME_tt=nfft_second()-MEASURE_TIME_tt;                               \
      ths->MEASURE_TIME_t[(a)]+=MEASURE_TIME_tt;                              \
    }                                                                         \
  ths->MEASURE_TIME_t[(a)]/=MEASURE_TIME_r;                                   \

#else
#define TIC(a)
#define TOC(a)
#endif

#ifdef MEASURE_TIME_FFTW
#define TIC_FFTW(a) TIC(a)
#define TOC_FFTW(a) TOC(a)
#else
#define TIC_FFTW(a)
#define TOC_FFTW(a)
#endif

/* options.h */
