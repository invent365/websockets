#include <iostream>
#include <string>
class ServerPublish
{
public:
    bool retain = false, qos_upper = false, qos_lower = false, dup = false, payload_present = true;
    std::byte fixed_header;
    std::byte variable_header[160];
    std::byte null_byte[8]; //correlation data
    std::string f[2]; //user property
    std::byte userproperty[12], sub_id[8], topic_id[8], container[3], message_expiry[5], topic_alias[3], response_topic_[12], username_[8], content_type_[8], message_[64];

    std::string bool_to_string(bool a)
    {
        return a ? "1" : "0";
    }
    auto string_to_bool(char a)
    {
        return (a == '1') ? true : false;
    }

    std::byte fixed_header_fill()
    {
        std::string fixed_header_byte = "0011" + bool_to_string(dup) + bool_to_string(qos_lower) + bool_to_string(qos_upper) + bool_to_string(retain);
        int temp = std::stoi(fixed_header_byte, nullptr, 2);
        return (std::byte)temp;
    }

    void packet_id()
    {
        //return new byte[2];
    }
    int property_length() //work on this as soon as possible
    {
        return 0;
    }
    std::byte payload_format_indicator(bool payload_present)
    {
        std::byte nbyte = payload_present ? (std::byte)0x01 : (std::byte)0x00;
        return nbyte;
    }

    void subscription_id(int m, std::byte g[]) //this is a variable integer
    {
        g[0] = (std::byte)0x0B;
        int numlength = sizeof m;
        std::byte c[8];
        for (int i = 0; i < numlength; i++)
        {
            c[i] = static_cast<std::byte>((m >> (i * 8)) & 0xFF);
        }
        std::copy(c, c+8, g+1);
    }

    void copy_bytes(std::byte *f, std::byte *out, int start, int end)
    {
        std::copy(f + start, f + end, out);
    }
    void user_property(std::string f[], std::byte f_[], int n)
    {

        f_[0] = (std::byte)0x26;
        for (int i = 1; i < n; i++)
        {
            f_[i] = (std::byte)f[1][i - 1];

        };
    };
    void mint_variable_header(std::string topic_name, std::string response_topic, std::string content_type, std::string username, std::string message, int sub_ = 0x01)
    {
        f[0] = ""; f[1] = username;
        variable_header[0] = fixed_header_fill();
        std::byte* f1 = reinterpret_cast<std::byte*>((char*)topic_name.c_str());
        std::copy(f1, f1 + strlen(topic_name.c_str()), topic_id);
        std::copy(topic_id, topic_id + 8, variable_header + 1);
        std::copy(container, container + 3, variable_header + 9);
        variable_header[12] = static_cast<std::byte>(0x00);
        variable_header[13] = static_cast<std::byte>(0x01); //payload format indicator
        message_expiry[0] = (std::byte)0x02;
        std::copy(message_expiry, message_expiry + 5, variable_header + 14);
        topic_alias[0] = (std::byte)0x23;
        std::copy(topic_alias, topic_alias + 3, variable_header + 19);

        std::byte* d = reinterpret_cast<std::byte*>((char*)response_topic.c_str());
        response_topic_[0] = (std::byte)0x08;
        std::copy(d, d + response_topic.length(), response_topic_);
        std::copy(response_topic_, response_topic_ + 12, variable_header + 22);

        null_byte[0] = (std::byte)0x09;
        std::copy(null_byte, null_byte+8, variable_header+34);

        std::byte* d1 = reinterpret_cast<std::byte*>((char*)username.c_str());
        userproperty[0] = (std::byte)0x26;
        std::copy(d1, d1 + username.length(), username_);
        std::copy(username_, username_ + 8, variable_header+50);

        sub_id[0] = (std::byte)0x0b;
        sub_id[1] = (std::byte)sub_;
        std::copy(sub_id, sub_id + 8, variable_header + 58);
  
        std::byte* d2 = reinterpret_cast<std::byte*>((char*)content_type.c_str());
        content_type_[0] = (std::byte)0x03;
        std::copy(d2, d2 + content_type.length(),  content_type_+1);
        std::copy(content_type_, content_type_ + 8, variable_header + 66);

        std::byte* d3 = reinterpret_cast<std::byte*>((char*)message.c_str());
        std::copy(d3, d3 + message.length(), message_);
        std::copy(message_, message_ + 64, variable_header + 74);
    }

};
