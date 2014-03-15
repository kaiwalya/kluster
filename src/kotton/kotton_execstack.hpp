#if !defined(_IMPL_KOTTON_EXECSTACK_HPP_)
#define _IMPL_KOTTON_EXECSTACK_HPP_

#include <cstddef>
#include <setjmp.h>

#include "kotton/userfunc.hpp"

namespace kotton {

	//A note on custom stacks:
	//http://rethinkdb.com/blog/handling-stack-overflow-on-custom-stacks/
	
	
	struct stack {
		const size_t sz;
		char * const loc;
		
		stack(size_t size = 0);
		
		~stack() {
			checkGuard();
			free(loc);
		}
		
		inline bool isCurrent () const {
			char t;
			char * pt = &t;
			
			if (pt >= loc && (pt + 1) <= (sz + loc)) {
				return true;
			}
			return false;
		}
		
		inline size_t freeSpace() const{
			assert(isCurrent());
			char t;
			char * pt = &t;
			return pt - loc;
		}
		
		void installGuard() const;
		
		bool checkGuard() const;
	};
	
	enum class exec_state {
		notReady, paused, playing, finished
	};
	
	/**
		Maintains two jump buffs, one is valid (mostly, unless you are in swap() function)
		Call self->yield only from self, like return from a function (thread will jump to whoever called you last time)
		Call other->proceed to jump the thread to someone else's stack
		
		TODO:
			We should be able to do this with just one jumpbuf?
			The concept of exitbuf should be on the tls. Where it should jump to a scheduling function.
			For cyclic jumps, A proceeds to B and B proceeds to A the exitbuf will be lost, i think...
	*/
	struct execution {
		
		execution(userfunc & f, const stack & s)
			:m_state(exec_state::notReady),
			m_f(f),
			m_s(s){
				enter();
		}
		
		bool proceed();
		void yield();
		
		const exec_state & state() {return m_state;}
		
	private:

		bool enter();
		void return_barrier();
		
		void swap();
		exec_state m_state;
		jmp_buf m_targetbuff;
		jmp_buf m_exitBuff;
		const userfunc m_f;
		const stack & m_s;
	};

}
#endif
