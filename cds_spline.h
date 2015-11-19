/* cds_spline.h
 * Experiments with metaprogramming in C.
 * No warranty implied; use at your own risk.
 *
 * Do this:
 *   #define CDS_SPLINE_IMPLEMENTATION
 * before including this file in *one* C/C++ file to provide the function
 * implementations.
 *
 * For a unit test on gcc/Clang:
 *   cc -Wall -std=c89 -g -x c -DCDS_SPLINE_TEST -o test_cds_spline.exe cds_spline.h
 *
 * For a unit test on Visual C++:
 *   "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat"
 *   cl -W4 -nologo -TC -DCDS_SPLINE_TEST /Fetest_cds_spline.exe cds_spline.h
 * Debug-mode:
 *   cl -W4 -Od -Z7 -FC -MTd -nologo -TC -DCDS_SPLINE_TEST /Fetest_cds_spline.exe cds_spline.h
 *
 * LICENSE:
 * This software is in the public domain. Where that dedication is not
 * recognized, you are granted a perpetual, irrevocable license to
 * copy, distribute, and modify this file as you see fit.
 */

#if !defined(CDS_SPLINE_H)
#define CDS_SPLINE_H

#if   defined(_MSC_VER)
#   define CDS_SPLINE_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#   define CDS_SPLINE_PLATFORM_OSX
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   include <unistd.h>
#   define CDS_SPLINE_PLATFORM_POSIX
#else
#   error Unsupported platform
#endif

#if defined(_MSC_VER)
#   define CDS_SPLINE_COMPILER_MSVC
#elif defined(__clang__)
#   define CDS_SPLINE_COMPILER_CLANG
#elif defined(__GNUC__)
#   define CDS_SPLINE_COMPILER_GCC
#else
#   error Unsupported compiler
#endif

#if defined(CDS_SPLINE_STATIC)
#   define CDS_SPLINE_DEF static
#else
#   define CDS_SPLINE_DEF extern
#endif

#if   defined(CDS_SPLINE_PLATFORM_WINDOWS)
#   define CDS_SPLINE_INLINE __forceinline
#elif defined(CDS_SPLINE_PLATFORM_OSX)
#   ifdef __cplusplus
#       define CDS_SPLINE_INLINE inline
#   else
#       define CDS_SPLINE_INLINE
#   endif
#elif defined(CDS_SPLINE_PLATFORM_POSIX)
#   ifdef __cplusplus
#       define CDS_SPLINE_INLINE inline
#   else
#       define CDS_SPLINE_INLINE
#   endif
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1700)
    /* no stdint.h in VS2010 and earlier.
     * LONG and DWORD are guaranteed to be 32 bits forever, though.
     */
    typedef INT8  cds_spline_s8;
    typedef BYTE  cds_spline_u8;
    typedef LONG  cds_spline_s32;
    typedef DWORD cds_spline_u32;
#else
#   include <stdint.h>
    typedef  int8_t  cds_spline_s8;
    typedef uint8_t  cds_spline_u8;
    typedef  int32_t cds_spline_s32;
    typedef uint32_t cds_spline_u32;
#endif

typedef float  cds_spline_r32;
typedef double cds_spline_r64;
typedef cds_spline_s32 cds_spline_bool32_t;
typedef cds_spline_s32 cds_spline_error_t;

#if defined(CDS_SPLINE_COMPILER_MSVC)
#   define CDS_SPLINE_ALIGN(n) __declspec(align(n))
#elif defined(CDS_SPLINE_COMPILER_CLANG) || defined(CDS_SPLINE_COMPILER_GCC)
#   define CDS_SPLINE_ALIGN(n) __attribute__((__aligned__(n)))
#else
#   error Unsupporter compiler
#endif

/**
 * Uses of splines:
 * 1 Hey, I have a bunch of values of a parameter, and want to interpolate smoothly.
 *   I'll be handing you a bunch of points up-front, and querying for points along
 *   the path after that. I know exactly how many points I'll need up front
 * 2 Hey, I'm just like the case #1, but now I want to edit the parameter values
 *   and see the path update interactively, preferably in an O(1) fashion. This may
 *   inclulde inserting/removing control points.
 * 3 Hey, I'm making some sort of demoscene effect, and I want an infinite procedural spline.
 *   I'm going to adding points to the end, and removing them from the beginning as things
 *   fade out. I want to specify how many points to store simultaneously,
 *
 * What about dimensionality? 1D, 2D, and 3D are all reasonably common. 4D seems plausible.
 * Higher than that seems a bit esoteric.
 * So...templates, with specialization for common cases? Or just implement the common cases,
 * live with some code duplication, and leave >4D unsupported?
 *
 * Operations on splines:
 * - Create (with memory arena based on maximum length)
 * - 
 */

