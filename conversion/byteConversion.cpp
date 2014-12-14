#include "byteConversion.h"

using namespace std;


unsigned char *anyToByteArray(unsigned char *dst, const void *src, const size_t srcSize)
{
    if(dst == 0 ||  srcSize <= 0)
    return 0;

    memcpy(static_cast<void*>(dst), src, srcSize);

    return dst;
}



void *anyFromByteArray(void *dst, const unsigned char *src, const size_t srcSize)
{
    if(dst == 0 ||  srcSize <= 0)
    return 0;

    memcpy(dst, static_cast<const void*>(src), srcSize);

    return dst;
}



unsigned char *cstrToByteArray(unsigned char *dst, const char *src, const size_t srcSize)
{
    return anyToByteArray(dst, src, srcSize);
}


unsigned char *cstrToByteArray(unsigned char *dst, const char *src)
{
    return cstrToByteArray(dst, src, (strlen(src) + 1)*sizeof(char));
}

unsigned char *strToByteArray(unsigned char *dst, const std::string *src, const size_t srcSize)
{
    return anyToByteArray(dst, (void*) src->c_str(), srcSize);
}

unsigned char *strToByteArray(unsigned char *dst, const std::string *src)
{
    return strToByteArray(dst, src, src->size() + 1);
}








char *cstrFromByteArray(char *dst, const unsigned char *src, const size_t srcSize)
{
    return (char*) anyFromByteArray((void*) dst, src, srcSize);
}

char *cstrFromByteArray(char *dst, const unsigned char *src)
{
    return cstrFromByteArray(dst, src, (strlen((char*) src) + 1)*sizeof(char));
}

std::string *strFromByteArray(std::string *dst, const unsigned char *src, const size_t srcSize)
{
    char *tmp = new char[srcSize];

    cstrFromByteArray(tmp, src, srcSize);

    *dst = tmp;

    delete[] tmp;

    return dst;
}


std::string *strFromByteArray(std::string *dst, const unsigned char *src)
{
    size_t srcSize((strlen((char*) src) + 1)*sizeof(char));

    return strFromByteArray(dst, src, srcSize);
}





short hexCharToInt(const char c)
{
    switch(c)
    {
        case '0':
        return 0;

        case '1':
        return 1;

        case '2':
        return 2;

        case '3':
        return 3;

        case '4':
        return 4;

        case '5':
        return 5;

        case '6':
        return 6;

        case '7':
        return 7;

        case '8':
        return 8;

        case '9':
        return 9;

        case 'A':
        case 'a':
        return 10;

        case 'B':
        case 'b':
        return 11;

        case 'C':
        case 'c':
        return 12;

        case 'D':
        case 'd':
        return 13;

        case 'E':
        case 'e':
        return 14;

        case 'F':
        case 'f':
        return 15;

        default:
        return -1;
    }
}





int64_t hexCharArrayToInt(const char *c, size_t nbElement)
{
    int64_t ret = 0;

    size_t i(0);

    while(i < nbElement)
    {
        short tmp(hexCharToInt(c[i]));
        if(tmp == -1)
        return -1;

        ret *= 16;
        ret += tmp;

        ++i;
    }

    return ret;
}





int64_t hexValueToInt(const char* hexValue)
{
    int s(strlen(hexValue));

    return hexCharArrayToInt(hexValue, s);
}



unsigned char hexValueToByte(const char* hexValue)
{
    return static_cast<unsigned char>(hexValueToInt(hexValue));
}

std::vector<unsigned char> hexValuesStringToByteVector(const std::string& hexValues)
{
    return hexValuesStringToVector<unsigned char>(hexValues);
}


const std::string byteVectorToHexValuesString(const std::vector<unsigned char> &byteVector)
{
    std::ostringstream oss;

    for(size_t i(0), c(byteVector.size()); i < c; ++i)
    oss << std::hex << (int) byteVector[i] << ' ';

    return oss.str();
}


const string byteArrayToHexValuesString(const unsigned char *byteArray, const size_t arrSize)
{
    std::ostringstream oss;

    size_t i(0);
    for(i = 0; i < arrSize - 1; ++i)
    {
        int tmp = byteArray[i];
        oss << std::hex << tmp;
    }

    if(i < arrSize)
    {
        int tmp = byteArray[i];
        oss << std::hex << tmp;
    }

    return oss.str();
}


