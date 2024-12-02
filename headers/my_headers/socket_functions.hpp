

#include <iostream>
#include <bitset>
#include <vector>
#include<algorithm>
#include<iterator>
#include <map>
#include <regex>
#include<typeinfo>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h> 
#include <openssl/evp.h>
#include "../my_headers/decoder_connect.hpp"
#include "../my_headers/encoder_connack.hpp"
#include "../my_headers/general_decoder.hpp"
#include "../my_headers/decoder_publish.hpp"
#include "../my_headers/encoder_puback.hpp"
#include "../my_headers/decoder_subscribe.hpp"
#include "../my_headers/decoder_disconnect.hpp"
#include "../my_headers/decoder_unsubscribe.hpp"
#include "../my_headers/encoder_disconnect.hpp"
#include "../my_headers/encoder_suback.hpp"
#include "../my_headers/encoder_unsuback.hpp"
#include "../my_headers/decoder_pubrel.hpp"
#include "../my_headers/connect_flags.hpp"

#include "../my_headers/serverdisonnect.hpp"
#include "../my_headers/serverpublish.hpp"


using namespace std;

#ifndef SOCKET_FUNCTIONS
#define SOCKET_FUNCTIONS


std:: string char_to_string(char* f)
{
    auto g = reinterpret_cast<const char*>(f);
    return std::string(g);
}


void connect_(connack_decoder con_dec, connack con, std::map<string, string>& session_ids, std::map<string, Will_message>& wills, char recvbuf[8]) //std::map<string, string>&session_ids
{

    //Clients are allowed to send further MQTT Control Packets immediately after sending a CONNECT packet; Clients need not wait for a CONNACK packet to arrive from the Server.
//If the Server rejects the CONNECT, it MUST NOT process any data sent by the Client after the CONNECT packet except AUTH packets
//The Client accepts that any data that it sends before it receives a CONNACK packet from the Server will not be processed if the Server rejects the connection.
//Clients that send MQTT Control Packets before they receive CONNACK will be unaware of the Server constraints and whether any existing Session is being used.
//The Server can limit reading from the Network Connection or close the Network Connection if the Client sends too much data before authentication is complete.
//This is suggested as a way of avoiding denial of service attacks.

    std::cout << "connecting..." << endl;
    
    std::byte auth_data_[1];
    decoder decoder_;
    ServerPublish spub = ServerPublish();
    spub.mint_variable_header(char_to_string(con_dec.payload_.will_topic), char_to_string(con_dec.payload_.res_topic), char_to_string(con_dec.payload_.content_type), char_to_string(con_dec.payload_.username), char_to_string(con_dec.payload_.will_payload));
    // std::cout << "queue size: " << SSLState.size() << endl;
    con_dec.decode_packet(recvbuf);
    con_dec.parse_byte_8();
    con_dec.view();
    std::string s = string(con_dec.header.value_handle);
    std::cout << "Your username is " << s << endl;
    connect_flag_actions cfa = connect_flag_actions(con_dec.connect_flags_, con_dec.payload_, string(con_dec.payload_.username), string(con_dec.payload_.password));
    cfa.check_execute(session_ids, wills, con_dec.payload_.client_id);

    //you can authenticate the connection for business critical data. Send NO response if authentication fails...
    con.mint_variable_header(con_dec.payload_.username, "success", "", "", "", auth_data_, reinterpret_cast<std::byte*>(con_dec.header.session_expiry), con_dec.session_present, reinterpret_cast<std::byte*>(con_dec.header.keep_alive)); //mint a connack response and send...
    //view_int(reinterpret_cast<char*>(con.variable_header), sizeof con.variable_header);
    //SSL_write(ssl, con.variable_header, sizeof con.variable_header);
    reinterpret_cast<const char*>(con.variable_header);

};

