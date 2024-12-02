#include <algorithm>
#include <iostream>
#include <string>
#ifndef DECODER
#define DECODER
class decoder{
    public:
    int decodes[7] = {16, 48, -126, -94, -32, -64, 62};
    std::string decode_strings[7] = {"connect", "publish", "subscribe", "unsubscribe", "disconnect", "pingreq", "pubrel"};
    std::string decode_bytes(char c[])
    {
        std::string s = "";
        if (c[0] == 54||c[0] == 112||c[0]==52||c[0]==50)
        {
            s = decode_strings[1];
            return s;
        }
       
        try
        {
            int length = sizeof(decodes)/sizeof(decodes[0]);
            auto x = std::distance(decodes, std::find(decodes, decodes + (length-1), (int)c[0]));
           s = decode_strings[static_cast<int>(x)];
    }
        catch(const std::exception& e)
        {
           
            std::cerr << e.what() << '\n';
            s = "MALFORMED";
        };
        return s;
    } 
        
};
#endif