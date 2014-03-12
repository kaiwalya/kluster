
#include <iostream>
#include <exception>
#include <assert.h>
#include "kotton/kotton.hpp"

namespace kotton {
	
	struct _Ctx;
	using Ctx = std::shared_ptr<_Ctx>;
	struct _Ref;
	using Ref = std::shared_ptr<_Ref>;
	
	static __thread _Ctx * currentContext;
	struct _Ctx: _ctx {

		static Ctx current() {
			if (currentContext) {
				assert(!currentContext->mThis.expired());
				return currentContext->mThis.lock();
			}
			else {
				Ctx ret;
				new _Ctx(ret);
				assert(ret);
				return ret;
			}
		}
		
		const message & top() override {
			throw std::bad_function_call();
		};
		
		ref make(userfunc && f) override {
			throw std::bad_function_call();
		}
		
	private:
		std::weak_ptr<_Ctx> mThis;
		_Ctx(Ctx & refOut){
			refOut.reset(this);
			mThis = refOut;
		}
	};
	
	static __thread _Ref * currentRef;
	struct _Ref: _ref {
		static Ref current() {
			if (currentRef) {
				assert(!currentRef->mThis.expired());
				return currentRef->mThis.lock();
			}
			else {
				Ref ret;
				new _Ref(ret, _Ctx::current());
				assert(ret);
				return ret;
			}
		}
		
		void send(message &&) override {
		};
		
		void waitForQuit() override {
		};
	private:
		std::weak_ptr<_Ref> mThis;
		Ctx mCtx;
		_Ref(Ref & refOut, Ctx && ctx) {
			refOut.reset(this);
			mThis = refOut;
			mCtx = ctx;
		}
	};
	
	
	
	ctx me() {
		return _Ctx::current();
	}
	
	ref self() {
		return _Ref::current();
	}
}
