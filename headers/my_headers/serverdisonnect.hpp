#include <iostream>
#include <string>

class Disconnect {
public:
    int reason_codes[29] = { 0x00, 0x04, 0x80,  0x81, 0x82, 0x83, 0x87, 0x89, 0x8b, 0x8d,  0x8e, 0x8f, 0x90, 0x93,  0x94,  0x95, 0x96, 0x97, 0x98, 0x99,  0x9a,  0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2 };
    std::string reasons[29] = { "Normal disconnection", "Disconnect with Will Message", "Unspecified error", "Malformed Packet", "Protocol Error", "Implementation specific error",
    "Not authorized", "Server busy", "Server shutting down", "Keep Alive timeout", "Session taken over", "Topic Filter invalid", "Topic Name invalid", "Receive Maximum exceeded",
    "Topic Alias invalid", "Packet too large", "Message rate too high", "Quota exceeded", "Administrative action", "Payload format invalid", "Retain not supported", "QoS not supported",
    "Use another server", "Server moved", "Shared Subscriptions not supported", "Connection rate exceeded", "Maximum connect time", "Subscription Identifiers not supported", "Wildcard Subscriptions not supported" };
    std::byte variable_header[32];

    std::byte fixed_header_fill()
    {
        return (std::byte)std::stoi("11100000", nullptr, 2);
        return (std::byte)std::stoi("11100000", nullptr, 2);
    }
    std::byte get_reason_code(int index)
    {

        return (std::byte)reason_codes[index];
    }
    void reason_string(std::string reason, std::byte g[12])
    {
        g[0] = (std::byte)0x1f;
        std::byte* h = reinterpret_cast<std::byte*>((char*)reason.c_str());
        std::copy(h, h + reason.length(), g);
    };

    std::byte property_length() //work on this as soon as possible
    {
        return (std::byte)0x00;
    }
    void user_property(std::string f[], std::byte f_[], int n)
    {

        f_[0] = (std::byte)0x26;
        for (int i = 1; i < n; i++)
        {
            f_[i] = (std::byte)f[1][i - 1];

        };
    };
    void mint_variable_header()
    {
        std::string reason = "success";
        std::string f[2];
        variable_header[0] = fixed_header_fill();
        variable_header[1] = property_length();
        std::byte temp[5];//session expiry...
        memset(temp, 0, 5);
        std::copy(temp, temp + 5, variable_header+2);
        std::byte temp2[12];
        reason_string(reason, temp2);
        std::copy(temp2, temp2 + 12, variable_header + 7);
        std::byte temp3[12];
        user_property(f, temp3, static_cast<int>(f[1].length()));
        std::copy(temp3, temp3+12, variable_header+19);
    }
};