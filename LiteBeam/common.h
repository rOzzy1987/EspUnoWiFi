#ifndef __COMMON_H__
#define __COMMON_H__

#define Max(a,b) a > b ? a : b
#define Min(a,b) a < b ? a : b

inline uint8_t HexToByte(char c) {
    return (c >= '0' && c <= '9')
        ? c - '0'
        : (c >= 'a' && c <= 'f')
            ? c + 10 - 'a'
            : (c >= 'A' && c <= 'F')
                ? c + 10 - 'A'
                : 0;
} 


#endif // __COMMON_H__
