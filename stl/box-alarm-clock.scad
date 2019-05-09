/*
 * Alarm clock kit box
 * 
 * The Cave, 2019
 * https://thecave.cz
 * 
 * Licensed under MIT License (see LICENSE file for details)
 *
 */
$pcbW = 79;
$pcbH = 28;
$pcbThickness = 1.6;
$innerZ = 18;

$usbW = 12;
$usbH = 10;
$usbOffsetZ = 6.5;
$usbOffsetY = 0.5;

$wall = 1.2;
$radius = 5.08;
$fn=100;

$standZ = $innerZ-$pcbThickness;
$standDia = 5;

difference() {
    translate([$radius/2,$radius/2,$radius/2]) minkowski() {
        cube([$pcbW+$wall*2-$radius,$pcbH+$wall*2-$radius,$innerZ+$radius*2+$wall]);
        sphere(d=$radius);
    }
    
    translate([$wall,$wall,$wall]) cube([$pcbW,$pcbH,$innerZ+1]);
    translate([-1,-1,$wall+$innerZ]) cube([$pcbW+$wall*2+10,$pcbH+$wall*2+10,30]);
    
    translate([-1, (($wall*2+$pcbH-$usbW)/2)-$usbOffsetY, $wall+$innerZ-$usbOffsetZ-$usbH]) #cube([$wall*4,$usbW,$usbH]);
}

translate([$wall,$wall,$wall]) {
    difference() {
        union() {
            translate([$standDia/2,$standDia/2,0]) cylinder(d=$standDia,h=$standZ);
            cube([$standDia,$standDia/2,$standZ]);
            cube([$standDia/2,$standDia,$standZ]);
            
            translate([$pcbW-$standDia/2,$standDia/2,0]) cylinder(d=$standDia,h=$standZ);
            translate([$pcbW-$standDia,0,0]) cube([$standDia,$standDia/2,$standZ]);
            translate([$pcbW-$standDia/2,0,0]) cube([$standDia/2,$standDia,$standZ]);
            
            translate([$standDia/2,$pcbH-$standDia/2,0]) cylinder(d=$standDia,h=$standZ);
            translate([0,$pcbH-$standDia/2,0]) cube([$standDia,$standDia/2,$standZ]);
            translate([0,$pcbH-$standDia,0]) cube([$standDia/2,$standDia,$standZ]);
            
            translate([$pcbW-$standDia/2,$pcbH-$standDia/2,0]) cylinder(d=$standDia,h=$standZ);
            translate([$pcbW-$standDia,$pcbH-$standDia/2,0]) cube([$standDia,$standDia/2,$standZ]);
            translate([$pcbW-$standDia/2,$pcbH-$standDia,0]) cube([$standDia/2,$standDia,$standZ]);
        }
        
        translate([$standDia/2,$standDia/2,0]) #cylinder(d=2.7,h=$standZ+5);
        translate([$pcbW-$standDia/2,$standDia/2,0]) #cylinder(d=2.7,h=$standZ+5);
        translate([$standDia/2,$pcbH-$standDia/2,0]) #cylinder(d=2.7,h=$standZ+5);
        translate([$pcbW-$standDia/2,$pcbH-$standDia/2,0]) #cylinder(d=2.7,h=$standZ+5);
    }
}

$legOffset = -5;
$legWidth = 8;
module leg() { polyhedron(
    points=[[0,0,$radius/2],[0,0,$wall+$innerZ],[0,$legOffset,$wall+$innerZ-2],[$legWidth,0,$radius/2],[$legWidth,0,$wall+$innerZ],[$legWidth,$legOffset,$wall+$innerZ-2]],
    faces=[[0,1,2],[2,1,4,5],[5,4,3],[3,4,1,0],[3,0,2,5]],
    convexity=1
);
}

translate([10,0,0]) leg();
translate([$wall*2+$pcbW-$legWidth-10,0,0]) leg();

