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
 *   cc -Wall -std=c89 -g -x c -DCDS_SPLINE_TEST -o test_cds_spline.exe cds_spline.h -lm
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

#if defined(CDS_SPLINE_COMPILER_MSVC)
#   define CDS_SPLINE_ALIGN(n) __declspec(align(n))
#elif defined(CDS_SPLINE_COMPILER_CLANG) || defined(CDS_SPLINE_COMPILER_GCC)
#   define CDS_SPLINE_ALIGN(n) __attribute__((__aligned__(n)))
#else
#   error Unsupported compiler
#endif

#ifdef CDS_SPLINE_COMPILER_MSVC
#   pragma warning(push)
#   pragma warning(disable:4201) /* nameless struct/union */
#endif
typedef union cds_spline_vec1 {
    struct {
        cds_spline_r32 x;
    };
    cds_spline_r32 elems[1];
} cds_spline_vec1;

typedef union cds_spline_vec2 {
    struct {
        cds_spline_r32 x,y;
    };
    cds_spline_r32 elems[2];
} cds_spline_vec2;

typedef union cds_spline_vec3 {
    struct {
        cds_spline_r32 x,y,z;
    };
    cds_spline_r32 elems[3];
} cds_spline_vec3;

typedef union cds_spline_vec4 {
    struct {
        cds_spline_r32 x,y,z,w;
    };
    cds_spline_r32 elems[4];
} cds_spline_vec4;

typedef union cds_spline_mat14 {
    struct {
        cds_spline_r32 m00;
        cds_spline_r32 m10;
        cds_spline_r32 m20;
        cds_spline_r32 m30;
    };
    cds_spline_vec1 rows[4];
    cds_spline_r32 elems[4];
} cds_spline_mat14;

typedef union cds_spline_mat24 {
    struct {
        cds_spline_r32 m00,m01;
        cds_spline_r32 m10,m11;
        cds_spline_r32 m20,m21;
        cds_spline_r32 m30,m31;
    };
    cds_spline_vec2 rows[4];
    cds_spline_r32 elems[8];
} cds_spline_mat24;

typedef union cds_spline_mat34 {
    struct {
        cds_spline_r32 m00,m01,m02;
        cds_spline_r32 m10,m11,m12;
        cds_spline_r32 m20,m21,m22;
        cds_spline_r32 m30,m31,m32;
    };
    cds_spline_vec3 rows[4];
    cds_spline_r32 elems[12];
} cds_spline_mat34;

typedef union cds_spline_mat44 {
    struct {
        cds_spline_r32 m00,m01,m02,m03;
        cds_spline_r32 m10,m11,m12,m13;
        cds_spline_r32 m20,m21,m22,m23;
        cds_spline_r32 m30,m31,m32,m33;
    };
    cds_spline_vec4 rows[4];
    cds_spline_r32 elems[16];
} cds_spline_mat44;
#ifdef CDS_SPLINE_COMPILER_MSVC
#   pragma warning(pop)
#endif

typedef struct cds_spline_knot1 {
    cds_spline_vec1 position;
    cds_spline_vec1 tangent;
} cds_spline_knot1;

typedef struct cds_spline_knot2 {
    cds_spline_vec2 position;
    cds_spline_vec2 tangent;
} cds_spline_knot2;

typedef struct cds_spline_knot3 {
    cds_spline_vec3 position;
    cds_spline_vec3 tangent;
} cds_spline_knot3;

typedef struct cds_spline_knot4 {
    cds_spline_vec4 position;
    cds_spline_vec4 tangent;
} cds_spline_knot4;

typedef enum cds_spline_interp_style {
    kCdsSplineInterpStyleHermite               = 1,
    kCdsSplineInterpStyleBezier                = 2,
    kCdsSplineInterpStyleCardinal              = 3, /** Use difference between neighboring points as the tangent */
    kCdsSplineInterpStyleCentripetalCatmullRom = 4, /** Use difference between neighboring points as the tangent, avoiding cusps and self-intersection. */
} cds_spline_interp_style;

