import os

try:
    os.environ["OMP_NUM_THREADS"] = "1"
    os.environ["NUMEXPR_NUM_THREADS"] = "1"
    os.environ["MKL_NUM_THREADS"] = "1"
    import numpy as np
finally:
    del os.environ["OMP_NUM_THREADS"]
    del os.environ["NUMEXPR_NUM_THREADS"]
    del os.environ["MKL_NUM_THREADS"]

import utils


def oddevenSort(x):
    sorted = False
    while not sorted:
        sorted = True
        for i in range(0, len(x) - 1, 2):
            if x[i] > x[i + 1]:
                x[i], x[i + 1] = x[i + 1], x[i]
                sorted = False
        for i in range(1, len(x) - 1, 2):
            if x[i] > x[i + 1]:
                x[i], x[i + 1] = x[i + 1], x[i]
                sorted = False
    return x


print(oddevenSort(np.load(utils.get_numbers_file())))
