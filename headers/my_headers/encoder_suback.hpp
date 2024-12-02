#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"

#ifndef ENCODER_SUBACK
#define ENCODER_SUBACK
class suback
{
    unsigned int reason_codes[12] = {0x00, 0x01, 0x02, 0x80, 0x82,0x87, 0x8f,0x91,0x97, 0x9e, 0xa1, 0xa2};
    std::byte user_property_[8], reason_string_[8];
    public:
    std::byte variable_header[32];
     std::byte property_length()
    {
        return (std::byte)0;
    }
    
    void mint_variable_header(std::byte subscribe_packet_identifier[2], char* username)
    {
        string f[2] = {"", ""};
        variable_header[0] = fixed_header_fill_obj("10010000");
        std::copy(subscribe_packet_identifier, subscribe_packet_identifier + 2, variable_header + 1);
        variable_header[3] = property_length();
        std::string s = "success";
        std::byte* f1 = reinterpret_cast<std::byte*>((char*)s.c_str());
        reason_string_[0] = (std::byte)0x1f;
        std::copy(f1, f1 + strlen(s.c_str()), reason_string_+1);
        std::copy(reason_string_, reason_string_ + 8, variable_header + 4);
        std::byte* username_in_bytes = reinterpret_cast<std::byte*>(username);
        std::copy(username_in_bytes, username_in_bytes + 8, variable_header+12);
        variable_header[20] = (std::byte) reason_codes[0];//payload
    };
};
#endif