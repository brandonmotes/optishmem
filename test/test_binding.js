const Optishmem = require("../lib/binding.js");
const assert = require("assert");

assert(Optishmem.ConnectToMemory, "The expected 'connectSharedMemory' function is undefined");
// assert(binding.readSharedMemory, "The expected 'readSharedMemory' function is undefined");

// Must be called first
function testConnect() {
  Optishmem.ConnectToMemory("test-shmem", 32); // Updated to match the name in create_test_shmem.py
}

function testReadDouble() {
  const result1 = Optishmem.ReadDouble(0);
  const result2 = Optishmem.ReadDouble(16);

  assert(Math.abs(result1 - 1.0) < 1e-10, `First expected value is approximately '1.0', got ${result1}`);
  assert(Math.abs(result2 - 3.0) < 1e-10, `Second expected value is approximately '3.0', got ${result2}`);
}

function testWriteDouble() {
  Optishmem.WriteDouble(0, -4);

  const result = Optishmem.ReadDouble(0);
  assert(Math.abs(result + 4) < 1e-10, `Expected written value to be '-4.0', read back ${result}`);

  Optishmem.WriteDouble(0, 1.0);
}

function testReadArray() {
  const buffer = Optishmem.ReadArray(0, 32);
  const array = new Float64Array(buffer.buffer);
  console.log(array);
}

assert.doesNotThrow(testConnect, undefined, "testSharedMemory threw an exception.");
console.log("Test passed: Shared memory is correctly initialized and accessible!");

assert.doesNotThrow(testReadDouble, undefined, "testReadDouble threw an exception.");
assert.doesNotThrow(testWriteDouble, undefined, "testWriteDouble threw an exception.");
assert.doesNotThrow(testReadArray, undefined, "testReadArray threw an exception.");

console.log("Tests passed - everything looks OK!");
