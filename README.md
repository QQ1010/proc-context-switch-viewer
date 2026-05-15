# proc-context-switch-viewer

A small C learning project that reads Linux `/proc/<pid>/status` and displays per-process context switch counters.
Develop on MacOS therefore, I use Docker for testing.

### Steps
```
docker build -t proc-context-switch-viewer .
docker run --rm -it proc-context-switch-viewer sh
sleep 1000 &
yes > /dev/null &
./proc_switch_viewer
```

### Expected Output
```
proc_switch_viewer.c
PID      NAME                     VOLUNTARY      NONVOLUNTARY   TOTAL     
8        yes                      0          5          5         
9        proc_switch_vie          1          0          1  
```
CPU-bound processes such as `yes > /dev/null` tend to show nonvoluntary context switches.
Sleeping or I/O-blocked processes tend to show voluntary context switches.