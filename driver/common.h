#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <wsk.h>

#define log(...) DbgPrintEx(0, 0, __VA_ARGS__);

#include "intdefs.h"
#include "imports.h"
#include "utils.h"
#include "berkeley.h"
#include "ksocket.h"
#include "server.h"