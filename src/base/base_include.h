#ifndef BASE_INCLUDE_H
#define BASE_INCLUDE_H

#include "base_core.h"
#include "base_types.h"
#include "base_time.h"
#include "base_vector.h"
#include "base_arena.h"
#include "base_string.h"
#include "base_context.h"
#include "base_threads.h"
#include "base_memory.h"
#include "base_hash.h"
#include "base_color.h"
#include "base_profile.h"
#include "base_files.h"
#include "base_log.h"
#include "base_process.h"
#include "base_entrypoint.h"

#if OS_LINUX
# include "src/linux/base/linux_base.h"
#elif OS_WINDOWS
# include "src/win32/base/win32_base.h"
#else
# error no backend for base_include.h on this operating system
#endif

#endif // BASE_INCLUDE_H
