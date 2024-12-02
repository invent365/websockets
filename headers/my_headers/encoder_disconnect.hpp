#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"

#ifndef ENCODER_DISCONNECT
#define ENCODER_DISCONNECT
class disconnect{
    unsigned int disconn_codes[29] = {0x00, 0x04, 0x80, 0x81, 0x82, 0x83, 0x87,0x89, 0x8b, 0x8d, 0x8e, 0x8f,0x90, 0x93, 0x94,0x95, 0x96,0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2};
    std::byte user_property_[8], session_expiry[5];
    std::byte server_reference_[8], reason_string_[8];
        public:
    std::byte variable_header[32];
     std::byte property_length()
    {
        return (std::byte)0;
    }
    
    
    
    void mint_variable_header()
    {
        string server_reference = "";
        variable_header[0] = fixed_header_fill_obj("11100000");
        variable_header[1] = (std::byte)0x00;
        variable_header[2] = (std::byte)0x00;
        session_expiry[0] = (std::byte)0x11;
        std::copy(session_expiry, session_expiry + 5, variable_header + 3);
        string s = "success";
        std::byte* f1 = reinterpret_cast<std::byte*>((char*)s.c_str());
        reason_string_[0] = (std::byte)0x1f;
        std::copy(f1, f1 + strlen(s.c_str()), reason_string_ + 1);
        std::copy(reason_string_, reason_string_ + 8, variable_header + 8);
        string s2 = "rambo";
        std::byte* f2 = reinterpret_cast<std::byte*>((char*)s2.c_str());
        user_property_[0] = (std::byte)0x26;
        std::copy(f2, f2 + strlen(s2.c_str()), user_property_ + 1);
        std::copy(user_property_, user_property_ + 8, variable_header + 16);
        std::byte* f3 = reinterpret_cast<std::byte*>((char*)server_reference.c_str());
        server_reference_[0] = (std::byte)0x1c;
        std::copy(f3, f3 + strlen(server_reference.c_str()), server_reference_ + 1);
        std::copy(server_reference_, server_reference_ + 8, variable_header + 24);
    }
    

};
#endif