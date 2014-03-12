#if !defined(_KOTTON_H_)
#define _KOTTON_H_

#include <memory>
#include <functional>
#include <stack>
#include <exception>
#include <thread>
#include <typeinfo>
#include <typeindex>

namespace kotton {
	using userfunc = std::function<void(void)>;
	
	/*
		Beware when using heap stack, you need to protect the stack using a mutex as it moves around cores
	*/
	
	/**
		value represents any state variable in a processor.
		Things stored in this variable are preserved as the processor is scheduled on different threads.
		Use this when things cannot be kept of the stack.
	*/
	struct value{
	};
	
	/**
		Copyable value which can be passed around between processors
		Can be used to become a publisher or subscriber on this topic
	*/
	using topic = std::string;
	
	/**
		Get a pointer to this when you subscribe to a topic
	*/
	struct subscription {
		bool nextMessage();
	};
	using Subscription = std::shared_ptr<subscription>;
	
	/**
		Get a pointer to this when you are publisher for a topic
	*/
	struct publisher {
		void postMessage();
	};
	using Publisher = std::shared_ptr<publisher>;
	
	
	struct context;
	using Context = std::shared_ptr<context>;
	struct context {
		//virtual Subscription subscribe(topic &) = 0;
		//virtual Publisher publish(topic &) = 0;
		Publisher delegate(std::string && name, userfunc && f) {return delegate(name, f);};
		Publisher delegate(std::string & name, userfunc && f) {return delegate(name, f);};
		Publisher delegate(std::string && name, userfunc & f) {return delegate(name, f);};
		virtual Publisher delegate(std::string & name, userfunc & f) = 0;
		
		virtual void become(userfunc f) = 0;
		virtual void unbecome() = 0;
		virtual bool nextMessage() = 0;
	};
	
	/*
		Returns the Global System Context or the Context which created the current thread
	*/
	Context self();
	
	/*
		Creates a new context
	*/
	Context create();
}

#endif

