# node-obdii

The `node-obdii` has just been a pet project of making the `obdii` C API available in Node.js. It hasn't been tested in the natural environment, of course, since it's dangerous to break some car controller :smile:

I was seeking the ECUsim 2000 to test it with the ELM327 adapter, but it's not available in my country.

It requires setting up a virtual CAN network:

```sh
$ modprobe can
$ modprobe can_raw
$ modprobe vcan
$ ip link add dev vcan0 type vcan
$ ip link set up vcan0
```

There's a `Dockerfile` since SocketCAN is available only in Linux. You have to have Linux as a host OS since the `node-obdii` shares the host kernel and will require `can` and `vcan` modules installed on the host. There's no way to do that within a container since the `modprobe` command is unavailable. I'd go by running Linux in VirtualBox.

```sh
$ docker build -t node-obdii
$ docker run -it node-obdii /bin/bash
root@e8f0435a3df5:/usr/src/app# node
Welcome to Node.js v16.15.0.
> addon = require('bindings')('node_obdii')
> obdii = new addon.NodeOBDII()
> socketFd = obdii.openSocket()
> ...
> obdii.closeSocket(socketFd)
```
