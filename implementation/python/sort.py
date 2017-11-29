from mpi4py import MPI
import numpy as np

import constants

comm = MPI.COMM_WORLD
rank = comm.Get_rank()  # processId of the current process
size = comm.Get_size()  # total number of processes in the communicator


def sort():
    global_unsorted_array = np.load(constants.NUMBER_FILE)
    if global_unsorted_array.size % 2 is not 0:
        raise ValueError("cannot sort odd number of elements")

    local_data = np.array_split(global_unsorted_array, size)[rank]
    for phase in range(size + 1):

        global_sorted_array = gather_data_to_root_node(local_data)
        output_sorted_array(global_sorted_array, global_unsorted_array)

        local_data = np.sort(local_data)  # quicksort

        partner = get_next_partner(phase)
        # TODO get out of loop to improve performance, partner and oddPartner can be calculated in advance
        if partner < 0 or partner >= size:
            continue

        local_data = iterate_phase(local_data, partner)


def output_sorted_array(global_sorted_array, global_unsorted_array):
    if rank == 0:
        print(global_sorted_array.flatten())


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
            mini = np.argmin(partner_data)
            maxi = np.argmax(local_data)

            if partner_data[mini] < local_data[maxi]:
                temp = partner_data[mini]
                partner_data[mini] = local_data[maxi]
                local_data[maxi] = temp
            else:
                break
    else:
        while True:
            maxi = np.argmax(partner_data)
            mini = np.argmin(local_data)

            if partner_data[maxi] > local_data[mini]:
                temp = partner_data[maxi]
                partner_data[maxi] = local_data[mini]
                local_data[mini] = temp
            else:
                break
    return local_data


def exchange_with_partner(local_data, partner):
    partner_data = np.empty(local_data.size, dtype=np.int)
    comm.Send(local_data, dest=partner, tag=42)
    comm.Recv(partner_data, source=partner, tag=42)
    return partner_data


try:
    sort()
except Exception as e:
    print(str(e))
