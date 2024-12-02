
#ifndef PRIM_SOCK
#define PRIM_SOCK
#include <functional>
#include <iostream>
#include <thread>
#include <string>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <map>
#include <openssl/ssl.h>
#include "my_headers/connect_flags.hpp"
#include "my_headers/decoder_publish.hpp"
#include "my_headers/encoder_puback.hpp"
#include "my_headers/socket_functions.hpp"


//int recvbuflen = DEFAULT_BUFLEN;
//char recvbuf[DEFAULT_BUFLEN];

class Primitive_
{
	//Modus operandi
	//a selector appends all  s making a connection to the primary  . Each time there is activity on any of the  s, 
	//the main thread wakes up and searches all the  s from the primary   to the client  s.
	//If the primary   has a valid new connection, it is appended to the list of  s to listen for.
	//If any of the clients have data, it is processed immediately and sent back through the trigger  .
public:
	decoder decoder_ = decoder();
	connack_decoder con_dec = connack_decoder();
	connack con = connack();
	publish_decoder pub_dec = publish_decoder();
	puback pub = puback();
	subscribe_decoder sub_dec = subscribe_decoder();
	suback sub = suback();
	disconnect_decoder discon_dec = disconnect_decoder();
	disconnect discon = disconnect();
	unsubscribe_decoder unsub_dec = unsubscribe_decoder();
	unsuback unsub = unsuback();
	pubrel_decoder prel_dec = pubrel_decoder();
	pubrec pubr = pubrec();
	pubcomp pcomp = pubcomp();
	Disconnect server_discon = Disconnect();
	std::map<string, string> session_ids;
	std::map<string, Will_message> wills;
	std::mutex our_mutex;
	std::condition_variable con_var;
	struct timeval timeout;
	int response_code = 0, max_sd = 0, activity = 0;
	long long timer_ = 0;
	fd_set  set;
	bool state = true, turn = false;
	Primitive_()
	{

	}



	void evaluate(string s)
	{
		cout << s << endl;

		if (s == "connect")
		{
			connect_(con_dec, con, std::ref(session_ids), std::ref(wills));
			return;

		}
		else if (s == "publish")
		{
			publish_(pub_dec, pubr);
			return;
		}
		else if (s == "subscribe")
		{
			try
			{
				subscribe_(sub_dec, sub);
				return;
			}
			catch (exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
		else if (s == "unsubscribe")
		{
			unsubscribe_(unsub_dec, unsub);
			return;
		}

		else if (s == "disconnect")
		{
			disconnect_(discon_dec, discon);
			return;
		}

		else if (s == "pubrel")
		{
			pubrel_(prel_dec, pcomp);
			return;
		}
		else if (s == "pingreq")
		{
			//old_time = std::chrono::steady_clock::now(); //reset time to 0
			pingresp_();
			return;
		}
		else
		{
			std::cout << "unidentified token. Shut down..." << std::endl;
			return;
		}
		return;
	}
};
#endif//