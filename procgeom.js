/* jshint browser: true
   , strict: true
   , bitwise: true
   , eqeqeq: true
   , forin: true
   , freeze: true
   , latedef: true
   , newcap: true
   , noarg: true
   , undef: true
   , plusplus: true
*/

/* global THREE
   , console
   , dat
   , Detector
   , Stats
   , SPLINE
*/

var procGeomScene = function() {
	"use strict";
	
	var scene, camera, renderer, stats;
	var tweakables;

    function _toVec3(p) {
        return new THREE.Vector3(p.x, p.y, p.z);
    }
	
    function _computeNormals(unitTangent, oldUnitNormal) {
        var uBinormal = unitTangent.clone().cross(oldUnitNormal).normalize();
        var uNormal = uBinormal.clone().cross(unitTangent).normalize();
        return {
            unitNormal: uNormal,
            unitBinormal: uBinormal,
        };
    }

	function makeProcGeom() {
		var iVert, iSeg, iKnot, iRing;
        var knots = [];
        var knotCount = Math.floor(5 + 5*Math.random());
        var segmentCount;
        var spline;
        var ringsPerSegment;
        var ringCenter;
        var tangent, normals;
        var lastUnitNormal = new THREE.Vector3(0,1,0);
        var vertsPerRing = 36;
        var ringRadius;
        var vert;
        var geom = new THREE.Geometry();
        var mesh;
        var segLength;
        var lastRingBaseIndex = 0;
        var p0,p1,p2,p3;
        var t;
        for(iKnot=0; iKnot < knotCount; iKnot += 1) {
            // TODO: minimum distance between neighboring knots?
            knots.push({
                x:  2*Math.random()-1,
                y:  2*Math.random()-1,
                z:  2*Math.random()-1,
                w:  0.2*Math.random()+0.05,
                tx: 2*Math.random()-1,
                ty: 2*Math.random()-1,
                tz: 2*Math.random()-1,
                tw: 0.1*Math.random()-0.05,
            });
        }
        knots.push(knots[0]); // Duplicate knots to loop the curve
        spline = SPLINE.makeSplineBezier(knots);
        segmentCount = spline.segments.length;
        for(iSeg=0; iSeg<segmentCount; iSeg += 1) {
            p1 = knots[iSeg+0];
            p2 = knots[iSeg+1];
            // Rough heuristic: distance between segments -> number of rings per segment
            segLength = Math.sqrt( (p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) + (p2.z-p1.z)*(p2.z-p1.z) );
            ringsPerSegment = 4 + Math.ceil(segLength * 20); // tune this!
            for(iRing=0; iRing<=ringsPerSegment; iRing += 1) {
                if (iRing === ringsPerSegment && iSeg < segmentCount-1)
                    continue;
                t = iSeg + iRing/ringsPerSegment;
                ringCenter = _toVec3( spline.evalPos(t) );
                tangent = _toVec3( spline.evalDPos(t) ).normalize();
                normals = _computeNormals(tangent, lastUnitNormal);
                lastUnitNormal = normals.unitNormal;
                ringRadius = spline.evalPos(t).w;
                for(iVert=0; iVert<vertsPerRing; iVert += 1) {
                    vert = ringCenter.clone();
                    vert.add( normals.unitBinormal.clone().multiplyScalar( ringRadius*Math.cos(2*Math.PI*iVert/vertsPerRing) ) );
                    vert.add(   normals.unitNormal.clone().multiplyScalar( ringRadius*Math.sin(2*Math.PI*iVert/vertsPerRing) ) );
                    geom.vertices.push(vert);
                }
                if (!(iSeg===0 && iRing===0)) {
                    for(iVert=0; iVert<vertsPerRing-1; iVert += 1) {
                        geom.faces.push( new THREE.Face3(lastRingBaseIndex+iVert,
                                                         lastRingBaseIndex+iVert+vertsPerRing,
                                                         lastRingBaseIndex+iVert+1) );
                        geom.faces.push( new THREE.Face3(lastRingBaseIndex+iVert+1,
                                                         lastRingBaseIndex+iVert+vertsPerRing,
                                                         lastRingBaseIndex+iVert+vertsPerRing+1) );
                    }
                    geom.faces.push( new THREE.Face3(lastRingBaseIndex+iVert,
                                                     lastRingBaseIndex+iVert+vertsPerRing,
                                                     lastRingBaseIndex+0) );
                    geom.faces.push( new THREE.Face3(lastRingBaseIndex+0,
                                                     lastRingBaseIndex+iVert+vertsPerRing,
                                                     lastRingBaseIndex+vertsPerRing) );
                    lastRingBaseIndex += vertsPerRing;
                }
            }
        }
        geom.computeFaceNormals();
        geom.computeVertexNormals();
        mesh = new THREE.Mesh(geom, new THREE.MeshNormalMaterial({wireframe:false}));
        return mesh;
	}

	function init() {
	    if (!Detector.webgl) {
		    Detector.addGetWebGLMessage();
	    }
	
	    stats = new Stats();
	    stats.setMode(0);
	    stats.domElement.style.position = 'absolute';
	    stats.domElement.style.left = '0';
	    stats.domElement.style.top = '0';
	    document.body.appendChild(stats.domElement);
	    
	    scene = new THREE.Scene();
	    //scene.fog = new THREE.FogExp2( 0xefd1b5, 0.0025 );
	    
	    var aspectRatio = window.innerWidth / window.innerHeight;
	    camera = new THREE.PerspectiveCamera(45, aspectRatio, 0.001, 10);
	    //camera = new THREE.OrthographicCamera(-20*aspectRatio, 20*aspectRatio, 20, -20, 0.1, 1000);
        
	    renderer = new THREE.WebGLRenderer();
	    renderer.setClearColor(0x000000, 1.0);
	    renderer.setSize(window.innerWidth, window.innerHeight);
        
	    // Light
	    var ambLight = new THREE.AmbientLight(0x404040);
	    scene.add(ambLight);
        
        // ProcGeom
        var pg = makeProcGeom();
	    scene.add(pg);
        
	    tweakables = {
		    cameraRot: 0,
	    };
	    var gui = new dat.GUI();
	    gui.add(tweakables, 'cameraRot', 0, 360);
        
	    document.body.appendChild(renderer.domElement);
        
	    render(scene, camera);
    }

	function render() {
		var timeSec = Date.now() * 0.001;

		var cameraRotRads = tweakables.cameraRot * 2.0 * Math.PI / 360;
		camera.position.set( 5*Math.sin(cameraRotRads), 0, 5*Math.cos(cameraRotRads) );
		camera.lookAt(scene.position);

		stats.update();
		renderer.render(scene, camera);
		requestAnimationFrame(render);
	}
	
	window.onload = init;
}();
