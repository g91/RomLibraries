// ========================================================================
// $File: //jeffr/granny/rt/granny_bspline_buffers.cpp $
// $DateTime: 2006/12/01 16:48:27 $
// $Change: 13835 $
// $Revision: #20 $
//
// (C) Copyright 1999-2006 by RAD Game Tools, All Rights Reserved.
// ========================================================================

#if !defined(GRANNY_BSPLINE_BUFFERS_H)
#include "granny_bspline_buffers.h"
#endif

#if !defined(GRANNY_ASSERT_H)
#include "granny_assert.h"
#endif

#if !defined(GRANNY_CURVE_H)
#include "granny_curve.h"
#endif

#if !defined(GRANNY_MEMORY_H)
#include "granny_memory.h"
#endif

#if !defined(GRANNY_STATISTICS_H)
#include "granny_statistics.h"
#endif

#if !defined(GRANNY_MATH_H)
#include "granny_math.h"
#endif

#if !defined(GRANNY_BSPLINE_H)
#include "granny_bspline.h"
#endif

#define SubsystemCode BSplineLogMessage

#if !defined(GRANNY_PARAMETER_CHECKING_H)
#include "granny_parameter_checking.h"
#endif

#if !defined(GRANNY_CPP_SETTINGS_H)
// This should always be the last header included
#include "granny_cpp_settings.h"
#endif

USING_GRANNY_NAMESPACE;

/*
  Note: the FindKnot routines return the knot that's _ahead of_ where you
  are at time t. However, it clamps to KnotCount-1, so it will never reference
  off the end of the array.
 */


int32x GRANNY
FindKnot(int32x KnotCount, real32 const *Knots, real32 t)
{
#if 1
#if DEBUG
    real32 const *OriginalKnots = Knots;
#endif
    int32x KnotWindow = KnotCount;
    int32x KnotIndex = 0;

    while(KnotWindow > 1)
    {
        if(KnotWindow & 1)
        {
            // It's odd
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t >= Knots[KnotWindow])
            {
                KnotIndex += KnotWindow;
                Knots += KnotWindow;
            }

            ++KnotWindow;
        }
        else
        {
            // It's even
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t >= Knots[KnotWindow])
            {
                if((KnotIndex+KnotWindow+1 >= KnotCount) || (t < Knots[KnotWindow + 1]))
                {
                    KnotIndex += KnotWindow;
                    Knots += KnotWindow;
                    break;
                }
                else
                {
                    KnotIndex += KnotWindow + 1;
                    Knots += KnotWindow + 1;
                    --KnotWindow;
                }
            }
        }
    }

    // Slight adjustment for duplicate knots.
    while ( ( KnotIndex < KnotCount ) && ( Knots[0] <= t ) )
    {
        ++KnotIndex;
        ++Knots;
    }

#if DEBUG
    Assert((KnotIndex >= 0) && (KnotIndex <= KnotCount));
    if ( KnotIndex < KnotCount )
    {
        Assert ( OriginalKnots[KnotIndex] > t );
    }
    if ( KnotIndex > 0 )
    {
        Assert ( OriginalKnots[KnotIndex-1] <= t );
    }
#endif

    if ( ( KnotIndex == KnotCount ) && ( KnotIndex > 0 ) )
    {
        // Stop it falling off the end of the knot array.
        // This is slightly artificial, but calling code gets confused otherwise.
        KnotIndex--;
    }

    return(KnotIndex);



