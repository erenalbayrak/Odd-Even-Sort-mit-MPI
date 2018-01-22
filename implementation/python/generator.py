import numpy as np  # numpy can be used multithreaded for generation on master
import sys, utils

'''
generate numbers with e.g.: "python3 generator.py 16"
to numbers.npy as binary numpy array
'''

size = int(sys.argv[1]) if len(sys.argv) > 1 else 128
random_range = np.arange(size)
np.random.shuffle(random_range)
np.save(utils.get_numbers_file(), random_range)
