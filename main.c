#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    FILE *file;
    char filename[] = "sample/data.txt";

//    char filename[100];
//    strcpy(filename, argv[1]);

    file = fopen(filename, "r");
//    if (file != NULL) {
//        printf("File Opened\n");
//    }
    int no_proc;
    int no_ticks;
    int proc_id = 0;

    fscanf(file, "%d", &no_ticks);
    fscanf(file, "%d", &no_proc);

    int *tau = (int *) malloc(no_proc * sizeof(int));
    int *alpha = (int *) malloc(no_proc * sizeof(int));
    int *proc[no_proc];

    for (int i = 0; i < no_proc; i++) {
        proc[i] = (int *) malloc(no_ticks * sizeof(int));
    }

    int *s_curr_proc = (int *) malloc(no_proc * sizeof(int));
    int *s_proc_no = (int *) malloc(no_proc * sizeof(int));
    int p_num = 0;
    int *tau_time[no_proc];

    for (int i = 0; i < no_proc; i++) {
        tau_time[i] = (int *) malloc(no_ticks * sizeof(int));
    }

    while (p_num < no_proc) {
        int cur_tau;
        float cur_alpha;
        fscanf(file, "%d", &proc_id);
        fscanf(file, "%d", &cur_tau);
        fscanf(file, "%file", &cur_alpha);
        tau[p_num] = cur_tau;
        alpha[p_num] = cur_alpha;

        for (int k = 0; k < no_ticks; k++) {

            int p_time;
            fscanf(file, "%d", &p_time);
            proc[p_num][k] = p_time;
            if (k == 0) {
                tau_time[p_num][k] = cur_tau;
            } else {
                tau_time[p_num][k] = (cur_alpha * proc[p_num][k - 1] + ((1 - cur_alpha) * tau_time[p_num][k - 1]));
            }
        }
        p_num++;
    }

    int *proc1[no_proc];

    for (int i = 0; i < no_proc; i++) {
        proc1[i] = (int *) malloc(no_ticks * sizeof(int));
    }

    for (int i = 0; i < no_proc; i++) {
        for (int j = 0; j < no_ticks; j++) {
            proc1[i][j] = proc[i][j];
        }
    }

    fclose(file);

    int l = 0;
    int tot_time = 0;
    int turn_time = 0;
    int wait_time = 0;

    //Simulate the shortest job first.
    printf("--Shortest-Job-First--\n");

    for (int l = 0; l < no_ticks; l++) {

        int tick_time = 0;
        printf("Simulating %dth round of process @ time %d: \n", l, tot_time);

        for (int p = 0; p < no_proc; p++) {
            int small = 999;
            int small_id = 0;
            for (int q = 0; q < no_proc; q++) {
                if (small > proc[q][l]) {
                    small = proc[q][l];
                    small_id = q;
                }
            }

            s_curr_proc[p] = small;
            s_proc_no[p] = small_id;
            proc[small_id][l] = 999;
        }

        for (int z = 0; z < no_proc; z++) {
            printf(" Process %d took %d\n", s_proc_no[z], s_curr_proc[z]);
            turn_time = turn_time + tick_time + s_curr_proc[z];
            tot_time = tot_time + s_curr_proc[z];
            wait_time = wait_time + tick_time;
            tick_time = tick_time + s_curr_proc[z];
        }
    }

    printf("Turnaround time: %d\n", turn_time);
    printf("Waiting time: %d\n\n", wait_time);

    tot_time = 0;
    turn_time = 0;
    wait_time = 0;
    int est_error = 0;

//Simulate the shortest job live algorithm.

    printf("--Shortest-Job-First Live--\n");

    for (int l = 0; l < no_ticks; l++) {
        int tick_time = 0;
        printf("Simulating %dth round of process @ time %d: \n", l, tot_time);

        for (int p = 0; p < no_proc; p++) {
            int small = 999;
            int small_id = 0;

            for (int q = 0; q < no_proc; q++) {
                if (small > tau_time[q][l]) {
                    small = tau_time[q][l];
                    small_id = q;
                }
            }
            s_curr_proc[p] = small;
            s_proc_no[p] = small_id;
            tau_time[small_id][l] = 999;
        }

        for (int z = 0; z < no_proc; z++) {

            int cur_error = 0;
            printf(" Process %d was estimated for %d but took %d\n", s_proc_no[z], s_curr_proc[z],
                   proc1[s_proc_no[z]][l]);

            if (proc1[s_proc_no[z]][l] > s_curr_proc[z]) {
                cur_error = proc1[s_proc_no[z]][l] - s_curr_proc[z];
            } else {
                cur_error = s_curr_proc[z] - proc1[s_proc_no[z]][l];
            }

            turn_time = turn_time + tick_time + proc1[s_proc_no[z]][l];
            tot_time = tot_time + proc1[s_proc_no[z]][l];
            wait_time = wait_time + tick_time;
            tick_time = tick_time + proc1[s_proc_no[z]][l];
            est_error = est_error + cur_error;
        }
    }

    printf("Turnaround time: %d \n", turn_time);
    printf("Waiting time: %d \n", wait_time);
    printf("Estimation Error: %d\n", est_error);

    return 0;
}