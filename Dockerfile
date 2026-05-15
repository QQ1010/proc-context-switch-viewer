FROM gcc:latest

WORKDIR /proc_switch_viewer
COPY . .

RUN make

CMD ["./proc_switch_viewer"]
