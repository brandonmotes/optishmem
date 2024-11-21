#include <napi.h>
#include <sys/mman.h> // for shared memory
#include <fcntl.h>    // for O_* constants
#include <unistd.h>   // for close
#include <cstring>    // for memcpy

Napi::Value Method(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Check for the correct number of arguments
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsNumber())
  {
    throw Napi::TypeError::New(env, "Expected a string (shared memory name) and a number (length) as arguments");
  }

  // Get the shared memory name from the first argument
  std::string shmName = info[0].As<Napi::String>();

  // Get the shared memory length from the second argument
  size_t shmLength = info[1].As<Napi::Number>().Uint32Value();
  if (shmLength == 0)
  {
    throw Napi::Error::New(env, "Shared memory length must be greater than zero");
  }

  // Open the existing shared memory object (do not create)
  int shmFd = shm_open(shmName.c_str(), O_RDWR, 0666);
  if (shmFd == -1)
  {
    throw Napi::Error::New(env, "Failed to open existing shared memory");
  }

  // Map the shared memory into the process's address space
  void *shmPtr = mmap(nullptr, shmLength, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
  if (shmPtr == MAP_FAILED)
  {
    close(shmFd);
    throw Napi::Error::New(env, "Failed to map shared memory");
  }

  // Close the shared memory file descriptor, as it's no longer needed
  close(shmFd);

  // Create a Napi::Buffer that directly references the shared memory
  auto buffer = Napi::Buffer<uint8_t>::New(
      env,
      static_cast<uint8_t *>(shmPtr),      // Pointer to shared memory
      shmLength,                           // Number of uint8_t
      [shmLength](Napi::Env, void *data) { // Finalizer to clean up
        munmap(data, shmLength);           // Correctly use shmLength for unmapping
      });

  return buffer;
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "Optishmem"), Napi::Function::New(env, Method));
  return exports;
}

NODE_API_MODULE(binding, Init)
