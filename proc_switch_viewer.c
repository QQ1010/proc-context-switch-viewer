#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_PROCS 4

typedef struct ProcSample {
    int pid;
    char name[64];
    long voluntary;
    long nonvoluntary;
} ProcSample;

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
        if(!isdigit(*name)) {
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
    ProcSample samples[MAX_PROCS];
    int count = scan_proc(samples, MAX_PROCS);
    print_sample(samples, count);
    return 0;
}