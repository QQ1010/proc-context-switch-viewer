#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_PROCS 4096

typedef struct ProcSample {
    int pid;
    char name[64];
    long voluntary;
    long nonvoluntary;
} ProcSample;

static const ProcSample *find_by_pid(const ProcSample samples[], int count, int pid) {
    for(int i = 0 ; i < count; i++) {
        if(samples[i].pid == pid) {
            return &samples[i];
        }
    }
    return NULL;
}

static void print_deltas(const ProcSample old_samples[], int old_count, const ProcSample new_samples[], int new_count) {
    printf("%-8s %-24s %-14s %-14s %-10s\n",
           "PID", "NAME", "VOLUNTARY", "NONVOLUNTARY", "TOTAL");

    for (int i = 0; i < new_count; i++) {
        const ProcSample *old = find_by_pid(old_samples, old_count, new_samples[i].pid);

        if (old == NULL) {
            continue;
        }

        long delta_vol = new_samples[i].voluntary - old->voluntary;
        long delta_invol = new_samples[i].nonvoluntary - old->nonvoluntary;
        long total = delta_vol + delta_invol;

        if (total == 0) {
            continue;
        }

        printf("%-8d %-24s %-10ld %-10ld %-10ld\n",
               new_samples[i].pid,
               new_samples[i].name,
               delta_vol,
               delta_invol,
               total);
    }

}

static void print_sample(const ProcSample samples[], int count) {
    printf("%-8s %-24s %-14s %-14s\n",
           "PID", "NAME", "VOLUNTARY", "NONVOLUNTARY");

    for (int i = 0; i < count; i++) {
        printf("%-8d %-24s %-14ld %-14ld\n",
               samples[i].pid,
               samples[i].name,
               samples[i].voluntary,
               samples[i].nonvoluntary);
    }
}

static bool is_pid(const char *name) {
    // check null string
    if(*name == '\0') {
        return false;
    }
    
    // check is digit
    while(*name) {
        if(!isdigit((unsigned char)*name)) {
            return false;
        }
        name ++;
    }
    return true;
}

static bool read_proc_status(int pid, ProcSample *sample) {
    char path[256];
    snprintf(path, sizeof(path),"/proc/%d/status",pid);
    
    // open /proc as file
    FILE *fp = fopen(path, "r");
    if(!fp) {
        perror("[Error]: fopen fail");
        return false;
    }
    
    // init
    sample->pid = pid;
    sample->name[0] = '\0';
    sample->voluntary = 0;
    sample->nonvoluntary = 0;

    // split info and get needed info
    char line[256];
    while(fgets(line, sizeof(line), fp)) {
        if(strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:%63s", sample->name);
        }
        else if(strncmp(line, "voluntary_ctxt_switches:", 24) == 0) {
            sscanf(line, "voluntary_ctxt_switches:%ld", &sample->voluntary);
        }
        else if(strncmp(line, "nonvoluntary_ctxt_switches:", 27) == 0) {
            sscanf(line, "nonvoluntary_ctxt_switches:%ld", &sample->nonvoluntary);
        }
    }
    fclose(fp);
    return true;
}

/*
struct dirent {
    ino_t          d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    unsigned char  d_type;
    char           d_name[256];
};
*/
static int scan_proc(ProcSample samples[], int max_samples) {
    // open folder
    DIR *dir = opendir("/proc");

    if(!dir) {
        perror("[Error]: opendir");
        return 0;
    }

    // check valid pid and read status
    struct dirent *entry;
    int count = 0;
    while((entry = readdir(dir)) != NULL) {
        if(!is_pid(entry->d_name)) {
            continue;
        }
        if(count >= max_samples) {
            break;
        }
        int pid = atoi(entry->d_name);
        if(read_proc_status(pid, &samples[count])) {
            count ++;
        }
    }
    closedir(dir);
    return count;
}

int main(void) {
    printf("proc_switch_viewer.c\n");
    // ProcSample samples[MAX_PROCS];
    // int count = scan_proc(samples, MAX_PROCS);
    // print_sample(samples, count);
    
    ProcSample old_samples[MAX_PROCS];
    ProcSample new_samples[MAX_PROCS];

    int old_count = scan_proc(old_samples, MAX_PROCS);
    sleep(1);
    int new_count = scan_proc(new_samples, MAX_PROCS);

    print_deltas(old_samples, old_count, new_samples, new_count);
    return 0;
}