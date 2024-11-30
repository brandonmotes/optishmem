#include <napi.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h> // for shared memory
#include <fcntl.h>    // for O_* constants
#include <unistd.h>   // for close
#endif
#include <cstring>    // for memcpy
#include <stdexcept>  // for exception handling

#include <iostream>

std::string globalShmName;
void *globalShmPtr = nullptr;
size_t globalShmLength = 0;
#ifdef _WIN32
HANDLE globalShmHandle = nullptr;
#else
int globalShmFd = -1;
#endif

using namespace Napi;

Napi::Boolean ConnectToMemory(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Check for the correct number of arguments
  if (info.Length() < 2 || !info[0].IsString() || !info[1].IsNumber())
  {
    throw Napi::TypeError::New(env, "Expected a string (shared memory name) and a number (length) as arguments");
  }

  // Get the shared memory name from the first argument
  globalShmName = info[0].As<Napi::String>();

  // Get the shared memory length from the second argument
  globalShmLength = info[1].As<Napi::Number>().Uint32Value();
  if (globalShmLength == 0)
  {
    throw Napi::Error::New(env, "Shared memory length must be greater than zero");
  }

  // Cleanup any previously opened shared memory
  if (globalShmPtr != nullptr)
  {
#ifdef _WIN32
    UnmapViewOfFile(globalShmPtr);
    CloseHandle(globalShmHandle);
    globalShmHandle = nullptr;
#else
    munmap(globalShmPtr, globalShmLength);
    close(globalShmFd);
    globalShmFd = -1;
#endif
    globalShmPtr = nullptr;
  }

#ifdef _WIN32
  globalShmHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, globalShmName.c_str());
  if (globalShmHandle == nullptr)
  {
    throw Napi::Error::New(env, "Failed to open existing shared memory");
  }

  globalShmPtr = MapViewOfFile(globalShmHandle, FILE_MAP_ALL_ACCESS, 0, 0, globalShmLength);
  if (globalShmPtr == nullptr)
  {
    CloseHandle(globalShmHandle);
    globalShmHandle = nullptr;
    throw Napi::Error::New(env, "Failed to map shared memory");
  }
#else
  globalShmFd = shm_open(globalShmName.c_str(), O_RDWR, 0666);
  if (globalShmFd == -1)
  {
    throw Napi::Error::New(env, "Failed to open existing shared memory");
  }

  globalShmPtr = mmap(nullptr, globalShmLength, PROT_READ | PROT_WRITE, MAP_SHARED, globalShmFd, 0);
  if (globalShmPtr == MAP_FAILED)
  {
    close(globalShmFd);
    globalShmFd = -1;
    globalShmLength = 0;
    throw Napi::Error::New(env, "Failed to map shared memory");
  }
#endif

  return Napi::Boolean::New(env, true);
}

void CleanupMemoryConnection(const Napi::CallbackInfo &info)
{
  // Check if the shared memory has been initialized
  if (globalShmPtr != nullptr)
  {
#ifdef _WIN32
    UnmapViewOfFile(globalShmPtr);
    CloseHandle(globalShmHandle);
    globalShmHandle = nullptr;
#else
    munmap(globalShmPtr, globalShmLength);
    close(globalShmFd);
    globalShmFd = -1;
#endif
    globalShmPtr = nullptr;
  }

  // Reset the shared memory length
  globalShmLength = 0;
}

Napi::Uint8Array ReadArray(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the shared memory length from the second argument
  uint32_t arrayOffsetBytes = info[0].As<Napi::Number>().Uint32Value();

  // Get the shared memory length from the second argument
  uint32_t arraySize = info[1].As<Napi::Number>().Uint32Value();
  if (globalShmLength == 0)
  {
    throw Napi::Error::New(env, "Shared memory read size must be greater than zero.");
  }

  // Allocate a new Napi buffer
  Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::New(env, arraySize);

  // Copy the shared memory content to the new buffer
  std::memcpy(buffer.Data(), static_cast<uint8_t *>(globalShmPtr) + arrayOffsetBytes, arraySize);

  return buffer;
}

Napi::Number ReadDouble(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the shared memory length from the second argument
  uint32_t memOffsetBytes = info[0].As<Napi::Number>().Uint32Value();

  uint8_t *memPtr = static_cast<uint8_t *>(globalShmPtr) + memOffsetBytes;

  double value;

  memcpy(&value, memPtr, 8);

  return Napi::Number::New(env, value);
}

Napi::Number ReadUint32(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  // Get the shared memory length from the second argument
  uint32_t memOffsetBytes = info[0].As<Napi::Number>().Uint32Value();

  uint8_t *memPtr = static_cast<uint8_t *>(globalShmPtr) + memOffsetBytes;

  uint32_t value;

  memcpy(&value, memPtr, 4);

  return Napi::Number::New(env, value);
}

void WriteDouble(const Napi::CallbackInfo &info)
{
  // Napi::Env env = info.Env();

  // Get the shared memory length from the second argument
  uint32_t memOffsetBytes = info[0].As<Napi::Number>().Uint32Value();

  double value = info[1].As<Napi::Number>().DoubleValue();

  uint8_t *memPtr = static_cast<uint8_t *>(globalShmPtr) + memOffsetBytes;

  memcpy(memPtr, &value, 8);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "ConnectToMemory"),
              Napi::Function::New(env, ConnectToMemory));
  exports.Set(Napi::String::New(env, "CleanupMemoryConnection"),
              Napi::Function::New(env, CleanupMemoryConnection));
  exports.Set(Napi::String::New(env, "ReadArray"),
              Napi::Function::New(env, ReadArray));
  exports.Set(Napi::String::New(env, "ReadDouble"),
              Napi::Function::New(env, ReadDouble));
  exports.Set(Napi::String::New(env, "WriteDouble"),
              Napi::Function::New(env, WriteDouble));
  exports.Set(Napi::String::New(env, "ReadUint32"),
              Napi::Function::New(env, ReadUint32));
  return exports;
}

NODE_API_MODULE(addon, Init)
