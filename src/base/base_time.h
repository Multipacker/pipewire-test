#ifndef BASE_TIME_H
#define BASE_TIME_H

typedef enum {
    Month_Jan,
    Month_Feb,
    Month_Mar,
    Month_Apr,
    Month_May,
    Month_Jun,
    Month_Jul,
    Month_Aug,
    Month_Sep,
    Month_Oct,
    Month_Nov,
    Month_Dec,
} Month;

typedef enum {
    Day_Monday,
    Day_Tuesday,
    Day_Wednesday,
    Day_Thursday,
    Day_Friday,
    Day_Saturday,
    Day_Sunday,
} Day;

typedef U64 DenseTime;

typedef struct {
    U16 millisecond; // [0, 999]
    U8  second;      // [0, 60] 60 in the case of leap seconds
    U8  minute;      // [0, 59]
    U8  hour;        // [0, 23]
    U8  day;         // [0, 30]
    U8  month;       // [0, 11]
    S16 year;        // 1 = 1 CE; 2020 = 2020 CE, 0 = 1 BCE; -100 = 101 BCE; etc.
} DateTime;

internal DenseTime dense_time_from_date_time(DateTime *date_time);
internal DateTime  date_time_from_dense_time(DenseTime dense_time);

// NOTE(simon): @os_implementation
internal U64      time_now_nanoseconds(Void);
internal DateTime time_now_universal(Void);
internal DateTime time_local_from_universal(DateTime *date_time);
internal DateTime time_universal_from_local(DateTime *date_time);
internal Void     sleep_milliseconds(U64 time);

#endif // BASE_TIME_H
