#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <type_traits>
#include <chrono>
#include <thread>

#include <immintrin.h>

#include <Windows.h>
#include <TlHelp32.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32")

#define log(...) std::printf(__VA_ARGS__)

#include "xorstr.hpp"
#include "sock.hpp"
#include "memory.hpp"