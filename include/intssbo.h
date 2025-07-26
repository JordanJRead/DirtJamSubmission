#ifndef INT_SSBO_H
#define INT_SSBO_H

#include "OpenGLObjects/BUF.h"
#include <vector>

class IntSSBO {
public:
	IntSSBO(int bindingIndex);
	void UploadData(const std::vector<int> data, int usage) const;

private:
	BUF mBuf;
};

#endif