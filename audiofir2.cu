//
// Created by bartek on 07.11.18.
//
#include "audiofir.h"
#include "helper_cuda.h"

#define K 512

__global__
static void audiofir_kernel(float *yout, const float *yin, const float *coeff, int n) {
    float s = 0;
    int i = threadIdx.x + blockIdx.x * blockDim.x;

    for (int k = 0; k <= n; k++) {
        s += yin[i - k] * coeff[k];
    }
    yout[i] = s;

}


void audiofir(float *yout, float *yin, float *coeff, int n, int len, ...) {
    checkCudaErrors(cudaSetDevice(0));
    int l1 = K * (len + K - 1) / K;
    //allocate memory
    int coeff_size = (1 + n) * sizeof(float);
    float *dev_coeff_ptr, *dev_yin_ptr, *dev_yout_ptr;

    checkCudaErrors(cudaMalloc(&dev_coeff_ptr, coeff_size));
    checkCudaErrors(cudaMalloc(&dev_yin_ptr, 2 * (n + l1) * sizeof(float)));
    checkCudaErrors(cudaMalloc(&dev_yout_ptr, 2 * len * sizeof(float)));
    // copy to device
    checkCudaErrors(cudaMemcpy(dev_coeff_ptr, coeff, coeff_size, cudaMemcpyHostToDevice));

    checkCudaErrors(cudaMemset(dev_yin_ptr, 0, 2 * (n + l1) * sizeof(float)));
    checkCudaErrors(cudaMemcpy(dev_yin_ptr + n, yin, len * sizeof(float), cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(dev_yin_ptr + n + l1 + n, yin + len, len * sizeof(float), cudaMemcpyHostToDevice));

    cudaEvent_t start, stop; // pomiar czasu wykonania jądra
    checkCudaErrors(cudaEventCreate(&start));
    checkCudaErrors(cudaEventCreate(&stop));
    checkCudaErrors(cudaEventRecord(start, 0));

    audiofir_kernel << < (len + K - 1) / K, K >> >
                                            (dev_yout_ptr, dev_yin_ptr + n, dev_coeff_ptr, n);
    checkCudaErrors(cudaGetLastError());

    // jumping
    checkCudaErrors(cudaMemset(dev_yin_ptr, 0, n * sizeof(float)));
//    dev_yin_ptr += len;

    audiofir_kernel << < (len + K - 1) / K, K >> >
                                            (dev_yout_ptr + len, dev_yin_ptr + n + l1 + n, dev_coeff_ptr, n);

    checkCudaErrors(cudaGetLastError());

    checkCudaErrors(cudaEventRecord(stop, 0));
    checkCudaErrors(cudaEventSynchronize(stop));
    float elapsedTime;
    checkCudaErrors(cudaEventElapsedTime(&elapsedTime,
                                         start, stop));
    checkCudaErrors(cudaEventDestroy(start));
    checkCudaErrors(cudaEventDestroy(stop));

    checkCudaErrors(cudaDeviceSynchronize());


    int y_size = 2 * len * sizeof(float);
    checkCudaErrors(cudaMemcpy(yout, dev_yout_ptr, y_size, cudaMemcpyDeviceToHost));

    checkCudaErrors(cudaFree(dev_coeff_ptr));
    checkCudaErrors(cudaFree(dev_yin_ptr));
    checkCudaErrors(cudaFree(dev_yout_ptr));

    checkCudaErrors(cudaDeviceReset()); // dla debuggera
//
    double flop = 2 * ((double) n + 1) * 2 * ((double) len);
    printf("GPU (total!) time = %.3f ms (%6.3f GFLOP/s)\n",
           elapsedTime, 1e-6 * flop / elapsedTime);

}

