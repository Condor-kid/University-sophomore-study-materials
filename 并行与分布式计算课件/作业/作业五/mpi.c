#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 100000
// #define MAX_PROCESSOR_NAME 20
#define MAX_TASKS 32
#define TEST_TIMES 32

int main(int argc, char* argv[]) {
    // create communication buffer
    char buffer[BUF_SIZE] = {};
    memset(buffer, 'x', BUF_SIZE);
    // processor name of current host
    char host_name[MPI_MAX_PROCESSOR_NAME];
    int host_name_length;
    // name of each node
    char host_map[MAX_TASKS][MPI_MAX_PROCESSOR_NAME];
    memset(host_map, 0, MAX_TASKS * MPI_MAX_PROCESSOR_NAME); 
    // partner of current node
    int partner_rank;
    int task_pair[MAX_TASKS] = {};
    memset(buffer, 0, MAX_TASKS);
    // store timings for each pair of task
    double timings[MAX_TASKS][4];
    memset(timings, 0, MAX_TASKS * 3);
    int tag = 1;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_size % 2 != 0) {
        printf("There must be an even number of tasks.\n");
        int error_code = 1;
        MPI_Abort(MPI_COMM_WORLD, error_code);
        exit(1);
    }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Get_processor_name(host_name, &host_name_length);
    MPI_Gather(&host_name, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, &host_map, MPI_MAX_PROCESSOR_NAME, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (world_rank < world_size / 2) {
        partner_rank = world_size / 2 + world_rank;
    } else {
        partner_rank = world_rank - world_size / 2;
    }
    // task_pair[world_rank] = partner_rank;
    MPI_Gather(&partner_rank, 1, MPI_INT, &task_pair, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (world_rank == 0) {
        puts("**********MPI BANDWIDTH TEST**********");
        printf("MPI world size: %d\n", world_size);
        printf("Test message size: %d\n", BUF_SIZE);
        printf("MPI_Wtick resolution: %e\n", MPI_Wtick());
        for (int i = 0; i < world_size; ++i) {
            printf("Task %d is on %s with partner %d\n", i, host_map[i], task_pair[i]);
        }
        puts("**************************************");
    }
    if (world_rank < world_size / 2) {
        double best_bw = .0, worst_bw = .99E+99, total_bw = .0;
        double total_time = .0;
        for (int i = 0; i < TEST_TIMES; ++i) {
            double nbytes = sizeof(char) * BUF_SIZE;
            double start_time = MPI_Wtime();
            MPI_Send(&buffer, BUF_SIZE, MPI_CHAR, partner_rank, tag, MPI_COMM_WORLD);
            MPI_Recv(&buffer, BUF_SIZE, MPI_CHAR, partner_rank, tag, MPI_COMM_WORLD, &status);
            double end_time = MPI_Wtime();
            double run_time = end_time - start_time;
            double bw = (2 * nbytes) / run_time;
            total_bw += bw;
            best_bw = bw > best_bw ? bw : best_bw;
            worst_bw = bw < worst_bw ? bw : worst_bw;
            total_time += run_time;
        }
        best_bw /= 1000000.0;
        worst_bw /= 1000000.0;
        double avg_bw = (total_bw / 1000000.0) / TEST_TIMES;
        total_time /= TEST_TIMES;
        if (world_rank == 0) {
            // first record himself
            timings[0][0] = best_bw;
            timings[0][1] = avg_bw;
            timings[0][2] = worst_bw;
            timings[0][3] = total_time;
            double best_all = .0, worst_all = .0, avg_all = .0;
            double time_all = .0;
            // then receive records from other nodes
            for (int j = 1; j < world_size / 2; ++j) {
                MPI_Recv(&timings[j], 4, MPI_DOUBLE, j, tag, MPI_COMM_WORLD, &status);
            }
            for (int j = 0; j < world_size / 2; ++j) {
                printf("Task pair: %d - %d: best: %lf, avg: %lf, worst: %lf, time: %lf\n", j, task_pair[j], timings[j][0], timings[j][1], timings[j][2], timings[j][3]);
                best_all += timings[j][0];
                avg_all += timings[j][1];
                worst_all += timings[j][2];
                time_all += timings[j][3];
            }
            printf("Total avg: best: %lf, avg: %lf, worst: %lf, time: %lf\n", best_all / (world_size / 2), avg_all / (world_size / 2), worst_all / (world_size / 2), time_all / (world_size / 2));
        } else {
            double tmp_timings[4];
            tmp_timings[0] = best_bw;
            tmp_timings[1] = avg_bw;
            tmp_timings[2] = worst_bw;
            tmp_timings[3] = total_time;
            MPI_Send(tmp_timings, 4, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }
    } else {
        // these nodes, need only to receive and send data from/to its partner
         for (int i = 0; i < TEST_TIMES; ++i) {
            MPI_Recv(&buffer, BUF_SIZE, MPI_CHAR, partner_rank, tag, MPI_COMM_WORLD, &status);
            MPI_Send(&buffer, BUF_SIZE, MPI_CHAR, partner_rank, tag, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}