typedef enum cds_spline_error_t {
    kCdsSplineErrorNone                   = 0x00000000,

    kCdsSplineErrorInit_BufferSize        = 0x80000001,

    kCdsSplineErrorInsertKnot_KnotIndex   = 0x80010001,
    kCdsSplineErrorInsertKnot_MaxNumKnots = 0x80010002,

    kCdsSplineErrorSetKnot_KnotIndex      = 0x80020001,

    kCdsSplineErrorRemoveKnot_KnotIndex   = 0x80030001,
} cds_spline_error_t;

typedef struct cds_spline3 {
    cds_spline_mat34 *segmentMatrices;
    cds_spline_interp_style interpStyle;
    cds_spline_r32 tension;

    cds_spline_knot3 *knots;
    cds_spline_s32 numKnots;
    cds_spline_s32 maxNumKnots;
    cds_spline_s32 numSegments; /** Automatically kept up to date based on interpStyle and numKnots */
} cds_spline3;

CDS_SPLINE_DEF size_t
cds_spline3_buffer_size(cds_spline_interp_style interpStyle, cds_spline_s32 maxKnotCount);

CDS_SPLINE_DEF cds_spline_error_t
cds_spline3_init(cds_spline3 *outSpline, cds_spline_interp_style interpStyle, cds_spline_s32 maxKnotCount,
    void *buffer, size_t bufferSize);

CDS_SPLINE_DEF cds_spline_error_t
cds_spline3_set_tension(cds_spline3 *outSpline, cds_spline_r32 tension);

CDS_SPLINE_DEF cds_spline_error_t
cds_spline3_insert_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex, cds_spline_knot3 knot);

CDS_SPLINE_DEF cds_spline_error_t
cds_spline3_set_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex, cds_spline_knot3 knot);

CDS_SPLINE_DEF cds_spline_error_t
cds_spline3_remove_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex);

CDS_SPLINE_DEF cds_spline_vec3
cds_spline3_eval(const cds_spline3 *spline, cds_spline_r32 t);

CDS_SPLINE_DEF cds_spline_vec3
cds_spline3_evald(const cds_spline3 *spline, cds_spline_r32 t);

CDS_SPLINE_DEF cds_spline_vec3
cds_spline3_evaldd(const cds_spline3 *spline, cds_spline_r32 t);

#endif /*-------------- end header file ------------------------*/

/*-------------------- begin implementation --------------------*/
#if defined(CDS_SPLINE_TEST)
#   define CDS_SPLINE_IMPLEMENTATION
#endif
#if defined(CDS_SPLINE_IMPLEMENTATION)

#define CDS_SPLINE_ALIGN_TO(n,po2) ( ((n)+(po2)-1) & ~((po2)-1) )

#ifndef CDS_SPLINE_ASSERT
#   include <assert.h>
#   define CDS_SPLINE_ASSERT assert
#endif

#include <math.h>

#define CDS_SPLINE_MIN(x,y) ((x)<(y) ? (x) : (y))
#define CDS_SPLINE_MAX(x,y) ((x)>(y) ? (x) : (y))

static CDS_SPLINE_INLINE cds_spline_r32
cds_spline__roundf(cds_spline_r32 x) {
    cds_spline_r32 result = (float)floor((float)x);
    return result;
}

static CDS_SPLINE_INLINE void
cds_spline__get_int_and_frac(cds_spline_s32 numKnots, cds_spline_r32 t, cds_spline_s32 *outInt, cds_spline_r32 *outFrac) {
    cds_spline_r32 tMax = (cds_spline_r32)(numKnots-1);
    if (numKnots < 2 || t <= 0) {
        *outInt  = 0;
        *outFrac = 0.0f;
    } else if (t >= tMax) {
        *outInt = numKnots-2;
        *outFrac = 1.0f;
    } else {
        cds_spline_r32 tFloor = cds_spline__roundf(t);
        *outInt = (cds_spline_s32)tFloor;
        *outFrac = t - tFloor;
    }
}

