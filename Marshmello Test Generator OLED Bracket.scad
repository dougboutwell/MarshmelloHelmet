
difference() {   
  cube([15, 32.5, 2]);
  translate([7.5,5,-1]) cylinder(d=3.2, h=20, $fn=16);
  translate([7.5,27.5,-1]) cylinder(d=3.2, h=20, $fn=16);
}

translate([0, 11.25, 2]) cube([15, 10, 4]);