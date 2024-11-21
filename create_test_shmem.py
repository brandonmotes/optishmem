from multiprocessing.shared_memory import SharedMemory
import numpy as np
import sys
import time

# Create shared memory
data = np.array([1.0, 2.0, 3.0, 4.0], dtype=np.float64)
shm = SharedMemory(name="test-shmem", create=True, size=data.nbytes)

# Write data to shared memory
buffer = np.ndarray(data.shape, dtype=data.dtype, buffer=shm.buf)
buffer[:] = data[:]

print(f"Shared memory name: {shm.name}")


def wait_for_input():
    """Wait for user input to determine if single or double Enter press."""
    start_time = time.time()
    first_press = input()
    if time.time() - start_time < 0.5:
        second_press = input()
        if not second_press:  # Double Enter detected
            return "double"
    return "single"


# Keep the program running to allow access
print("\nPress Enter to print values. Double Enter to exit.")
try:
    while True:
        action = wait_for_input()
        if action == "single":
            print(f"Shared Memory Values: {buffer[:]}")
        elif action == "double":
            print("Exiting...")
            break
finally:
    shm.close()
    shm.unlink()
