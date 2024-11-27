const Optishmem = require("../lib/binding.js");
const assert = require("assert");

assert(Optishmem.ConnectToMemory, "The expected 'connectSharedMemory' function is undefined");
// assert(binding.readSharedMemory, "The expected 'readSharedMemory' function is undefined");

// Must be called first
function testConnect() {
  Optishmem.ConnectToMemory("/test-shmem", 32);
}

function testReadDouble() {
  const result1 = Optishmem.ReadDouble(0);
  const result2 = Optishmem.ReadDouble(16);

  assert(Math.abs(result1 - 1.0) < 1e-10, `First expected value is approximately '1.0', got ${result1}`);
  assert(Math.abs(result2 - 3.0) < 1e-10, `Second expected value is approximately '3.0', got ${result2}`);

}

// function testReadDouble() {
//   const result1 = Optishmem.ReadDouble(0);
//   const result2 = Optishmem.ReadDouble(16);

//   assert(Math.abs(result1 - 1.0) < 1e-10, `First expected value is approximately '1.0', got ${result1}`);
//   assert(Math.abs(result2 - 3.0) < 1e-10, `Second expected value is approximately '3.0', got ${result2}`);

// }

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


// function testWriteDouble() {
//   // Connect to shared memory
//   connectSy("/test-shmem", 32);

//   // Read the shared memory contents
//   const array = readSharedMemory();

//   console.log("Shared memory contents:", array);

//   // Verify the shared memory array has the expected length
//   assert.strictEqual(array.length, 32, `Shared memory should have length 32, got ${array.length}`);

//   // Assume the shared memory holds 4 doubles (64-bit floats), validate values
//   const values = new Float64Array(array.map(Number)); // Convert array elements to Float64Array
//   assert.strictEqual(values[0], 1.0, `First value should be 1.0, read ${values[0]}`);
//   assert.strictEqual(values[1], 2.0, `Second value should be 2.0, read ${values[1]}`);
//   assert.strictEqual(values[2], 3.0, `Third value should be 3.0, read ${values[2]}`);
//   assert.strictEqual(values[3], 4.0, `Fourth value should be 4.0, read ${values[3]}`);

//   console.log("All values in shared memory are correct:", values);

//   // Modify the shared memory contents locally (this doesn't affect actual shared memory)
//   // values[0] = 1.1;
// }

assert.doesNotThrow(testConnect, undefined, "testSharedMemory threw an exception.");
console.log("Test passed: Shared memory is correctly initialized and accessible!");

assert.doesNotThrow(testReadDouble, undefined, "testReadDouble threw an exception.");
assert.doesNotThrow(testWriteDouble, undefined, "testWriteDouble threw an exception.");
assert.doesNotThrow(testReadArray, undefined, "testReadArray threw an exception.");

console.log("Tests passed - everything looks OK!");