static CDS_SPLINE_INLINE void
cds_spline3__compute_segment_matrix(cds_spline3 *outSpline, cds_spline_s32 segmentIndex) {
    if (segmentIndex >= 0 && segmentIndex < outSpline->numSegments) {
        cds_spline_mat34 *m = outSpline->segmentMatrices+segmentIndex;
        switch(outSpline->interpStyle) {
        case kCdsSplineInterpStyleHermite: {
            const cds_spline_vec3 *p0 = &outSpline->knots[segmentIndex+0].position;
            const cds_spline_vec3 *p1 = &outSpline->knots[segmentIndex+1].position;
            const cds_spline_vec3 *t0 = &outSpline->knots[segmentIndex+0].tangent;
            const cds_spline_vec3 *t1 = &outSpline->knots[segmentIndex+1].tangent;
            m->m00 = p0->x;
            m->m01 = p0->y;
            m->m02 = p0->z;
            m->m10 = t0->x;
            m->m11 = t0->y;
            m->m12 = t0->z;
            m->m20 = (-3)*p0->x +  (3)*p1->x + (-2)*t0->x + (-1)*t1->x;
            m->m21 = (-3)*p0->y +  (3)*p1->y + (-2)*t0->y + (-1)*t1->y;
            m->m22 = (-3)*p0->z +  (3)*p1->z + (-2)*t0->z + (-1)*t1->z;
            m->m30 =  (2)*p0->x + (-2)*p1->x +      t0->x +      t1->x;
            m->m31 =  (2)*p0->y + (-2)*p1->y +      t0->y +      t1->y;
            m->m32 =  (2)*p0->z + (-2)*p1->z +      t0->z +      t1->z;
            break;
        }
        case kCdsSplineInterpStyleBezier: {
            const cds_spline_vec3 *p0 = &outSpline->knots[segmentIndex+0].position;
            const cds_spline_vec3 *p3 = &outSpline->knots[segmentIndex+1].tangent;
            cds_spline_vec3 p1, p2;
            p1.x = p0->x + outSpline->knots[segmentIndex+0].tangent.x;
            p1.y = p0->y + outSpline->knots[segmentIndex+0].tangent.y;
            p1.z = p0->z + outSpline->knots[segmentIndex+0].tangent.z;
            p2.x = p3->x - outSpline->knots[segmentIndex+1].tangent.x;
            p2.y = p3->y - outSpline->knots[segmentIndex+1].tangent.y;
            p2.z = p3->z - outSpline->knots[segmentIndex+1].tangent.z;
            m->m00 = p0->x;
            m->m01 = p0->y;
            m->m02 = p0->z;
            m->m10 = (-3)*p0->x + ( 3)*p1.x;
            m->m11 = (-3)*p0->y + ( 3)*p1.y;
            m->m12 = (-3)*p0->z + ( 3)*p1.z;
            m->m20 = ( 3)*p0->x + (-6)*p1.x + ( 3)*p2.x;
            m->m21 = ( 3)*p0->y + (-6)*p1.y + ( 3)*p2.y;
            m->m22 = ( 3)*p0->z + (-6)*p1.z + ( 3)*p2.z;
            m->m30 = (-1)*p0->x + ( 3)*p1.x + (-3)*p2.x + p3->x;
            m->m31 = (-1)*p0->y + ( 3)*p1.y + (-3)*p2.y + p3->y;
            m->m32 = (-1)*p0->z + ( 3)*p1.z + (-3)*p2.z + p3->z;
            break;
        }
        case kCdsSplineInterpStyleCardinal: {
            const cds_spline_vec3 *p0 = &outSpline->knots[segmentIndex+0].position;
            const cds_spline_vec3 *p1 = &outSpline->knots[segmentIndex+1].position;
            const cds_spline_vec3 *p2 = &outSpline->knots[segmentIndex+2].tangent;
            const cds_spline_vec3 *p3 = &outSpline->knots[segmentIndex+3].tangent;
            const cds_spline_r32 tau = outSpline->tension;
            m->m00 = p1->x;
            m->m01 = p1->y;
            m->m02 = p1->z;
            m->m10 =  (-tau)*p0->x +   (tau)*p2->x;
            m->m11 =  (-tau)*p0->y +   (tau)*p2->y;
            m->m12 =  (-tau)*p0->z +   (tau)*p2->z;
            m->m20 = (2*tau)*p0->x + (tau-3)*p1->x + (3-2*tau)*p2->x + (-tau)*p3->x;
            m->m21 = (2*tau)*p0->y + (tau-3)*p1->y + (3-2*tau)*p2->y + (-tau)*p3->y;
            m->m22 = (2*tau)*p0->z + (tau-3)*p1->z + (3-2*tau)*p2->z + (-tau)*p3->z;
            m->m30 =  (-tau)*p0->x + (2-tau)*p1->x +   (tau-2)*p2->x +  (tau)*p3->x;
            m->m31 =  (-tau)*p0->y + (2-tau)*p1->y +   (tau-2)*p2->y +  (tau)*p3->y;
            m->m32 =  (-tau)*p0->z + (2-tau)*p1->z +   (tau-2)*p2->z +  (tau)*p3->z;
            break;
        }
        case kCdsSplineInterpStyleCentripetalCatmullRom: {
            const cds_spline_r32 alpha = outSpline->tension;
            const cds_spline_vec3 *p0 = &outSpline->knots[segmentIndex+0].position;
            const cds_spline_vec3 *p1 = &outSpline->knots[segmentIndex+1].position;
            const cds_spline_vec3 *p2 = &outSpline->knots[segmentIndex+2].position;
            const cds_spline_vec3 *p3 = &outSpline->knots[segmentIndex+3].position;
            const cds_spline_r32 t0 = 0.0f;
            const cds_spline_r32 t1 = t0 + (cds_spline_r32)pow(
                (p1->x - p0->x)*(p1->x - p0->x) +
                (p1->y - p0->y)*(p1->y - p0->y) +
                (p1->z - p0->z)*(p1->z - p0->z),
                alpha*0.5f );
            const cds_spline_r32 t2 = t1 + (cds_spline_r32)pow(
                (p2->x - p1->x)*(p2->x - p1->x) +
                (p2->y - p1->y)*(p2->y - p1->y) +
                (p2->z - p1->z)*(p2->z - p1->z),
                alpha*0.5f );
            const cds_spline_r32 t3 = t2 + (cds_spline_r32)pow(
                (p3->x - p2->x)*(p3->x - p2->x) +
                (p3->y - p2->y)*(p3->y - p2->y) +
                (p3->z - p2->z)*(p3->z - p2->z),
                alpha*0.5f );
            /* Simplification: reduce centripetal Catmull-Rom to Hermite, as described in
             * https://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
             * Basically, given P0,P1,P2,P3 and t0,t1,t2,t3, compute tangents at P1 and P2:
             *   tan1 = (P1 - P0) / (t1 - t0) - (P2 - P0) / (t2 - t0) + (P2 - P1) / (t2 - t1)
             *   tan2 = (P2 - P1) / (t2 - t1) - (P3 - P1) / (t3 - t1) + (P3 - P2) / (t3 - t2)
             * And plug into the standard Hermite basis matrix. If evaluating the segment from
             * P1 to P2, the tangents must be scaled by (t2-t1) to put them in the appropriate range.
             */
            cds_spline_vec3 tan1, tan2;
            tan1.x = (p1->x - p0->x)/(t1-t0) - (p2->x - p0->x)/(t2-t0) + (p2->x - p1->x)/(t2-t1);
            tan1.y = (p1->y - p0->y)/(t1-t0) - (p2->y - p0->y)/(t2-t0) + (p2->y - p1->y)/(t2-t1);
            tan1.z = (p1->z - p0->z)/(t1-t0) - (p2->z - p0->z)/(t2-t0) + (p2->z - p1->z)/(t2-t1);
            tan2.x = (p2->x - p1->x)/(t2-t1) - (p3->x - p1->x)/(t3-t1) + (p3->x - p2->x)/(t3-t2);
            tan2.y = (p2->y - p1->y)/(t2-t1) - (p3->y - p1->y)/(t3-t1) + (p3->y - p2->y)/(t3-t2);
            tan2.z = (p2->z - p1->z)/(t2-t1) - (p3->z - p1->z)/(t3-t1) + (p3->z - p2->z)/(t3-t2);
            tan1.x *= (t2-t1);
            tan1.y *= (t2-t1);
            tan1.z *= (t2-t1);
            tan2.x *= (t2-t1);
            tan2.y *= (t2-t1);
            tan2.z *= (t2-t1);
            m->m00 = p1->x;
            m->m01 = p1->y;
            m->m02 = p1->z;
            m->m10 = tan1.x;
            m->m11 = tan1.y;
            m->m12 = tan1.z;
            m->m20 = (-3)*p1->x + ( 3)*p2->x + (-2)*tan1.x + (-1)*tan2.x;
            m->m21 = (-3)*p1->y + ( 3)*p2->y + (-2)*tan1.y + (-1)*tan2.y;
            m->m22 = (-3)*p1->z + ( 3)*p2->z + (-2)*tan1.z + (-1)*tan2.z;
            m->m30 = ( 2)*p1->x + (-2)*p2->x +      tan1.x +      tan2.x;
            m->m31 = ( 2)*p1->y + (-2)*p2->y +      tan1.y +      tan2.y;
            m->m32 = ( 2)*p1->z + (-2)*p2->z +      tan1.z +      tan2.z;
            break;
        }
        }
    }
}

