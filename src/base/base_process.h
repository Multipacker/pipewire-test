#ifndef BASE_PROCESS_H
#define BASE_PROCESS_H

// NOTE(simon): @os_implementation
internal Void get_entropy(Void *data, U64 size);

// NOTE(simon): @os_implementation
internal Str8 current_directory(Arena *arena);

// NOTE(simon): @os_implementation
internal B32  console_run(Str8 program, Str8List arguments);
internal Void console_print(Str8 string);

// NOTE(simon): @os_implementation
internal Void restart_self(Void);
internal Void exit_self(S32 exit_code);

#endif // BASE_PROCESS_H
