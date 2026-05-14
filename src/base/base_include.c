#include "base_types.c"
#include "base_vector.c"
#include "base_memory.c"
#include "base_string.c"
#include "base_context.c"
#include "base_hash.c"
#include "base_color.c"
#include "base_log.c"

#if OS_LINUX
# include "src/linux/base/linux_base.c"
#elif OS_WINDOWS
# include "src/win32/base/win32_base.c"
#else
# error no backend for os_include.c on this operating system
#endif
