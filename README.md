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
libssh-devel (version >= 0.8.7)

## Build

```bash
qmake ApplicationStatusMonitor.pro
make 
```

## Run

```bash
./ApplicationStatusMonitor &
```

## Example of work

![Application Base Statistics](https://github.com/AnthonySnow887/application-status-monitor/blob/master/App_Base.png?raw=true)
![Application Opend Network Ports](https://github.com/AnthonySnow887/application-status-monitor/blob/master/App_Net.png?raw=true)
![Host Statistics](https://github.com/AnthonySnow887/application-status-monitor/blob/master/Host_Stat.png?raw=true)
