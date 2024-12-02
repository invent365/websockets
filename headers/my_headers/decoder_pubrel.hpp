#include <iostream>
#include <bitset>
#include <vector>
#include <algorithm>
#include "../my_headers/misc.hpp"
#include "../my_headers/general_decoder.hpp"

#ifndef DECODER_PUBREL
#define DECODER_PUBREL


//FOR PUBREL, PUBCOMP, PUBREC and PUBACK responses, ENSURE THAT THE SERVER DOES NOT SEND THE REASON STRING OR USER PROPERTY IF NOT REQUIRED.
class pubrel_decoder //true for pubrel, false for pubcomp...client sends pubrel upon recieving a pubrec and the server responds with a pubcomp packet.
{
public:
    std::byte packet_id_[2], reason_string_[8];
    int fixed_header = 0, reason_code=0, property_length=0, index=0;
    unsigned int reason_codes[2] = { 0x00, 0x92 };
    string reason_strings[2] = { "Success", "Packet Identifier not found" };
    std::byte user_property_[12], reason_string_bytes[12], username[11];
    pubrel_decoder(){}
    void decode_packet(std::byte* variable_header)
    {
        string reason_string_f = "";
        string f[] = { "", "" };
        fixed_header = (int)variable_header[0];
        std::copy(variable_header+1, variable_header+3, packet_id_);
        reason_code = (int)variable_header[3];
        property_length = (int)variable_header[4];
        index = get_index((int*) reason_codes, reason_code);
        std::copy(variable_header+5, variable_header+17, reason_string_bytes);
        //parse_packet((char*)reason_strings[index].c_str(), (char*) reason_string_bytes, 0, sizeof reason_string_bytes / sizeof reason_string_bytes[0]);
        std::copy(variable_header+17, variable_header+29, user_property_);
        std::copy(variable_header+18, variable_header+29, username);
    }
    int get_index(int rc[], int query)
    {
        int length = sizeof *rc / sizeof rc[0];
        auto ptr = std::find(rc, rc + length, query);
        return static_cast<int>(ptr - rc);
    }
    void view()
    {
        cout << "fixed header" << fixed_header << endl;
        cout << "username: ";
        cout << std::string(reinterpret_cast<const char*>(user_property_)) << endl;
        cout << "reason_code" << reason_code << endl;
        cout << "reason_string" << endl;
        view_char((char *) reason_string_bytes, sizeof reason_string_bytes / sizeof reason_string_bytes[0]);
    }
    int parse_fixed_header()
    {
        string g = std::bitset<8>(fixed_header).to_string();
        if (g[7] != 0 && g[6] != 1 && g[5] != 0 && g[4] != 0)
        {
            perror("Malformed pubrel packet");
            return 1;
        }
        return 0;
    }

};

#endif