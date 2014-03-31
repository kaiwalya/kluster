//
//  main.cpp
//  kluster-console
//
//  Created by Kaiwalya Kher on 3/31/14.
//  Copyright (c) 2014 Kaiwalya Kher. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <thread>
#include "zmq.hpp"

struct logger {
	
	using stream_type = std::ostringstream;
	struct server {
		server(){
			
			std::mutex m;
			std::condition_variable c;
			std::unique_lock<std::mutex> l(m);

			bool bInit = false;
			
			std::thread tTemp([this, &m, &c, &bInit](){
				zmq::socket_t sock(mContext, ZMQ_PULL);
				{
					std::unique_lock<std::mutex> l(m);
					sock.bind("inproc://logger");
					bInit = true;
					c.notify_all();
				}
				serve(sock);
			});
			
			while(!bInit) {
				c.wait(l);
			}
			
			mThread.swap(tTemp);
			
		}
		
		~server() {
			logger(*this).quit();
			mThread.join();
		}
		
	private:
		zmq::context_t mContext;
		std::thread mThread;
		
		
		zmq::context_t & context() {
			return mContext;
		}
		
		void serve(zmq::socket_t & sock) {
			zmq::message_t msg;
			while (sock.recv(&msg)) {
				if (!msg.size()) return;
				std::cout <<std::string((char *)msg.data(), msg.size()) << std::endl;
			}
		}
		
		friend class logger;
	};
	
	logger(server & server): mContext(server.context()), mSock(mContext, ZMQ_PUSH) {
		mSock.connect("inproc://logger");
	}

	
	void trace(std::ostream & stream) {
		log(dynamic_cast<std::ostringstream &>(stream));
	}

	void trace(std::ostream && stream) { trace(stream); }
	
	stream_type & stream() {
		return mStream;
	}
	
	
	void quit() {
		mSock.send(nullptr, 0);
	}
private:
	zmq::context_t & mContext;
	zmq::socket_t mSock;
	stream_type mStream;
	
	void log(stream_type & stream, int level = 0) {
		mSock.send(stream.str().c_str(), stream.str().size());
		if (&stream == &mStream) {
			stream.str("");
			stream.clear();
		}
	}
};

int main(int argc, const char * argv[])
{
	logger::server logServer;
	
	logger log(logServer);
	log.trace(log.stream() << "Waiting for connections...");
	
	zmq::context_t ctx;
	zmq::socket_t server(ctx, ZMQ_PULL);
	server.bind("tcp://127.0.0.1:4784");
	zmq::message_t msg;
	while(server.recv(&msg)) {
		if (msg.size() == 0) break;
		log.trace(log.stream() << "Got: " << std::string((const char *)msg.data(), msg.size()));
	}
}

