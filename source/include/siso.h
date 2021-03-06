/* File siso.h
   
   Description: General functions used to implement SISO decoding.   

   Copyright (C) 2005-2006 Matthew C. Valenti

   Last updated on Mar. 14, 2006

   Function siso are part of the Iterative Solutions 
   Coded Modulation Library. The Iterative Solutions Coded Modulation 
   Library is free software; you can redistribute it and/or modify it 
   under the terms of the GNU Lesser General Public License as published 
   by the Free Software Foundation; either version 2.1 of the License, 
   or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
  
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

/* Note: Need to also include convolutional.h and maxstar.h */

/* Function siso()

  Description: Uses the (max)-log-MAP algorithm to perform soft-input, soft-output
  decoding of a convolutional code.

	Input parameters:
		out0[]		The output bits for each state if input is a 0 (generated by rsc_transit).
		state0[]	The next state if input is a 0 (generated by rsc_transit).
		out1[]		The output bits for each state if input is a 1 (generated by rsc_transit).
		state1[]	The next state if input is a 1 (generated by rsc_transit).
		r[]			The received signal in LLR-form. Must be in form r = 2*a*y/(sigma^2).
		z_in[]		The APP input.  This is the extrinsic output from the other decoder.
		KK			The constraint length of the convolutional code.
		LL			The number of data bits.
		DecOpts		Decoder termination option = 0 for unterminated and = 1 for terminated.
		DecoderType	    = 0 Linear approximation to log-MAP correction function.
						= 1 For max-log-MAP algorithm (i.e. max*(x,y) = max(x,y) )
						= 2 For Constant-log-MAP algorithm
						= 3 For log-MAP, correction factor using piecewise linear approximation
						= 4 For log-MAP, correction factor uses C function calls

	Output parameters:
		llr[]		The log-likelihood ratio of each data bit.
		z_out[]		The extrinsic information of each data bit.
	
  This function is used by turbo_decode()   
  
*/

