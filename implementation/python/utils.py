import os

_use_abs_location = False

_numbers_file = "numbers.npy"
# absolute location of numbers binary file for execution on cluster
_numbers_file_abs = "/mnt/cluster_128/Folder_Gruppe_1/python/numbers.npy"


def get_numbers_file():
    return _numbers_file_abs if _use_abs_location else _numbers_file