size_t
cds_spline3_buffer_size(cds_spline_interp_style interpStyle, cds_spline_s32 maxKnotCount) {
    (void)interpStyle;
    if (maxKnotCount <= 0)
        return 0;
    /* TODO: cardinal and catmull-rom splines need two fewer segment matrices */
    return maxKnotCount*sizeof(cds_spline_knot3) + (maxKnotCount-1)*sizeof(cds_spline_mat34);
}

cds_spline_error_t
cds_spline3_init(cds_spline3 *outSpline, cds_spline_interp_style interpStyle, cds_spline_s32 maxKnotCount,
    void *buffer, size_t bufferSize) {
    size_t minBufferSize = cds_spline3_buffer_size(interpStyle, maxKnotCount);
    if (bufferSize < minBufferSize)
        return kCdsSplineErrorInit_BufferSize;
    
    cds_spline_u8 *bufferNext = (cds_spline_u8*)buffer;
    outSpline->segmentMatrices = (cds_spline_mat34*)bufferNext;
    bufferNext += (maxKnotCount-1)*sizeof(cds_spline_mat34);
    outSpline->knots = (cds_spline_knot3*)bufferNext;
    bufferNext += maxKnotCount*sizeof(cds_spline_knot3);
    CDS_SPLINE_ASSERT( (intptr_t)bufferNext - (intptr_t)buffer == (intptr_t)minBufferSize );
    
    outSpline->interpStyle = interpStyle;
    outSpline->tension = 0.5;
    outSpline->numKnots = 0;
    outSpline->maxNumKnots = maxKnotCount;
    outSpline->numSegments = 0;

    return kCdsSplineErrorNone;
}

