declare module 'node-clhash' {
  interface ClhashResult {
    key: BigInt;
    hashed: BigInt;
  }

  function clhash(
    input: string,
    callback: (error: Error | null, result?: ClhashResult) => void
  ): void;

  namespace clhash {
    function sync(input: string): ClhashResult;
  }

  export { clhash };
}
