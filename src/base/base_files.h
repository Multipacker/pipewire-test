#ifndef BASE_FILES_H
#define BASE_FILES_H

typedef enum {
    SystemPath_Binary,
    SystemPath_UserData,
    SystemPath_TemporaryData,
    SystemPath_Count,
} SystemPath;

typedef struct FileIterator FileIterator;
struct FileIterator {
    U8 data[700];
};

typedef enum {
    FilePropertyFlags_IsFolder = (1 << 0),
} FilePropertyFlags;

// NOTE: DenseTime is in universal time by default.
typedef struct FileProperties FileProperties;
struct FileProperties {
    U64 size;
    FilePropertyFlags flags;
    DenseTime create_time;
    DenseTime modify_time;
    DataAccessFlags access;
};

typedef struct FileInfo FileInfo;
struct FileInfo {
    Str8           name;
    FileProperties properties;
};


// NOTE(simon): @os_implementation
internal Str8 file_path(Arena *arena, SystemPath path);

// NOTE(simon): @os_implementation Files.
internal B32            file_read(Arena *arena, Str8 file_name, Str8 *result);
internal B32            file_write(Str8 file_name, Str8List data);
internal FileProperties file_properties(Str8 file_name);
internal B32            file_delete(Str8 file_name);
// Moves the file if neccessary and replaces existing files.
internal B32 file_rename(Str8 old_name, Str8 new_name);
internal B32 file_make_directory(Str8 path);
// The directory must be empty.
internal B32 file_delete_directory(Str8 path);

// NOTE(simon): @os_implementation File iteration.
internal FileIterator *file_iterator_begin(Arena *arena, Str8 path);
internal B32           file_iterator_next(Arena *arena, FileIterator *iterator, FileInfo *info);
internal Void          file_iterator_end(FileIterator *iterator);

#endif // BASE_FILES_H