cds_spline_error_t
cds_spline3_set_tension(cds_spline3 *outSpline, cds_spline_r32 tension) {
    cds_spline_s32 iSeg;
    if (outSpline->tension != tension) {
        outSpline->tension = tension;
        for(iSeg=0; iSeg<outSpline->numSegments; iSeg += 1) {
            cds_spline3__compute_segment_matrix(outSpline, iSeg);
        }
    }
    return kCdsSplineErrorNone;
}

cds_spline_error_t
cds_spline3_insert_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex, cds_spline_knot3 knot) {
    cds_spline_s32 iKnot, iSeg;
    if (outSpline->numKnots == outSpline->maxNumKnots)
        return kCdsSplineErrorInsertKnot_MaxNumKnots;
    if (knotIndex < 0 || knotIndex > outSpline->numKnots)
        return kCdsSplineErrorInsertKnot_KnotIndex;
    for(iKnot=outSpline->numKnots-1; iKnot>=knotIndex; iKnot -= 1) {
        outSpline->knots[iKnot] = outSpline->knots[iKnot-1];
    }
    for(iSeg=outSpline->numKnots-2; iSeg>=knotIndex; iSeg -= 1) {/* TODO: adjust copy bounds; we're overwriting some of these anyway. */
        outSpline->segmentMatrices[iSeg] = outSpline->segmentMatrices[iSeg-1];
    }
    outSpline->numKnots += 1;
    switch(outSpline->interpStyle) {
    case kCdsSplineInterpStyleHermite:
    case kCdsSplineInterpStyleBezier:
        outSpline->numSegments = CDS_SPLINE_MAX(outSpline->numKnots-1, 0);
        break;
    case kCdsSplineInterpStyleCardinal:
    case kCdsSplineInterpStyleCentripetalCatmullRom:
        outSpline->numSegments = CDS_SPLINE_MAX(outSpline->numKnots-3, 0);
        break;
    }
    return cds_spline3_set_knot(outSpline, knotIndex, knot);
}

