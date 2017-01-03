#ifndef __MASK_H__
#define __MASK_H__

#include <stdint.h>
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/iteration.hpp>
#include <boost/preprocessor/arithmetic.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#ifndef __aligned__
	#define __aligned__ __attribute__((aligned(32)))
#endif

#define SHD_SSE_LENGTH		128
#define SSE_BYTE_NUM		16
#define SHD_AVX_LENGTH		256	
#define AVX_BYTE_NUM		32
#define MAX_ERROR_SSE		8
#define MAX_ERROR_AVX		16

extern uint8_t *MASK_SSE_END;
extern uint8_t *MASK_SSE_BEG;
extern uint8_t *MASK_AVX_END;
extern uint8_t *MASK_AVX_BEG;

extern uint8_t *MASK_SRS;

#endif // __MASK_H__
