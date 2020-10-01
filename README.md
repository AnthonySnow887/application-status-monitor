# application-status-monitor

This utility displays the main characteristics of a running process:
- cpu usage
- memory usage
- opend files descriptors
- threads
- opend tcp/udp ports

General characteristics of the host on which the application is running:
- cpu statistics
- memory statistics
- network statistics
- disk statistics

The utility allows you to receive information from a remote server using ssh.

NOTE: Only Linux.

## Dependencies

libqt4-devel (or libqt5-devel)

## Build

```bash
qmake ApplicationStatusMonitor.pro
make 
```

## Run

```bash
./ApplicationStatusMonitor &
```
