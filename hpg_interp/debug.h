#pragma once

#ifdef _DEBUG
#define dprintf(...) printf(__VA_ARGS__);
#else
#define dprintf(...)
#endif
