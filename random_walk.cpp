#include <iostream>
#include <cstdlib> // For atoi, rand, srand
#include <ctime>   // For time
#include <mpi.h>

void walker_process();
void controller_process();

int domain_size;
int max_steps;
int world_rank;
int world_size;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc != 3)
    {
        if (world_rank == 0)
        {
            std::cerr << "Usage: mpirun -np <p> " << argv[0] << " <domain_size> <max_steps>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    domain_size = atoi(argv[1]);
    max_steps = atoi(argv[2]);

    if (world_rank == 0)
    {
        controller_process();
    }
    else
    {
        walker_process();
    }

    MPI_Finalize();
    return 0;
}

void walker_process()
{
    srand(time(NULL) + world_rank);

    int position = 0;
    for (int step = 1; step <= max_steps; step++)
    {
        int move = (rand() % 2 == 0) ? -1 : 1;
        position += move;

        if (position < -domain_size || position > domain_size)
        {
            std::cout << "Rank " << world_rank << ": Walker finished in " << step << " steps." << std::endl;
            int done = 1;
            MPI_Send(&done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            return;
        }
    }

    std::cout << "Rank " << world_rank << ": Walker finished in max steps (" << max_steps << ")." << std::endl;
    int done = 1;
    MPI_Send(&done, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

void controller_process()
{
    int walkers = world_size - 1;
    for (int i = 0; i < walkers; i++)
    {
        int msg;
        MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    std::cout << "Controller: All " << walkers << " walkers have finished." << std::endl;
}
