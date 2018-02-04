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

from mpi4py import MPI
import utils

'''
sort generated numbers stored in "numbers.npy" with e.g.: "mpiexec -np 4 python3 sort.py"
time mpiexec -hostfile hosts -np 8 python3 /mnt/cluster_128/Folder_Gruppe_1/python/parallel-odd-even-sort.py

'''

comm = MPI.COMM_WORLD
rank = comm.Get_rank()  # processId of the current process
size = comm.Get_size()  # total number of processes in the communicator


def sort():
    global_unsorted_array = np.load(utils.get_numbers_file())
    validate_generated_numbers(global_unsorted_array)
    local_data = np.array_split(global_unsorted_array, size)[rank]
    local_data = do_odd_even_sort(local_data)
    output(global_unsorted_array, local_data)


def validate_generated_numbers(global_unsorted_array):
    if global_unsorted_array.size % 2 is not 0:
        raise ValueError("cannot sort odd number of elements")
    if (global_unsorted_array.size / size) % 1 != 0.0:
        raise ValueError("number of elements must be divisible by number of cores")


def do_odd_even_sort(local_data):
    partners = calculate_partners()
    for phase in range(size):
        partner = partners[phase % 2]
        if partner is None:
            continue
        local_data = iterate_phase(local_data, partner)
    return local_data


def calculate_partners():
    even_partner = validate_partner(rank + 1 if rank % 2 == 0 else rank - 1)
    odd_partner = validate_partner(rank - 1 if rank % 2 == 0 else rank + 1)
    return {0: even_partner, 1: odd_partner}


def validate_partner(p):
    return None if p < 0 or p >= size else p


def iterate_phase(local_data, partner):
    partner_data = exchange_partner_data(local_data, partner)
    data = np.sort(np.concatenate([local_data, partner_data]))
    return data[:local_data.size] if rank < partner else data[local_data.size:]


def exchange_partner_data(local_data, partner):
    partner_data = np.empty(local_data.size, dtype=np.int)
    comm.Sendrecv(local_data, dest=partner, recvbuf=partner_data, source=partner)
    return partner_data


def output(global_unsorted_array, local_data):
    global_sorted_array = gather_data_to_root_node(local_data)
    if rank == 0:
        print(global_unsorted_array)
        print(global_sorted_array.flatten())


def gather_data_to_root_node(local_data):
    global_sorted_array = None
    if rank == 0:
        global_sorted_array = np.empty([size, local_data.size], dtype=np.int)
    comm.Gather(local_data, global_sorted_array, root=0)
    return global_sorted_array


sort()
