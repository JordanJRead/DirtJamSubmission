#ifndef CPU_GPU_PAIR_H
#define CPU_GPU_PAIR_H

template <typename T>
struct CPUGPUPair {
	CPUGPUPair(T gpu, T cpu) : mGPU{ gpu }, mCPU{ cpu } {}
	CPUGPUPair(T value) : mGPU{ value }, mCPU{ value } {}
	T mGPU;
	T mCPU;

	bool hasDiff() { return mGPU != mCPU; }
};

#endif