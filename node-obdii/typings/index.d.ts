declare module 'node-obdii' {
  interface OBDIICommand {
    pid: number;
    mode: number;
    name: string;
  }

  interface OBDIIResponse {
    success: 0 | 1;
    bitfieldValue?: number;
    numericValue?: number;
    stringValue?: number;
    dtcs?: number[];
  }

  class NodeOBDII {
    isOpened(socketFd: number): never | boolean;
    openSocket(): number | null;
    closeSocket(socketFd: number): never | boolean;
    getSupportedCommands(socketFd: number): never | OBDIICommand[] | null;
    performQuery(socketFd: number, commandPid: number): never | null | OBDIIResponse;
  }

  export { NodeOBDII };
}
