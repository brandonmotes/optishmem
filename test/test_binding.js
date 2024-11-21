const Optishmem = require("../lib/binding.js");
const assert = require("assert");

assert(Optishmem, "The expected function is undefined");

function testSharedMemory() {
    // Get the shared memory buffer
    const buffer = Optishmem("/test-shmem", 32);

    console.log(buffer);
  
    // Verify that the buffer holds 4 doubles
    // assert.strictEqual(buffer.length, 4, `Buffer should contain 4 doubles, contains ${buffer.length}`);
  
    // Read and validate each value
    const values = new Float64Array(buffer.buffer);
    assert.strictEqual(values[0], 1.0, `First value should be 1.0, read ${values[0]}`);
    assert.strictEqual(values[1], 2.0, `Second value should be 2.0, read ${values[1]}`);
    assert.strictEqual(values[2], 3.0, `Third value should be 3.0, read ${values[2]}`);
    assert.strictEqual(values[3], 4.0, `Fourth value should be 4.0, read ${values[3]}`);
  
    console.log('All values in shared memory are correct:', values);

    values[0] = 1.1;
  }

assert.doesNotThrow(testSharedMemory, undefined, "testSharedMemory threw an expection");
console.log('Test passed: Shared memory is correctly initialized and accessible!');

console.log("Tests passed - everything looks OK!");