void publish_(publish_decoder pub_dec, pubrec pub) //work on this function next...
{
    //The Server uses a PUBLISH packet to send an Application Message to each Client which has a matching subscription.
    //When Clients make subscriptions with Topic Filters that include wildcards,
      //  it is possible for a Client’s subscriptions to overlap so that a published message might match multiple filters.
       // In this case the Server MUST deliver the message to the Client respecting the maximum QoS of all the matching subscriptions.
       // protected Object clone() throws CloneNotSupportedException {
       // In addition, the Server MAY deliver further copies of the message, one for each additional matching subscription and respecting the subscription’s QoS in each case.

    bool retain_available = true; //REVISIT
    char recvbuf[8];
    pub_dec.decode_packet(recvbuf, retain_available);
    pub_dec.view_packet();
    std::cout << "Bitstring: " << pub_dec.bitstring << endl;
    std::cout << "username: " << pub_dec.username << endl;

    std::cout << "topic_name: " << string(pub_dec.topic_name) << endl;
    //SSLState[string(reinterpret_cast<const char*>(pub_dec.username))] = ssl;
    // try
    // {
        // auto h = pg.retrieve_query((string)"SELECT DISTINCT sub FROM mqtt_subs WHERE topic ='" + string(pub_dec.topic_name) + (string)"'");
         //pleas try to add the sub identifier..
    std::cout << "connected  s are: " << std::endl;
    ServerPublish spub = ServerPublish();
    spub.mint_variable_header(string(reinterpret_cast<const char*>(pub_dec.topic_name)), string(reinterpret_cast<const char*>(pub_dec.response_topic)), "text", string(reinterpret_cast<const char*>(pub_dec.username)), reinterpret_cast<const char*>(pub_dec.message));
    // for (auto [h_] : h)
    // {

         //THIS CODE BELOW IS NOT CORRECT. THE SERVER HAS THE ABILITY TO PUBLISH PACKETS TO THE CLIENTS...
         //take, for every matching client which has subscribed to a specific topic from Postgres, their subscription identifiers and add it to the packets...
         //Only a subscription id responsible for routing the publish packets, or causing their flow can be added to the packets.
         ///In shared subscriptions, only sub ids that correspond to an individual client are given to the clients.
         //if the recieve_maximum has been exceeded on the server or client, a disconnect packet must be sent with a code of 0x93. (Receive Maximum exceeded) 
         //the server must not delay the sending of any other type of packets apart from the publish packets due to delays in responses to recieve maximum. 
        // std::std::cout << "h: " << h_ << "  : " <<  get_socket_address( .at(h_)) << '\n';
         //std::std::cout << "size: " << SSLState.size() << endl;
        // string s = (string)"menu";
         //screen_map(SSLState);
      //   int response_code = send( .at(h_), reinterpret_cast<const char*>(spub.variable_header), sizeof spub.variable_header / sizeof spub.variable_header[0], 0);
         //int response_code = SSL_write(SSLState.at(s), spub.variable_header, sizeof spub.variable_header);
        // std::std::cout << " : " <<  get_socket_address( [h_]) << "Response_code: " << response_code << std::endl;
     //}
// }
// catch (exception& e)
// {
//      std::std::cout << e.what() << std::endl;
 // }
    if (pub_dec.qos_num == 0)
    {
        return;
    }
    if (pub_dec.qos_num == 1)
    {
        puback pub2 = puback();
        pub2.mint_var_header();
        //return reinterpret_cast<const char*>(pub2.variable_header); // sizeof pub2.variable_header / sizeof pub2.variable_header[0], 0);

        return;
    }
    pub.mint_variable_header((std::string)"hello");
    //std::std::cout << "PUB.VARIABLE_HEADER: ";
   // view_int(reinterpret_cast<char*>(pub.variable_header), 32);
   // std::cout << std::endl;
    //send(new_conn, reinterpret_cast<const char*>(pub.variable_header), sizeof pub.variable_header / sizeof pub.variable_header[0], 0);
};

void subscribe_(subscribe_decoder sub_dec, suback sub)
{
    //The Server need not use the same set of Subscription Identifiers in the retransmitted PUBLISH packet
    //The server can use old or new identifiers if the same type of subscription is submitted twice. However, after sending a publish packet, the old identifier becomes invalid.
      //if shared & old
        //{
        //Add the client to the shared topic but do not send any retained messages
        // sub_id gets replaced...
        //}
        //if shared & new
        //{Add the client to the shared topic but do not send any retained messages
        //}
        //if multiple topics are present in the packet, treat them sequentially, but produce only one suback containing their responses to the client. Each response must have an associated reason code and 
        // (optional) a max qos or failure. This qos is minimum(QOS of message being recieved and max qos of the server).
    ServerPublish spub = ServerPublish();
    char recvbuf[8];
    sub_dec.decode_packet(recvbuf);
    sub_dec.view();
    string x = std::to_string(decode_variable_integer(sub_dec.id_));//string(reinterpret_cast<const char*>(sub_dec.id_));//fault...decode the char array as an integer and convert to string...
    try
    {
        //pqxx::internal::result_iteration<std::string>  h = pg.retrieve_query("select exists(select 1 from topics where topic = '" + char_to_string(sub_dec.topic_filter) + "')");

    }
    catch (exception& e)
    {
        std::cout << "stoppage at line 166: " << e.what() << endl;
    }


    // if ((sub_dec.retain_handling_upper == 0||sub_dec.retain_handling_upper == 1) && (sub_dec.retain_handling_lower == 0))
   //  {
   //      auto h = pg.retrieve_query("select message from mqtt_messages where topic = '" + char_to_string(sub_dec.topic_filter) + "';");
   //      for (auto [h_] : h)
   //      {
    //         spub.mint_variable_header(sub_dec.topic_filter, "", "text", string(sub_dec.user_property_), h_);
   //      }


   //  }
    std::cout << "identifier: " << (int)sub_dec.sub_identifier[0] << endl;
    std::cout << "persona_string: " << string(sub_dec.topic_filter) << endl;
    std::cout << "topic: " << (int)sub_dec.user_property_[0] << endl;
    std::cout << "string: " << string(sub_dec.user_property_) << endl;
    // try
    // {
   //      pg.insert_query((string)"insert into mqtt_subs values ('" + x + "', '" + string(sub_dec.value_handle) + "', '" + string(sub_dec.topic_filter) + "');");
   //  }
   //  catch (pqxx::sql_error& e)
   //  {
   //      std::cerr << e.what() << endl;
   // }
    sub.mint_variable_header((std::byte*)sub_dec.packet_id_, sub_dec.user_property_);
    //send(new_conn, reinterpret_cast<const char*> (sub.variable_header), sizeof sub.variable_header / sizeof sub.variable_header[0], 0);
    view_int(reinterpret_cast<const char*> (sub.variable_header), 32);
    // SSL_write(ssl, reinterpret_cast<const void*> (sub.variable_header), sizeof sub.variable_header / sizeof sub.variable_header[0]);

};

