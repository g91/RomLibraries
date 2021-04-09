// *** RuExceptions.h ***

#ifndef _RUEXCEPTIONS_H_
#define _RUEXCEPTIONS_H_

#pragma unmanaged

class IRuException
{
public:
	virtual char *ToString(void) { return "Generic exception"; }
};

class CRuArgumentOutOfRangeException : public IRuException
{
public:
	virtual char *ToString(void) { return "Argument out of range exception"; }
};

class CRuStackEmptyException : public IRuException
{
public:
	virtual char *ToString(void) { return "Stack is empty exception"; }
};

class CRuHeapEmptyException : public IRuException
{
public:
	virtual char *ToString(void) { return "Heap is empty exception"; }
};

class CRuQueueEmptyException : public IRuException
{
public:
	virtual char *ToString(void) { return "Queue is empty exception"; }
};

class CRuInvalidCopyException : public IRuException
{
public:
	virtual char *ToString(void) { return "Invalid usage - cannot copy this object"; }
};

#pragma managed

#endif
