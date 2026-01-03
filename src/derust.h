#pragma once

#define DERUST_VERSION_BASE "0.0.1"

#ifdef _DEBUG
#define DERUST_VERSION DERUST_VERSION_BASE "-debug"
#else
#define DERUST_VERSION DERUST_VERSION_BASE
#endif