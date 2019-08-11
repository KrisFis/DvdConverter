
#pragma once

namespace sal
{
	// Basic "non-const" iterator class
	// Iterates thought allocator or any type of buffer
	template<typename ElementType>
	class TIterator
	{

	public: // Constructors and destructor
		
		// Constructor
		TIterator() = delete;

		TIterator(const TIterator<ElementType>& other)	{ CurrentAddress = other.CurrentAddress; }
		
		// Destructor
		virtual ~TIterator() { CurrentAddress = nullptr; }

		FORCEINLINE TIterator(byte* InAdress) : CurrentAddress(InAdress) { ENSURE_VALID(InAdress); }

	public: // operators

		TIterator& operator++()
		{
			Internal_Increment(1);
			return *(this);
		}

		TIterator operator++(int)
		{
			byte* oldAdress = CurrentAddress;
			Internal_Increment(1);
			return TIterator(oldAdress);
		}

		bool operator!=(const TIterator& other) const
		{
			return !(operator==(other));
		}

		bool operator==(const TIterator& other) const
		{
			return (CurrentAddress == other.CurrentAddress);
		}

		explicit operator byte*() const 
		{
			return CurrentAddress;
		}

		operator ElementType() const
		{
			return *reinterpret_cast<ElementType*>(CurrentAddress);
		}

		ElementType operator*() const
		{
			return *reinterpret_cast<ElementType*>(CurrentAddress);
		}

		ElementType* operator->() const
		{
			return reinterpret_cast<ElementType*>(CurrentAddress);
		}

	public: // Methods

		// Updates current iterator address
		// and returns itself
		TIterator<ElementType>& Update(byte* otherAddress)
		{
			ENSURE_VALID(otherAddress, *this);

			if (CurrentAddress != otherAddress) CurrentAddress = otherAddress;

			return *this;
		}

	private: // Internal Methods

		FORCEINLINE void Internal_Increment(const int8& PerTimes) { CurrentAddress += (sizeof(ElementType) * PerTimes); }

	private: // Fields
		byte* CurrentAddress;
	};
}