
#pragma once

namespace sal
{
	template<typename ElementType>
	ElementType&& MoveElement(ElementType& Reference)
	{
		return static_cast<ElementType&&>(Reference);
	}

	void LogWait();

	void LogMsg(const char* InText, bool InEndLine = true);

	void LogInt(const int InInt, bool InEndLine = true);
}