#else
    uint32 i = 1 << (Log2KnotCount - 1);
    switch(Log2KnotCount - 1)
    {
#define Level(N)                                \
        case N:                                 \
        if((i >= KnotCount) || (Knots[i] < t))  \
        {                                       \
            i -= 1 << (N - 1);                  \
        }                                       \
        else                                    \
        {                                       \
            i += 1 << (N - 1);                  \
        }

#define Level0
        case 0:

        break;

#if (MaximumBSplineKnotPower >= 31)
        Level(31);
#endif

#if (MaximumBSplineKnotPower >= 30)
        Level(30);
#endif

#if (MaximumBSplineKnotPower >= 29)
        Level(29);
#endif

#if (MaximumBSplineKnotPower >= 28)
        Level(28);
#endif

#if (MaximumBSplineKnotPower >= 27)
        Level(27);
#endif

#if (MaximumBSplineKnotPower >= 26)
        Level(26);
#endif

#if (MaximumBSplineKnotPower >= 25)
        Level(25);
#endif

#if (MaximumBSplineKnotPower >= 24)
        Level(24);
#endif

#if (MaximumBSplineKnotPower >= 23)
        Level(23);
#endif

#if (MaximumBSplineKnotPower >= 22)
        Level(22);
#endif

#if (MaximumBSplineKnotPower >= 21)
        Level(21);
#endif

#if (MaximumBSplineKnotPower >= 20)
        Level(20);
#endif

#if (MaximumBSplineKnotPower >= 19)
        Level(19);
#endif

#if (MaximumBSplineKnotPower >= 18)
        Level(18);
#endif

#if (MaximumBSplineKnotPower >= 17)
        Level(17);
#endif

#if (MaximumBSplineKnotPower >= 16)
        Level(16);
#endif

#if (MaximumBSplineKnotPower >= 15)
        Level(15);
#endif

#if (MaximumBSplineKnotPower >= 14)
        Level(14);
#endif

#if (MaximumBSplineKnotPower >= 13)
        Level(13);
#endif

#if (MaximumBSplineKnotPower >= 12)
        Level(12);
#endif

#if (MaximumBSplineKnotPower >= 11)
        Level(11);
#endif

#if (MaximumBSplineKnotPower >= 10)
        Level(10);
#endif

#if (MaximumBSplineKnotPower >= 9)
        Level(9);
#endif

#if (MaximumBSplineKnotPower >= 8)
        Level(8);
#endif

#if (MaximumBSplineKnotPower >= 7)
        Level(7);
#endif

#if (MaximumBSplineKnotPower >= 6)
        Level(6);
#endif

#if (MaximumBSplineKnotPower >= 5)
        Level(5);
#endif

#if (MaximumBSplineKnotPower >= 4)
        Level(4);
#endif

#if (MaximumBSplineKnotPower >= 3)
        Level(3);
#endif

#if (MaximumBSplineKnotPower >= 2)
        Level(2);
#endif

#if (MaximumBSplineKnotPower >= 1)
        Level(1);
#endif

        Level0;

        default:
        {
            InvalidCodePath();
        } break;
    }
#endif
}


#if 0   // Ugh.
int32x GRANNY
FindCloseKnot(int32x KnotCount, real32 const *Knots,
              real32 t, int32x StartingIndex)
{
    if(KnotCount > 0)
    {
        int32x KnotIndex = StartingIndex;
        if(KnotIndex == 0)
        {
            ++KnotIndex;
        }

        real32 PrevT = Knots[KnotIndex - 1];
        while(KnotIndex < KnotCount)
        {
            real32 ThisT = Knots[KnotIndex];
            if((PrevT <= t) && (ThisT > t))
            {
#if DEBUG
                Assert((KnotIndex >= 0) && (KnotIndex <= KnotCount));
                if ( KnotIndex < KnotCount )
                {
                    Assert ( Knots[KnotIndex] > t );
                }
                if ( KnotIndex > 0 )
                {
                    Assert ( Knots[KnotIndex-1] <= t );
                }
#endif
                return(KnotIndex);
            }

            PrevT = ThisT;
            ++KnotIndex;
        }

        KnotIndex = 1;
        while(KnotIndex < StartingIndex)
        {
            if(Knots[KnotIndex] >= t)
            {
#if DEBUG
                Assert((KnotIndex >= 0) && (KnotIndex <= KnotCount));
                if ( KnotIndex < KnotCount )
                {
                    Assert ( Knots[KnotIndex] > t );
                }
                if ( KnotIndex > 0 )
                {
                    Assert ( Knots[KnotIndex-1] <= t );
                }
#endif
                return(KnotIndex);
            }

            ++KnotIndex;
        }
    }

    return(0);
}
#else
int32x GRANNY
FindCloseKnot(int32x KnotCount, real32 const *Knots,
              real32 t, int32x StartingIndex)
{
    return FindKnot ( KnotCount, Knots, t );
}
#endif




// Identical to FindKnot, except with the function declaration changed!
int32x GRANNY
FindKnotUint16(int32x KnotCount, uint16 const *Knots, uint16 t)
{
#if DEBUG
    uint16 const *OriginalKnots = Knots;
#endif
    int32x KnotWindow = KnotCount;
    int32x KnotIndex = 0;

    while(KnotWindow > 1)
    {
        if(KnotWindow & 1)
        {
            // It's odd
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t >= Knots[KnotWindow])
            {
                KnotIndex += KnotWindow;
                Knots += KnotWindow;
            }

            ++KnotWindow;
        }
        else
        {
            // It's even
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t >= Knots[KnotWindow])
            {
                if((KnotIndex+KnotWindow+1 >= KnotCount) || (t < Knots[KnotWindow + 1]))
                {
                    KnotIndex += KnotWindow;
                    Knots += KnotWindow;
                    break;
                }
                else
                {
                    KnotIndex += KnotWindow + 1;
                    Knots += KnotWindow + 1;
                    --KnotWindow;
                }
            }
        }
    }

    // Slight adjustment for duplicate knots.
    while ( ( KnotIndex < KnotCount ) && ( Knots[0] <= t ) )
    {
        ++KnotIndex;
        ++Knots;
    }

