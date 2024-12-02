#include <iostream>
#include <bitset>
#include <vector>
#include <limits>
#include <algorithm>
#include "../my_headers/misc.hpp"

using namespace std;

#define STRING_SIZE 8
#ifndef ENCODER_CONNACK
#define ENCODER_CONNACK
class connack //encodes connack response to connect response...
{
    unsigned char fixed_header = (unsigned char)0x00;
    unsigned int reason_codes_[22] = {0x00, 0x80, 0x81,0x82,0x83, 0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0X8C,0X90,0X95,0X97,0X99,
        0X9A, 0X9B, 0X9C, 0X9D,0X9F};
    string reasons[22] = {"Success", "Unspecified error", "Malformed Packet", "Protocol Error", "Implementation specific error",
    "Unsupported Protocol Version", "Client Identifier not valid", "Bad User Name or Password", "Not authorized", "Server unavailable",
    "Server busy", "Banned", "Bad authentication method", "Topic Name invalid", "Packet too large", "Quota exceeded", "Payload format invalid",
    "Retain not supported", "QoS not supported", "Use another server", "Server moved", "Connection rate exceeded"};
    string descriptions[22]={
        "The Connection is accepted.", 
        "The Server does not wish to reveal the reason for the failure, or none of the other Reason Codes apply.",
        "Data within the CONNECT packet could not be correctly parsed.",
        "Data in the CONNECT packet does not conform to this specification.",
        "The CONNECT is valid but is not accepted by this Server.",
        "The Server does not support the version of the MQTT protocol requested by the Client.",
        "The Client Identifier is a valid string but is not allowed by the Server.", 
        "The Server does not accept the User Name or Password specified by the Client",
        "The Client is not authorized to connect.",
        "The MQTT Server is not available.",
        "The Server is busy. Try again later.",
        "This Client has been banned by administrative action. Contact the server administrator.",
        "The authentication method is not supported or does not match the authentication method currently in use.",
        "The Will Topic Name is not malformed, but is not accepted by this Server.",
        "The CONNECT packet exceeded the maximum permissible size.",
        "An implementation or administrative imposed limit has been exceeded.",
        "The Will Payload does not match the specified Payload Format Indicator.",
        "The Server does not support retained messages, and Will Retain was set to 1.",
        "The Server does not support the QoS set in Will QoS.",
        "The Client should temporarily use another server.",
        "The Client should permanently use another server.",
        "The connection rate limit has been exceeded."
    };

    public:
    connack(){}
    std::byte variable_header[120];
    std::byte user_property_[STRING_SIZE+1], auth_data_self[STRING_SIZE+1], server_ref[STRING_SIZE+1], res_top[STRING_SIZE+1], auth_method_[STRING_SIZE+1], wildcard_sub[2], sub_id_available[2], 
        shared_sub_available[2], client_id[STRING_SIZE+1], reason_string_[STRING_SIZE+1], max_qos[3], retain_available[2], session_expiry[5], recieve_maximum[3], max_packet_size[5],
    topic_alias_max[3], server_keepalive[3];
    bool use_own_session_exp = true;

    void fixed_header_fill()
    {
        variable_header[0] = (std::byte)32;
    };
    void connack_flags(bool session_present)
    {
        int g = session_present?0x00:0x01;
        variable_header[1] = ((std::byte)g);
    };
    
    void assign_reason_codes()
    {
        variable_header[2] = (std::byte) reason_codes_[0];
    }
   void confirm_connack_flags()
   {
    if((int) variable_header[2] != 0)
    {
        variable_header[1] = (std::byte) 0x00;
        //if connect packet code contains clean state of 1, variable_header[1] must == 0
        //else if connect packet code contains clean state of 0, variable_header[0] must == 1
    }
   }
    auto property_length()
    {
        variable_header[3] = ((std::byte)0);
    }


 

    void one_byte_after(std::byte a, std::byte g[2], bool b)
    {
        g[0] = a;
        g[1] =  b?(std::byte)0x01:(std::byte)0x00;
    }

    void prototype_response_info(std::byte hb, string h,  std::byte f[8])
    {

        f[0] = hb;
        for(int i=1; i < 8;i++)
        {
            f[i] = (std::byte) h[i];
        };

    };

    void auth_data(std::byte p[8], std::byte c_[])
    {
        int length = sizeof(*c_)/sizeof(c_[0]);
        p[0] = (std::byte)0x16;
        for(int i=0; i < length; i++)
        {
            p[i] = c_[i];
        };
    };

