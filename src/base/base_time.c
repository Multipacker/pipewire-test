internal DenseTime dense_time_from_date_time(DateTime *date_time) {
    DenseTime result = 0;
    result += (U32) ((S32) date_time->year + 0x8000);
    result *= 12;
    result += date_time->month;
    result *= 31;
    result += date_time->day;
    result *= 24;
    result += date_time->hour;
    result *= 60;
    result += date_time->minute;
    result *= 61;
    result += date_time->second;
    result *= 1000;
    result += date_time->millisecond;
    return result;
}

internal DateTime date_time_from_dense_time(DenseTime dense_time) {
    DateTime result;
    result.millisecond = dense_time % 1000;
    dense_time /= 1000;
    result.second = dense_time % 61;
    dense_time /= 61;
    result.minute = dense_time % 60;
    dense_time /= 60;
    result.hour = dense_time % 24;
    dense_time /= 24;
    result.day = dense_time % 31;
    dense_time /= 31;
    result.month = dense_time % 12;
    dense_time /= 12;
    result.year = (S16) ((S32) dense_time - 0x8000);
    return result;
}