#if DEBUG
    Assert((KnotIndex >= 0) && (KnotIndex <= KnotCount));
    if ( KnotIndex < KnotCount )
    {
        Assert ( OriginalKnots[KnotIndex] > t );
    }
    if ( KnotIndex > 0 )
    {
        Assert ( OriginalKnots[KnotIndex-1] <= t );
    }
#endif

    if ( ( KnotIndex == KnotCount ) && ( KnotIndex > 0 ) )
    {
        // Stop it falling off the end of the knot array.
        // This is slightly artificial, but calling code gets confused otherwise.
        KnotIndex--;
    }

    return(KnotIndex);
}


// Identical to FindKnot, except with the function declaration changed!
int32x GRANNY
FindKnotUint8(int32x KnotCount, uint8 const *Knots, uint8 t)
{
#if DEBUG
    uint8 const *OriginalKnots = Knots;
#endif
    int32x KnotWindow = KnotCount;
    int32x KnotIndex = 0;

    while(KnotWindow > 1)
    {
        if(KnotWindow & 1)
        {
            // It's odd
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t > Knots[KnotWindow])
            {
                KnotIndex += KnotWindow;
                Knots += KnotWindow;
            }

            ++KnotWindow;
        }
        else
        {
            // It's even
            KnotWindow = (KnotWindow / 2);
            Assert ( KnotIndex+KnotWindow < KnotCount );
            if(t > Knots[KnotWindow])
            {
                if((KnotIndex+KnotWindow+1 >= KnotCount) || (t <= Knots[KnotWindow + 1]))
                {
                    KnotIndex += KnotWindow;
                    Knots += KnotWindow;
                    break;
                }
                else
                {
                    KnotIndex += KnotWindow + 1;
                    Knots += KnotWindow + 1;
                    --KnotWindow;
                }
            }
        }
    }

    // Slight adjustment for duplicate knots.
    while ( ( KnotIndex < KnotCount ) && ( Knots[0] <= t ) )
    {
        ++KnotIndex;
        ++Knots;
    }

#if DEBUG
    Assert((KnotIndex >= 0) && (KnotIndex <= KnotCount));
    if ( KnotIndex < KnotCount )
    {
        Assert ( OriginalKnots[KnotIndex] > t );
    }
    if ( KnotIndex > 0 )
    {
        Assert ( OriginalKnots[KnotIndex-1] <= t );
    }
#endif

    if ( ( KnotIndex == KnotCount ) && ( KnotIndex > 0 ) )
    {
        // Stop it falling off the end of the knot array.
        // This is slightly artificial, but calling code gets confused otherwise.
        KnotIndex--;
    }

    return(KnotIndex);
}



int32x GRANNY
FindCloseKnotUint16(int32x KnotCount, uint16 const *Knots,
                    uint16 t, int32x StartingIndex)
{
    return FindKnotUint16 ( KnotCount, Knots, t );
}

int32x GRANNY
FindCloseKnotUint8(int32x KnotCount, uint8 const *Knots,
                    uint8 t, int32x StartingIndex)
{
    return FindKnotUint8 ( KnotCount, Knots, t );
}



#if 0
#define WriteKnot(Curve, KnotIndex, Offset) \
*tiWrite++ = (Curve).Knots[KnotIndex] + Offset; \
Copy32(Dimension, &(Curve).Controls[(KnotIndex) * Dimension], piWrite); \
piWrite += Dimension;
#endif

