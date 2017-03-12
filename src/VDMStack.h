/*
 * NmeaStack.h
 *
 *  Created on: Mar 11, 2017
 *      Author: kbp
 */

#ifndef VDMSTACK_H_
#define VDMSTACK_H_

#include <stdint.h>
#include <cstring>

class VDMStack {
public:
	VDMStack(uint8_t stackSize);
	virtual ~VDMStack();

	unsigned int push (
			uint8_t pkgCnt,
	  	  	uint8_t pkgNmb,
	  	  	unsigned int seqMessageId,
	  	  	unsigned int length,
	  	  	char* buf);

	bool pop(unsigned int seqMessageId, char* buf);

private:
	unsigned int stackSize;
	unsigned int nextId;
	uint8_t pkgNmbOne;

	struct VDMelement {
		unsigned int stackId;
		uint8_t nextIdx;

		uint8_t pkgCnt;
		uint8_t pkgNmb;
		unsigned int seqMessageid;
		unsigned int length;
		char* buf;
	};
	struct VDMelement *stack;


	void pushFragment(
			uint8_t pkgCnt,
	  	  	uint8_t pkgNmb,
	  	  	unsigned int seqMessageId,
	  	  	unsigned int length,
	  	  	char* buf);

	struct VDMelement* getStackEntry(unsigned int length);
	bool isComplete(unsigned int seqMessageId);
	unsigned int getLength(unsigned int seqMessageId);
};

#endif /* VDMSTACK_H_ */
