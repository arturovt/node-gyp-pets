const addon = require('bindings')('node_obdii');

try {
  const obdii = new addon.NodeOBDII();
  const socketFd = obdii.openSocket();

  const isOpened = obdii.isOpened(socketFd);
  console.log(isOpened);

  const supportedCommands = obdii.getSupportedCommands(socketFd);
  console.log(supportedCommands);

  const response = obdii.performQuery(
    socketFd,
    /* command PID */
    /* 0x1F is `Run time since engine start` */
    0x1f
  );

  console.log(response);

  obdii.closeSocket(socketFd);
} catch (error) {
  console.warn(error);
}
