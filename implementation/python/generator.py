import numpy as np
import sys

import constants

'''
generate numbers with e.g.: "python3 generator.py 16"
to numbers.npy as binary numpy array
'''

size = int(sys.argv[1]) if len(sys.argv) > 1 else 128
random_range = np.random.choice(np.arange(size), size, replace=False)
np.save(constants.NUMBER_FILE, random_range)
