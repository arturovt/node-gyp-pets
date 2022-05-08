declare module 'node-clhash' {
  function clhash(input: string): { key: BigInt; hashed: BigInt };
  export { clhash };
}