typedef union cds_spline_vec2 {
    struct {
        float x,y;
    };
    float elems[2];
} cds_spline_vec2;

typedef union cds_spline_vec3 {
    struct {
        float x,y,z;
    };
    float elems[3];
} cds_spline_vec3;

typedef union cds_spline_vec4 {
    struct {
        float x,y,z,w;
    };
    float elems[4];
} cds_spline_vec4;

typedef union cds_spline_mat44 {
    struct {
        float m00,m01,m02,m03;
        float m10,m11,m12,m13;
        float m20,m21,m22,m23;
        float m30,m31,m32,m33;
    };
    cds_spline_vec4 rows[4];
    float elems[16];
} cds_spline_mat44;

typedef struct cds_spline_cr2_segment { /* TODO: align to 16 */
    cds_spline_mat44 geomMatrix;
    cds_spline_vec2 controlPoint[2]; /* point at the beginning of this segment */
    cds_spline_s32 next;
    cds_spline_s32 prev;
} cds_spline_cr2_segment;

typedef struct cds_spline_cr2
{
    cds_spline_cr2_segment *segments;
    cds_spline_s32 first, last;
    cds_spline_s32 numSegments;
    cds_spline_s32 nextFree;
    cds_spline_s32 capacity;
} cds_spline_cr2;

#endif /*-------------- end header file ------------------------*/

/*-------------------- begin implementation --------------------*/
#if defined(CDS_SPLINE_TEST)
#   define CDS_SPLINE_IMPLEMENTATION
#endif
#if defined(CDS_SPLINE_IMPLEMENTATION)

#define CDS_SPLINE_ALIGN_TO(n,po2) ( ((n)+(po2)-1) & ~((po2)-1) )
    
#endif /*------------ end implementation ------------------------*/



#if defined(CDS_SPLINE_TEST) /*---------- self-test section -----*/

#include <assert.h>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cds_spline_cr2 spline;
    cds_spline_vec2 controlPoints[] = {
        {{ 0, 1}},
        {{ 1, 0}},
        {{ 0,-1}},
        {{-1, 0}},
        {{ 0, 1}},
        {{ 1, 0}},
        {{ 0,-1}},
    };
    cds_spline_s32 controlPointCount = sizeof(controlPoints) / sizeof(controlPoints[0]);
    cds_spline_s32 segmentCount = controlPointCount-3;
    cds_spline_s32 iSeg;
    const float tau = 0.5f;
    assert(segmentCount >= 0);
    spline.segments = (cds_spline_cr2_segment*)memalign(16, segmentCount*sizeof(cds_spline_cr2_segment));
    for(iSeg=0; iSeg<segmentCount; ++iSeg) {
        cds_spline_s32 iSamp;
        cds_spline_mat44 *m = &spline.segments[iSeg].geomMatrix;
        cds_spline_vec2 *p0 = controlPoints+iSeg, *p1 = p0+1, *p2 = p1+1, *p3 = p2+1;
        m->m00 = p1->x;
        m->m01 = p1->y;
        m->m02 = 0;
        m->m03 = 0;
        m->m10 = (-tau)*p0->x + (tau)*p2->x;
        m->m11 = (-tau)*p0->y + (tau)*p2->y;
        m->m12 = 0;
        m->m13 = 0;
        m->m20 = (2.0f*tau)*p0->x + (tau-3.0f)*p1->x + (3.0f-2.0f*tau)*p2->x + (-tau)*p3->x;
        m->m21 = (2.0f*tau)*p0->y + (tau-3.0f)*p1->y + (3.0f-2.0f*tau)*p2->y + (-tau)*p3->y;
        m->m22 = 0;
        m->m23 = 0;
        m->m30 = (-tau)*p0->x + (2.0f-tau)*p1->x + (tau-2.0f)*p2->x + (tau)*p3->x;
        m->m31 = (-tau)*p0->y + (2.0f-tau)*p1->y + (tau-2.0f)*p2->y + (tau)*p3->y;
        m->m32 = 0;
        m->m33 = 0;

        for(iSamp=0; iSamp<=16; ++iSamp) {
            float u = (float)iSamp / 16.0f;
            cds_spline_vec2 samp = {
                {
                    ((m->m30*u + m->m20)*u + m->m10)*u + m->m00,
                    ((m->m31*u + m->m21)*u + m->m11)*u + m->m01,
                }
            };
            printf("seg=%2d u=%.3f samp=[%11.8f %11.8f]\n", iSeg, u, samp.x, samp.y);
        }
    }



    free(spline.segments);
    return 0;
}

#endif /*------------------- end self-test section ------------*/