static void siso(
				float  output_u[],
				float  output_c[],
				int    out0[], 
				int    state0[], 
				int    out1[], 
				int    state1[],
				float  input_u[],
				float  input_c[],
				int    KK,
				int    nn,
				int    LL,
				int	   DecoderType
				)
{
	int mm;			/* Memory of the RSC encoder. */
	int max_states;	/* Number of states in the RSC encoder. */
	int i, k, state;/* Counting variables. */
	float num;	/* Normalization values. */
	float delta1, delta2;	/* Arguments passed to max_star operation. */
	float num_llr_u, den_llr_u;	/* Temporary variable used to compute LLR. */
	float *num_llr_c, *den_llr_c; /* Temp variable for LLR of code bits */
	int symbol0, symbol1;	/* Symbols associated with data 0 and data 1 */
	int  number_symbols; /* number of symbols */
	float *metric_c;	/* Set of all possible branch metrics */
	float *beta;		/* Reverse trellis metrics for the entire trellis. */
	float *alpha;		/* Forward trellis metric for the current stage only. */
	float *alpha_prime;	/* Forward trellis metrics for the last stage only. */
	float *rec_array;   /* Received values for one trellis section */
	float sys_in, par_in, app_in;
	int mask;
	
	float (*max_star[])(float, float) =
	{ 
		max_star0, max_star1, max_star2, max_star3, max_star4
	};

	/* derived constants */
	mm = KK-1;
	max_states = 1 << mm;			/* 2^mm */
	number_symbols = 1 << nn;	    /* 2^nn */
	
	/* initialize internal arrays */
	alpha = (float*)calloc( max_states, sizeof(float) );
	alpha_prime = (float*)calloc( max_states, sizeof(float) );
	beta = (float*)calloc( max_states*(LL+KK), sizeof(float) );
	/* following has been changed on 3-14-06 */
	/* metric_c = (int*)calloc( number_symbols, sizeof(int) ); */
	metric_c = (float*)calloc( number_symbols, sizeof(float) );
	rec_array = (float*)calloc( nn, sizeof(float) );
	num_llr_c = (float*)calloc( nn, sizeof(float) );
	den_llr_c = (float*)calloc( nn, sizeof(float) );

	/* initialize alphas */
	for (state=1;state<max_states;state++)
		alpha_prime[state] = -MAXLOG;
	alpha_prime[0] = 0;
	
	/* initialize betas */
	for (state=1;state<max_states;state++) {
			beta[(LL+mm)*max_states+state] = -MAXLOG;
		}
	
	/* compute betas */
	for (k=LL+mm-1;k>=0;k--) {	
		if (k<LL)
			app_in = input_u[k];
		else
			app_in = 0;

		/* precompute all possible branch metrics */
		for (i=0;i<nn;i++)
			rec_array[i] = input_c[nn*k+i];

		for (i=0;i<number_symbols;i++)
			metric_c[i] = Gamma( rec_array, i, nn );

		/* step through all states */
		for ( state=0; state< max_states; state++ ) {		
			/* data 0 branch */
			delta1 = beta[ (k+1)*max_states + state0[ state ]] + metric_c[ out0[ state ] ];
			
			/* data 1 branch */
			delta2 = beta[ (k+1)*max_states + state1[ state ]] + metric_c[ out1[ state ] ] + app_in;
			
			/* update beta */
			beta[ k*max_states + state] = ( *max_star[DecoderType] )(delta1, delta2);							
		}	

		/* normalize */
		for (state=1;state<max_states;state++)
			beta[k*max_states+state] = beta[k*max_states+state] - beta[k*max_states];
		beta[k*max_states] = 0;
	}
	
	/* compute alphas and LLRs */
	for (k=1;k<=LL+mm;k++) {	
		/* den_llr and num_llr are used to compute the LLR and extrinsic info */
		den_llr_u = -MAXLOG;
		num_llr_u = -MAXLOG;	

		/* intialize alpha */
		for (state=0;state<max_states;state++) 
			alpha[state] = -MAXLOG;

		/* assign inputs */
		if (k-1 < LL)
			app_in = input_u[k-1];
		else
			app_in = 0;

		/* precompute all possible branch metrics */
		for (i=0;i<nn;i++) {
			den_llr_c[i] = -MAXLOG;
			num_llr_c[i] = -MAXLOG;
			rec_array[i] = input_c[nn*(k-1)+i];
		}

		for (i=0;i<number_symbols;i++)
			metric_c[i] = Gamma( rec_array, i, nn ); 
		
		/* step through all states and find alpha */
		for ( state=0; state<max_states; state++ ) {		
			/* Data 0 branch */
			delta1 = alpha_prime[ state ] + metric_c[ out0[ state ] ];
			delta2 = alpha[ state0[state] ];
			alpha[ state0[state] ] = ( *max_star[DecoderType] )(delta1, delta2);

			/* Data 1 branch */
			delta1 = alpha_prime[ state ] + metric_c[ out1[ state ] ] + app_in;
			delta2 = alpha[ state1[state] ];
			alpha[ state1[state] ] = ( *max_star[DecoderType] )(delta1, delta2);		
		}

		/* compute the LLRs */	
		for (state=0;state<max_states;state++)  {
			symbol0 = out0[state];
			symbol1 = out1[state];
			
			/* data 0 branch (departing) */
			delta1 = alpha_prime[state] + metric_c[ symbol0 ] + beta[k*max_states+state0[state]];

			/* the information bit */
			delta2 = den_llr_u;
			den_llr_u = ( *max_star[DecoderType] )( delta1, delta2 );

			mask = 1<<(nn-1);
			/* go through all the code bits */
			for (i=0;i<nn;i++) {
				if ( symbol0&mask ) {
					/* this code bit 1 */
					delta2 = num_llr_c[i];
					num_llr_c[i] = ( *max_star[DecoderType] )( delta1, delta2 );
				} else {
					/* this code bit is 0 */
					delta2 = den_llr_c[i];
					den_llr_c[i] = ( *max_star[DecoderType] )( delta1, delta2 );
				}
				mask = mask>>1;
			}
			
			/* data 1 branch (departing) */
			delta1 = alpha_prime[state] + metric_c[ symbol1] + beta[k*max_states+state1[state]] + app_in;
				
			/* the information bit */
			delta2 = num_llr_u;
			num_llr_u = ( *max_star[DecoderType] )( delta1, delta2 );
						
			mask = 1<<(nn-1);
			/* go through all the code bits */
			for (i=0;i<nn;i++) {
				if ( symbol1&mask ) {
					/* this code bit 1 */
					delta2 = num_llr_c[i];
					num_llr_c[i] = ( *max_star[DecoderType] )( delta1, delta2 );
				} else {
					/* this code bit is 0 */
					delta2 = den_llr_c[i];
					den_llr_c[i] = ( *max_star[DecoderType] )( delta1, delta2 );
				}
				mask = mask>>1;
			}

			/* normalize and shift */
			alpha_prime[state] = alpha[state] - alpha[0];

		}		

		if (k-1<LL)
			output_u[k-1] = num_llr_u - den_llr_u;

		for (i=0;i<nn;i++)
			output_c[nn*(k-1)+i] = num_llr_c[i] - den_llr_c[i];	
	}

	free(alpha_prime);
	free(beta);
	free(alpha);
	free(metric_c);
	free(rec_array);
	free( num_llr_c );
	free( den_llr_c );	
}
