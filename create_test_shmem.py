from multiprocessing.shared_memory import SharedMemory
import numpy as np

# Create shared memory
data = np.array([1.0, 2.0, 3.0, 4.0], dtype=np.float64)
shm = SharedMemory(create=True, size=data.nbytes)

# Write data to shared memory
buffer = np.ndarray(data.shape, dtype=data.dtype, buffer=shm.buf)
buffer[:] = data[:]

print(f"Shared memory name: {shm.name}")

# Keep the program running to allow access
try:
    input("Press Enter to terminate...")
finally:
    shm.close()
    shm.unlink()
