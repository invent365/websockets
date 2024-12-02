#include <iostream>
#include <bitset>
#include <string>
#include "my_headers/decoder_connect.hpp"
#ifndef CONNECT_FLAGS
#define CONNECT_FLAGS



//This header includes options for connect flags that decode instructions for the server, depending on the connack_flags obtained from the connect packet in the client
//Instantiate these classes after the bytes have been completely decoded or parsed by the superclass.
class Will_message
{

public:
    char* will_delay = NULL, * payload_format_indicator = NULL, * message_expiry = NULL, * content_type = NULL, * response_topic = NULL, * correlation_data = NULL, * user_property_field = NULL,
        * will_topic = NULL, * will_payload = NULL;
    Will_message() {};
    Will_message(connack_decoder::payload& payload)
    {
        will_delay = payload.will_delay_interval;
        payload_format_indicator = payload.payload_format_indicator;
        message_expiry = payload.message_expiry;
        content_type = payload.content_type;
        response_topic = payload.res_topic;
        correlation_data = payload.null_byte;
        user_property_field = payload.user_property;
        will_topic = payload.will_topic;
        will_payload = payload.will_payload;
    };
};



class connect_flag_actions
{
    bool clean_start = false;
    bool will_flags = false;
    char will_qos_upper = false;
    char will_qos_lower = false;
    bool will_retain = false;
    bool password_flag = false;
    bool username_flag = false;
    bool cap = false;
    bool cap2 = false;
    std::string repr = "";
    std::string username = "", password = "";
    Will_message will_message;
    bool disconnect = false;
    //Session session = NULL;
public:
    bool session_present = false;
    struct output
    {
        bool discard_session = false;
        bool session_present_flag = false;
    };
    output output_ = output();
    connect_flag_actions(connack_decoder::connect_flags flag, connack_decoder::payload& payload, std::string username, std::string password) //constructor for connect_flag_decoder...
    {
        cap = selector(repr[7]);
        clean_start = flag.clean_start;
        will_flags = flag.will_flag;
        will_qos_upper = flag.will_qos_hb;
        will_qos_lower = flag.will_qos_lb;
        will_retain = flag.will_retain;
        password_flag = flag.password;
        username_flag = flag.username;
        this->username = username;
        this->password = password;
        will_message = Will_message(payload);


    }
    bool selector(char s)
    {
        return s == '1';
    }
    void check_clean_start(std::map<std::string, std::string> sessions, char clientid[8])
    {
        //Clean_state= this bit specifies whether the Connection starts a new Session or is a continuation of an existing Session. 
        if (clean_start)
        {
            output_.discard_session = true;//if true, start a new Session.
            output_.session_present_flag = false; //the Session Present flag in CONNACK is always set to 0 if Clean Start is set to 1.
        }
        else if (!clean_start)
        {
            auto value = sessions[username];
            if (session_present)
            {
                //the Server MUST resume communications with the Client based on state from the existing Session [MQTT-3.1.2-5].
            }
            else
            {
                //if client id is present in the connect header, use it. else, the server will assign a new client id.
                std::string h = "new session"; //ADDRESS THIS ISSUE. Randomly generate a hexadecimal UUID for this purpose.
                if (std::string(clientid) != "")
                {
                    h = std::string(clientid);
                }
                //ADDRESS THIS ISSUE
                sessions[username] = h;//the Server MUST create a new Session.

            }
        }
    }
    bool check_filled()
    {
        return will_message.content_type != NULL && will_message.correlation_data != NULL && will_message.message_expiry != NULL && will_message.payload_format_indicator != NULL
            && will_message.response_topic != NULL && will_message.user_property_field != NULL && will_message.will_delay != NULL && will_message.will_payload != NULL && will_message.will_topic != NULL;
    }
    int check_will_flags(std::map<std::string, Will_message>& Wills)
    {
        if (will_flags)
        {
            // If the Will Flag is set to 1, the Will Properties, Will Topic, and Will Payload fields MUST be present in the Payload.
            if (!check_filled())
            {
                perror("Some (or all) will attributes are not present in the payload");
               return 1;

            }
            Wills[username] = will_message;
          //  if ( [username] == NULL) //&& elapsed(will_delay) && elapsed(session)
           // {
                //The Will Message MUST be published
                    //unless the Will Message has been deleted by the Server on receipt of a DISCONNECT packet with Reason Code 0x00 (Normal disconnection)
          //  }
            //Situations in which the Will Message is published include, but are not limited to 
            // An I / O error or network failure detected by the Server.
            //The Client fails to communicate within the Keep Alive time.
            //The Client closes the Network Connection without first sending a DISCONNECT packet with a Reason Code 0x00 (Normal disconnection).
            //The Server closes the Network Connection without first receiving a DISCONNECT packet with a Reason Code 0x00 (Normal disconnection).
            //The Will Message MUST be removed from the stored Session State in the Server once it has been published or the Server has received a DISCONNECT packet 
            //with a Reason Code of 0x00 (Normal disconnection) from the Client.
            //The Server SHOULD publish Will Messages promptly after the Network Connection is closed and the Will Delay Interval has passed, or when the Session ends, 
            // whichever occurs first.
            // In the case of a Server shutdown or failure, the Server MAY defer publication of Will Messages until a subsequent restart.
            // If this happens, there might be a delay between the time the Server experienced failure and when the Will Message is published.
            return 0;
        }
    }
    int check_will_qos()
    {
        if (will_qos_lower > 1 || will_qos_upper > 1)
        {
            perror("Malformed packet for will_qos");
            return 1;
        }
        if (!will_flags)
        {
            if (will_qos_upper > 0 || will_qos_lower > 0)
            {
                perror("Will_qos must be set to ZERO");
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    void check_will_retain()
    {
        //This bit specifies if the Will Message is to be retained when it is published.
        if (!will_flags)
        {
            will_retain = false;
        }
        else
        {
            if (!will_retain)
            {
                //publish the Will Message as a non - retained message
            }
            else

            {
                //the Server MUST publish the Will Message as a retained message.
            }
        }

    };
    void check_username()
    {
        if (username_flag)
        {

            username_flag && (username == "") ? [] {perror("Username must be present where username_flag evaluates to true"); return 1; } : [] {return 0; };
        }
        else
        {
            !username_flag && (username != "") ? [] {perror("Username must not be present where username_flag evaluates to false"); return 1; } : [] {return 0; };
        }
    }

    void check_password()
    {
        if (password_flag)
        {
            password_flag && (password == "") ? [] {perror("Password must be present where password_flag evaluates to true"); return 1;  } : [] {return 0; };
        }
        else
        {
            !password_flag && (password != "") ? [] {perror("Password must not be present where password_flag evaluates to false"); return 1; } : [] {return 0; };
        }
    }

    void check_execute(std::map<std::string, std::string>& sessions, std::map<std::string, Will_message>& Wills, char clientid[8])
    {
        this->check_clean_start(sessions, clientid);
        this->check_will_flags(Wills);
        this->check_will_qos();
        this->check_will_retain();
        this->check_username();
        this->check_password();
    }


};

#endif // !CONNECT_FLAGS