void unsubscribe_(unsubscribe_decoder unsub_dec, unsuback unsub)
{
    //Even where no Topic Subscriptions are deleted, the Server MUST respond with an UNSUBACK
   //If a Server receives multiple packets in one UNSUBSCRIBE packet, it MUST process all topics sequentially, except that it sends just one UNSUBACK response.
    //If a Topic has a Shared Subscription, this Session is detached from the Shared Subscription.
    //f this Session was the only Session that the Shared Subscription was associated with, the Shared Subscription is deleted.
    char recvbuf[8];
    unsub_dec.decode_packet(recvbuf);
    unsub_dec.view();
    ServerPublish spub = ServerPublish();
    // try
    // {
    //     auto g = pg.retrieve_query("select message from mqtt_messages join mqtt_subs on mqtt_messages.topic = mqtt_subs.topic where mqtt_messages.topic = '" + std::string(reinterpret_cast<const char*>(unsub_dec.topic)) + "';");
     //    for (auto [g_] : g)
    //     {
    //         spub.mint_variable_header(std::string(reinterpret_cast<const char*>(unsub_dec.topic)), "", "text", unsub_dec.user_property_, g_);
    //         send(new_conn, reinterpret_cast<const char*>(spub.variable_header), sizeof spub.variable_header / sizeof spub.variable_header[0], 0);
             //SSL_write(ssl, spub.variable_header, sizeof spub.variable_header);
      //   }
    //}
    //catch (exception& e)
    // {
    //     std::cout << e.what() << endl;
    // }
   // try
    // {
  //       auto g1 = pg.retrieve_query("delete from mqtt_subs where sub='" + std::string(reinterpret_cast<const char*>(unsub_dec.user_property_)) + "' and topic = '" + std::string(reinterpret_cast<const char*>(unsub_dec.topic)) + "';");
    //}
    // catch (exception& e)
   //  {
     //    std::cout << e.what() << endl;
   //  }

    unsub.mint_variable_header(std::string("success"), unsub_dec.user_property_, unsub_dec.packet_id);
    view_int(reinterpret_cast<char*>(unsub.variable_header), 32);
    // send(new_conn, reinterpret_cast<const char*>(unsub.variable_header), sizeof unsub.variable_header / sizeof unsub.variable_header[0], 0);
};

void disconnect_( disconnect_decoder discon_dec, disconnect discon)
{
    //On receipt of DISCONNECT with a Reason Code of 0x00 (Success)the Server MUST discard any Will Message associated with the current Connection without publishing it
    char recvbuf[8];
    discon_dec.decode_packet(recvbuf);
        discon_dec.view();
        std::cout << "string_discon: " << string(discon_dec.username) << endl;
        //remove   attached to string from   map
        discon.mint_variable_header();
        int len = sizeof(discon.variable_header) / sizeof(discon.variable_header[0]);
        //send(new_conn, reinterpret_cast<const char*>(discon.variable_header), sizeof discon.variable_header / sizeof discon.variable_header[0], 0);
       
}

void pingresp_()
{
  //The Server MUST send a PINGRESP packet in response to a PINGREQ packet
 
    char resp[2] = { (char)0xd0, };
        resp[1] = (char)0x00;
        //send(new_conn, reinterpret_cast<const char*>(resp), 2, 0);
        //SSL_write(ssl, reinterpret_cast<const void*>(resp), 2);
}

void pubrel_( pubrel_decoder prel, pubcomp pub, char recvbuf[])
{
    
    prel.decode_packet(reinterpret_cast<std::byte*>(recvbuf));
    const char* s = reinterpret_cast<const char*>(prel.username);
    std::cout << "username_true: " << string(s) << endl;
    prel.view();
    pub.mint_packet(string(s));
  //  send(new_conn, reinterpret_cast<const char*>(pub.variable_header), sizeof pub.variable_header / sizeof pub.variable_header[0], 0);

}


vector<string> splitString(std::string str)
{
    char delimiter = ':';
    vector<string> strings;
    auto h = str.find(delimiter);
    strings.push_back(str.substr(0, h));
    strings.push_back(str.substr(h + 1, str.length())); //size of delimiter is one...
    return strings;
};

string decode_function(char recvbuf[8])
{
    decoder decoder_;
    string s = decoder_.decode_bytes(recvbuf);
    return s;
};

#endif