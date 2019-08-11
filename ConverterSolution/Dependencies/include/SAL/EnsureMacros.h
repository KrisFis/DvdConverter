
#pragma once

#include "EnsuresAssertion.h"

#ifdef _DEBUG
	
	// Ensure inside if statement
	#define ENSURE_CONDITION(expression) !(expression)) throw; if(!(expression)

	// Ensures validity with return
	#define ENSURE_TRUE(expression, ...) if(!(expression)) { throw; return __VA_ARGS__;}
	#define ENSURE_VALID(ptr, ...) if(!IsValid(ptr)) {throw; return __VA_ARGS__;}
	#define ENSURE_NO_ENTRY(...) {throw; return __VA_ARGS__;}
#else
	#define ENSURE_CONDITION(expression)

	// Ensures takes no effect when not in debug mode
	#define ENSURE_TRUE(expression, ...)
	#define ENSURE_VALID(ptr, ...)
	#define ENSURE_NO_ENTRY(...)
#endif