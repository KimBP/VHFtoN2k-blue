/*
 * NmeaStack.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: kbp
 */

#include <VDMStack.h>

VDMStack::VDMStack(uint8_t stackSize)
: stackSize(stackSize), nextId(stackSize), pkgNmbOne(stackSize), stack(0)
{
	stack = new VDMelement [stackSize];

	for(uint8_t i = 0; i < stackSize; i++) {
		stack[i].buf = 0;
		stack[i].stackId = i;
		stack[i].seqMessageid = 0;
		stack[i].nextIdx = stackSize;
	}
}

VDMStack::~VDMStack() {
	for(uint8_t i = 0; i < stackSize; i++) {
		if (stack[i].buf) {
			delete[] stack[i].buf;
		}
	}
	delete[] stack;
}

unsigned int VDMStack::push (
		uint8_t pkgCnt,
  	  	uint8_t pkgNmb,
  	  	unsigned int seqMessageId,
  	  	unsigned int length,
  	  	char* buf)
{
	pushFragment(pkgCnt, pkgNmb, seqMessageId, length, buf);

	if (pkgCnt == pkgNmb) {
		if (isComplete(seqMessageId)) {
			return getLength();
		}
	}
	pkgNmbOne = stackSize;

	return 0;
}

struct VDMStack::VDMelement* VDMStack::getStackEntry(unsigned int length)
{
	uint8_t i;
	unsigned int oldestId = nextId;
	struct VDMelement* pOldest = 0;

	// Find oldest
	for(i = 0; i < stackSize; i++) {
		if (stack[i].stackId < oldestId) {
			oldestId = stack[i].stackId;
			pOldest = &stack[i];
		}
	}

	// Clean up old data:
	delete[] pOldest->buf;

	// Prepare for new data
	pOldest->length = length;
	pOldest->buf = new char [length];
	pOldest->stackId = nextId;
	pOldest->nextIdx = stackSize;

	nextId++;

	return pOldest;
}

void VDMStack::pushFragment(
		uint8_t pkgCnt,
  	  	uint8_t pkgNmb,
  	  	unsigned int seqMessageId,
  	  	unsigned int length,
  	  	char* buf)
{
	struct VDMelement* entry = getStackEntry(length);
	entry->pkgCnt = pkgCnt;
	entry->pkgNmb = pkgNmb;
	entry->seqMessageid = seqMessageId;
	memcpy(entry->buf, buf, length);
}

bool VDMStack::isComplete(unsigned int seqMessageId)
{
  // Find first with seqMessageId
	uint8_t i;
	for (i=0; i < stackSize; i++) {
		if ((stack[i].seqMessageid == seqMessageId) &&
			( 1 == stack[i].pkgNmb) ) {
			pkgNmbOne = i;
			break;
		}
	}

	if (i == stackSize) {
		// didn't even find it
		return false;
	}

	uint8_t pkgCnt = stack[i].pkgCnt;
	uint8_t pkgNmb = 1; // We know number one if found
	uint8_t prevIdx = i;
	while (pkgNmb < pkgCnt) {
		pkgNmb++;
		for (i=0; i < stackSize; i++) {
			if (stack[i].pkgNmb == pkgNmb) {
				// Found fragment
				stack[prevIdx].nextIdx = i;
				prevIdx = i;
				break;
			}
		}
		if (i == stackSize) {
			// Didn't find fragment
			return false;
		}
	}
	stack[i].nextIdx = stackSize;
	return true;
}

unsigned int VDMStack::getLength()
{
  unsigned int totalLen = 0;
  uint8_t i = pkgNmbOne;
  while (stack[i].nextIdx != stackSize) {
	  totalLen += stack[i].length;
	  i = stack[i].nextIdx;
  }
  return totalLen;
}

bool VDMStack::pop(unsigned int seqMessageId, char* buf)
{
	// Expects to be called right after push has returned a positive length
	// Sanity checks
	if (pkgNmbOne == stackSize) return false;
	if (stack[pkgNmbOne].seqMessageid != seqMessageId) return false;

	uint8_t pkgCnt = stack[pkgNmbOne].pkgCnt;
	uint8_t pkgNmb = 0;
	uint8_t i = pkgNmbOne;

	while (pkgNmb < pkgCnt) {
		pkgNmb++;
		unsigned int length = stack[i].length;
		memcpy(buf, stack[i].buf, length);
		buf += length;
		i = stack[i].nextIdx;
		// No need to free - will happen when allocated next time
	}
	return true;
}