cds_spline_error_t
cds_spline3_set_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex, cds_spline_knot3 knot) {
    cds_spline_s32 iSeg, firstSegment=-1, lastSegment=-1;
    if (knotIndex < 0 || knotIndex >= outSpline->numKnots)
        return kCdsSplineErrorSetKnot_KnotIndex;
    outSpline->knots[knotIndex] = knot;
    switch(outSpline->interpStyle) {
    case kCdsSplineInterpStyleHermite:
    case kCdsSplineInterpStyleBezier:
        firstSegment = knotIndex-1;
        lastSegment = knotIndex;
        break;
    case kCdsSplineInterpStyleCardinal:
    case kCdsSplineInterpStyleCentripetalCatmullRom:
        firstSegment = knotIndex-3;
        lastSegment = knotIndex;
        break;
    }
    for(iSeg=firstSegment; iSeg<=lastSegment; iSeg += 1) {
        cds_spline3__compute_segment_matrix(outSpline, iSeg);
    }
    return kCdsSplineErrorNone;
}

cds_spline_error_t
cds_spline3_remove_knot(cds_spline3 *outSpline, cds_spline_s32 knotIndex) {
    cds_spline_s32 iKnot, iSeg, firstSegment=-1, lastSegment=-1;
    if (knotIndex < 0 || knotIndex >= outSpline->numKnots)
        return kCdsSplineErrorRemoveKnot_KnotIndex;
    for(iKnot=knotIndex; iKnot<outSpline->numKnots-1; iKnot += 1) {
        outSpline->knots[iKnot] = outSpline->knots[iKnot+1];
    }
    for(iSeg=knotIndex; iSeg<outSpline->numKnots-2; iSeg += 1) { /* TODO: adjust copy bounds; we're overwriting mat[ki+1] anyway */
        outSpline->segmentMatrices[iKnot] = outSpline->segmentMatrices[iKnot+1];
    }
    outSpline->numKnots -= 1;
    switch(outSpline->interpStyle) {
    case kCdsSplineInterpStyleHermite:
    case kCdsSplineInterpStyleBezier:
        outSpline->numSegments = CDS_SPLINE_MAX(outSpline->numKnots-1, 0);
        firstSegment = knotIndex-1;
        lastSegment = knotIndex;
        break;
    case kCdsSplineInterpStyleCardinal:
    case kCdsSplineInterpStyleCentripetalCatmullRom:
        outSpline->numSegments = CDS_SPLINE_MAX(outSpline->numKnots-3, 0);
        firstSegment = knotIndex-3;
        lastSegment = knotIndex;
        break;
    }
    for(iSeg=firstSegment; iSeg<=lastSegment; iSeg += 1) {
        cds_spline3__compute_segment_matrix(outSpline, iSeg);
    }
    return kCdsSplineErrorNone;
}

