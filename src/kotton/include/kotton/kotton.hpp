#include <memory>
#include <functional>
#include <stack>
#include <exception>
#include <thread>
#include <typeinfo>
#include <typeindex>

namespace kotton {
	using userfunc = std::function<void(void)>;
	struct message;
	
	struct _ref;
	using ref = std::shared_ptr<_ref>;
	
	/**
		Denotes an address of a mailbox. You can most messages to an address
	*/
	struct _ref {
		/**
			Send a message to this address
		*/
		virtual void send(message &&) = 0;
		
		/**
			Wait for the process servicing the address to quit
		*/
		virtual void waitForQuit() = 0;
	};
	
	/**
	Static function get get your own address
	*/
	ref self();
	
	struct _ctx;
	using ctx = std::shared_ptr<_ctx>;
	/**
		Represents a mailbox and a processor servicing it.
	*/
	struct _ctx {
		/**Read message form the mailbox.*/
		virtual const message & top() = 0;
		/**Create a new mailbox and use userfunc as the service routine*/
		virtual ref make(userfunc && f) = 0;
	};
	/**Static method to get the current mailbox */
	ctx me();
	
	/**A message which can be sent to a mailbox*/
	struct message {
		virtual ~message(){}

		/**Checks if a message is of a particular type*/
		template<typename T> bool isA() const{
			return typeid(*this) == typeid(T);
		}
		
		/**Convert a message to the given type*/
		template<typename T> const T & asA() const{
			return dynamic_cast<const T &>(*this);
		}
	};
	
	struct start_message: message {};
	struct stop_message: message {};
}