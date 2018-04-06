#ifndef __LTKUTL_H__
#define __LTKUTL_H__

void LTKrint(uint64_t, const char *, ...);
uint32_t LTKrand(uint32_t);
void *LTKArray(size_t, ...);

#define RANDOM(seed) ((((seed) = LTKrand(seed)) >> 16) & ~0x8000)

#endif /* __LTKUTL_H__ */