    void mint_variable_header(std::string username, string response, string response_topic, string server_reference, string auth_method, std::byte auth_data_[], std::byte session_expiry_con[], bool session_present
    , std::byte* client_keep_alive)
    {
        //c++ can use degfault values like python
        string f[2] = {"", "rambo"};
        std::byte * auth_cache = new std::byte[8];
        //memset(auth_cache, 0, 8);

        variable_header[0] = (std::byte)32;
        variable_header[1] = (std::byte)1;
        variable_header[2] = (std::byte)0;
        variable_header[3] = (std::byte)0;
        if (use_own_session_exp)
        {
            session_expiry[0] = (std::byte)0x11;
            std::copy(session_expiry, session_expiry+4, variable_header + 4);
            //delete subscription id if present in the database...
        }
        else
        {
            std::copy(session_expiry_con, session_expiry_con +4, variable_header + 4);
        }
        recieve_maximum[0] = (std::byte)0x33;
        //encode integer into bytes
        std::copy(recieve_maximum, recieve_maximum + 2, variable_header + 8);

        max_qos[0] = (std::byte)0x24;  max_qos[1] = (std::byte)0x00;
        std::copy(max_qos, max_qos + 2, variable_header + 11);

        retain_available[0] = (std::byte)0x25; retain_available[1] = (std::byte)0x01;
        std::copy(retain_available, retain_available + 2, variable_header + 13);

       max_packet_size[0] = (std::byte)0x27; //default value = 200
       std::copy(max_packet_size, max_packet_size + 4, variable_header + 15);

       std::byte* usernamebytes = reinterpret_cast<std::byte*>((char*)username.c_str());
       client_id[0] = (std::byte)0x12;
       std::copy(usernamebytes, usernamebytes + 8, client_id+1);
       std::copy(client_id, client_id + 9, variable_header + 19);

       topic_alias_max[0] = (std::byte)0x22;//default value = 0x01
       std::copy(topic_alias_max, topic_alias_max + 3, variable_header+28);

       string rs = "success";
       std::byte* reason_string = reinterpret_cast<std::byte*>((char*)rs.c_str());
       reason_string_[0] = (std::byte)0x1f;
       std::copy(reason_string, reason_string + strlen(rs.c_str()), reason_string_+1);
       std::copy(reason_string_, reason_string_ + 8, variable_header + 31);

       //std::byte* userpropertystring = reinterpret_cast<std::byte*>((char*)user_proper)
       user_property_[0] = (std::byte)0x26;
       std::copy(user_property_, user_property_ + 8, variable_header + 39);

        //======================WILDCARD==SUBSCRIPTION=========================================================
        //If present, this byte declares whether the Server supports Wildcard Subscriptions
        //A value is 0 means that Wildcard Subscriptions are not supported.
        //A value of 1 means Wildcard Subscriptions are supported.
        //If not present, then Wildcard Subscriptions are supported
        //If the Server receives a SUBSCRIBE packet containing a Wildcard Subscription and it does not support Wildcard Subscriptions, 
        //This is a Protocol Error.
        //The Server uses DISCONNECT with Reason Code 0xA2 (Wildcard Subscriptions not supported) as described in section 4.13.
        //If a Server supports Wildcard Subscriptions, it can still reject a particular subscribe request containing a Wildcard Subscription.
        //In this case the Server MAY send a SUBACK Control Packet with a Reason Code 0xA2 (Wildcard Subscriptions not supported).

        one_byte_after((std::byte)0x28, wildcard_sub, false);
        std::copy(wildcard_sub, wildcard_sub + 2, variable_header+47);
        //=======================SUBSCRIPTION_IDS_AVAILABLE========================================================
        //If present, this byte declares whether the Server supports Subscription Identifiers.
        //A value is 0 means that Subscription Identifiers are not supported.
        //A value of 1 means Subscription Identifiers are supported.
        //If not present, then Subscription Identifiers are supported.
        //It is a Protocol Error to include the Subscription Identifier Available more than once, or to send a value other than 0 or 1.
        //If the Server receives a SUBSCRIBE packet containing Subscription Identifier and it does not support Subscription Identifiers, 
        //this is a Protocol Error.The Server uses DISCONNECT with Reason Code of 0xA1 (Subscription Identifiers not supported) as described in section 4.13.
        one_byte_after((std::byte) 0x29, sub_id_available,true);
        std::copy(sub_id_available, sub_id_available + 2, variable_header + 49);

        //======================SHARED_SUB_AVAILABLE========================================================================
        //f present, this byte declares whether the Server supports Shared Subscriptions. 
        //A value is 0 means that Shared Subscriptions are not supported. 
        //A value of 1 means Shared Subscriptions are supported. 
        //If not present, then Shared Subscriptions are supported. 
        //If the Server receives a SUBSCRIBE packet containing Shared Subscriptions and it does not support Shared Subscriptions, 
        //this is a Protocol Error.The Server uses DISCONNECT with Reason Code 0x9E (Shared Subscriptions not supported) as described in section 4.13.
        one_byte_after((std::byte)0x2a, shared_sub_available, true);
        std::copy(shared_sub_available, shared_sub_available+2, variable_header+51);
        //======================SERVER=KEEPALIVE===============================================================================
        //Followed by a Two Byte Integer with the Keep Alive time assigned by the Server. 
        //If the Server sends a Server Keep Alive on the CONNACK packet, the Client MUST use this value instead of the Keep Alive value the Client sent on CONNECT. 
        //If the Server does not send the Server Keep Alive, the Server MUST use the Keep Alive value set by the Client on CONNECT. 
        // It is a Protocol Error to include the Server Keep Alive more than once.
        //The primary use of the Server Keep Alive is for the Server to inform the Client that it will disconnect the Client for inactivity sooner than the Keep Alive specified by the Client.
        //if (client_keep_alive != NULL)
       // {
       //     std::copy(client_keep_alive, client_keep_alive + 3, variable_header + 53);
        //}
        //else
        //{
            server_keepalive[0] = (std::byte) 0x13;
            std::copy(server_keepalive, server_keepalive + 3, variable_header + 53);
        //}
        //===================RESPONSE=TOPIC==========================================================================================
        //Followed by a UTF - 8 Encoded String which is used as the basis for creating a Response Topic.
        //The way in which the Client creates a Response Topic from the Response Information is not defined by this specification.
        //It is a Protocol Error to include the Response Information more than once.
        //If the Client sends a Request Response Information with a value 1, it is OPTIONAL for the Server to send the Response Information in the CONNACK.
        // A common use of this is to pass a globally unique portion of the topic tree which is reserved for this Client for at least the lifetime of its Session.//NEED TO IMPLEMENT THIS
        // This often cannot just be a random name as both the requesting Client and the responding Client need to be authorized to use it.
        // It is normal to use this as the root of a topic tree for a particular Client.
        // For the Server to return this information, it normally needs to be correctly configured.
        //Using this mechanism allows this configuration to be done once in the Server rather than in each Client.
        string rstop = response_topic;
        std::byte* res_topic = reinterpret_cast<std::byte*>((char*)rstop.c_str());
        res_top[0] = (std::byte)0x1a;
        std::copy(res_topic, res_topic + strlen(rstop.c_str()), res_top+ 1);
        std::copy(res_top, res_top + 8, variable_header + 56);

        //====================SERVER=REFERENCE=======================================================================================
        //Followed by a UTF - 8 Encoded String which can be used by the Client to identify another Server to use.
        //It is a Protocol Error to include the Server Reference more than once.
        //The Server uses a Server Reference in either a CONNACK or DISCONNECT packet with Reason code of 0x9C (Use another server) or Reason Code 0x9D (Server moved) as described in section 4.13.
        std::byte* servref = reinterpret_cast<std::byte*>((char*)server_reference.c_str());
        server_ref[0] = (std::byte)0x1c;
        std::copy(servref, servref + strlen(server_reference.c_str()), server_ref + 1);
        std::copy(server_ref, server_ref + 8, variable_header + 64);

        //===========================AUTH=METHOD==================================================================================
        //Followed by a UTF - 8 Encoded String containing the name of the authentication method.
        //It is a Protocol Error to include the Authentication Method more than once.Refer to section 4.12 for more information about extended authentication.
        std::byte* authm = reinterpret_cast<std::byte*>((char*)auth_method.c_str());
        auth_method_[0] = (std::byte)0x15;
        std::copy(authm, authm + strlen(auth_method.c_str()), auth_method_ + 1);
        std::copy(auth_method_, auth_method_ + 8, variable_header + 72);
       
        //===========================AUTH=DATA============================================================================================
        //Followed by Binary Data containing authentication data.
        //The contents of this data are defined by the authentication method and the state of already exchanged authentication data.
        //It is a Protocol Error to include the Authentication Data more than once.Refer to section 4.12 for more information about extended authentication.
        auth_data_self[0] = (std::byte)0x16;
        std::copy(auth_data_,auth_data_+1, auth_data_self+ 1); //increase auth data size to 8
        std::copy(auth_data_self, auth_data_self + 8, variable_header + 80);
    };

};
#endif