bool GRANNY
ConstructBSplineBuffers(int32x Dimension,
                        curve2 const *PrevCurve,
                        curve2 const &Curve,
                        curve2 const *NextCurve,
                        real32 PrevCurveDuration,
                        real32 CurveDuration,
                        real32 NextCurveDuration,
                        int32x KnotIndex,
                        real32 * NOALIAS ti, real32 * NOALIAS pi,
                        real32 *&tiPtr, real32 *&piPtr,
                        real32 const *IdentityVector)
{
    // ---------------------------------------------
    // REMOVEDCODE: Old-style raw curves (3/28/2006)
    // ---------------------------------------------

    // However, PrevCurve and NextCurve _can_ be identity, constant, etc.
    Assert ( !CurveIsIdentity ( Curve ) );
    Assert ( !CurveIsConstantOrIdentity ( Curve ) );

    bool OverOrUnderFlow = false;
    int32x Degree = CurveGetDegree ( Curve );
    // I don't really want to check every single time because of the perf. hit. Asserts will have to do.
    Assert ( ( PrevCurve == NULL ) || ( Degree == CurveGetDegree ( *PrevCurve ) ) );
    Assert ( ( NextCurve == NULL ) || ( Degree == CurveGetDegree ( *NextCurve ) ) );

    int32x KnotCount = CurveGetKnotCount ( Curve );
    Assert ( KnotIndex >= 0 );
    Assert ( KnotIndex <= KnotCount );
    if ( ( KnotCount > 0 ) && ( NextCurve != NULL ) )
    {
        // When NextCurve is present, it means we are forward-wrapping.
        // Thus, we need to treat our final knot as simply a timing
        // value, because in reality it's really the first knot of
        // the next curve, and we want to treat it as an overflow
        // if we would have to use it (since we should grab it from
        // NextCurve, not Curve, and thus we need to use the overflow
        // copying).
        --KnotCount;
    }


    // To evaluate a curve of degree n, centered on knot I,
    // we need knot times I-n to I+n-1, and control values I-n to I.
    // So for a curve of degree 3, where KnotIndex==8, we need:
    // Knot times 5 to 10
    // Control values 5 to 8
    // However, it's a lot easier to just get both knots and controls for 5 to 10
    // TODO: Make the slight speed optimisation to only get controls 5 to 8

    int32x KnotWindow = 2*Degree;
    int32x BaseKnotIndex = KnotIndex - Degree;
    bool const Underflow = (BaseKnotIndex < 0);
    bool const Overflow = ((BaseKnotIndex + KnotWindow) > KnotCount);
    OverOrUnderFlow = (Underflow || Overflow);

    if(OverOrUnderFlow)
    {
        // This is the slow path, which only happens on boundary
        // conditions. We need to sample parts of the adjacent curves,
        // or replicate the first/last knots of the subject curve
        // if there are no adjacent curves.

        tiPtr = ti + Degree;
        piPtr = pi + (Degree * Dimension);


        // Insert the correct chunk of Curve into the buffer.
        int32x FirstCurveIndex = BaseKnotIndex;
        int32x NumCurveKnots = Degree * 2;
        int32x FirstLocalIndex = 0;
        if ( Underflow )
        {
            // FirstCurveIndex is -ve, so we need to shift the place we write to up a bit.
            int32x UnderflowBy = -FirstCurveIndex;
            Assert ( UnderflowBy > 0 );
            NumCurveKnots -= UnderflowBy;
            FirstLocalIndex += UnderflowBy;
            FirstCurveIndex = 0;
        }

        if ( Overflow )
        {
            int32x OverflowBy = (FirstCurveIndex + NumCurveKnots) - KnotCount;
            Assert ( OverflowBy > 0 );
            NumCurveKnots -= OverflowBy;
        }

        Assert ( NumCurveKnots >= 0 );
        Assert ( FirstCurveIndex >= 0 );
        Assert ( FirstCurveIndex + NumCurveKnots <= KnotCount );
        Assert ( FirstLocalIndex >= 0 );
        Assert ( FirstLocalIndex + NumCurveKnots <= Degree * 2 );
        if ( NumCurveKnots > 0 )
        {
            CurveExtractKnotValues ( Curve, FirstCurveIndex, NumCurveKnots, ti + FirstLocalIndex, pi + FirstLocalIndex * Dimension, IdentityVector );
        }
        else
        {
            Assert ( !"Surprising, but not fatal" );
        }


        // Fill in the underflow.
        if ( Underflow )
        {
            if ( PrevCurve == NULL )
            {
                // Simply replicate the first knot of the curve.
                real32 *tiDst = ti;
                real32 *piDst = pi;
                real32 *tiSrc = ti + FirstLocalIndex;
                real32 *piSrc = pi + FirstLocalIndex * Dimension;
                {for ( int32x CurIndex = 0; CurIndex < FirstLocalIndex; CurIndex++ )
                {
                    *tiDst++ = *tiSrc;
                    Copy32 ( Dimension, piSrc, piDst );
                    piDst += Dimension;
                }}
            }
            else if ( CurveIsIdentity ( *PrevCurve ) )
            {
                // Copy in the caller's idea of what "identity" is.
                // TODO: What times should the knots be given?
                real32 *tiDst = ti;
                real32 *piDst = pi;
                real32 CurrentTime = ti[FirstLocalIndex] - (real32)FirstLocalIndex * PrevCurveDuration;
                {for ( int32x CurIndex = 0; CurIndex < FirstLocalIndex; CurIndex++ )
                {
                    *tiDst++ = CurrentTime;
                    Copy32 ( Dimension, IdentityVector, piDst );
                    piDst += Dimension;
                    CurrentTime += PrevCurveDuration;
                }}
            }
            else
            {
                // There's an actual curve before it.
                // This could be slightly faster and get multiple knots at a time, but this way it's extremely robust.
                int32x KnotCountPrev = CurveGetKnotCount ( *PrevCurve );
                // Note that the last knot value is not used when wrapping -
                // it is simply a time reference that says how long the curve is.
                if ( KnotCountPrev > 1 )
                {
                    KnotCountPrev--;
                }
                Assert ( KnotCountPrev > 0 );
                int32x CurrentKnot = KnotCountPrev - 1;
                real32 *tiDst = ti + ( FirstLocalIndex - 1 );
                real32 *piDst = pi + ( FirstLocalIndex - 1 ) * Dimension;
                real32 TimeOffset = -PrevCurveDuration;
                {for ( int32x CurLocalIndex = FirstLocalIndex - 1; CurLocalIndex >= 0; CurLocalIndex-- )
                {
                    CurveExtractKnotValues ( *PrevCurve, CurrentKnot, 1, tiDst, piDst, IdentityVector );
                    *tiDst += TimeOffset;
                    Assert ( *tiDst <= 0.0f );
                    tiDst--;
                    piDst -= Dimension;
                    CurrentKnot--;
                    if ( CurrentKnot < 0 )
                    {
                        // Wrap yet again!
                        Assert ( !"Not sure if this is even possible, so never tested" );
                        CurrentKnot = KnotCountPrev - 1;
                        TimeOffset -= PrevCurveDuration;
                    }
                }}
            }
        }

        // Fill in the overflow.
        if ( Overflow )
        {
            int32x LastLocalIndex = FirstLocalIndex + NumCurveKnots - 1;
            if ( NextCurve == NULL )
            {
                // Simply replicate the last knot of the curve.
                real32 *tiSrc = ti + LastLocalIndex;
                real32 *piSrc = pi + LastLocalIndex * Dimension;
                real32 *tiDst = tiSrc + 1;
                real32 *piDst = piSrc + Dimension;
                {for ( int32x CurIndex = LastLocalIndex + 1; CurIndex < KnotWindow; CurIndex++ )
                {
                    *tiDst++ = *tiSrc;
                    Copy32 ( Dimension, piSrc, piDst );
                    piDst += Dimension;
                }}
            }
            else if ( CurveIsIdentity ( *NextCurve ) )
            {
                // Copy in the caller's idea of what "identity" is.
                // TODO: What times should the knots be given?
                real32 *tiDst = ti + LastLocalIndex + 1;
                real32 *piDst = pi + ( LastLocalIndex + 1 ) * Dimension;
                real32 CurrentTime = ti[LastLocalIndex];
                {for ( int32x CurIndex = 0; CurIndex < FirstLocalIndex; CurIndex++ )
                {
                    CurrentTime += NextCurveDuration;
                    *tiDst++ = CurrentTime;
                    Copy32 ( Dimension, IdentityVector, piDst );
                    piDst += Dimension;
                }}
            }
            else
            {
                // There's an actual curve after it.
                // This could be slightly faster and get multiple knots at a time, but this way it's extremely robust.
                int32x KnotCountNext = CurveGetKnotCount ( *NextCurve );
                // Note that the last knot value is not used when wrapping -
                // it is simply a time reference that says how long the curve is.
                if ( KnotCountNext > 1 )
                {
                    KnotCountNext--;
                }
                Assert ( KnotCountNext > 0 );
                int32x CurrentKnot = 0;
                real32 *tiDst = ti + ( LastLocalIndex + 1 );
                real32 *piDst = pi + ( LastLocalIndex + 1 ) * Dimension;
                real32 TimeOffset = CurveDuration;
                {for ( int32x CurIndex = LastLocalIndex + 1; CurIndex < KnotWindow; CurIndex++ )
                {
                    CurveExtractKnotValues ( *NextCurve, CurrentKnot, 1, tiDst, piDst, IdentityVector );
                    *tiDst += TimeOffset;
                    tiDst++;
                    piDst += Dimension;
                    CurrentKnot++;
                    if ( CurrentKnot >= KnotCountNext )
                    {
                        // Wrap yet again!
                        CurrentKnot = 0;
                        TimeOffset += NextCurveDuration;
                    }
                }}
            }
        }
    }
    else
    {
        // Fast path - no thinking, just extract the knots and controls we need.
        curve_data_da_k32f_c32f *CurveDataDaK32fC32f = CurveGetContentsOfDaK32fC32f ( Curve );
        if ( CurveDataDaK32fC32f != NULL )
        {
            // Even faster - it's an uncompressed curve, so we can just point into the middle of it.
            tiPtr = &(CurveDataDaK32fC32f->Knots[KnotIndex]);
            piPtr = &(CurveDataDaK32fC32f->Controls[Dimension * KnotIndex]);
        }
        else
        {
            // Some decompression required.
            tiPtr = ti + Degree;
            piPtr = pi + (Degree * Dimension);
            CurveExtractKnotValues ( Curve, BaseKnotIndex, Degree * 2, ti, pi, IdentityVector );
        }
    }

#if DEBUG
    // Just check the time values are increasing (or constant).
    {for ( int32x KnotNum = -Degree + 1; KnotNum < Degree; KnotNum++ )
    {
        Assert ( tiPtr[KnotNum-1] <= tiPtr[KnotNum] );
    }}
#endif

    return(OverOrUnderFlow);
}

