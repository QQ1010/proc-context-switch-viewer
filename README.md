# proc-context-switch-viewer

A small C learning project that reads Linux `/proc/<pid>/status` and displays per-process context switch counters.
Develop on MacOS therefore, I use Docker for testing.

### Steps
```
docker build -t proc-context-switch-viewer .
docker run --rm proc-context-switch-viewer
```