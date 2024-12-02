#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"

#ifndef ENCODER_PUBACK
#define ENCODER_PUBACK

class puback{
    unsigned int reason_codes[9] = {0x00, 0x10, 0x80,0x83,0x87,0x90,0x91,0x97, 0x99};
    std::string reason_descriptions [9] = {"Success", "No matching subscribers", "Unspecified error","Implementation specific error","Not authorized","Topic Name invalid",
    "Packet Identifier in use","Quota exceeded", "Payload format invalid"};
    std::string ack_messages[9] = {"The message is accepted. Publication of the QoS 2 message proceeds.", "The message is accepted but there are no subscribers",
    "The receiver does not accept the publish", "The PUBLISH is valid but the receiver is not willing to accept it.", "The PUBLISH is not authorized.",
    "The Topic Name is not malformed, but is not accepted by this Client or Server.", "The Packet Identifier is already in use",
    "An implementation or administrative imposed limit has been exceeded.", "The payload format does not match the one specified in the Payload Format Indicator."};
    std::byte user_property_[12], packet_id_[2], reason_string_[12];

    public:
    std::byte variable_header[32];
    std::byte fixed_header_fill()
    {
        return fixed_header_fill_obj("1000000");
    }
    
    std::byte property_length() //work on this as soon as possible
        {
            return (std::byte)0x00;
        }

    void mint_var_header()
    {
        std::string f_[2] = {"", ""};
        variable_header[0] = (std::byte)64;
        std::copy(packet_id_, packet_id_ + 2, variable_header + 1);
        variable_header[3] = (std::byte)0x00;// get_reason_code(0, reason_codes);
        variable_header[4] = (std::byte)0x00; //property_length();
std::string s = "success";
        std::byte* f = reinterpret_cast<std::byte*>((char*)s.c_str());
        reason_string_[0] = (std::byte)0x1f;
        std::copy(f, f + strlen(s.c_str()), reason_string_ + 1);
        std::copy(reason_string_, reason_string_ + 12, variable_header + 5);
        std::string s2 = "";
        std::byte* f2 = reinterpret_cast<std::byte*>((char*)s2.c_str());
        user_property_[0] = (std::byte)0x26;
        std::copy(f2, f2 + strlen(s2.c_str()), user_property_ + 1);
        std::copy(user_property_, user_property_ + 12, variable_header + 17);
    }
    
};

class pubrec //the broker sends a pubrecieved as part of qos2 to the client
{
    unsigned int records[9] = {0x00, 0x10, 0x80, 0x83, 0x87, 0x90,0x91,0x97,0x99};
    std::byte user_property_[8], packet_id_[2], reason_string_bytes[8];
    public:
    std::byte variable_header[32];
    
    std::byte property_length()
    {
        return (std::byte)0;
    }
    void mint_variable_header(std::string username)
    {
        std::string reason_string_ = "success";
        variable_header[0] = fixed_header_fill_obj("01010000");
        std::copy( packet_id_, packet_id_+2, variable_header+1);
        variable_header[3] = (std::byte)0x00;
        variable_header[4] = (std::byte)0x00;

       std::string m = "success";
        std::byte* flo = reinterpret_cast<std::byte*>((char*)m.c_str());
       reason_string_bytes[0] = (std::byte)0x1f;
       std::copy(flo, flo + strlen(m.c_str()), reason_string_bytes + 1);
       std::copy(reason_string_bytes, reason_string_bytes+8, variable_header+5);
       std::byte* f2 = reinterpret_cast<std::byte*>((char*)username.c_str());
       user_property_[0] = (std::byte)0x26;
       std::copy(f2, f2 + strlen(username.c_str()), user_property_+1);
       std::copy(user_property_, user_property_ + 8, variable_header + 13);
    }

};

class pubcomp //true for pubrel, false for pubcomp...client sends pubrel upon recieving a pubrec and the server responds with a pubcomp packet.
{
    int fixed_header=0, reason_code=0, property_length=0;
    unsigned int reason_codes[2] = {0x00, 0x92}; 
    public:
    std::byte variable_header[32], packet_id_[2], reason_string_[8], user_property_[8], reason_string_bytes[8];
    pubcomp(){}

    void mint_packet(std::string username)
    {
        std::string reason_string_f="success";
        std::string f[]={"", username};
        variable_header[0] = fixed_header_fill_obj("01110000");
        std::copy(packet_id_, packet_id_ + 2, variable_header + 1);
        variable_header[3] = (std::byte)reason_codes[0];
        variable_header[4] = (std::byte) 0;
        std::string c = "success";
        std::byte* f1 = reinterpret_cast<std::byte*>((char*)c.c_str());
        reason_string_bytes[0] = (std::byte)0x1f;
        std::copy(f1, f1 + 8, reason_string_bytes + 1);
        std::copy(reason_string_bytes, reason_string_bytes + 8, variable_header + 5);
        std::byte* f2 = reinterpret_cast<std::byte*>((char*)username.c_str());
        user_property_[0] = (std::byte)0x26;
        std::copy(f2, f2 + strlen(username.c_str()), user_property_);
        std::copy(user_property_, user_property_ + 8, variable_header + 13);
    }

};
#endif