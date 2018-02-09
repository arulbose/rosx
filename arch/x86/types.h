
#ifndef __X86_TYPES_H
#define __X86_TYPES_H

/*      
 * These aren't exported outside the kernel to avoid name space clashes
 */     
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;
                        
typedef signed int s32; 
typedef unsigned int u32;
        
typedef signed long long s64;
typedef unsigned long long u64;

#define BITS_PER_LONG 32

#endif
