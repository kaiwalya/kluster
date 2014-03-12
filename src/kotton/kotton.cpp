
#include <iostream>
#include <exception>
#include <assert.h>
#include <stdio.h>
#include <memory>

#include "kotton/kotton.hpp"

namespace kotton {
	struct acontext;
	using AContext = std::shared_ptr<acontext>;
	
	struct processor;
	using Processor = std::shared_ptr<processor>;
}

static __thread kotton::acontext * __self = nullptr;
static __thread kotton::processor * __self_processor = nullptr;


static kotton::acontext * _self() {
	return __self;
}

static kotton::processor * _self_processor() {
	return __self_processor;
}

static kotton::Context _globalContext = kotton::create();

namespace kotton {

	struct err_wrong_context: std::exception {
		const char * what() const noexcept override {
			return "Wrong thread. Wrong context.";
		}
	};
	struct err_not_implemented: std::exception {
		const char * what() const noexcept override {
			return "Not implemented yet!";
		}
	};
	
	struct processor {
		acontext * mContext;
		std::string mName;
		userfunc mUserfunc;
	};
	
	struct acontext: context {
		virtual Publisher delegate(std::string & name, userfunc & f) override {
			throw err_not_implemented();
			if (this == _self()) {
				assert(_self_processor());
				//There is code executing on our thread, this means there is already a processor running.
			}
			else {
				//We are getting this request from another context. We should create a top level processor
				auto p = new processor();
				p->mContext = this;
				p->mName = name;
				p->mUserfunc = f;
				
			}
		};
		
		virtual void become(userfunc f) override {
			confirmSelfOrThrow();
		};
		
		virtual void unbecome() override {
			confirmSelfOrThrow();
		};
		
		virtual bool nextMessage() override {
			confirmSelfOrThrow();
			return false;
		};
		
		Context self() {
			confirmSelfOrThrow();
			return mThis.lock();
		}

		acontext(AContext & ctx, std::string name) {
			ctx.reset(this);
			mThis = ctx;
			mName = name;
		}
	private:
		std::weak_ptr<acontext> mThis;
		std::string mName;
		
		void confirmSelfOrThrow() {
			if (this != _self())
				throw err_wrong_context();
		}
		
	};
	
	Context self() {
		acontext * c = _self();
		return c ? c->self() : _globalContext;
	}
	
	Context create() {
		AContext ret;
		new acontext(ret, "global");
		return ret;
	}
}
