#include <napi.h>
#include <sys/mman.h> // for shared memory
#include <fcntl.h>    // for O_* constants
#include <unistd.h>   // for close
#include <cstring>    // for memcpy

Napi::Value Method(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Name of the shared memory segment
  const char *shmName = "/test-shmem";

  // Size of the shared memory segment (4 doubles)
  size_t shmSize = 4 * sizeof(double);

  // Open the existing shared memory object (do not create)
  int shmFd = shm_open(shmName, O_RDWR, 0666);
  if (shmFd == -1)
  {
    throw Napi::Error::New(env, "Failed to open existing shared memory");
  }

  // Map the shared memory into the process's address space
  void *shmPtr = mmap(nullptr, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
  if (shmPtr == MAP_FAILED)
  {
    close(shmFd);
    throw Napi::Error::New(env, "Failed to map shared memory");
  }

  // Close the shared memory file descriptor, as it's no longer needed
  close(shmFd);

  // Create a Napi::Buffer that directly references the shared memory
  auto buffer = Napi::Buffer<double>::New(
      env,
      static_cast<double *>(shmPtr), // Pointer to shared memory
      4,                             // Number of doubles
      [](Napi::Env, void *data) {    // Finalizer to clean up
        munmap(data, 4 * sizeof(double));
      });

  return buffer;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "Optishmem"), Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(binding, Init)
