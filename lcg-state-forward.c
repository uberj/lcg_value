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
        printf("actual(0) s1=%d s2=%d\n", *s1, *s2);
        modMult_(s1, s2);

        printf("actual(1) s1=%d s2=%d\n", *s1, *s2);
        *z = *s1 - *s2;
        printf("actual(2) z=%.14f\n", *z);
        if (*z < 1)
                *z += 2147483562;
        printf("actual(3) z=%.14f\n", *z);
        *z *= 4.656613e-10;
        printf("actual(4) z=%.14f\n", *z);
}

int lcg_value_backwards(int s1, double z) {
        // Given s1 and z, what is s2
        double z_maybe;
        int s2_mod_maybe;
        int s2;
        int a, b, c, q;
        long m;

        // *z *= 4.656613e-10;
        printf("backwards(1) z=%.14f\n", z);
        z /= 4.656613e-10;
        printf("backwards(2) z=%.14f\n", z);
        // if (*z < 1)
        //      *z += 2147483562;
        z_maybe = z - 2147483562;

        printf("backwards(2.5) z_maybe=%.14f\n", z_maybe);
        printf("backwards(3) z=%.14f\n", z);

        MODMULT(53668, 40014, 12211, 2147483563L, s1);

        // *z  = *s1 - *s2
        // +s2    +s2
        // -z     -z
        // ----------------
        // s2 = s1 - z
        s2 = s1 - z;
        // if s2 is negative, we chose the wrong z, switch it to z_maybe
        if (s2 < 0) {
                printf("backwards(4) s2 was neg s1=%d s2=%d\n", s1, s2);
                s2 = s1 - z_maybe;
        }
        printf("backwards(5) s1=%d s2=%d\n", s1, s2);

        s2_mod_maybe = s2 - m;
        if (s2_mod_maybe < 0) {
                s2 = s2_mod_maybe;
        }
        printf("backwards(6) s1=%d s2=%d\n", s1, s2);

        // Need mod inverse
        // Forward PRNG goes:
        // s2_next = s2_cur * b (mod m)
        // Backwards needs s2_cur and has s2_next
        // for try_s2 in range(0, 2*32):
        //      if (s2_cur * b % (mod m)):
        //              this is the right s2_cur
        // (there has to be a short-cut here)
        /* MODMULT computes s*b mod m, provided that m=a*b+c and 0<=c<m */
        /* #define MODMULT(a, b, c, m, s) q = s/a;s=b*(s-a*q)-c*q;if(s<0)s+=m*/
        /* a = 52774;*/
        /* b = 40692;*/
        /* c = 3791;*/
        m = 2147483399;
        /*
         * GCD calc for b (mod m)
         * >>> for i in xrange(0, 2147483399):
         * ...    if (i * 40692) % 2147483399 == 1:
         * ...       print i
         * ...       break
         * ... 
         *
         * 1481316021
         *
         */
        for (int try_s2 = 0; try_s2 < pow(2, 31); try_s2++) {
                int try_s2_ = try_s2;
                // modmult for s2 taken from original lcg_value
                MODMULT(52774, 40692, 3791, 2147483399L, try_s2_);
                if (try_s2_ == s2) {
                        double test = s2 * 1481316021;
                        while (test > m) {
                                test -= m;
                        }
                        printf("backwards(7) test=%d s1=%d s2=%d\n", test, s1, s2);
                        return try_s2;
                }
        }
        printf("FAIL!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        return s2;
}

typedef struct {
        int s1;
        int s2;
        double z;
} LCGState;

void calc_states(int seed_s1, int seed_s2, int state_size, LCGState *states_to_seed, int test_backwards) {
        for (int i = 0; i < state_size; i++) {
                printf("(%d)---------------------------------\n", i);
                if (i == 0) {
                        // init
                        states_to_seed[i].s1 = seed_s1;
                        states_to_seed[i].s2 = seed_s2;
                } else {
                        // seed
                        states_to_seed[i].s1 = states_to_seed[i - 1].s1;
                        states_to_seed[i].s2 = states_to_seed[i - 1].s2;
                }
                int s1_initial = states_to_seed[i].s1;
                int s2_initial = states_to_seed[i].s2;
                lcg_value(&states_to_seed[i].z, &states_to_seed[i].s1, &states_to_seed[i].s2);
                printf("%d: s1=%d s2=%d lcg_value=%.14f\n",
                                i + 1,
                                states_to_seed[i].s1,
                                states_to_seed[i].s2,
                                states_to_seed[i].z);
                if (test_backwards != 0) {
                        int s2_guess = lcg_value_backwards(s1_initial, states_to_seed[i].z);
                        printf("Guess at:             s2=%d (actual s2=%d)\n", s2_guess, s2_initial);
                        double z2_guess;
                        lcg_value(&z2_guess, &s1_initial, &s2_guess);
                        printf("Guess closeness:             z - z_guess = %.14f\n", states_to_seed[i].z - z2_guess);
                }


        }
}

void attempt_to_break(LCGState *target_states, int sample_size, double D_THRESHOLD) {
        double indicator;
        int s1_guess = -1;
        double sample_z = target_states[0].z;
        for (;;) {
                s1_guess++; // pick s1
                if (s1_guess > (int) pow(2, 31)) {
                        break;
                }
                int s1 = s1_guess;
                int s2_guess = lcg_value_backwards(s1, sample_z);
                int s2 = s2_guess;
                double z_candidate;
                int incorrect_guess = 0;
                // Using our chosen s1 and our calculated s2, compute z
                lcg_value(&z_candidate, &s1, &s2);
                // If z is what we expect, then we guessed s1 correctly
                if (fabs(z_candidate - sample_z) < D_THRESHOLD) {
                        printf("Looking good at s1=%d s2=%d lcg_value=%.14f\n", s1, s2, z_candidate);
                        // seed another states array with our guesses and compare them to the target states
                        LCGState *test_states = malloc(sample_size * sizeof(LCGState));
                        calc_states(s1, s2, sample_size, test_states, 0);
                        incorrect_guess = 0;
                        for (int i = 0; i < sample_size; i++) {
                                if (!fabs(target_states[i].z - test_states[i].z) < D_THRESHOLD) {
                                        incorrect_guess = 1;
                                        break;
                                }
                        }

                        if (incorrect_guess == 0) {
                                printf("Found initial states of s1=%d s2=%d!", s1_guess, s2_guess);
                                return;
                        }
                }
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
        float D_THRESHOLD = 0.00000001;
        calc_states(s1_entered, s2_entered, sample_size, target_states, 1);

	// now that we have some states, lets see if we can reconstruct them
        /*attempt_to_break(target_states, sample_size, D_THRESHOLD);*/


	return 0;
}
