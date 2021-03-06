// CollatzInverse.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <exception>

using namespace std;

#define PRINTSEQ(...) printf(__VA_ARGS__)
#define PRINTSEQ(...)

#define PRINTSTATUS(...) printf(__VA_ARGS__)
#define PRINTSTATUS(...)

// This can be improved by making use of a heap stack to prevent blowing the stack.
// the stak will need to be a sequence length to evaluate, each case is put on the stack to be evaluted
// The stack will fuction more like an instruction stack.
// Each iteration will just process the top of the stack next instruction, until instrcution stack has been reduce.
void computeReverseCollatzSequenceLengthIterator(long sequenceLength, long value, long upperLimitOverTwo, long& longestSequence, long& longestCollatzSequenceSeed)
{
	sequenceLength++;

	if ((value & 0x01) == 0x00)
	{
		// Swapping the order of processing the lower value first, will ensure that when their are sequence that have identical number of iteration
		// it will pefectly match with the brute force method for validation.
		long nextValue = (value - 1);

		if ((nextValue % 3) == 0)
		{
			// This may not reduce to 1, because that is the start see then we go back in a circle again.
			computeReverseCollatzSequenceLengthIterator(sequenceLength, nextValue / 3, upperLimitOverTwo, longestSequence, longestCollatzSequenceSeed);
		}

		if (value > upperLimitOverTwo)
		{
			if (sequenceLength > longestSequence)
			{
				longestSequence = sequenceLength;
				longestCollatzSequenceSeed = value;		// typicall first come first server;			
			}
		}
		else
		{
			computeReverseCollatzSequenceLengthIterator(sequenceLength, value * 2, upperLimitOverTwo, longestSequence, longestCollatzSequenceSeed);
		}		
	}
	else
	{
		if (value > upperLimitOverTwo)
		{
			if (sequenceLength > longestSequence)
			{
				longestSequence = sequenceLength;
				longestCollatzSequenceSeed = value;		// typicall first come first server;			
			}
		}
		else
		{
			computeReverseCollatzSequenceLengthIterator(sequenceLength, value * 2, upperLimitOverTwo, longestSequence, longestCollatzSequenceSeed);
		}		
	}
}

void computeReverseCollatzSequenceLength(long upperLimit, long& longestSequence, long& longestCollatzSequenceSeed)
{
	// 1,2,4 are exceptions, ebcause 4 can reduce to one, which we don't every want to process again.
	// so simple step of correcting this for performance reasons with out adding an additional check just start at 8.
	
	if (upperLimit == 1)
		longestCollatzSequenceSeed = longestSequence = 1;
	else if (upperLimit < 4)
		longestCollatzSequenceSeed = longestSequence = 2;
	else if (upperLimit < 8)
		longestCollatzSequenceSeed = longestSequence = 3;
	else
		computeReverseCollatzSequenceLengthIterator(3, 8, upperLimit / 2, longestSequence, longestCollatzSequenceSeed);
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
		PRINTSEQ("%li -> ", value);

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
			PRINTSEQ("L\n");
			return 0; // Throw upper limit exception, soft error.
		}

		sequenceLength++;
	}

	PRINTSEQ("1\n");

	return sequenceLength;
}


int computerCollatzSeqeuenceBrutForceUsingReduction(long upperLimit, long& longestSequence, long& longestCollatzSequenceSeed)
{
	
	// existing program validation.

	PRINTSTATUS("----------------------------------------\n");

	longestSequence = 0;
	longestCollatzSequenceSeed = -1;

	// This is typicall brutforce.
	for (long sequence = 1; sequence <= upperLimit; sequence++)
	{
		PRINTSEQ("-----------------------------\n");
		long sequenceLength = computeCollatzSequenceLength(upperLimit, sequence);

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
		PRINTSTATUS("Their exist no longest collatz Sequence with this upper limit\n");
		return 1;
	}
	else
	{
		PRINTSTATUS("Longest Collatz Sequence is %li and start with %li\n", ValidlongestSequence, ValidlongestCollatzSequenceSeed);
		return 0;
	}

}

int computeReverseCollatzSequence(long upperLimit, long& longestSequence, long& longestCollatzSequenceSeed)
{
	PRINTSTATUS("-----------------------------\n");
	
	longestSequence = 0;
	longestCollatzSequenceSeed = -1;

	computeReverseCollatzSequenceLength(upperLimit, longestSequence, longestCollatzSequenceSeed);

	if (longestCollatzSequenceSeed == -1)
	{
		PRINTSTATUS("Their exist no longest collatz Sequence with this upper limit\n");
		return 1;
	}
	else
	{
		PRINTSTATUS("Longest Collatz Sequence is %li and start with %li\n", longestSequence, longestCollatzSequenceSeed);
		return 0;
	}	
}

int validateInverseCollatzSequence()
{
	// first failure.
	// Upper Collatz Limit : 81376 - 86000 their seem to be ablot of duplicates.
	// FailedLongest Collatz Sequence is 198 and start with 57048

	// Just two differant chains were identified, 2nd lot every.
	// computeCollatzSequenceLength(81376, 57144);
	// computeCollatzSequenceLength(81376, 57048);
	// Inverse funciton validated to work for upper limit of 1 throught 688127

	for (long upperLimit = 1; upperLimit < 20000; upperLimit++)
	{
		if ((upperLimit & 0x00FF) == 0x00FF)
			printf("Upper Collatz Limit: %li\n", upperLimit);

		long longestSequenceReverse = 0;
		long longestCollatzSequenceSeedReverse = -1;

		long longestSequenceReduction = 0;
		long longestCollatzSequenceSeedReduction = -1;

		computeReverseCollatzSequence(upperLimit, longestSequenceReverse, longestCollatzSequenceSeedReverse);

		computerCollatzSeqeuenceBrutForceUsingReduction(upperLimit, longestSequenceReduction, longestCollatzSequenceSeedReduction);

		if (longestSequenceReverse == longestSequenceReduction)
		{
			PRINTSTATUS("Match\n");
		}
		else
		{
			printf("Failed\n");
			printf("Longest Collatz Sequence is %li and start with %li\n", longestSequenceReverse, longestCollatzSequenceSeedReverse);
			printf("Longest Collatz Sequence is %li and start with %li\n", longestSequenceReduction, longestCollatzSequenceSeedReduction);
			return 1;
		}
	}

	return 0;
}

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
	
	if (upperLimit < 0)
	{
		printf("Upper collatz Limit, must be a positive number.\n");
		return 1;
	}

	//return validateInverseCollatzSequence();
	
	long longestSeqeuenceLength, temp2;
	computeReverseCollatzSequence(upperLimit, longestSeqeuenceLength, temp2);

	//computerCollatzSeqeuenceBrutForceUsingReduction(upperLimit, longestSeqeuenceLength, temp2);

	printf("Longest Sequence Length %d", longestSeqeuenceLength);
	
	return 0;
}

