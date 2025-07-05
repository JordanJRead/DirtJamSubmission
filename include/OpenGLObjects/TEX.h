#ifndef TEX_H
#define TEX_H

#include <glad/glad.h>

/// <summary>
/// Wrapper class for automatically managing OpenGL object memory
/// </summary>
class TEX {
public:
	TEX() { glGenTextures(1, &mID); }
	~TEX() { glDeleteTextures(1, &mID); }
	operator unsigned int() const { return mID; }
	void use(int target) const { glBindTexture(target, mID); }

	TEX(const TEX&) = delete;
	TEX(TEX&&) = delete;
	TEX& operator=(const TEX&) = delete;
	TEX& operator=(TEX&&) = delete;

private:
	unsigned int mID;
};

#endif