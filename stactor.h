#pragma once
#ifndef C_STACK_VECTOR_H
#define C_STACK_VECTOR_H
	
	/*
		stactor is a C based-vector library oriented around using a stack-memory allocated type.
		The neat thing about C is that you can type-alias any struct that conforms to the memory
		layout of stactor and have generics with stactor.

		Example, see struct definition of stactor below and compare:
		typedef struct stactor_int { size_t typesize, length, iterator; int stack[128]; } stactor_int;

		The final stack array is a different size, but the memory layout is otherwise the same
		allowing you to cast stactor_int -> stactor and perform the same operations.
	*/

	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include <stdint.h>
	
	#ifndef C_UNIFORM_TYPES
	#define C_UNIFORM_TYPES
		typedef uint8_t uint08_t;
		typedef float float32_t;
		typedef double float64_t;
		typedef bool bool_t;
		typedef char char_t;
		typedef int8_t int08_t;
		typedef void void_t;
	#endif

	#define STACTOR_MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
	#define STACTOR_MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
	#define STACTOR_CLAMP(min, max, value) STACTOR_MAX(min, STACTOR_MIN(value, max))
	#if !defined(STACTOR_PARAMS)
		#define STACTOR_SIZEOF_PARAMS (sizeof(size_t) * 4)
	#endif

	// size (sizeof struct), type (sizeof stack type), length (length of stack), iterator (position of stack).
	typedef struct stactor {
		size_t size, type, length, iterator;
		int08_t stack[256];
	} stactor;

	void_t stactor_fill(void_t* stact, size_t structsize, size_t typesize) {
		memset(stact, (int08_t)NULL, structsize);
		((stactor*)stact)->size = structsize;
		((stactor*)stact)->type = typesize;
		((stactor*)stact)->length = structsize - STACTOR_SIZEOF_PARAMS;
	}

	size_t stactor_count(void_t* stact) { return ((stactor*)stact)->iterator; }

	size_t stactor_max(void_t* stact) { return ((stactor*)stact)->length / ((stactor*)stact)->type; }

	void_t stactor_move(void_t* stact, size_t move_iter) { ((stactor*)stact)->iterator = move_iter; }

	void_t stactor_clear(void_t* stact) { memset(((stactor*)stact)->stack, (int08_t)NULL, ((stactor*)stact)->length); }

	typedef int(__cdecl* qsort_callback)(void const*, void const*);
	void_t stactor_qsort(void_t* stact, qsort_callback sorter) { qsort(((stactor*)stact)->stack, ((stactor*)stact)->length, ((stactor*)stact)->type, sorter); }

	void_t* stactor_find(void_t* stact, size_t index) { return ((stactor*)stact)->stack + (index * ((stactor*)stact)->type); }

	void_t stactor_clone(void_t* src, void_t* dst) { memcpy(dst, src, ((stactor*)src)->size); }

	void_t stactor_copy(void_t* dst, void_t* src, size_t dstoff, size_t srcoff, size_t length) {
		stactor* src_ptr = src, * dst_ptr = dst;
		size_t dstlen = STACTOR_CLAMP(0, dst_ptr->length - dstoff, length), srclen = STACTOR_CLAMP(0, src_ptr->length - srcoff, length);
		memcpy_s(dst_ptr->stack + dstoff, dstlen, src_ptr->stack + srcoff, srclen);
	}

	bool_t stactor_replace(void_t* stact, size_t index, const void_t* data) {
		stactor* stact_ref = (stactor*) stact;
		index = STACTOR_CLAMP(0, stact_ref->iterator, index);
		if (index >= stact_ref->length) return false;
		memcpy(stact_ref->stack + (index * stact_ref->type), data, stact_ref->type);
		if (index == stact_ref->iterator) stact_ref->iterator ++;
		return false;
	}

	bool_t stactor_replaceext(void_t* stact, size_t index, const void_t* data, size_t datalen) {
		stactor* stact_ref = (stactor*)stact;
		index = STACTOR_CLAMP(0, stact_ref->iterator, index);
		if (index >= stact_ref->length) return false;
		memcpy(stact_ref->stack + (index * stact_ref->type), data, STACTOR_CLAMP(0, stact_ref->type, datalen));
		if (index == stact_ref->iterator) stact_ref->iterator++;
		return false;
	}

	bool_t stactor_push(void_t* stact, const void_t* data) {
		stactor* stact_ref = (stactor*)stact;
		size_t iter = stact_ref->iterator * stact_ref->type;
		if (iter >= stact_ref->length) return false;
		memcpy(stact_ref->stack + iter, data, stact_ref->type);
		stact_ref->iterator ++;
		return true;
	}

	bool_t stactor_pushext(void_t* stact, const void_t* data, size_t datalen) {
		stactor* stact_ref = (stactor*)stact;
		size_t iter = stact_ref->iterator * stact_ref->type;
		if (iter >= stact_ref->length) return false;
		
		memcpy(stact_ref->stack + iter, data, STACTOR_CLAMP(0, stact_ref->type, datalen));
		
		stact_ref->iterator ++;
		return true;
	}

	bool_t stactor_insert(void_t* stact, size_t index, const void_t* data) {
		stactor* stact_ref = (stactor*)stact;
		index = STACTOR_CLAMP(0, stact_ref->iterator, index);

		void_t* dst = ((int08_t*)stact_ref->stack) + ((index + 1) * stact_ref->type);
		void_t* src = ((int08_t*)stact_ref->stack) + (index * stact_ref->type);
		size_t length = stact_ref->length - (index * stact_ref->type);
		
		memmove(dst, src, length);
		memcpy(src, data, stact_ref->type);
		stact_ref->iterator ++;
		return true;
	}

	bool_t stactor_insertext(void_t* stact, size_t index, const void_t* data, size_t datalen) {
		stactor* stact_ref = (stactor*)stact;
		index = STACTOR_CLAMP(0, stact_ref->iterator, index);

		void_t* dst = ((int08_t*)stact_ref->stack) + ((index + 1) * stact_ref->type);
		void_t* src = ((int08_t*)stact_ref->stack) + (index * stact_ref->type);
		size_t length = stact_ref->length - ((index + 1) * stact_ref->type);

		memmove(dst, src, length);
		memcpy(src, data, stact_ref->type);
		stact_ref->iterator ++;
		return true;
	}

	bool_t stactor_remove(void_t* stact, size_t index) {
		stactor* stact_ref = (stactor*)stact;
		index = STACTOR_CLAMP(0, stact_ref->iterator, index);

		memmove(stact_ref->stack + (index * stact_ref->type), stact_ref + ((index + 1) * stact_ref->type), stact_ref->length - (index * stact_ref->type));
		
		stact_ref->iterator --;
		return true;
	}

#endif
