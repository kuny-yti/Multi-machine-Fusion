
#ifndef __CCPUID_HPP_INCLUDED
#define __CCPUID_HPP_INCLUDED

#include "ccpuid.h"

#if defined __cplusplus
extern "C" {
#endif

#define MAX_CPUIDINFO	0x100	// CCPUID类中最多保存多少条CPUIDINFO信息。

#if defined __cplusplus
};
#endif

#include <QString>
#include <QDebug>

// CPUID工具类.
class CCPUID{
public:
	enum {
		CPUFDescLen = 296	// CPUIDFIELD描述信息数组的长度.
	};
	static const CPUIDFIELDDESC	CPUFDesc[CPUFDescLen];	// CPUIDFIELD描述信息数组.
	static const char*	CacheDesc[0x100];	// 缓存描述信息数组.
	static const char*	SseNames[7];	// SSE级别的名称.
	static const char*	AvxNames[3];	// AVX级别的名称.
	
	CPUIDINFO	Info[MAX_CPUIDINFO+1];	// CPUID信息数组.

	CCPUID();
	static CCPUID& cur() { if(0==_cur._InfoCount){ _cur.RefreshAll(); } return _cur; }	// 当前处理器的CCPUID.
    void  test_main_();
    QString serial_number();
    int InfoCount() const { return _InfoCount; }	// Info数组的有效项目数.
	void RefreshInfo();	// 刷新信息.
	void RefreshProperty();	// 刷新属性.
	void RefreshAll();	// 刷新所有.
	LPCCPUIDINFO GetInfo(uint32_t InfoType, uint32_t ECXValue=0) const;	// 取得信息.
	void GetData(uint32_t CPUInfo[4], uint32_t InfoType, uint32_t ECXValue=0) const;	// 取得数据.
	uint32_t GetField(CPUIDFIELD cpuf) const;	// 取得CPUID字段
	// Property
	uint32_t LFuncStd() const { return _LFuncStd; }	// 最大的主功能号.
	uint32_t LFuncExt() const { return _LFuncExt; }	// 最大的扩展功能号.
	const char* Vendor() const { return _Vendor; }	// 厂商.
	const char* Brand() const { return _Brand; }	// 商标.
	const char* BrandTrim() const { return _BrandTrim; }	// 去掉首都空格后的商标.
	int mmx() const;	// 系统支持MMX.
	int hwmmx() const { return _hwmmx; }	// 硬件支持MMX.
	int sse() const;	// 系统支持SSE.
	int hwsse() const { return _hwsse; }	// 硬件支持SSE.
	int avx() const { return _avx; }	// 系统支持AVX.
	int hwavx() const { return _hwavx; }	// 硬件支持AVX.

private:
	static CCPUID _cur;	// 当前处理器的CCPUID. 为了方便日常使用.

	int _InfoCount;	// Info数组的有效项目数.

	// Property
	uint32_t _LFuncStd;	// 最大的主功能号.
	uint32_t _LFuncExt;	// 最大的扩展功能号.
	char _Vendor[13];	// 厂商.
	char _Brand[49];	// 商标.
	const char* _BrandTrim;	// 去掉首都空格后的商标.
	mutable int _mmx;	// 系统支持MMX.
	int _hwmmx;	// 硬件支持MMX.
	mutable int _sse;	// 系统支持SSE.
	int _hwsse;	// 硬件支持SSE.
	int _avx;	// 系统支持AVX.
	int _hwavx;	// 硬件支持AVX.

	void RefreshInfo_Put(uint32_t fid, uint32_t fidsub, uint32_t CPUInfo[4]);
	int	simd_mmx(int* phwmmx) const;
	int	simd_sse_level(int* phwsse) const;
	int	simd_avx_level(int* phwavx) const;

};



#endif	// #ifndef __CCPUID_HPP_INCLUDED
