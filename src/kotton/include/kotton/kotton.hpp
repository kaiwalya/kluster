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
	struct _ref {
		virtual void send(message &&) = 0;
		virtual void waitForQuit() = 0;
	};
	ref self();
	
	struct _ctx;
	using ctx = std::shared_ptr<_ctx>;
	struct _ctx {
		virtual const message & top() = 0;
		virtual ref make(userfunc && f) = 0;
	};
	ctx me();
	
	struct message {
		virtual ~message(){}
		
		template<typename T>
		bool isA() const{
			return typeid(*this) == typeid(T);
		}
		
		template<typename T>
		const T & asA() const{
			return dynamic_cast<const T &>(*this);
		}
	};
	
	struct start_message: message {};
	struct stop_message: message {};
}