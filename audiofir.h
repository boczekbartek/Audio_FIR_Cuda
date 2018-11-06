//
// Created by bartek on 06.11.18.
//

#ifndef FIR_CUDA_AUDIOFIR_H
#define FIR_CUDA_AUDIOFIR_H
#if defined __cplusplus
extern "C"
#endif

void audiofir(float *yout, float *yin, float *coeff, int n, int len, ...);

#endif //FIR_CUDA_AUDIOFIR_H
