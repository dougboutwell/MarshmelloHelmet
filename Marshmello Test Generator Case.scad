tolerance = 0.2;
shaft_diameter = 7 + tolerance;

module shaft_hole() {
  cylinder(d=7.1, h=12, $fn=30);
}

module shaft_mount() {
  difference() {
    cylinder(d=20,h=2);
    translate([0,0,-1]) shaft_hole();
    translate([2,-1.25,-1]) cube([10, 2.5, 12]);
  }
}

module rotary_mount() {
  difference() {   
    translate([-16,-6,0]) {
      cube([8,12,9]);
      translate([24,0,0]) cube([8,12,9]);
    }
    
    translate([-12,0,-1]) cylinder(d=4,h=12,$fn=16);
    translate([12,0,-1]) cylinder(d=4,h=12,$fn=16);
  }
}

// Front plate
difference() {
cube([140,80,2]);

// OLED hole
translate([115/2, 50, -1]) cube([25,10,4]);

translate([25,25,0]) {
translate([0,0,-1]) shaft_hole();
translate([30,0,-1]) shaft_hole();
translate([60,0,-1]) shaft_hole();
translate([90,0,-1]) shaft_hole();

translate([0,30,-1]) shaft_hole();
translate([90,30,-1]) shaft_hole();

};
}

// Potentiometer mounts
translate([25,25,2]) {
  shaft_mount();
  translate([30,0,0]) shaft_mount();
  translate([60,0,0]) shaft_mount();
  translate([90,0,0]) shaft_mount();  
}

// Rotary encoder mounts
translate([25,55,0]) rotary_mount();
translate([115,55,0]) rotary_mount();

// Shell / sides
difference() {
    cube([140,80,30]);
    translate([2,2,-1]) cube([136,76,32]);
    translate([60,77,20]) cube([20,4,11]);
}

// OLED Frame
translate([115/2 - 17 - .25, 55 - 16.25, 2])
difference() {
  cube([60, 32.5, 5]);
  translate([10,10,-1]) cube([40, 12.5, 10]);
  translate([30,5,-1]) cylinder(d=4, h=20, $fn=16);
  translate([30,27.5,-1]) cylinder(d=4, h=20, $fn=16);
}
