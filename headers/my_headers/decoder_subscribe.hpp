#include <algorithm>
#include <iostream>
#include "my_headers/general_decoder.hpp"
#include "my_headers/misc.hpp"


#ifndef DECODER_SUBSCRIBE
#define DECODER_SUBSCRIBE
constexpr auto BYTE_SIZE = 12;
class subscribe_decoder
{
    
    decoder subscribe_decoder_ = decoder();
    int fixed_header = 0;
    
    public:
       subscribe_decoder() {};
   
    int property_length = 0, subscription_options=0;
    char packet_id_[2], sub_identifier[5], user_property_[BYTE_SIZE], topic_filter[BYTE_SIZE], mlk[BYTE_SIZE];
    char key_handle[3], value_handle[BYTE_SIZE], id_[4];
    bool qos_lower = false, qos_upper = false, no_local_option = false, retain_as_published = false, retain_handling_upper = false, retain_handling_lower = false;
    struct output
    {
        bool max_qos_hb = false;
        bool max_qos_lb = false;
        bool include_client_own_id = false;
        bool keep_retain_flag = false;
        int retain_state = 0;
        

        //Bit 2 of the Subscription Options represents the No Local option.If the value is 1, Application Messages MUST NOT be forwarded to a connection with a ClientID equal to the ClientID of the publishing connection
        //It is a Protocol Error to set the No Local bit to 1 on a Shared Subscription
    };
    output output_ = output();
    void parse_sub_identifier()
    {
        std::copy(sub_identifier + 1, sub_identifier + 4, id_);
    }
    void parse_user_bytes()
    {
        std::copy(user_property_, user_property_ + 1, key_handle);
        std::copy(user_property_ + 1, user_property_ + 12, value_handle);
    }

    int parse_options()
    {
        //=========================================OPTIONS=========================================================
    
       // Bit 3 -> Retain As Published option.If 1, Application Messages to this subscription keep the RETAIN flag they were published with.
       // If 0, Application Messages forwarded using this subscription have the RETAIN flag set to 0.
       // Retained messages sent when the subscription is established have the RETAIN flag set to 1.
       // 
       //Bits 4 and 5 -> Retain Handling option.
       //This option specifies whether retained messages are sent when the subscription is established.
       //This does not affect the sending of retained messages at any point after the subscribe.
       // If there are no retained messages matching the Topic Filter, all of these values act the same.The values are :
       // 0 = Send retained messages at the time of the subscribe
       // 1 = Send retained messages at subscribe only if the subscription does not currently exist
       // 2 = Do not send retained messages at the time of the subscribe
       // It is a Protocol Error to send a Retain Handling value of 3.
       // 
       
       // THE BELOW ARE NOT IMPORTANT CURRENTLY
       // Not sending retained messages for an existing subscription is useful when a reconnect is done and the Client is not certain whether the subscriptions were completed in the previous connection to the Session.
       // Not sending stored retained messages because of a new subscription is useful where a Client wishes to receive change notifications and does not need to know the initial state.
       // For a Server that indicates it does not support retained messages, all valid values of Retain As Published and Retain Handling give the same result which is to not send any retained messages at subscribe and to set the RETAIN flag to 0 for all messages.

        std::string h = std::bitset<8>(subscription_options).to_string();
        bool reserved1 = string_to_bool(h[0]), reserved2 = string_to_bool(h[1]);
        if (reserved1 != false && reserved2 != false)
        {
            perror("Malformed subsrciption packet...");

        }
        retain_handling_upper = string_to_bool(h[2]);
        retain_handling_lower = string_to_bool(h[3]);
        retain_as_published = string_to_bool(h[4]);
        no_local_option = string_to_bool(h[5]);
        qos_upper = string_to_bool(h[6]);
        qos_lower = string_to_bool(h[7]);
        output_.include_client_own_id = !no_local_option;//Application Messages MUST NOT be forwarded to a connection with a ClientID equal to the ClientID of the publishing connection
        output_.keep_retain_flag = retain_as_published;
        std:: string s{ h[2] }, s1{ h[3] };
        output_.retain_state = std::stoi(s + s1, nullptr, 2);//these affect only messages AT THE TIME OF SUBSCRIPTION, and not afterwards.
        return 0;
    }


    void decode_packet(char *packet)
    {
        cout << "subscribing..." << endl;
        fixed_header = (int) packet[0];
        std::copy(packet + 1, packet + 3, packet_id_);
        property_length = (int) packet[3];
        //=====================================SUB_IDENTIFIER==========================================
        //Followed by a Variable Byte Integer representing the identifier of the subscription.
        //The Subscription Identifier can have the value of 1 to 268, 435, 455. 
        //It is a Protocol Error if the Subscription Identifier has a value of 0. 
        //It is a Protocol Error to include the Subscription Identifier more than oncE.
        //The Subscription Identifier is associated with any subscription created or modified as the result of this SUBSCRIBE packet.
        //If there is a Subscription Identifier, it is stored with the subscription.
        //If this property is not specified, then the absence of a Subscription Identifier is stored with the subscription.
        std::copy(packet + 4, packet + 9, sub_identifier);
        std::copy(packet + 9, packet + 21, user_property_);  
       std::copy(packet+21, packet+33, topic_filter);
        subscription_options = (int) packet[33];
        parse_user_bytes();
        parse_sub_identifier();
        parse_options();
    }
    void view()
    {
        cout << "subscription view" << endl;
        cout << "property length: " << property_length << endl;
        cout << "sub_id: \n";
        view_int(sub_identifier, sizeof sub_identifier / sizeof sub_identifier[0]);
        cout << "\nextracted_id: ";
        view_int(id_, sizeof id_ / sizeof id_[0]);
        cout << "user_property: \n";
        view_int(user_property_, sizeof user_property_ / sizeof user_property_[0]);
        cout << "topic filter: \n";
        view_int(topic_filter, sizeof topic_filter / sizeof topic_filter[0]);
    }
};
#endif