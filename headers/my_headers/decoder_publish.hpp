#include <iostream>
#include <bitset>
#include <vector>
#include "../my_headers/misc.hpp"
#include "../my_headers/general_decoder.hpp"

#define STRINGSIZE 12
#ifndef DECODER_PUBLISH
#define DECODER_PUBLISH

///If the PUBLISH packet contains a Topic Alias, the receiver processes it as follows:
//A Topic Alias value of 0 or greater than the Maximum Topic Alias is a Protocol Error, 
//the receiver uses DISCONNECT with Reason Code of 0x94 (Topic Alias invalid) as described in section 4.13.
// If alias is accepted (instead), then
// If the packet has a zero length Topic Name, the receiver processes it using the Topic Name that corresponds to the Topic Alias
//If the packet contains a non-zero length Topic Name, 
//the receiver processes the packet using that Topic Name and updates its mapping for the Topic Alias to the Topic Name 
//from the incoming packet
//If the receiver does not already have a mapping for this Topic Alias
//If the packet has a zero length Topic Name field it is a Protocol Error and the receiver uses DISCONNECT with Reason Code of 0x82 
//(Protocol Error)
//If the packet contains a Topic Name with a non-zero length, 
//the receiver processes the packet using that Topic Name and sets its mappings for the Topic Alias to Topic Name from the incoming packet.

class publish_decoder //done
{
    decoder publish_decoder_ = decoder();
    int fixed_header = 0;

