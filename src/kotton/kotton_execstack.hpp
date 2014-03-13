#if !defined(_IMPL_KOTTON_EXECSTACK_HPP_)
#define _IMPL_KOTTON_EXECSTACK_HPP_

#include <cstddef>
#include <setjmp.h>

#include "kotton/userfunc.hpp"

namespace kotton {
	struct stack {
		const size_t sz;
		char * const loc;
		
		stack(size_t size = 0);
		
		~stack() {
			checkGuard();
			delete [] loc;
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
	struct execution {
		
		execution(userfunc & f, const stack & s)
			:m_state(exec_state::notReady),
			m_f(f),
			m_s(s){
				enter();
		}
		
		bool proceed();
		void yield();
		
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
