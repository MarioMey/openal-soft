#include "config.h"

#ifdef HAVE_XMMINTRIN_H
#include <xmmintrin.h>
#endif

#include "AL/al.h"
#include "AL/alc.h"
#include "alMain.h"
#include "alu.h"


static __inline void ApplyCoeffs(ALuint Offset, ALfloat (*RESTRICT Values)[2],
                                 ALfloat (*RESTRICT Coeffs)[2],
                                 ALfloat left, ALfloat right)
{
    const __m128 lrlr = { left, right, left, right };
    ALuint c;
    for(c = 0;c < HRIR_LENGTH;c += 2)
    {
        const ALuint o0 = (Offset++)&HRIR_MASK;
        const ALuint o1 = (Offset++)&HRIR_MASK;
        __m128 vals = { 0.0f, 0.0f, 0.0f, 0.0f };
        __m128 coeffs = { 0.0f, 0.0f, 0.0f, 0.0f };

        vals = _mm_loadl_pi(vals, (__m64*)&Values[o0][0]);
        vals = _mm_loadh_pi(vals, (__m64*)&Values[o1][0]);
        coeffs = _mm_loadl_pi(coeffs, (__m64*)&Coeffs[c  ][0]);
        coeffs = _mm_loadh_pi(coeffs, (__m64*)&Coeffs[c+1][0]);

        vals = _mm_add_ps(vals, _mm_mul_ps(coeffs, lrlr));

        _mm_storel_pi((__m64*)&Values[o0][0], vals);
        _mm_storeh_pi((__m64*)&Values[o1][0], vals);
    }
}

#define SUFFIX SSE
#define SAMPLER point32
#include "mixer_inc.c"
#undef SAMPLER
#define SAMPLER lerp32
#include "mixer_inc.c"
#undef SAMPLER
#define SAMPLER cubic32
#include "mixer_inc.c"
#undef SAMPLER
#undef SUFFIX