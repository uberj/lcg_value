/* 
 * To compile: gcc lcg-state-forward.c -o lcg-state-forward  
 *
 * This program permutes the internal state of PHP 5.3's LCG (PRNG)
 *
 * For testing against my php-lcg-reverse.c which can reverse internal state
 *
 * -samy kamkar, code@samy.pl, 08/22/09
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
/* MODMULT computes s*b mod m, provided that m=a*b+c and 0<=c<m */

void modMult_(int *s1, int *s2) {
	int q;
        MODMULT(53668, 40014, 12211, 2147483563L, *s1);
        MODMULT(52774, 40692, 3791, 2147483399L, *s2);
}

void lcg_value(double *z, int *s1, int *s2) {
        modMult_(s1, s2);

        *z = *s1 - *s2;
        if (*z < 1)
                *z += 2147483562;
        *z *= 4.656613e-10;
}

int lcv_value_backwards(int s1, double z) {
        // Given s1 and z, what is s2
        // *z *= 4.656613e-10;
        int z_maybe;
        int s2_mod_maybe;
        int s2;
        int a, b, c, q;
        long m;

        z /= 4.656613e-10;
        // if (*z < 1)
        //      *z += 2147483562;
        z_maybe -= 2147483562;

        if (z_maybe >= 0) {
                z = z_maybe;
        }

        // *z  = *s1 - *s2
        // +s2    +s2
        // -z     -z
        // ----------------
        // s2 = s1 - z
        s2 = (int) s1 - z;

        // #define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
        /* MODMULT computes s*b mod m, provided that m=a*b+c and 0<=c<m */
        /*
         * q = s / a
         * s = b * (s - a * q) - c * q
         * if (s < 0)
         *      s += m
         */
        s2_mod_maybe = s2 - m;
        if (s2_mod_maybe >= 0) {
                s2 = s2_mod_maybe;
        }

        // #define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m
        // MODMULT(52774, 40692, 3791, 2147483399L, *s2);
        a = 52774;
        b = 40692;
        c = 3791;
        m = 2147483399L;

        // s                    = b * (s - a * q) - c * q
        //
        // s + c * q            = b * (s - a * q)
        //
        // s + c * q
        // ---------            = s - a * q
        //     b
        //
        // s + c * q
        // --------- + a * q    = s
        //     b
        s2 = ((s2 + c * q) / b) + a * q;
        return s2;
}

typedef struct {
        int s1;
        int s2;
        double z;
} LCGState;

void calc_states(int seed_s1, int seed_s2, int state_size, LCGState *states_to_seed) {
        for (int i = 0; i < state_size; i++) {
                if (i == 0) {
                        // init
                        states_to_seed[i].s1 = seed_s1;
                        states_to_seed[i].s2 = seed_s2;
                } else {
                        // seed
                        states_to_seed[i].s1 = states_to_seed[i - 1].s1;
                        states_to_seed[i].s2 = states_to_seed[i - 1].s2;
                }
                lcg_value(&states_to_seed[i].z, &states_to_seed[i].s1, &states_to_seed[i].s2);
        }
}

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		printf("usage: %s <s1 [-1251222200]> <s2 [98137]> <times>\n", argv[0]);
		return -1;
	}

        int sample_size = atoi(argv[3]);
        LCGState *target_states = malloc(sample_size * sizeof(LCGState));
        int s1_entered = atoi(argv[1]);
        int s2_entered = atoi(argv[2]);
        double D_THRESHOLD = 0.00000001;
        calc_states(s1_entered, s2_entered, sample_size, target_states);

        for (int i = 0; i < sample_size; i++) {
                printf("%d: s1=%d s2=%d lcg_value=%.14f\n",
                                i + 1,
                                target_states[i].s1,
                                target_states[i].s2,
                                target_states[i].z);
        }

	// now that we have some states, lets see if we can reconstruct them
        double indicator;
        int s1_guess = -1;
        for (;;) {
                s1_guess++; // pick s1
                if (s1_guess == sample_size) {
                        break;
                }
                if (s1_guess > (int) pow(2, 31)) {
                        break;
                }
                LCGState state = target_states[s1_guess];
                int s1 = s1_guess;
                int s2_guess = lcv_value_backwards(s1, state.z);
                int s2 = s2_guess;
                double z_candidate;
                int incorrect_guess = 0;
                // Using our chosen s1 and our calculated s2, compute z
                lcg_value(&z_candidate, &s1, &s2);
                // If z is what we expect, then we guessed s1 correctly
                if (fabs(z_candidate - state.z) < D_THRESHOLD) {
                        printf("Looking good at s1=%d s2=%d lcg_value=%.14f\n", s1, s2, z_candidate);
                        // seed another states array with our guesses and compare them to the target states
                        LCGState *test_states = malloc(sample_size * sizeof(LCGState));
                        calc_states(s1, s2, sample_size, test_states);
                        incorrect_guess = 0;
                        for (int i = 0; i < sample_size; i++) {
                                if (!fabs(target_states[i].z - test_states[i].z) < D_THRESHOLD) {
                                        incorrect_guess = 1;
                                        break;
                                }
                        }

                        if (incorrect_guess == 0) {
                                printf("Found initial states of s1=%d s2=%d!", s1_guess, s2_guess);
                                return 0;
                        }
                }
        }

	return 0;
}
