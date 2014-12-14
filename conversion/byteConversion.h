#ifndef BYTECONVERSION_H
#define BYTECONVERSION_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <stdint.h>
#include <cstring>


unsigned char *anyToByteArray(unsigned char *dst, const void *src, const uint64_t srcSize);

void *anyFromByteArray(void *dst, const unsigned char *src, const uint64_t srcSize);


unsigned char *cstrToByteArray(unsigned char *dst, const char *src, const uint64_t srcSize);

unsigned char *cstrToByteArray(unsigned char *dst, const char *src);

unsigned char *strToByteArray(unsigned char *dst, const std::string *src, const uint64_t srcSize);

unsigned char *strToByteArray(unsigned char *dst, const std::string *src);


char *cstrFromByteArray(char *dst, const unsigned char *src, const uint64_t srcSize);

char *cstrFromByteArray(char *dst, const unsigned char *src);


std::string *strFromByteArray(std::string *dst, const unsigned char *src, const uint64_t srcSize);


std::string *strFromByteArray(std::string *dst, const unsigned char *src);


template<typename T>
unsigned char *toByteArray(unsigned char *dst, const T *src, const uint64_t srcSize)
{
    return anyToByteArray(dst, (void*) src, srcSize);
}

template<typename T>
unsigned char *toByteArray(unsigned char *dst, const T *src)
{
    return toByteArray(dst, src, sizeof(T));
}


template<>
inline unsigned char *toByteArray(unsigned char *dst, const char *src, const uint64_t srcSize)
{
    return cstrToByteArray(dst, src, srcSize);
}

template<>
inline unsigned char *toByteArray(unsigned char *dst, const char *src)
{
    return cstrToByteArray(dst, src);
}

template<>
inline unsigned char *toByteArray(unsigned char *dst, const std::string *src, const uint64_t srcSize)
{
    return strToByteArray(dst, src, srcSize);
}

template<>
inline unsigned char *toByteArray(unsigned char *dst, const std::string *src)
{
    return strToByteArray(dst, src);
}




template<typename T>
T *fromByteArray(T *dst, const unsigned char *src, const uint64_t dstSize)
{
    return (T*) anyFromByteArray((void*) dst, src, dstSize);
}


template<typename T>
T *fromByteArray(T *dst, const unsigned char *src)
{
    return fromByteArray(dst, src, sizeof(T));
}


template<>
inline char *fromByteArray(char *dst, const unsigned char *src, const uint64_t dstSize)
{
    return cstrFromByteArray(dst, src, dstSize);
}


template<>
inline char *fromByteArray(char *dst, const unsigned char *src)
{
    return cstrFromByteArray(dst, src);
}

template<>
inline std::string *fromByteArray(std::string *dst, const unsigned char *src, const uint64_t dstSize)
{
    return strFromByteArray(dst, src, dstSize);
}


template<>
inline std::string *fromByteArray(std::string *dst, const unsigned char *src)
{
    return strFromByteArray(dst, src);
}





template<typename IN, typename OUT>
OUT &toOtherBase(OUT &out, IN src, const char *base, int width, char c)
{
    std::stringstream ss;
    ss << src;
    if(strcmp(base, "dec") == 0)
    ss >> std::dec;
    else if(strcmp(base, "oct") == 0)
    ss >> std::oct;
    else if(strcmp(base, "hex") == 0)
    ss >> std::hex;

    if(width > 1)
    ss >> std::setw(width) >> std::setfill(c);

    ss >> out;

    return out;
}


short hexCharToInt(const char c);



int64_t hexCharArrayToInt(const char *c, size_t nbElement);


template<typename T, typename IN>
T fromHexValue(IN &hexValue, T &out)
{
    out = 0;

    hexValue >> std::hex >> out;

    return out;
}


int64_t hexValueToInt(const char* hexValue);

unsigned char hexValueToByte(const char* hexValue);


template<typename T>
std::vector<T> hexValuesStringToVector(const std::string& hexValues)
{
    std::vector<T> vect;
    int64_t out(-1);
    
    size_t i(0), nbChar(sizeof(T)*2);
    size_t c(hexValues.size());
    const char *cstr = hexValues.c_str();

    while(i + nbChar <= c)
    {
        if(*(cstr + i) == ' ' || *(cstr + i) == '\t' || *(cstr + i) == '\n')
        {
            ++i;
            continue;
        }

        out = hexCharArrayToInt(cstr + i, nbChar);

        if(out == -1)
        {
            return std::vector<T>();
        }
        else
        {
            vect.push_back((T) out);
        }

        i += nbChar;
    }

    return vect;
}



std::vector<unsigned char> hexValuesStringToByteVector(const std::string& hexValues);


 const std::string byteVectorToHexValuesString(const std::vector<unsigned char> &byteVector);

 const std::string byteArrayToHexValuesString(const unsigned char *byteArray, const uint64_t arrSize);


#endif // BYTECONVERSION_H
