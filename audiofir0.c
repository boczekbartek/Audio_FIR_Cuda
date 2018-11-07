#include "audiofir.h"

static void audiofir_kernel(int i, float *yout, const float *yin, const float *coeff, int n, int len) {
    float s = 0;
//    if (i < 0) {
//        yout[i] = 0;
//    } else {
        for (int k = 0; k <= n; k++) {
            if (i >= k) s += yin[i - k] * coeff[k];
        }
        yout[i] = s;
//    }
}

void audiofir(float *yout, float *yin, float *coeff, int n, int len, ...) {
    int i;
    for (i = 0; i < len; i++)
        audiofir_kernel(i, yout, yin, coeff, n, len);
    for (i = 0; i < len; i++)
        audiofir_kernel(i, yout + len, yin + len, coeff, n, len);
}
