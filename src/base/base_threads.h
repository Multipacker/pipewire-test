#ifndef BASE_THREADS_H
#define BASE_THREADS_H

typedef struct Thread Thread;
struct Thread {
    U64 u64[1];
};

typedef Void ThreadFunction(Void *data);

typedef struct Mutex Mutex;
struct Mutex {
    U64 u64[1];
};

typedef struct ConditionVariable ConditionVariable;
struct ConditionVariable {
    U64 u64[1];
};

// NOTE(simon): @os_implementation Threads
internal Thread thread_start(ThreadFunction entry_point, Void *data);
internal Void   thread_set_name(Thread handle, Str8 name);
internal B32    thread_join(Thread thread);
internal Void   thread_detach(Thread handle);

// NOTE(simon): @os_implementation Mutexes.
internal Mutex mutex_create(Void);
internal Void  mutex_destroy(Mutex mutex);
internal Void  mutex_lock(Mutex mutex);
internal Void  mutex_unlock(Mutex mutex);
#define mutex_scope(mutex) defer_loop(mutex_lock(mutex), mutex_unlock(mutex))

// NOTE(simon): @os_implementation Condition variables.
internal ConditionVariable condition_variable_create(Void);
internal Void              condition_variable_destroy(ConditionVariable condition_variable);
internal Void              condition_variable_signal(ConditionVariable condition_variable);
internal Void              condition_variable_broadcast(ConditionVariable condition_variable);
// NOTE(simon): A end_ns of U64_MAX means wait forever.
internal B32               condition_variable_wait(ConditionVariable condition_variable, Mutex mutex, U64 end_ns);

#endif // BASE_THREADS_H
