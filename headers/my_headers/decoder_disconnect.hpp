
#include "../my_headers/misc.hpp"
#include "../my_headers/general_decoder.hpp"
#include <algorithm>
#ifndef DECODER_DISCONNECT
#define DECODER_DISCONNECT

class disconnect_decoder
{
    decoder unsubscribe_decoder = decoder();
    int fixed_header=0, property_length=0, reason_code=0;

    public:
    char session_expiry[5], reason_string[12], user_property_[12], username[11]; 
    void decode_packet(char*packet)
    {
        fixed_header = (int) packet[0];
        reason_code = (int) packet[1];
        property_length = (int) packet[2];
        std::copy(packet+3, packet+8, session_expiry);
        std::copy(packet+8, packet+20, reason_string);
        std::copy(packet+20, packet+32, user_property_);
        std::copy(packet+21, packet+32, username);
    }

    void view()
    {
        cout << "disconnect packet: " << endl;
        cout << "reason_code: " << reason_code << endl;
        cout << "session_expiry: " << endl;
        view_int(session_expiry, 5);
        cout << "\nreason_string: " << endl;
        view_char(reason_string, 8);
        cout << "\nuser_property: " << endl;
        view_char(user_property_, 12);

    }
};
#endif