from scipy.signal import lfilter
import struct
import numpy as np

def testfir(yorg,  Fs, coeffs, l, yin=None):
    """ Apply FIR filter on `yorg` vector using `coeffs` """
    if yin is None:
        yin = yorg
    n = len(coeffs) - 1
    yref = lfilter(coeffs, 1, yin)

    with open('audiofir_in.dat', 'wb') as f:
        f.write(struct.pack('i', n))
        f.write(struct.pack('i', l))
        f.write(struct.pack('f'*len(coeffs), *coeffs))
        for r in yin:
            f.write(struct.pack('f'*len(r), *r))
        for r in yref:
            f.write(struct.pack('f'*len(r), *r))


if __name__ == '__main__':
    Fs = 44100
    yorg = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9], dtype='float64')
    l = len(yorg)
    yorg = np.array([yorg, yorg*-1])
    coeffs = np.array([1e0, 1e1, 1e2, 1e3])
    testfir(yorg, Fs, coeffs,l)
