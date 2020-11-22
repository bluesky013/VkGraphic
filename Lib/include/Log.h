#pragma once

#include <stdio.h>

#define LOG_ERROR(tag, fmt, ...) LOG("ERROR", tag, fmt, ##__VA_ARGS__)
#define LOG_INFO(tag, fmt, ...) LOG("INFO", tag, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(tag, fmt, ...) LOG("DEBUG", tag, fmt, ##__VA_ARGS__)

#define LOG(level, tag, fmt, ...) \
{ \
    printf("[-%s], %s, -%u, -%s", level, __FILE__, __LINE__, tag); \
    printf(fmt, ##__VA_ARGS__); \
}