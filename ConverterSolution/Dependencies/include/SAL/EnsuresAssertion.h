
#pragma once

#include "BaseMacros.h"

namespace sal
{
	FORCEINLINE bool IsValid(void* InPtr) { return (InPtr != nullptr && InPtr != NULL); }
}