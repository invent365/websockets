#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"
#include "../my_headers/general_decoder.hpp"

#ifndef DECODER_UNSUBSCRIBE
#define DECODER_UNSUBSCRIBE
class unsubscribe_decoder
{
    decoder unsubscribe_decoder_ = decoder();
    int fixed_header=0, property_length=0;

    public:
    char packet_id[2], user_property_[12], topic[12];
    int parse_fixed_header()
    {
        string h = std::bitset<8>(fixed_header).to_string();
        if ((h[7] != '0') && (h[6] != '1') && (h[5] != '0') && (h[4] != '0'))
        {
            perror("malformed packet");
            return 1;
        }
        return 0;
    }
    void decode_packet(char * packet)
    {
        fixed_header = (int) packet[0];
        std::copy(packet + 1, packet + 3, packet_id);
        property_length = packet[3];
        std::copy(packet + 4, packet + 16, user_property_);
        std::copy(packet + 16, packet + 28, topic);
        if (string(topic) == "")
        {
            perror("Payload must contatin topic...");
        }
        parse_fixed_header();
    }
    void view()
    {
        cout << "unsubscribe_packet: " << endl;
        cout << "user_property: " << endl;
        view_char(user_property_, sizeof user_property_ / sizeof user_property_[0]);
        cout << "\n topic: " << endl;
        view_char(topic, sizeof topic / sizeof topic[0]);
    }
};
#endif