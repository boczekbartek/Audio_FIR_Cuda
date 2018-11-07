/* Filtracja sygna³u AUDIO filtrem FIR - szkielet */

//#ifdef _MSC_VER
//# define _CRT_SECURE_NO_WARNINGS
//#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "audiofir.h"

/**********************************************************/

void alloc_mem(int n, int len,
               float **coeff_ptr, float **yin_ptr,
               float **yref_ptr, float **yout_ptr) {
    *coeff_ptr = (float *) malloc((1 + n) * sizeof(float));
    *yin_ptr = (float *) malloc(2 * len * sizeof(float));
    *yref_ptr = (float *) malloc(2 * len * sizeof(float));
    *yout_ptr = (float *) malloc(2 * len * sizeof(float));
}

void free_mem(float *coeff_ptr, float *yin_ptr,
              float *yref_ptr, float *yout_ptr) {
    free(coeff_ptr);
    free(yin_ptr);
    free(yref_ptr);
    free(yout_ptr);
}

void read_data(int *n_ptr, int *len_ptr,
               float **coeff_ptr, float **yin_ptr,
               float **yref_ptr, float **yout_ptr) {
    FILE *f = fopen("/home/bartek/Devel/Audio_FIR_Cuda/audiofir_in.matlab.dat", "rb");

    fread(n_ptr, sizeof(int), 1, f);
    fread(len_ptr, sizeof(int), 1, f);

    alloc_mem(*n_ptr, *len_ptr,
              coeff_ptr, yin_ptr, yref_ptr, yout_ptr);
    fread(*coeff_ptr, sizeof(float), 1 + *n_ptr, f);
    fread(*yin_ptr, sizeof(float), 2 * *len_ptr, f);
    fread(*yref_ptr, sizeof(float), 2 * *len_ptr, f);

    fclose(f);
}

void write_data(int len, float *y) {
    FILE *f = fopen("audiofir_out.dat", "wb");
    fwrite(y, sizeof(float), 2 * len, f);
    fclose(f);
}

/**********************************************************/

void audiocmp(float *yout, float *yref, int len) {
    int k;
    float d, e = -1.0f;
    for (k = 0; k < 2* len; k++) {
        //printf("%lf - %lf\n", yout[k], yref[k]);
        if ((d = fabsf(yout[k] - yref[k])) > e)
            e = d;
    }
    printf("max. abs. err. = %.1e\n", e);
}

/**********************************************************/

#ifdef _WIN32

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

typedef LARGE_INTEGER app_timer_t;

#define timer(t_ptr) QueryPerformanceCounter(t_ptr)

void elapsed_time(app_timer_t start, app_timer_t stop,
                  double flop)
{
  double etime;
  LARGE_INTEGER clk_freq;
  QueryPerformanceFrequency(&clk_freq);
  etime = (stop.QuadPart - start.QuadPart) /
          (double) clk_freq.QuadPart;
  printf("CPU (total!) time = %.3f ms (%6.3f GFLOP/s)\n",
         etime * 1e3, 1e-9 * flop / etime);
}

#else

#include <time.h> /* requires linking with rt library
                     (command line option -lrt) */

typedef struct timespec app_timer_t;

#define timer(t_ptr) clock_gettime(CLOCK_MONOTONIC, t_ptr)

void elapsed_time(app_timer_t start, app_timer_t stop, double flop) {
    double etime;
    etime = 1e+3 * (stop.tv_sec - start.tv_sec) +
            1e-6 * (stop.tv_nsec - start.tv_nsec);
    printf("CPU (total!) time = %.3f ms (%6.3f GFLOP/s)\n",
           etime, 1e-6 * flop / etime);
}

#endif

/**********************************************************/

int main(int argc, char *argv[]) {
    app_timer_t start, stop;
    int n, len;
    float *coeff, *yin, *yref, *yout;
    read_data(&n, &len, &coeff, &yin, &yref, &yout);
    timer(&start);
    audiofir(yout, yin, coeff, n, len, argc, argv);
    timer(&stop);
    elapsed_time(start, stop,
                 2 * ((double) n + 1) * 2 * ((double) len));
    audiocmp(yout, yref, len);
    write_data(len, yout);
    free_mem(coeff, yin, yref, yout);
//    if (IsDebuggerPresent()) getchar();
    return 0;
}