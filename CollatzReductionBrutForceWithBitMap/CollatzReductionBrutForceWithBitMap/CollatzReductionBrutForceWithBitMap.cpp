// Collatz_1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <exception>

using namespace std;
typedef unsigned int byte32;

// Typcially these class should temlated and specialize in a 32/bit and 64bit or potentially 128bit register size,
// depending on the micro controller and the size of its full associate array mapping, which would determine the efficiency
// of loading memory from into the register from the L1, L2, L3, fully associate map or maintain large section in the map.
class BitMap
{
public:

	BitMap(byte32 flags)
	{
		flags = flags / sizeof(byte32);	// This should be done with insight to the registers that the micro controller has avaliable to prevent alignment falts.
		flags++;

		PageSizeInBytes = sizeof(byte32);
		PageSizeInBits = PageSizeInBits * 8;

		Map = (byte32*)malloc(flags);
		MapEnd = Map + flags * PageSizeInBytes;
	}

	// Their are currently no virtual functions, so we don't have to worry about the deconstructor require to be virtual to ensrue we don't leak memory and things like that.
	~BitMap()
	{
		free(Map);
		Map = nullptr;
		MapEnd = nullptr;
	}

protected:

	virtual void setVisited(byte32 value)
	{
		byte32 *mapPage, offsetBitInPage;

		valueToMapPageAndOffset(value, mapPage, offsetBitInPage);

		*mapPage = *mapPage | offsetBitInPage;
	}

	virtual byte32 valueToMapPageAndOffset(byte32 value, byte32*& mapPage, byte32& offsetBitInPage)
	{
		byte32 page = value / PageSizeInBits;
		byte32 offset = value % PageSizeInBits;

		mapPage = Map + page * PageSizeInBytes;

		offsetBitInPage = 1 << offset;

		return offset;
	}

	byte32* Map = nullptr;
	byte32* MapEnd = nullptr;
	size_t PageSizeInBytes;
	size_t PageSizeInBits;
};

// This probably could be done using and iterator class aswell.
class NextCollatzSequence : protected BitMap
{
public:
	NextCollatzSequence(long upper_limit) : BitMap((size_t)upper_limit),
		UpperLimit(upper_limit),
		SeedValue(0)
	{

	}

	~NextCollatzSequence()
	{
	}


	long getUpperLimit()
	{
		return UpperLimit;
	}

	long NextSeed()
	{
		SeedValue++;
		ensureSeedValueNotUsedByAnExistingChain();

		return SeedValue;
	}

private:

	virtual void ensureSeedValueNotUsedByAnExistingChain()
	{
		byte32 *page, *mapPage, offsetBitInPage;

		byte32 offset = valueToMapPageAndOffset(SeedValue, mapPage, offsetBitInPage);

		if (*mapPage & offsetBitInPage)
		{
			SeedValue = SeedValue - offset;

			for (page = mapPage; page <= MapEnd; page += PageSizeInBytes)
			{
				if ((*page & ~(byte32)0x0) == ~(byte32)0x0)
				{
					SeedValue += PageSizeInBits;
					continue;
				}
				else
				{
					break;
				}
			}

			byte32 pageBytes = *page;	// Make this computation look invarient.
			byte32 bitOffset = 0x1;

			for (int offset = 0; offset < 32; offset++)
			{
				if (!!(pageBytes & bitOffset))
					break;
			}

			SeedValue += offset;
		}
	}

private:
	long UpperLimit;
	long SeedValue;
};

// Typically one could have look at using a factory as well.

class CollatzSequenceOrchatratorV2 : public NextCollatzSequence
{
public:
	CollatzSequenceOrchatratorV2(long upper_limit) : NextCollatzSequence(upper_limit)
	{

	}

	~CollatzSequenceOrchatratorV2()
	{

	}

	int compute()
	{
		long sequence = 1;

		printf("Upper Collatz Limit: %li\n", getUpperLimit());

		long longestSequence = 0;
		long longestCollatzSequenceSeed = -1;

		// This is typicall brutforce.
		for (long sequence = NextSeed(); sequence <= getUpperLimit(); sequence = NextSeed())
		{
			printf("-----------------------------\n");
			long sequenceLength = computeCollatzSequenceLength(getUpperLimit(), sequence);

			if (sequenceLength > longestSequence)
			{
				longestSequence = sequenceLength;
				longestCollatzSequenceSeed = sequence;
			}
			else if (sequenceLength < -1)
			{
				return (-1 * sequenceLength);
			}
		}

		if (longestCollatzSequenceSeed == -1)
		{
			printf("Their exist no longest collatz Sequence with this upper limit\n");
		}
		else
		{
			printf("Longest Collatz Sequence is %li and start with %li\n", longestSequence, longestCollatzSequenceSeed);
		}

		return 0;
	}

	long computeCollatzSequenceLength(long upperLimit, long value)
	{
		if (value > upperLimit)
			return -3;	// Throw and upper limit exception.

		if (value <= 0)
			return -4; // Initial value of number must be larger than 1, seed number is out of range.

					   // Reduce the upper limit such that we can evalute it before apply the growth case, which can leed to 
					   // register overflow, cause the number to become negative and the upper limit check failing, craushing the problem
					   // never stop as it is running in undefined domain range.
		upperLimit = (upperLimit - 1) / 3;

		long sequenceLength = 1;

		while (value != 1)
		{
			setVisited(value);

			printf("%li -> ", value);

			if ((value & 0x01) == 0x00)
			{
				// Even
				value /= 2;
			}
			else if (value <= upperLimit)
			{
				value = (3 * value) + 1;
			}
			else
			{
				printf("L\n");
				return 0; // Throw upper limit exception, soft error.
			}

			sequenceLength++;
		}

		printf("1\n");

		return sequenceLength;
	}
};


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Required a commandline parameter positive integer, that defines the upper limit for Collatz algorithm.");

		return 1;
	}

	const string upperLimitStr(argv[1]);
	size_t sizeofChar = sizeof(char);
	long upperLimit = 0;


	try
	{
		upperLimit = stol(upperLimitStr, &sizeofChar);
	}
	catch (invalid_argument& e)
	{
		// Handling invalid_arugement and out_of_range exceptions explicitly, to ensure we don't handle some other unexpected error, those others should be re-thrown.
		printf(e.what());
		return 2;
	}
	catch (out_of_range &e)
	{
		printf(e.what());
		return 3;
	}


	CollatzSequenceOrchatratorV2 sq2(upperLimit);

	return sq2.compute();

	return 0;
}



