#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    FILE *file;
//    char filename[] = "D:\\Git\\SJF-scheduling\\sample\\data.txt"; // File to read data from

    char filename[100]; // File to read data from
    strcpy(filename, argv[1]);

    file = fopen(filename, "r");
    if (file == NULL) { // Check if file opened successfully
        printf("Error opening file\n");
    } else {
        printf("File successfully opened\n");
    }

    // Read number of processes and ticks from file
    int numProcesses;
    int numTicks;
    fscanf(file, "%d", &numTicks);
    fscanf(file, "%d", &numProcesses);

    // Allocate memory for arrays
    int processId = 0;
    int processNum = 0;
    int *tau = (int *) malloc(numProcesses * sizeof(int));
    int *alpha = (int *) malloc(numProcesses * sizeof(int));
    int *proc[numProcesses];
    int *currProcess = (int *) malloc(numProcesses * sizeof(int));
    int *tau_time[numProcesses];
    int *processNo = (int *) malloc(numProcesses * sizeof(int));

    // Allocate memory for each process
    for (int i = 0; i < numProcesses; i++) {
        proc[i] = (int *) malloc(numTicks * sizeof(int));
    }
    // Allocate memory for tau_time array
    for (int i = 0; i < numProcesses; i++) {
        tau_time[i] = (int *) malloc(numTicks * sizeof(int));
    }

    // Read data from file for each process
    while (processNum < numProcesses) {
        int cur_tau;
        float cur_alpha;
        fscanf(file, "%d", &processId);
        fscanf(file, "%d", &cur_tau);
        fscanf(file, "%file", &cur_alpha);
        tau[processNum] = cur_tau;
        alpha[processNum] = cur_alpha;

        // Read process time for each tick
        for (int i = 0; i < numTicks; i++) {
            int p_time;
            fscanf(file, "%d", &p_time);
            proc[processNum][i] = p_time;
            // Calculate tau_time for each tick
            if (i == 0) {
                tau_time[processNum][i] = cur_tau;
            } else {
                tau_time[processNum][i] = (cur_alpha * proc[processNum][i - 1] + ((1 - cur_alpha) * tau_time[processNum][i - 1]));
            }
        }
        processNum++;
    }

    // Copy proc array to liveProc array
    int *liveProc[numProcesses];
    for (int i = 0; i < numProcesses; i++) {
        liveProc[i] = (int *) malloc(numTicks * sizeof(int));
    }
    for (int i = 0; i < numProcesses; i++) {
        for (int j = 0; j < numTicks; j++) {
            liveProc[i][j] = proc[i][j];
        }
    }

    fclose(file); // Close file

    // Variables to store total time, turnaround time, and wait time
    int tot_time = 0, turn_time = 0, wait_time = 0;

    //Simulate the shortest job first.
    printf("--Shortest-Job-First--\n");

    for (int l = 0; l < numTicks; l++) {

        int tick_time = 0;
        printf("Simulating %dth round of process @ time %d: \n", l, tot_time);

        for (int p = 0; p < numProcesses; p++) {
            int small = 999;
            int small_id = 0;
            for (int q = 0; q < numProcesses; q++) {
                if (small > proc[q][l]) {
                    small = proc[q][l];
                    small_id = q;
                }
            }

            currProcess[p] = small;
            processNo[p] = small_id;
            proc[small_id][l] = 999;
        }

        for (int z = 0; z < numProcesses; z++) {
            printf(" Process %d took %d\n", processNo[z], currProcess[z]);
            turn_time = turn_time + tick_time + currProcess[z];
            tot_time = tot_time + currProcess[z];
            wait_time = wait_time + tick_time;
            tick_time = tick_time + currProcess[z];
        }
    }

    printf("Turnaround time: %d\n", turn_time);
    printf("Waiting time: %d\n\n", wait_time);

    tot_time = 0;
    turn_time = 0;
    wait_time = 0;
    int est_error = 0;

    //Simulate the shortest job live algorithm.

    int tick_time = 0; // Initialize the time for each tick
    printf("--Shortest-Job-First Live--\n");

    // Find the process with the shortest job remaining
    for (int i = 0; i < numTicks; i++) {
        printf("Simulating %dth round of process @ time %d: \n", i, tot_time);
        int small = 999;
        int small_id = 0;
        for (int j = 0; j < numProcesses; j++) {

            for (int k = 0; k < numProcesses; k++) {
                if (small > tau_time[k][i]) {
                    small = tau_time[k][i];
                    small_id = k;
                }
            }
            currProcess[j] = small; // Store the current process time remaining
            processNo[j] = small_id; // Store the current process id
            tau_time[small_id][i] = 999; // Set the process time remaining for the current process to a very high value
        }

        // Simulate each process and calculate relevant times, taking into account the estimation error
        for (int z = 0; z < numProcesses; z++) {
            printf(" Process %d was estimated for %d but took %d\n", processNo[z], currProcess[z],
                   liveProc[processNo[z]][i]);

            turn_time = turn_time + tick_time + liveProc[processNo[z]][i];
            tot_time = tot_time + liveProc[processNo[z]][i];
            wait_time = wait_time + tick_time;
            tick_time = tick_time + liveProc[processNo[z]][i];

            int cur_error = 0; // Initialize the estimation error for the current process
            if (liveProc[processNo[z]][i] > currProcess[z]) {
                cur_error = liveProc[processNo[z]][i] - currProcess[z];
            } else {
                cur_error = currProcess[z] - liveProc[processNo[z]][i];
            }
            est_error = est_error + cur_error;
        }
    }

    printf("Turnaround time: %d \n", turn_time);
    printf("Waiting time: %d \n", wait_time);
    printf("Estimation Error: %d\n", est_error);

    return 0;
}