    public:
        int mb = 1, lb = 1, qos_num=0;
        int payload_fi = 0, msg_expiry = 0, t_alias=0;
    char topic_name[12];
    char packet_id[2];
    int property_length =0;
    char payload_format_indicator[2];
    char message_expiry[5];
    char topic_alias[3];
    char response_topic[12], correlation_data[12], user_property_[12], subscription_id_[12], content_type[12], message[64], username[8];
    std::string bitstring;
    struct parse_first_byte
    {
        bool DUP = false;
        bool qos_mb = false;
        bool qos_lb = false;
        bool retain = false;
    };
    parse_first_byte pfb = parse_first_byte();
     int parse_byte_1(bool retain_available)
    {
        int temp = this->fixed_header;
        bitstring = std::bitset<8>(temp).to_string();
        std::cout << "bitstring: " << bitstring << std::endl;
        int length = static_cast<int>(bitstring.length());
        //===========================RETAIN========================================================PENDING
        //When a new Non‑shared Subscription is made, the last retained message, if any, on each matching topic name is sent to the Client as directed by the Retain Handling Subscription Option.
        //These messages are sent with the RETAIN flag set to 1. Which retained messages are sent is controlled by the Retain Handling Subscription Option.
        //At the time of the Subscription :
        // If Retain Handling is set to 0 the Server MUST send the retained messages matching the Topic Filter of the subscription to the Client.
        // If Retain Handling is set to 1 then if the subscription did not already exist, the Server MUST send all retained message matching the Topic Filter of the subscription to the Client, 
        // and if the subscription did exist the Server MUST NOT send the retained messages.
        //If Retain Handling is set to 2, the Server MUST NOT send the retained messages.
        //If the current retained message for a Topic expires, it is discarded and there will be no retained message for that topic.
        //The setting of the RETAIN flag in an Application Message forwarded by the Server from an established connection is controlled by the Retain As Published subscription option.
        //If the value of Retain As Published subscription option is set to 0, the Server MUST set the RETAIN flag to 0 when forwarding an Application Message regardless of how the RETAIN flag was set 
        //in the received PUBLISH packet.
        //If the value of Retain As Published subscription option is set to 1, the Server MUST set the RETAIN flag equal to the RETAIN flag in the received PUBLISH packet.
        // Retained messages are useful where publishers send state messages on an irregular basis.A new non - shared subscriber will receive the most recent state.
        pfb.retain = string_to_bool(bitstring[length-1]);
        if (pfb.retain)
        {
            //the Server MUST replace any existing retained message for this topic and store the Application Message,
            // so that it can be delivered to future subscribers whose subscriptions match its Topic Name
            int message_length = sizeof message / sizeof message[0];
            if (message_length <=0)
            {
                //check any retained message with the same topic name and remove it so that future subscribers for the topic will not receive a retained message
                //do not store as a retained message
            }
        }
        if (!pfb.retain)
        {
            //the Server MUST NOT store the message as a retained message and MUST NOT remove or replace any existing retained message
            if (!retain_available)
            {
               //use the DISCONNECT Reason Code of 0x9A (Retain not supported).
            }
        }
        //====================================MAX=QOS==========================================DONE
        //Session state indicating max QOS for the server must be checked against the QOS flags in the publish fixed header,DONE
        //If max qos is exceeded, it uses DISCONNECT with Reason Code 0x9B (QoS not supported) as described in section 4.13 Handling errors.ADDRESSED
        //If a Server or Client receives a PUBLISH packet which has both QoS bits set to 1 it is a Malformed Packet.DONE
        //Use DISCONNECT with Reason Code 0x81 (Malformed Packet) as described in section 4.13. ADDRESSED
        pfb.qos_lb = string_to_bool(bitstring[length-2]);
        pfb.qos_mb = string_to_bool(bitstring[length-3]);
        if ((int)pfb.qos_mb >= 1 && (int)pfb.qos_lb >= 1) //one more error to check. If the specified QOS is larger than the max QOS
        {
            perror("Malformed packet. Evaluation of QOS bits cannot exceed 1");
            //Use DISCONNECT with Reason Code 0x81
           
        };
        if ((int)pfb.qos_mb > mb)
        {
            perror("Client pfb should not be larger than server max_qos");
            //uses DISCONNECT with Reason Code 0x9B (QoS not supported)
           
        }
        //=================================DUP============================================================DONE
        //If the DUP flag is set to 0, it indicates that this is the first occasion that the Client or Server has attempted to send this PUBLISH packet.DONE
        //If the DUP flag is set to 1, it indicates that this might be re - delivery of an earlier attempt to send the packet.DONE
        //The DUP flag MUST be set to 1 by the Client or Server when it attempts to re - deliver a PUBLISH packet.DONE
        //The DUP flag MUST be set to 0 for all QoS 0 messages.DONE
        //The value of the DUP flag from an incoming PUBLISH packet is not propagated when the PUBLISH packet is sent to subscribers by the Server.
        //The DUP flag in the outgoing PUBLISH packet is set independently to the incoming PUBLISH packet, its value MUST be determined solely by whether the outgoing PUBLISH packet is a 
        //retransmission[MQTT - 3.3.1 - 3].
        //The receiver of an MQTT Control Packet that contains the DUP flag set to 1 cannot assume that it has seen an earlier copy of this packet.
        //It is important to note that the DUP flag refers to the MQTT Control Packet itself and not to the Application Message that it contains.
        //When using QoS 1, it is possible for a Client to receive a PUBLISH packet with DUP flag set to 0 that contains a repetition of an Application Message that it received earlier, 
        //but with a different Packet Identifier.Section 2.2.1 provides more information about Packet Identifiers.
        pfb.DUP = string_to_bool(bitstring[length-4]);//DUP is independently set in the server or client when publishing...
        return 0;
    }
     void parse_username()
     {
         int length = sizeof username / sizeof username[0];
         for (int i = 0; i < length; i++)
         {
             username[i] = user_property_[i + 1];
         }
     }
    int decode_packet(char *variable_header, bool retain_available)
    {
        std::string f[] = {"",""};
        std::cout << "packet" << std::endl;
        std::string s = publish_decoder_.decode_bytes(variable_header);
        fixed_header = (int) variable_header[0];
        //==========================================TOPIC=NAME===============================================================================
        //The Topic Name identifies the information channel to which Payload data is published.DONE
        //The Topic Name MUST be present as the first field in the PUBLISH packet Variable Header.It MUST be a UTF - 8 Encoded String as defined in section 1.5.4[MQTT - 3.3.2 - 1].DONE
        //The Topic Name in the PUBLISH packet MUST NOT contain wildcard characters.DONE
        //The Topic Name in a PUBLISH packet sent by a Server to a subscribing Client MUST match the Subscription’s Topic Filter according to the matching process defined in section 4.7
        //However, as the Server is permitted to map the Topic Name to another name, it might not be the same as the Topic Name in the original PUBLISH packet.
        //tO reduce the size of the PUBLISH packet the sender can use a Topic Alias.NOT DONE
        //The Topic Alias is described in section 3.3.2.3.4.It is a Protocol Error if the Topic Name is zero length and there is no Topic Alias.DONE
        
    std::copy(variable_header + 1, variable_header + 12, topic_name);
        if (std::string(topic_name) == "")
        {
            perror("No topic present");
            
        }
       // if (std::string(topic_name).find('*')) //VALID ERROR
        //{
         //   perror("No wild characters expected");
           // return 1;
        //}
        if (len(topic_name) == 0 && len(topic_alias) == 0)
        {
            perror("Topic name or Topic alias must be present");
        
        }

        //=================================PACKET=ID==============================================================================
        //The Packet Identifier field is only present in PUBLISH packets where the QoS level is 1 or 2. Section 2.2.1 provides more information about Packet Identifiers.ADDRESSED
        std::copy(variable_header +12, variable_header + 14, packet_id);

        property_length = variable_header[14];

        //====================================PAYLOAD=FORMAT=INDICATOR===============================================================
        // Can be:
        //0 (0x00) Byte Indicates that the Payload is unspecified bytes, which is equivalent to not sending a Payload Format Indicator.
        //1 (0x01) Byte Indicates that the Payload is UTF - 8 Encoded Character Data.
        //The UTF - 8 data in the Payload MUST be well - formed UTF - 8 as defined by the Unicode specification[Unicode] and restated in RFC 3629[RFC3629].ADDRESSED
        //A Server MUST send the Payload Format Indicator unaltered to all subscribers receiving the Application Message.DONE
        //The receiver MAY validate that the Payload is of the format indicated, and if it is not, WILL ADDRESS
        //send a PUBACK, PUBREC, or DISCONNECT with Reason Code of 0x99 (Payload format invalid) as described in section 4.13.
        // Refer to section 5.4.9 for information about security issues in validating the payload format.
        std::copy(variable_header + 15, variable_header + 17, payload_format_indicator);
       // parse_packet(variable_header, payload_format_indicator, 15, 17);
        payload_fi = payload_format_indicator[1] - '0';
        //=================================MESSAGE=EXPIRY===============================================================================
        // If the Message Expiry Interval has passed and the Server has not managed to start onward delivery to a matching subscriber, 
        //then it MUST delete the copy of the message for that subscriber.NOT DONE
        //If absent, the Application Message does not expire.NOT DONE
        //The PUBLISH packet sent to a Client by the Server MUST contain a Message Expiry Interval set to the received value minus the time that the Application Message has been waiting in the Server
        //.Refer to section 4.1 for details and limitations of stored state.
        std::copy(variable_header + 17, variable_header + 22, message_expiry);
        //the code below is taken from: https://stackoverflow.com/questions/34943835/convert-four-bytes-to-integer-using-c
        msg_expiry = int((unsigned char)(message_expiry[1]) << 24 | (unsigned char)(message_expiry[2]) << 16 | (unsigned char)(message_expiry[3]) << 8 | (unsigned char)(message_expiry[4]));

        

        //=============================TOPIC=ALIAS=========================================================================================
        //Followed by the Two Byte integer representing the Topic Alias value. It is a Protocol Error to include the Topic Alias value more than once.
        //A Topic Alias is an integer value that is used to identify the Topic instead of using the Topic Name.
        //This reduces the size of the PUBLISH packet, and is useful when the Topic Names are long and the same Topic Names are used repetitively within a Network Connection.
        //The sender decides whether to use a Topic Alias and chooses the value.
        //It sets a Topic Alias mapping by including a non - zero length Topic Name and a Topic Alias in the PUBLISH packet.
        //The receiver processes the PUBLISH as normal but also sets the specified Topic Alias mapping to this Topic Name.
        //If a Topic Alias mapping has been set at the receiver, a sender can send a PUBLISH packet that contains that Topic Alias and a zero length Topic Name.
        //The receiver then treats the incoming PUBLISH as if it had contained the Topic Name of the Topic Alias.
        //A sender can modify the Topic Alias mapping by sending another PUBLISH in the same Network Connection with the same Topic Alias value and a different non - zero length Topic Name.
        //Topic Alias mappings exist only within a Network Connection and last only for the lifetime of that Network Connection.A receiver MUST NOT carry forward any Topic Alias mappings 
        //from one Network Connection to another.
        //A Topic Alias of 0 is not permitted.A sender MUST NOT send a PUBLISH packet containing a Topic Alias which has the value 0.
        //A Client MUST NOT send a PUBLISH packet with a Topic Alias greater than the Topic Alias Maximum value returned by the Server in the CONNACK packet
        //A Client MUST accept all Topic Alias values greater than 0 and less than or equal to the Topic Alias Maximum value that it sent in the CONNECT packet.
        //A Server MUST NOT send a PUBLISH packet with a Topic Alias greater than the Topic Alias Maximum value sent by the Client in the CONNECT packet.
        // A Server MUST accept all Topic Alias values greater than 0 and less than or equal to the Topic Alias Maximum value that it returned in the CONNACK packet.
        //The Topic Alias mappings used by the Client and Server are independent from each other.
        //Thus, when a Client sends a PUBLISH containing a Topic Alias value of 1 to a Server and the Server sends a PUBLISH with a Topic Alias value of 1 to that Client,
        // they will in general be referring to different Topics.

        //VALID ERROR
        std::copy(variable_header + 22, variable_header + 25, topic_alias);
        t_alias = int((unsigned char)(topic_alias[1]) << 8 | (unsigned char)(topic_alias[2]));
        if (t_alias == 0)
        {
           perror("Topic alias cannot be set to zero");
        }

        //====================================RESPONSE=TOPIC===============================================================================================
        //Followed by a UTF - 8 Encoded String which is used as the Topic Name for a response message.
        //The Response Topic MUST be a UTF - 8 Encoded String as defined in section 1.5.4.
        //The Response Topic MUST NOT contain wildcard characters.
        //It is a Protocol Error to include the Response Topic more than once.
        //The presence of a Response Topic identifies the Message as a Request.
        // The Server MUST send the Response Topic unaltered to all subscribers receiving the Application Message.
        //The receiver of an Application Message with a Response Topic sends a response by using the Response Topic as the Topic Name of a PUBLISH.
        //If the Request Message contains a Correlation Data, the receiver of the Request Message should also include this Correlation Data as a property in the PUBLISH packet of the Response Message.

        std::copy(variable_header + 25, variable_header + 37, response_topic);
        if (std::string(response_topic) == "")
        {
            perror("No topic present");
           // return 1;
        }
        if (std::string(response_topic).find('*'))
        {
            perror("No wild characters expected");
           // return 1;
        }

        //======================================CORRELATION=DATA========================================================================
        //The Correlation Data is used by the sender of the Request Message to identify which request the Response Message is for when it is received. 
        // It is a Protocol Error to include Correlation Data more than once. 
        //If the Correlation Data is not present, the Requester does not require any correlation data.
        //The Server MUST send the Correlation Data unaltered to all subscribers receiving the Application Message.
        // The value of the Correlation Data only has meaning to the sender of the Request Message and receiver of the Response Message.
        //The receiver of an Application Message which contains both a Response Topic and a Correlation Data sends a response by using the Response Topic as the Topic Name of a PUBLISH.
        //The Client should also send the Correlation Data unaltered as part of the PUBLISH of the responses.
        // If the Correlation Data contains information which can cause application failures if modified by the Client responding to the request, 
        //it should be encrypted and /or hashed to allow any alteration to be detected.
        std::copy(variable_header + 37, variable_header + 49, correlation_data);

        //=======================================USER_PROPERTY============================================================================
        //std::string pairs for name-value associations
        std::copy(variable_header + 49, variable_header + 61, user_property_);

        std::copy(variable_header + 61, variable_header + 66, subscription_id_);
        //std::cout << "sub_ID: " << std::string(reinterpret_cast<const char*>(subscription_id_));
        //=========================================CONTENT=TYPE===========================================================================
        std::copy(variable_header + 66, variable_header + 78, content_type);
      
        if (std::string(content_type) == "")
        {
            perror("Content type must be specified. Types include text");
            //return 1;
        }
        std::copy(variable_header + 78, variable_header + 78 + 64, message);
        parse_byte_1(retain_available);
        parse_username();
        bool g[2] = { pfb.qos_mb,pfb.qos_lb };
        qos_num = decode_bool(g);
        return 0;
    }

