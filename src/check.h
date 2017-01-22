#include <stdio.h>
#include <stdlib.h>
#include "la.h"
#include "constants.h"
#include <string.h>
#include "mpi_error.h"

int check_values(LA local_array, int world_size, Entry *check_buffer);
Entry *init_check_buffer(int world_size);
int check_init(LA local_array, int world_size);
