from mpi4py import MPI
import numpy as np

import constants

comm = MPI.COMM_WORLD
rank = comm.Get_rank()  # processId of the current process
size = comm.Get_size()  # total number of processes in the communicator


def sort():
    global_unsorted_array = np.load(constants.NUMBER_FILE)

    # if global_unsorted_array.size % 2 is not 0:
    #    raise ValueError("cannot sort odd number of elements")

    local_data = np.array_split(global_unsorted_array, size)[rank]

    local_data = do_odd_even_sort(local_data)

    output(global_unsorted_array, local_data)


def output(global_unsorted_array, local_data):
    global_sorted_array = gather_data_to_root_node(local_data)
    if rank == 0:
        print(global_unsorted_array)
        print(global_sorted_array.flatten())


def do_odd_even_sort(local_data):
    for phase in range(size + 1):
        local_data = np.sort(local_data)  # quicksort

        partner = get_next_partner(phase)
        # TODO get out of loop to improve performance, partner and oddPartner can be calculated in advance
        if partner < 0 or partner >= size:
            continue

        local_data = iterate_phase(local_data, partner)
    return local_data


def gather_data_to_root_node(local_data):
    global_sorted_array = None
    if rank == 0:
        global_sorted_array = np.empty([size, local_data.size], dtype=np.int)
    comm.Gather(local_data, global_sorted_array, root=0)
    return global_sorted_array


def get_next_partner(phase):
    if phase % 2 == 0:
        return rank + 1 if rank % 2 == 0 else rank - 1
    else:
        return rank - 1 if rank % 2 == 0 else rank + 1


def iterate_phase(local_data, partner):
    partner_data = exchange_with_partner(local_data, partner)

    if rank < partner:
        while True:
            min_idx = np.argmin(partner_data)
            max_idx = np.argmax(local_data)

            if partner_data[min_idx] < local_data[max_idx]:
                temp = partner_data[min_idx]
                partner_data[min_idx] = local_data[max_idx]
                local_data[max_idx] = temp
            else:
                break
    else:
        while True:
            max_idx = np.argmax(partner_data)
            min_idx = np.argmin(local_data)

            if partner_data[max_idx] > local_data[min_idx]:
                temp = partner_data[max_idx]
                partner_data[max_idx] = local_data[min_idx]
                local_data[min_idx] = temp
            else:
                break
    return local_data


def exchange_with_partner(local_data, partner):
    partner_data = np.empty(local_data.size, dtype=np.int)
    if rank % 2 == 0:  # avoid deadlock while sending and writing
        comm.Send(local_data, dest=partner, tag=42)
        comm.Recv(partner_data, source=partner, tag=42)
    else:
        comm.Recv(partner_data, source=partner, tag=42)
        comm.Send(local_data, dest=partner, tag=42)
    return partner_data


try:
    sort()
except Exception as e:
    print(str(e))