void GRANNY
EvaluateCurveAtT(int32x Dimension, bool Normalize,
                 bool BackwardsLoop,
                 curve2 const &Curve,
                 bool ForwardsLoop,
                 real32 CurveDuration,
                 real32 t, real32 *Result,
                 real32 const *IdentityVector)
{
    EvaluateCurveAtKnotIndex(Dimension, Normalize, BackwardsLoop, Curve, ForwardsLoop,
                             CurveDuration, CurveFindKnot(Curve, t),
                             t, Result, IdentityVector);
}

void GRANNY
EvaluateCurveAtKnotIndex(int32x Dimension, bool Normalize,
                         bool BackwardsLoop,
                         curve2 const &Curve,
                         bool ForwardsLoop,
                         real32 CurveDuration,
                         int32x KnotIndex, real32 t,
                         real32 *Result,
                         real32 const *IdentityVector)
{
    if ( CurveIsIdentity ( Curve ) )
    {
        Copy32 ( Dimension, IdentityVector, Result );
    }
    else if ( CurveIsConstantOrIdentity ( Curve ) )
    {
        CurveExtractKnotValue ( Curve, 0, Result, IdentityVector );
    }
    else
    {
        real32 tiBufferSpace[8];
        real32 piBufferSpace[4 * MaximumBSplineDimension];

        int32x Degree = CurveGetDegree ( Curve );
        Assert ( ( Degree + 1 ) < ArrayLength ( tiBufferSpace ) );
        Assert ( ( Degree + 1 ) * Dimension < ArrayLength ( piBufferSpace ) );

        real32 *tiBuffer;
        real32 *piBuffer;

        // If the time was exactly at the end of the curve, curve compression and
        // numerical precision can cause the local clock to be very slightly greater,
        // so it runs off the end. Which is bad.
        int32x KnotCount = CurveGetKnotCount ( Curve );
        if ( KnotIndex == KnotCount )
        {
            KnotIndex--;
        }
        Assert ( KnotIndex < KnotCount );


        if(ConstructBSplineBuffers(Dimension,
                                   BackwardsLoop ? &Curve : NULL,
                                   Curve,
                                   ForwardsLoop ? &Curve : NULL,
                                   CurveDuration, CurveDuration, CurveDuration,
                                   KnotIndex,
                                   tiBufferSpace, piBufferSpace,
                                   tiBuffer, piBuffer,
                                   IdentityVector))
        {
            if(Normalize && (Dimension == 4))
            {
                EnsureQuaternionContinuity(Degree + 1,
                                           piBufferSpace);
            }
        }

        SampleBSpline(Degree, Dimension, Normalize,
                      tiBuffer, piBuffer, t, Result);
    }
}

// ----------------------------------------------
// REMOVEDCODE: CurveFindNextCrossing (3/28/2006)
//  Never actually finished...
// ----------------------------------------------