cds_spline_vec3
cds_spline3_eval(const cds_spline3 *spline, cds_spline_r32 t) {
    cds_spline_s32 segment;
    cds_spline_r32 u;
    cds_spline__get_int_and_frac(spline->numKnots, t, &segment, &u);
    CDS_SPLINE_ASSERT(segment >= 0 && segment < spline->numSegments);
    const cds_spline_mat34 *m = spline->segmentMatrices + segment;
    cds_spline_vec3 pos;
    pos.x = ((m->m30*u + m->m20)*u + m->m10)*u + m->m00;
    pos.y = ((m->m31*u + m->m21)*u + m->m11)*u + m->m01;
    pos.z = ((m->m32*u + m->m22)*u + m->m12)*u + m->m02;
    return pos;
}

cds_spline_vec3
cds_spline3_evald(const cds_spline3 *spline, cds_spline_r32 t) {
    cds_spline_s32 segment;
    cds_spline_r32 u;
    cds_spline__get_int_and_frac(spline->numKnots, t, &segment, &u);
    CDS_SPLINE_ASSERT(segment >= 0 && segment < spline->numSegments);
    const cds_spline_mat34 *m = spline->segmentMatrices + segment;
    cds_spline_vec3 dpos;
    dpos.x = (3*m->m30*u + 2*m->m20)*u + m->m10;
    dpos.y = (3*m->m31*u + 2*m->m21)*u + m->m11;
    dpos.z = (3*m->m32*u + 2*m->m22)*u + m->m12;
    return dpos;
}

cds_spline_vec3
cds_spline3_evaldd(const cds_spline3 *spline, cds_spline_r32 t) {
    cds_spline_s32 segment;
    cds_spline_r32 u;
    cds_spline__get_int_and_frac(spline->numKnots, t, &segment, &u);
    CDS_SPLINE_ASSERT(segment >= 0 && segment < spline->numSegments);
    const cds_spline_mat34 *m = spline->segmentMatrices + segment;
    cds_spline_vec3 ddpos;
    ddpos.x = 6*m->m30*u + 2*m->m20;
    ddpos.y = 6*m->m31*u + 2*m->m21;
    ddpos.z = 6*m->m32*u + 2*m->m22;
    return ddpos;
}

   
#endif /*------------ end implementation ------------------------*/



#if defined(CDS_SPLINE_TEST) /*---------- self-test section -----*/

#include <assert.h>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    cds_spline_s32 iKnot, iSamp;
    cds_spline3 spline;
    cds_spline_knot3 knots[] = {
        { {{-1, 0, 0}}, {{ 0, 1, 0}} },
        { {{ 0, 1, 0}}, {{ 1, 0, 0}} },
        { {{ 1, 0, 0}}, {{ 0,-1, 0}} },
        { {{ 0,-1, 0}}, {{-1, 0, 0}} },
    };
    cds_spline_s32 knotCount = sizeof(knots) / sizeof(knots[0]);
    cds_spline_s32 maxKnotCount = 16;
    cds_spline_interp_style interpStyle = kCdsSplineInterpStyleHermite;
    cds_spline_s32 sampleCount = 64;
    CDS_SPLINE_ASSERT(knotCount >= 0);
    size_t bufferSize = cds_spline3_buffer_size(interpStyle, maxKnotCount);
    void *buffer = malloc(bufferSize);
    cds_spline_error_t splineError = cds_spline3_init(&spline, interpStyle, maxKnotCount, buffer, bufferSize);
    CDS_SPLINE_ASSERT(splineError == kCdsSplineErrorNone);
    for(iKnot=0; iKnot<knotCount; ++iKnot) {
        printf("spline.numKnots: %d\n", spline.numKnots);
        splineError = cds_spline3_insert_knot(&spline, iKnot, knots[iKnot]);
        printf("0x%08X\n", splineError);
        CDS_SPLINE_ASSERT(splineError == kCdsSplineErrorNone);
    }
    for(iSamp=0; iSamp<=sampleCount; ++iSamp) {
        float u = (float)iSamp * (float)spline.numSegments / (float)sampleCount;
        cds_spline_vec3 pos = cds_spline3_eval(&spline, u);
        printf("u=%.3f pos=[%11.8f %11.8f]\n", u, pos.x, pos.y);
    }

    free(buffer);
    return 0;
}

#endif /*------------------- end self-test section ------------*/

