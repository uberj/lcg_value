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

#include <stdio.h>
#include <stdlib.h>

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

typedef struct {
        int s1;
        int s2;
        double z;
} LCGState;

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf("usage: %s <s1 [-1251222200]> <s2 [98137]>\n", argv[0]);
		return -1;
	}

	int q;
	int s1 = atoi(argv[1]);
	int s2 = atoi(argv[2]);
	int times = 0;
        int sample_size = 100;
        LCGState *states = malloc(sample_size * sizeof(LCGState));
	double z;

        for (int i = 0; i < sample_size; i++) {
                if (i == 0) {
                        // init
                        states[i].s1 = s1;
                        states[i].s2 = s2;
                } else {
                        // seed
                        states[i].s1 = states[i - 1].s1;
                        states[i].s2 = states[i - 1].s2;
                }
                lcg_value(&states[i].z, &states[i].s1, &states[i].s2);
                printf("%d: s1=%d s2=%d lcg_value=%.14f\n", i, states[i].s1, states[i].s2, states[i].z);
        }

        /*int s1 = -1;*/
        /*int s2 = 0;*/
        /*while (true) {*/
                /*s1++;*/
                /*lcv_value(z, )*/
                /*for (int possible_s2; possible_s2 < 2**31; possible_s2++) {*/

                /*}*/
        /*}*/

	return 0;
}
