/* jshint browser: false
   , strict: true
   , eqeqeq: true
   , forin: true
   , freeze: true
   , latedef: true
   , noarg: true
   , undef: true
   , plusplus: true
*/

/* global console
*/

var SPLINE = function() {
    "use strict";

    function _clamp(x, xMin, xMax) {
        return (x<xMin) ? xMin : ( (x>xMax) ? xMax : x );
    }
    
    function _cross2(a, b) {
        return a.x*b.y - a.y*b.x;
    }
    function _cross3(a, b) {
        return {
            x: a.y*b.z - a.z*b.y,
            y: a.z*b.x - a.x*b.z,
            z: a.x*b.y - a.y*b.x,
        };
    }

    function _evalPos(spline, u) {
        var uClamp = _clamp(u, 0, spline.segments.length);
        var uInt = uClamp | 0, uFrac = uClamp - uInt;
        if (uClamp === spline.segments.length) {
            uInt  = uInt-1;
            uFrac = 1.0;
        }
        var m = spline.segments[uInt].geomMatrix;
        return {
            x: ((m.m30*uFrac + m.m20)*uFrac + m.m10)*uFrac + m.m00,
            y: ((m.m31*uFrac + m.m21)*uFrac + m.m11)*uFrac + m.m01,
            z: ((m.m32*uFrac + m.m22)*uFrac + m.m12)*uFrac + m.m02,
            w: ((m.m33*uFrac + m.m23)*uFrac + m.m13)*uFrac + m.m03
        };
    }
    
    function _evalDPos(spline, u) {
        var uClamp = _clamp(u, 0, spline.segments.length);
        var uInt = uClamp | 0, uFrac = uClamp - uInt;
        if (uClamp === spline.segments.length) {
            uInt  = uInt-1;
            uFrac = 1.0;
        }
        var m = spline.segments[uInt].geomMatrix;
        return {
            x: (3*m.m30*uFrac + 2*m.m20)*uFrac + m.m10,
            y: (3*m.m31*uFrac + 2*m.m21)*uFrac + m.m11,
            z: (3*m.m32*uFrac + 2*m.m22)*uFrac + m.m12,
            w: (3*m.m33*uFrac + 2*m.m23)*uFrac + m.m13,
        };
    }

    function _evalDDPos(spline, u) {
        var uClamp = _clamp(u, 0, spline.segments.length);
        var uInt = uClamp | 0, uFrac = uClamp - uInt;
        if (uClamp === spline.segments.length) {
            uInt  = uInt-1;
            uFrac = 1.0;
        }
        var m = spline.segments[uInt].geomMatrix;
        return {
            x: 6*m.m30*uFrac + 2*m.m20,
            y: 6*m.m31*uFrac + 2*m.m21,
            z: 6*m.m32*uFrac + 2*m.m22,
            w: 6*m.m33*uFrac + 2*m.m23,
        };
    }

    function _evalCurvature2(tan, dtan) {
        return Math.abs( _cross2(tan, dtan) ) / Math.pow(tan.x*tan.x + tan.y*tan.y, 1.5) ;
    }
    function _evalCurvature3(tan, dtan) {
        var c = _cross3(tan, dtan);
        return Math.sqrt(c.x*c.x + c.y*c.y * c.z*c.z) / Math.pow(tan.x*tan.x + tan.y*tan.y + tan.z*tan.z, 1.5);
    }

    function makeSplineCardinal(controlPoints, tau) {
        if (typeof(tau) === 'undefined') tau = 0.5;
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPoints.length-3; iSeg+=1) {
            var p0 = controlPoints[iSeg+0];
            var p1 = controlPoints[iSeg+1];
            var p2 = controlPoints[iSeg+2];
            var p3 = controlPoints[iSeg+3];
            p0 = {x: p0.x || 0, y: p0.y || 0, z: p0.z || 0, w: p0.w || 0};
            p1 = {x: p1.x || 0, y: p1.y || 0, z: p1.z || 0, w: p1.w || 0};
            p2 = {x: p2.x || 0, y: p2.y || 0, z: p2.z || 0, w: p2.w || 0};
            p3 = {x: p3.x || 0, y: p3.y || 0, z: p3.z || 0, w: p3.w || 0};
            var m = {
                m00: p1.x,
                m01: p1.y,
                m02: p1.z,
                m03: p1.w,
                m10: (-tau)*p0.x + (tau)*p2.x,
                m11: (-tau)*p0.y + (tau)*p2.y,
                m12: (-tau)*p0.z + (tau)*p2.z,
                m13: (-tau)*p0.w + (tau)*p2.w,
                m20: (2*tau)*p0.x + (tau-3)*p1.x + (3-2*tau)*p2.x + (-tau)*p3.x,
                m21: (2*tau)*p0.y + (tau-3)*p1.y + (3-2*tau)*p2.y + (-tau)*p3.y,
                m22: (2*tau)*p0.z + (tau-3)*p1.z + (3-2*tau)*p2.z + (-tau)*p3.z,
                m23: (2*tau)*p0.w + (tau-3)*p1.w + (3-2*tau)*p2.w + (-tau)*p3.w,
                m30: (-tau)*p0.x + (2-tau)*p1.x + (tau-2)*p2.x + (tau)*p3.x,
                m31: (-tau)*p0.y + (2-tau)*p1.y + (tau-2)*p2.y + (tau)*p3.y,
                m32: (-tau)*p0.z + (2-tau)*p1.z + (tau-2)*p2.z + (tau)*p3.z,
                m33: (-tau)*p0.w + (2-tau)*p1.w + (tau-2)*p2.w + (tau)*p3.w,
            };
            segs.push({
                geomMatrix: m,
            });
        }

        return {
            points: controlPoints,
            segments: segs,
            tau: tau,
            evalPos:   function(u) { return _evalPos(this, u); },
            evalDPos:  function(u) { return _evalDPos(this, u); },
            evalDDPos: function(u) { return _evalDDPos(this, u); },
            evalCurvature2: function(dpos, ddpos) { return _evalCurvature2(dpos, ddpos); },
            evalCurvature3: function(dpos, ddpos) { return _evalCurvature3(dpos, ddpos); },
        };
    }

    function makeSplineHermite(controlPointsAndTangents) {
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPointsAndTangents.length-1; iSeg+=1) {
            var p0 = controlPointsAndTangents[iSeg+0];
            var p1 = controlPointsAndTangents[iSeg+1];
            p0 = {  x: p0.x  || 0,  y: p0.y  || 0,  z: p0.z  || 0,  w: p0.w  || 0,
                   tx: p0.tx || 0, ty: p0.ty || 0, tz: p0.tz || 0, tw: p0.tw || 0 };
            p1 = {  x: p1.x  || 0,  y: p1.y  || 0,  z: p1.z  || 0,  w: p1.w  || 0,
                   tx: p1.tx || 0, ty: p1.ty || 0, tz: p1.tz || 0, tw: p1.tw || 0 };
            var m = {
                m00: p0.x,
                m01: p0.y,
                m02: p0.z,
                m03: p0.w,
                m10: p0.tx,
                m11: p0.ty,
                m12: p0.tz,
                m13: p0.tw,
                m20: (-3)*p0.x + (3)*p1.x + (-2)*p0.tx + (-1)*p1.tx,
                m21: (-3)*p0.y + (3)*p1.y + (-2)*p0.ty + (-1)*p1.ty,
                m22: (-3)*p0.z + (3)*p1.z + (-2)*p0.tz + (-1)*p1.tz,
                m23: (-3)*p0.w + (3)*p1.w + (-2)*p0.tw + (-1)*p1.tw,
                m30: (2)*p0.x + (-2)*p1.x + p0.tx + p1.tx,
                m31: (2)*p0.y + (-2)*p1.y + p0.ty + p1.ty,
                m32: (2)*p0.z + (-2)*p1.z + p0.tz + p1.tz,
                m33: (2)*p0.w + (-2)*p1.w + p0.tw + p1.tw
            };
            segs.push({
                geomMatrix: m,
            });
        }
        
        return {
            points: controlPointsAndTangents,
            segments: segs,
            evalPos:   function(u) { return _evalPos(this, u); },
            evalDPos:  function(u) { return _evalDPos(this, u); },
            evalDDPos: function(u) { return _evalDDPos(this, u); },
            evalCurvature2: function(dpos, ddpos) { return _evalCurvature2(dpos, ddpos); },
            evalCurvature3: function(dpos, ddpos) { return _evalCurvature3(dpos, ddpos); },
        };
    }

    function makeSplineBezier(controlPointsAndTangents) {
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPointsAndTangents.length-1; iSeg+=1) {
            var p0 = controlPointsAndTangents[iSeg+0];
            var p1 = {
                x:controlPointsAndTangents[iSeg+0].x + controlPointsAndTangents[iSeg+0].tx,
                y:controlPointsAndTangents[iSeg+0].y + controlPointsAndTangents[iSeg+0].ty,
                z:controlPointsAndTangents[iSeg+0].z + controlPointsAndTangents[iSeg+0].tz,
                w:controlPointsAndTangents[iSeg+0].w + controlPointsAndTangents[iSeg+0].tw,
            };
            var p2 = {
                x:controlPointsAndTangents[iSeg+1].x - controlPointsAndTangents[iSeg+1].tx,
                y:controlPointsAndTangents[iSeg+1].y - controlPointsAndTangents[iSeg+1].ty,
                z:controlPointsAndTangents[iSeg+1].z - controlPointsAndTangents[iSeg+1].tz,
                w:controlPointsAndTangents[iSeg+1].w - controlPointsAndTangents[iSeg+1].tw,
            };
            var p3 = controlPointsAndTangents[iSeg+1];
            p0 = {x: p0.x || 0, y: p0.y || 0, z: p0.z || 0, w: p0.w || 0};
            p1 = {x: p1.x || 0, y: p1.y || 0, z: p1.z || 0, w: p1.w || 0};
            p2 = {x: p2.x || 0, y: p2.y || 0, z: p2.z || 0, w: p2.w || 0};
            p3 = {x: p3.x || 0, y: p3.y || 0, z: p3.z || 0, w: p3.w || 0};
            var m = {
                m00: p0.x,
                m01: p0.y,
                m02: p0.z,
                m03: p0.w,
                m10: (-3)*p0.x + ( 3)*p1.x,
                m11: (-3)*p0.y + ( 3)*p1.y,
                m12: (-3)*p0.z + ( 3)*p1.z,
                m13: (-3)*p0.w + ( 3)*p1.w,
                m20: ( 3)*p0.x + (-6)*p1.x + ( 3)*p2.x,
                m21: ( 3)*p0.y + (-6)*p1.y + ( 3)*p2.y,
                m22: ( 3)*p0.z + (-6)*p1.z + ( 3)*p2.z,
                m23: ( 3)*p0.w + (-6)*p1.w + ( 3)*p2.w,
                m30: (-1)*p0.x + ( 3)*p1.x + (-3)*p2.x + p3.x,
                m31: (-1)*p0.y + ( 3)*p1.y + (-3)*p2.y + p3.y,
                m32: (-1)*p0.z + ( 3)*p1.z + (-3)*p2.z + p3.z,
                m33: (-1)*p0.w + ( 3)*p1.w + (-3)*p2.w + p3.w,
            };
            segs.push({
                geomMatrix: m,
            });
        }
      
        return {
            points: controlPointsAndTangents,
            segments: segs,
            evalPos:   function(u) { return _evalPos(this, u); },
            evalDPos:  function(u) { return _evalDPos(this, u); },
            evalDDPos: function(u) { return _evalDDPos(this, u); },
            evalCurvature2: function(dpos, ddpos) { return _evalCurvature2(dpos, ddpos); },
            evalCurvature3: function(dpos, ddpos) { return _evalCurvature3(dpos, ddpos); },
        };
    }

    function makeSplineCatmullRomCentripetal(controlPoints, alpha) {
        if (typeof(alpha) === 'undefined') alpha = 0.5;
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPoints.length-3; iSeg+=1) {
            var p0 = controlPoints[iSeg+0];
            var p1 = controlPoints[iSeg+1];
            var p2 = controlPoints[iSeg+2];
            var p3 = controlPoints[iSeg+3];
            p0 = {x: p0.x || 0, y: p0.y || 0, z: p0.z || 0, w: p0.w || 0};
            p1 = {x: p1.x || 0, y: p1.y || 0, z: p1.z || 0, w: p1.w || 0};
            p2 = {x: p2.x || 0, y: p2.y || 0, z: p2.z || 0, w: p2.w || 0};
            p3 = {x: p3.x || 0, y: p3.y || 0, z: p3.z || 0, w: p3.w || 0};
            var t0 = 0;
            var t1 = t0 + Math.pow( (p1.x-p0.x)*(p1.x-p0.x) +
                                    (p1.y-p0.y)*(p1.y-p0.y) +
                                    (p1.z-p0.z)*(p1.z-p0.z) +
                                    (p1.w-p0.w)*(p1.w-p0.w),
                                    alpha*0.5 );
            var t2 = t1 + Math.pow( (p2.x-p1.x)*(p2.x-p1.x) +
                                    (p2.y-p1.y)*(p2.y-p1.y) +
                                    (p2.z-p1.z)*(p2.z-p1.z) +
                                    (p2.w-p1.w)*(p2.w-p1.w),
                                    alpha*0.5 );
            var t3 = t2 + Math.pow( (p3.x-p2.x)*(p3.x-p2.x) +
                                    (p3.y-p2.y)*(p3.y-p2.y) +
                                    (p3.z-p2.z)*(p3.z-p2.z) +
                                    (p3.w-p2.w)*(p3.w-p2.w),
                                    alpha*0.5 );

            // Simplification: reduce centripetal Catmull-Rom to Hermite, as described in
            // https://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
            // Basically, given P0,P1,P2,P3 and t0,t1,t2,t3, compute tangents at P1 and P2:
            //   tan1 = (P1 - P0) / (t1 - t0) - (P2 - P0) / (t2 - t0) + (P2 - P1) / (t2 - t1)
            //   tan2 = (P2 - P1) / (t2 - t1) - (P3 - P1) / (t3 - t1) + (P3 - P2) / (t3 - t2)
            // And plug into the standard Hermite basis matrix. If evaluating the segment from
            // P1 to P2, the tangents must be scaled by (t2-t1) to put them in the appropriate range.
            var tan1 = {
                x: (p1.x-p0.x)/(t1-t0) - (p2.x-p0.x)/(t2-t0) + (p2.x-p1.x)/(t2-t1),
                y: (p1.y-p0.y)/(t1-t0) - (p2.y-p0.y)/(t2-t0) + (p2.y-p1.y)/(t2-t1),
                z: (p1.z-p0.z)/(t1-t0) - (p2.z-p0.z)/(t2-t0) + (p2.z-p1.z)/(t2-t1),
                w: (p1.w-p0.w)/(t1-t0) - (p2.w-p0.w)/(t2-t0) + (p2.w-p1.w)/(t2-t1),
            };
            var tan2 = {
                x: (p2.x-p1.x)/(t2-t1) - (p3.x-p1.x)/(t3-t1) + (p3.x-p2.x) / (t3-t2),
                y: (p2.y-p1.y)/(t2-t1) - (p3.y-p1.y)/(t3-t1) + (p3.y-p2.y) / (t3-t2),
                z: (p2.z-p1.z)/(t2-t1) - (p3.z-p1.z)/(t3-t1) + (p3.z-p2.z) / (t3-t2),
                w: (p2.w-p1.w)/(t2-t1) - (p3.w-p1.w)/(t3-t1) + (p3.w-p2.w) / (t3-t2),
            };
            tan1.x *= (t2-t1);
            tan1.y *= (t2-t1);
            tan1.z *= (t2-t1);
            tan1.w *= (t2-t1);
            tan2.x *= (t2-t1);
            tan2.y *= (t2-t1);
            tan2.z *= (t2-t1);
            tan2.w *= (t2-t1);
            var m = {
                m00: p1.x,
                m01: p1.y,
                m02: p1.z,
                m03: p1.w,
                m10: tan1.x,
                m11: tan1.y,
                m12: tan1.z,
                m13: tan1.w,
                m20: (-3)*p1.x + (3)*p2.x + (-2)*tan1.x + (-1)*tan2.x,
                m21: (-3)*p1.y + (3)*p2.y + (-2)*tan1.y + (-1)*tan2.y,
                m22: (-3)*p1.z + (3)*p2.z + (-2)*tan1.z + (-1)*tan2.z,
                m23: (-3)*p1.w + (3)*p2.w + (-2)*tan1.w + (-1)*tan2.w,
                m30: (2)*p1.x + (-2)*p2.x + tan1.x + tan2.x,
                m31: (2)*p1.y + (-2)*p2.y + tan1.y + tan2.y,
                m32: (2)*p1.z + (-2)*p2.z + tan1.z + tan2.z,
                m33: (2)*p1.w + (-2)*p2.w + tan1.w + tan2.w,
            };

            segs.push({
                geomMatrix: m
            });
        }
        
        return {
            points: controlPoints,
            segments: segs,
            alpha: alpha,
            evalPos:   function(u) { return _evalPos(this, u); },
            evalDPos:  function(u) { return _evalDPos(this, u); },
            evalDDPos: function(u) { return _evalDDPos(this, u); },
            evalCurvature2: function(dpos, ddpos) { return _evalCurvature2(dpos, ddpos); },
            evalCurvature3: function(dpos, ddpos) { return _evalCurvature3(dpos, ddpos); },
        };
    }

    return {
        makeSplineCardinal: makeSplineCardinal,
        makeSplineHermite: makeSplineHermite,
        makeSplineBezier: makeSplineBezier,
        makeSplineCatmullRomCentripetal: makeSplineCatmullRomCentripetal,
    };
}();
