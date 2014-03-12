
#include <iostream>
#include <exception>
#include <assert.h>
#include <stdio.h>
#include <memory>

#include "kotton/kotton.hpp"

namespace kotton {
	struct acontext;
	using AContext = std::shared_ptr<acontext>;
}
static __thread kotton::acontext * _self = nullptr;
static kotton::Context _globalContext = kotton::create();
namespace kotton {
	
	struct acontext: context {
		virtual Publisher delegate(userfunc f, std::string lastName) override {
			printf("In delegate\n");
			return nullptr;
		};
		virtual void become(userfunc f) override {
		};
		virtual void unbecome() override {
		};
		virtual bool nextMessage() override {
			return false;
		};
		
		Context self() {
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
		
	};
	
	Context self() {
		acontext * c = _self;
		return c ? c->self() : _globalContext;
	}
	
	Context create() {
		AContext ret;
		new acontext(ret, "global");
		return ret;
	}
}
