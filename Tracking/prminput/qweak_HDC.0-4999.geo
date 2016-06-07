# For simulated events
#
# Differences from data:
# - Removed all pitch/yaw/roll
# - All detectors in perfect position


# Region 2 HDC geometry

type = h
region = 2

spatial_resolution = 1e-2
track_resolution = 0.5e-1
slope_matching = 1e-2

detector_rotation = 0

# Geometry of a detector chamber
active_width_x = 28.575
active_width_y = 37.39
active_width_z = 0.0

# Element properties
number_of_elements = 32
element_spacing = 1.1684
element_offset = -18.1102

# Tilt of detector around the Z axis
detector_tilt = 0


[HDC_pkg1_front_plane01_x]
  plane = 1
  package = 1
  direction = x
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -342.305
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg1_front_plane02_u]
  plane = 2
  package = 1
  direction = u
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -340.325
  element_angle_cos = 0.6
  element_angle_sin = -0.8

[HDC_pkg1_front_plane03_v]
  plane = 3
  package = 1
  direction = v
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -338.345
  element_angle_cos = 0.6
  element_angle_sin = 0.8
  
[HDC_pkg1_front_plane04_x]
  plane = 4
  package = 1
  direction = x
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -336.365
  element_angle_cos = 1
  element_angle_sin = 0.001
  
[HDC_pkg1_front_plane05_u]
  plane = 5
  package = 1
  direction = u
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -334.385
  element_angle_cos = 0.6
  element_angle_sin = -0.8

[HDC_pkg1_front_plane06_v]
  plane = 6
  package = 1
  direction = v
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -332.405
  element_angle_cos = 0.6
  element_angle_sin = 0.8



[HDC_pkg2_front_plane01_x]
  plane = 1
  package = 2
  direction = x
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -342.305
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg2_front_plane02_u]
  plane = 2
  package = 2
  direction = u
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -340.325
  element_angle_cos = 0.6
  element_angle_sin = -0.8
  
[HDC_pkg2_front_plane03_v]
  plane = 3
  package = 2
  direction = v
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -338.345
  element_angle_cos = 0.6
  element_angle_sin = 0.8
  
[HDC_pkg2_front_plane04_x]
  plane = 4
  package = 2
  direction = x
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -336.365
  element_angle_cos = 1
  element_angle_sin = 0.001
  
[HDC_pkg2_front_plane05_u]
  plane = 5
  package = 2
  direction = u
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -334.385
  element_angle_cos = 0.6
  element_angle_sin = -0.8
  
[HDC_pkg2_front_plane06_v]
  plane = 6
  package = 2
  direction = v
  detector_origin_x = -0.276
  detector_origin_y = 51.104
  detector_origin_z = -332.405
  element_angle_cos = 0.6
  element_angle_sin = 0.8



[HDC_pkg1_back_plane07_x]
  plane = 7
  package = 1
  direction = x
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -299.627
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg1_back_plane08_u]
  plane = 8
  package = 1
  direction = u
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -297.647
  element_angle_cos = 0.6
  element_angle_sin = -0.8

[HDC_pkg1_back_plane09_v]
  plane = 9
  package = 1
  direction = v
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -295.667
  element_angle_cos = 0.6
  element_angle_sin = 0.8
  
[HDC_pkg1_back_plane10_x]
  plane = 10
  package = 1
  direction = x
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -293.687
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg1_back_plane11_u]
  plane = 11
  package = 1
  direction = u
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -291.707
  element_angle_cos = 0.6
  element_angle_sin = -0.8
  
[HDC_pkg1_back_plane12_v]
  plane = 12
  package = 1
  direction = v
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -289.727
  element_angle_cos = 0.6
  element_angle_sin = 0.8
  


[HDC_pkg2_back_plane07_x]
  plane = 7
  package = 2
  direction = x
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -299.627
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg2_back_plane08_u]
  plane = 8
  package = 2
  direction = u
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -297.647
  element_angle_cos = 0.6
  element_angle_sin = -0.8
  
[HDC_pkg2_back_plane09_v]
  plane = 9
  package = 2
  direction = v
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -295.667
  element_angle_cos = 0.6
  element_angle_sin = 0.8
  
[HDC_pkg2_back_plane10_x]
  active = false
  plane = 10
  package = 2
  direction = x
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -293.687
  element_angle_cos = 1
  element_angle_sin = 0.001

[HDC_pkg2_back_plane11_u]
  plane = 11
  package = 2
  direction = u
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -291.707
  element_angle_cos = 0.6
  element_angle_sin = -0.8

[HDC_pkg2_back_plane12_v]
  plane = 12
  package = 2
  direction = v
  detector_origin_x = -0.141
  detector_origin_y = 54.872
  detector_origin_z = -289.727
  element_angle_cos = 0.6
  element_angle_sin = 0.8
