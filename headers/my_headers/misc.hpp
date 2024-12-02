#include <iostream>
#include <bitset>
#include <vector>
#include<algorithm>
#include <string>
#include <format>
#include "map"


#ifndef MISC_
#define MISC_

int len(char* buf)
{
    return sizeof* buf / sizeof buf[0];
}
std::byte fixed_header_fill_obj(std::string s)
    {
       std::string bytestring = s;
       std::bitset<8> g(bytestring);
       const unsigned long d = g.to_ulong();
       return (std::byte)d; 
    };


std::string bool_to_string(bool a)
    {
        return a?"1":"0";
    }
auto string_to_bool(char a)
{
    return (a=='1')?true:false;
}


extern void view_char (char f[], int n)
{
    for(int i=0; i < n; i++)
    {
        std::cout << f[i] << ",";
    }
}

extern void view_int (char f[], int n)
{
    for(int i=0; i < n; i++)
    {
        std::cout << (int)f[i] << ",";
    }
}

void view_int (const char f[], int n)
{
    for(int i=0; i < n; i++)
    {
        std::cout << (int)f[i] << ",";
    }
}




int decode_variable_integer(char arr[])
{
    int multiplier = 1, value = 0;
    int count = 0, encodedByte=arr[0];
    {
        encodedByte = arr[count];

        value += (encodedByte & 127) * multiplier;

        if (multiplier > (128 * 128 * 128))
        {
            perror("Malformed Variable Byte Integer");
        };
        multiplier *= 128;
        count++;
    }
    while((encodedByte&128) != 0)
    return encodedByte;
}

int decode_bool(bool z[2])
{
    if (z[0] == 0 && z[1] == 0)
        return 0;
    if (z[0] == 0 && z[1] == 1)
        return 1;
    if (z[0] == 1 && z[1] == 0)
        return 2;
    if (z[0] == 1 && z[1] == 1)
        perror("Malformed boolean packet");
    if ((int)z[0] > 1 && (int)z[1] > 1)
        perror("Malformed boolean packet");
    return 0;
}

#endif