    void view_packet()
    {
        std::cout << "qos: " << qos_num << std::endl;
        std::cout << "topic_name: " << std::string(topic_name) << std::endl;
        std::cout << "packet_id: ";
        view_int(packet_id, 2);
         std::cout << std::endl;
        std::cout << "property_length: " << property_length << std::endl;
        std::cout << "message_expiry :";
        view_int(message_expiry, sizeof message_expiry / sizeof message_expiry[0]);
        std::cout << std::endl << "topic alias: ";
        view_int(topic_alias, sizeof topic_alias / sizeof topic_alias[0]);
        std::cout << std::endl << "response topic: ";
        view_int(response_topic, sizeof response_topic / sizeof response_topic[0]);
        std::cout << std::endl << "correlation data: ";
        view_int(correlation_data, sizeof correlation_data / sizeof correlation_data[0]);
        std::cout << std::endl << "user_property: ";
        view_int(user_property_, sizeof user_property_ / sizeof user_property_[0]);
        std::cout << std::endl << "sub_id: ";
        view_int(subscription_id_, sizeof subscription_id_ / sizeof subscription_id_[0]);
        std::cout << std::endl << "content_type: ";
        view_int(content_type, sizeof content_type / sizeof content_type[0]);
        std::cout << std::endl << "message: ";
        view_int(message, sizeof message / sizeof message[0]);
        std::cout << "texts" << std::endl;
        std::cout << "message: " << std::string(message) << std::endl;
        std::cout << "content type: " << std::string(content_type) << std::endl;
        std::cout << "user_property: " << std::string(user_property_) << std::endl;
        std::cout << "response_topic: " << std::string(response_topic) << std::endl;
    }
    
};
#endif