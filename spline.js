/* jshint browser: false
   , strict: true
   , eqeqeq: true
   , forin: true
   , freeze: true
   , latedef: true
   , newcap: true
   , noarg: true
   , undef: true
   , plusplus: true
*/

/* global console
*/

var SPLINE = function() {
    "use strict";

    function clamp(x, xMin, xMax) {
        return (x<xMin) ? xMin : ( (x>xMax) ? xMax : x );
    }

    function makeSplineCardinal(controlPoints, tau) {
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPoints.length-3; iSeg+=1) {
            var p0 = controlPoints[iSeg+0];
            var p1 = controlPoints[iSeg+1];
            var p2 = controlPoints[iSeg+2];
            var p3 = controlPoints[iSeg+3];
            var m = {
                m00: p1.x,
                m01: p1.y,
                m02: 0,
                m03: 0,
                m10: (-tau)*p0.x + (tau)*p2.x,
                m11: (-tau)*p0.y + (tau)*p2.y,
                m12: 0,
                m13: 0,
                m20: (2*tau)*p0.x + (tau-3)*p1.x + (3-2*tau)*p2.x + (-tau)*p3.x,
                m21: (2*tau)*p0.y + (tau-3)*p1.y + (3-2*tau)*p2.y + (-tau)*p3.y,
                m22: 0,
                m23: 0,
                m30: (-tau)*p0.x + (2-tau)*p1.x + (tau-2)*p2.x + (tau)*p3.x,
                m31: (-tau)*p0.y + (2-tau)*p1.y + (tau-2)*p2.y + (tau)*p3.y,
                m32: 0,
                m33: 0
            };
            segs.push({
                geomMatrix: m,
            });
        }

        function eval1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: ((m.m30*uFrac + m.m20)*uFrac + m.m10)*uFrac + m.m00,
                y: ((m.m31*uFrac + m.m21)*uFrac + m.m11)*uFrac + m.m01
            };
        }

        function evalTangent1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: (3*m.m30*uFrac + 2*m.m20)*uFrac + m.m10,
                y: (3*m.m31*uFrac + 2*m.m21)*uFrac + m.m11
            };
        }
        
        return {
            points: controlPoints,
            segments: segs,
            tau: tau,
            eval1: eval1,
            evalTangent1: evalTangent1
        };
    }

    function makeSplineHermite(controlPointsAndTangents) {
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPointsAndTangents.length-1; iSeg+=1) {
            var p0 = controlPointsAndTangents[iSeg+0];
            var p1 = controlPointsAndTangents[iSeg+1];
            var m = {
                m00: p0.x,
                m01: p0.y,
                m02: 0,
                m03: 0,
                m10: p0.tx,
                m11: p0.ty,
                m12: 0,
                m13: 0,
                m20: (-3)*p0.x + (3)*p1.x + (-2)*p0.tx + (-1)*p1.tx,
                m21: (-3)*p0.y + (3)*p1.y + (-2)*p0.ty + (-1)*p1.ty,
                m22: 0,
                m23: 0,
                m30: (2)*p0.x + (-2)*p1.x + p0.tx + p1.tx,
                m31: (2)*p0.y + (-2)*p1.y + p0.ty + p1.ty,
                m32: 0,
                m33: 0
            };
            segs.push({
                geomMatrix: m,
            });
        }
        
        function eval1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: ((m.m30*uFrac + m.m20)*uFrac + m.m10)*uFrac + m.m00,
                y: ((m.m31*uFrac + m.m21)*uFrac + m.m11)*uFrac + m.m01
            };
        }

        function evalTangent1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: (3*m.m30*uFrac + 2*m.m20)*uFrac + m.m10,
                y: (3*m.m31*uFrac + 2*m.m21)*uFrac + m.m11
            };
        }

        return {
            points: controlPointsAndTangents,
            segments: segs,
            eval1: eval1,
            evalTangent1: evalTangent1
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
            };
            var p2 = {
                x:controlPointsAndTangents[iSeg+1].x - controlPointsAndTangents[iSeg+1].tx,
                y:controlPointsAndTangents[iSeg+1].y - controlPointsAndTangents[iSeg+1].ty,
            };
            var p3 = controlPointsAndTangents[iSeg+1];
            var m = {
                m00: p0.x,
                m01: p0.y,
                m02: 0,
                m03: 0,
                m10: (-3)*p0.x + (3*p1.x),
                m11: (-3)*p0.y + (3*p1.y),
                m12: 0,
                m13: 0,
                m20: (3)*p0.x + (-6)*p1.x + (3)*p2.x,
                m21: (3)*p0.y + (-6)*p1.y + (3)*p2.y,
                m22: 0,
                m23: 0,
                m30: (-1)*p0.x + (3)*p1.x + (-3)*p2.x + p3.x,
                m31: (-1)*p0.y + (3)*p1.y + (-3)*p2.y + p3.y,
                m32: 0,
                m33: 0
            };
            segs.push({
                geomMatrix: m,
            });
        }
        
        function eval1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: ((m.m30*uFrac + m.m20)*uFrac + m.m10)*uFrac + m.m00,
                y: ((m.m31*uFrac + m.m21)*uFrac + m.m11)*uFrac + m.m01
            };
        }

        function evalTangent1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: (3*m.m30*uFrac + 2*m.m20)*uFrac + m.m10,
                y: (3*m.m31*uFrac + 2*m.m21)*uFrac + m.m11
            };
        }

        return {
            points: controlPointsAndTangents,
            segments: segs,
            eval1: eval1,
            evalTangent1: evalTangent1
        };
    }

    function makeSplineCatmullRomCentripetal(controlPoints, alpha) {
        var iSeg,iSamp;
        var segs = [];
        for(iSeg=0; iSeg<controlPoints.length-3; iSeg+=1) {
            var p0 = controlPoints[iSeg+0];
            var p1 = controlPoints[iSeg+1];
            var p2 = controlPoints[iSeg+2];
            var p3 = controlPoints[iSeg+3];
            var t0 = 0;
            var t1 = t0 + Math.pow( (p1.x-p0.x)*(p1.x-p0.x) + (p1.y-p0.y)*(p1.y-p0.y), alpha*0.5 );
            var t2 = t1 + Math.pow( (p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y), alpha*0.5 );
            var t3 = t2 + Math.pow( (p3.x-p2.x)*(p3.x-p2.x) + (p3.y-p2.y)*(p3.y-p2.y), alpha*0.5 );

            // Simplification: reduce centripetal Catmull-Rom to Hermite, as described in
            // https://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections/23980479#23980479
            // Basically, given P0,P1,P2,P3 and t0,t1,t2,t3, compute tangents at P1 and P2:
            //   tan1 = (P1 - P0) / (t1 - t0) - (P2 - P0) / (t2 - t0) + (P2 - P1) / (t2 - t1)
            //   tan2 = (P2 - P1) / (t2 - t1) - (P3 - P1) / (t3 - t1) + (P3 - P2) / (t3 - t2)
            // And plug into the standard Hermite basis matrix. If evaluating the segment from
            // P1 to P2, the tangents must be scaled by (t2-t1) to put them in the appropriate range.
            var tan1 = {
                x: (p1.x-p0.x)/(t1-t0) - (p2.x-p0.x)/(t2-t0) + (p2.x-p1.x)/(t2-t1),
                y: (p1.y-p0.y)/(t1-t0) - (p2.y-p0.y)/(t2-t0) + (p2.y-p1.y)/(t2-t1)
            };
            var tan2 = {
                x: (p2.x-p1.x)/(t2-t1) - (p3.x-p1.x)/(t3-t1) + (p3.x-p2.x) / (t3-t2),
                y: (p2.y-p1.y)/(t2-t1) - (p3.y-p1.y)/(t3-t1) + (p3.y-p2.y) / (t3-t2)
            };
            tan1.x *= (t2-t1);
            tan1.y *= (t2-t1);
            tan2.x *= (t2-t1);
            tan2.y *= (t2-t1);
            var m = {
                m00: p1.x,
                m01: p1.y,
                m02: 0,
                m03: 0,
                m10: tan1.x,
                m11: tan1.y,
                m12: 0,
                m13: 0,
                m20: (-3)*p1.x + (3)*p2.x + (-2)*tan1.x + (-1)*tan2.x,
                m21: (-3)*p1.y + (3)*p2.y + (-2)*tan1.y + (-1)*tan2.y,
                m22: 0,
                m23: 0,
                m30: (2)*p1.x + (-2)*p2.x + tan1.x + tan2.x,
                m31: (2)*p1.y + (-2)*p2.y + tan1.y + tan2.y,
                m32: 0,
                m33: 0
            };

            segs.push({
                geomMatrix: m
            });
        }
        
        function eval1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: ((m.m30*uFrac + m.m20)*uFrac + m.m10)*uFrac + m.m00,
                y: ((m.m31*uFrac + m.m21)*uFrac + m.m11)*uFrac + m.m01
            };
        }

        function evalTangent1(u) {
            var uClamp = clamp(u, 0, segs.length);
            var uInt = uClamp | 0, uFrac = uClamp - uInt;
            if (uClamp === segs.length) {
                uInt  = uInt-1;
                uFrac = 1.0;
            }
            var m = segs[uInt].geomMatrix;
            return {
                x: (3*m.m30*uFrac + 2*m.m20)*uFrac + m.m10,
                y: (3*m.m31*uFrac + 2*m.m21)*uFrac + m.m11
            };
        }

        return {
            points: controlPoints,
            segments: segs,
            alpha: alpha,
            eval1: eval1,
            evalTangent1: evalTangent1
        };
    }

    return {
        makeSplineCardinal: makeSplineCardinal,
        makeSplineHermite: makeSplineHermite,
        makeSplineBezier: makeSplineBezier,
        makeSplineCatmullRomCentripetal: makeSplineCatmullRomCentripetal,
    };
}();
