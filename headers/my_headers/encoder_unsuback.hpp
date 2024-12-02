#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"


#ifndef ENCODER_UNSUBACK
#define ENCODER_UNSUBACK
class unsuback
{
    unsigned int response_codes[7] = {0x00, 0x11,0x80, 0x83, 0x87, 0x8f, 0x91};
    string reasons[7] = { "Granted QoS 0" };
     std::byte user_property_[8], packet_id_[2], reason_string_[8];
    public:
    std::byte variable_header[32];
    
    std::byte property_length()
    {
       return (std::byte)0;
    }
    void mint_variable_header(string response, char* username, char * unsub_packet_id)
    {
        string f[2] = {"", username};
        variable_header[0] = fixed_header_fill_obj("10110000");
        std::byte* f1 = reinterpret_cast<std::byte*>(unsub_packet_id);
        std::copy(f1, f1 + 2, packet_id_);
        std::copy(packet_id_, packet_id_ + 2, variable_header + 1);
        variable_header[3] = property_length();
        reason_string_[0] = (std::byte)0x1f;
        std::string s = "success";
        std::byte* g = reinterpret_cast<std::byte*>((char*)s.c_str());
        std::copy(g, g + strlen(s.c_str()), reason_string_ + 1);
        std::copy(reason_string_, reason_string_ + 8, variable_header + 4);
        std::byte* g1 = reinterpret_cast<std::byte*>(username);
        std::copy(g1, g1 + sizeof g1 / sizeof g1[0], variable_header + 12);
        variable_header[20] = (std::byte)response_codes[0]; //payload
    };
};
#endif