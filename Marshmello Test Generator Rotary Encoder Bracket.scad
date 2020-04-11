difference() {   
  cube([32, 12, 1.5]);
  translate([4,6,-1]) cylinder(d=3.2, h=20, $fn=16);
  translate([28,6,-1]) cylinder(d=3.2, h=20, $fn=16);

  translate([10,6,-1]) cylinder(d=3.2, h=20, $fn=16);
  translate([22,6,-1]) cylinder(d=3.2, h=20, $fn=16);

}

