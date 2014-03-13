
#include <stdint.h>
#include <cstring>
#include <assert.h>

//Unix specific :( for getpagesize
#include <unistd.h>

#include "kotton_execstack.hpp"


static const size_t gPageSize = getpagesize();
static const size_t guardSize = sizeof(int64_t) * 2;
static const char guardData[sizeof(uint64_t)] = "kotton!";

namespace kotton {
	stack::stack(size_t size):sz(size ? size : gPageSize), loc(new char[sz]) {
		installGuard();
	}
	
	void stack::installGuard() const {
			assert(sz > guardSize);
			memset_pattern8(loc, guardData, guardSize);
	}
	
	bool stack::checkGuard() const {
		assert(sz > sizeof(int64_t));
		return memcmp(loc, guardData, guardSize) == 0;
	}
	
	bool execution::enter() {
		if (!m_s.isCurrent()) {
			auto stackTop = m_s.loc + m_s.sz;
			
			stackTop = (char *)((intptr_t)stackTop & ~(intptr_t)15);
			
			constexpr bool is32Bit = sizeof(void *) == 4;
			constexpr bool is64Bit = sizeof(void *) == 8;
			static_assert(is32Bit || is64Bit, "Current only 32 or 64 bit");
			
			if (is64Bit) {
				asm ("pushq %%rbp;"
					 "movq %%rsp, %%rbp;"
					 "movq %[stackTopR], %%rsp;"
					 :
					 :[stackTopR]"r"(stackTop)
					 :
					 );
				
				this->return_barrier();
				
				asm ("movq %rbp, %rsp;"
					 "popq %rbp;"
					 );
			}
			else if (is32Bit) {
				asm ("pushl %%ebp;"
					 "movl %%esp, %%ebp;"
					 "movl %[stackTopR], %%esp;"
					 :
					 :[stackTopR]"r"(stackTop)
					 :
					 );
				
				this->return_barrier();
				
				asm ("movl %ebp, %esp;"
					 "popl %ebp;"
					 );
			}
			
		}
		else {
			assert(false);
		}
		if (m_state == exec_state::notReady) {
			return true;
		}
		return false;
	}

	void execution::return_barrier() {
		swap();
	}

	void execution::swap() {
		if (m_state == exec_state::notReady) {
			m_s.installGuard();
			if (setjmp(m_targetbuff) == 0) {
				m_state = exec_state::paused;
				return;
			}
			m_f();
			m_s.checkGuard();
			m_state = exec_state::finished;
			longjmp(m_exitBuff, 1);
			assert(false);
		}
		if (m_state == exec_state::paused) {
			if (setjmp(m_exitBuff) == 0) {
				m_state = exec_state::playing;
				longjmp(m_targetbuff, 1);
			}
			return;
		}
		else if (m_state == exec_state::playing){
			if (setjmp(m_targetbuff) == 0) {
				m_state = exec_state::paused;
				longjmp(m_exitBuff, 1);
			}
			return;
		}
		else {
			assert(false);
		}
	}

	bool execution::proceed() {
		assert(!m_s.isCurrent());
		swap();
		return m_state == exec_state::paused;
	}

	void execution::yield() {
		assert(m_s.isCurrent());
		swap();
	}

}