

#include "my_headers/misc.hpp"
#include "my_headers/general_decoder.hpp"
#ifndef DECODER_CONNECT_H
#define DECODER_CONNECT_H

class connack_decoder{ //decodes connect bytes sent from the client.
    public:
    decoder connack_decoder_;
    struct variable_header
    {
    char fixed_header = '0';
    char protocol_name[6];
    int protocol_version=0;
    int connect_flag=0;
    char keep_alive[2];
    char session_expiry[5];
    char recieve_maximum[3]; //this value must be accessible to the publish function so as to ascertain if the client is sending more than the recieve_maximum size
    char maximum_bytes[5];
    char topic_alias_max[3];
    char req_res_info[2];
    char req_prob_info[2];
    char user_property[12];
    char auth_method[8];
    char auth_data[8];
    char key_handle[3];
    char value_handle[8];
    };
    struct payload
    {
        char client_id[8];
        char property_length;
        char will_delay_interval[5];
        char payload_format_indicator[2];
        char message_expiry[5];
        char content_type[8];
        char res_topic[8];
        char null_byte[8];
        char user_property[8];
        char will_topic[8];
        char will_payload[8];
        char username[8];
        char password[8];
    };
    struct connect_flags
    {
        bool username=false, password=false,will_retain=false,will_qos_hb=false, will_qos_lb=false, will_flag=false, clean_start=false;
    };
    struct output
    {
        int recieve_maximum = 0;
    };
    variable_header header = variable_header();
    payload payload_ = payload();
    connect_flags connect_flags_ = connect_flags();
    output output_ = output();
    bool session_present = false;

    void decode_packet(char * packet)
    {
        view_int(packet, 170);
        view_char(packet, 170);
        std::string descriptor = connack_decoder_.decode_bytes(packet);
        //a series of bytes is digested and the components absorbed into the decoder for use when minting a connack response
        std::copy(packet + 1, packet + 7, header.protocol_name);
        header.protocol_version = static_cast<int>(packet[7]);
       header.connect_flag= static_cast<int>(packet[8]);
       std::copy(packet + 9, packet + 11, header.keep_alive); //missing bytes between 11 and 15
       std::copy(packet + 15, packet + 20, header.session_expiry);
       std::copy(packet + 20, packet + 23, header.recieve_maximum);
       std::copy(packet + 23, packet + 28, header.maximum_bytes);   
       std::copy(packet + 28, packet + 31, header.topic_alias_max);
       std::copy(packet + 31, packet + 33, header.req_res_info);
       std::copy(packet + 33, packet + 35, header.req_prob_info);
       std::copy(packet + 35, packet + 47, header.user_property);
       header.auth_method[0] = packet[47];
       header.auth_data[0] = packet[48];
       std::copy(packet + 49, packet + 57, payload_.client_id);
        payload_.property_length=packet[57];
        std::copy(packet + 58, packet + 63, payload_.will_delay_interval);
        std::copy(packet + 63, packet + 65, payload_.payload_format_indicator);
        std::copy(packet + 65, packet + 70, payload_.message_expiry);
        std::copy(packet + 70, packet + 78, payload_.content_type);
        std::copy(packet + 78, packet + 86, payload_.res_topic);
        std::copy(packet + 86, packet + 94, payload_.null_byte);
        std::copy(packet + 94, packet + 102, payload_.user_property);
        std::copy(packet + 102, packet + 110, payload_.will_topic);
        std::copy(packet + 110, packet + 118, payload_.will_payload);
        std::copy(packet + 118, packet + 126, payload_.username);
        std::copy(packet + 126, packet + 134, payload_.password);
        parse_user_bytes();
        output_.recieve_maximum = decode_variable_integer(header.recieve_maximum);
    };
    void parse_byte_8()
    {
        std::string bin_string = std::bitset<8>(this->header.connect_flag).to_string();
        connect_flags_.username = string_to_bool(bin_string[0]);
        connect_flags_.password = string_to_bool(bin_string[1]);
        connect_flags_.will_retain = string_to_bool(bin_string[2]);
        connect_flags_.will_qos_hb =string_to_bool(bin_string[3]);
        connect_flags_.will_qos_lb = string_to_bool(bin_string[4]);
        connect_flags_.will_flag = string_to_bool(bin_string[5]);
        connect_flags_.clean_start = string_to_bool(bin_string[6]);
    }
    void parse_user_bytes()
    {
        int user_length = (int)sizeof(header.user_property) / sizeof(header.user_property[0]);
        for (int i = 0; i < 3; i++)
        {
           header.key_handle[i] = header.user_property[1 + i];
        }
        for (int i = 0; i < user_length - 4; i++)
        {
            header.value_handle[i] = header.user_property[i + 4];
        }
    }
    void view()
    {
        std::cout << "fixed header: " << header.fixed_header << std::endl;
        std::cout << "protocol name: ";
        view_char(header.protocol_name, 6);
        std::cout << "protocol version: " << header.protocol_version << std::endl;
        std::cout << "user property: ";
        view_char(header.user_property, 12);
        std::cout << "key handle: ";
        view_char(header.key_handle, 3);
        std::cout << "value handle: ";
        view_char(header.value_handle, 8);
        std::cout << "sesh exp id: " << (int)header.session_expiry[0] << std::endl;
    }

};
#endif