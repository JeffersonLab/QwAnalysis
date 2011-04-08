!
! Qweak coordinate frames to be respected in this file
!
! Global coordinate system:
! - global origin is the center of QTOR
! - global x is beam left (this is the origin of the global azimuthal angle phi)
! - global y is vertical upwards
! - global z is downstream along the beam line
!
! Octant assignments:
! - octant 1 (9 o'clock) is the octant at global phi = 0
! - octant 3 (12 o'clock) is the octant at global phi = 90 deg = pi/2 rad
! - ...
!
! Local coordinate systems:
! - local origin is the specified position of the detector
! - local x is radially outward in the global system
! - local y is azimuthally towards increasing phi in the global system
! - local z is identical to global z
! Only in octant 1 are local x, y, z direction == global x, y, z direction!
! In octants at phi the local coordinate system is rotated over phi around z.
!
! The local system can be rotated around the local y axis such that local x
! remains in the plane of the detector (region 2 HDCs are not rotated, but
! region 3 VDCs are rotated over a *negative* angle of 24 degrees).

!
! Order of the variables has to be preserved.
!

! Fields:
!   name,
!   type, local origin in global z {REGION 3 VDCs ONLY ---- (Package 2 on MD 1),
!   global z (P2 on MD2), global z (P2 on MD3), global z (P2 on MD8), global z (P2 on MD7) ---- }, 
!   rotation around local y, spatial resolution, tracking resolution, slope matching,
!   package, region, type, direction,
!   local origin in global x {REGION 3 VDCs ONLY ---- (Package 2 on MD1),
!   x (P2 on MD2), x (P2 on MD3), x (P2 on MD8), x (P2 on MD7), ----} 
!   y {REGION 3 VDCs ONLY ---- (Package 2 on MD1), x (P2 on MD2),
!   x (P2 on MD3), x (P2 on MD8), x (P2 on MD7), ----} 
!   active width in local x, y, z,
!   wire spacing, position of first wire,
!   cos of x/u/v axis angle with respect to local x and y,
!   sin of x/u/v axis angle with respect to local x and y,
!   tilt of chamber in xy about Z axis in Degrees {REGION 3 VDCs ONLY ---- (Package 2 on MD1),
!   tilt (P2 on MD2), tilt (P2 on MD3), tilt (P2 on MD8), tilt (P2 on MD7), ----} 
!   number of wires, ID
!
! Note that the x/u/v axis is perpendicular to the wires.  Wires are strung
! at constant x/u/v.
!
! This might change, notably the treatment of up/down package could be replaced
! by the global octant number, an intermediate local origin could be shifted to
! the beam line, and the detector origin specified in that coordinate system.
!

! For region 3: package right is u, package left is d
!Yoda v
NAME=VDC_LeftFront
 drift, 443.13, 442.83, 442.83, 443.54, 443.62, -24.55, -24.45, -24.41, -24.62, -24.64, 7e-2, 9e-2, 1e-1, d, 3, d, v, 1.13, 0.87, 0.83, 0.46, 0.05, 273.74, 273.99, 273.94, 273.45, 273.42, 204.47, 53.34, 3, 0.496965, -69.9, 0.894427, 0.447214, 0.23, 0.21, 0.25, 0.17, 0.2, 279, 0
!Yoda u
NAME=VDC_LeftFront
 drift, 445.44, 445.14, 445.15, 445.85, 445.92, -24.55, -24.45, -24.41, -24.62, -24.64, 7e-2, 9e-2, 1e-1, d, 3, d, u, 1.13, 0.87, 0.82, 0.46, 0.05, 272.68, 272.94, 272.89, 272.40, 272.36, 204.47, 53.34, 3, 0.496965, -69.9, 0.894427, -0.447214, 0.23, 0.21, 0.25, 0.17, 0.2, 279, 1
!Han v
NAME=VDC_LeftBack
 drift, 496.31, 495.98, 495.99, 496.74, 496.84, -24.61, -24.52, -24.48, -24.69, -24.71, 7e-2, 9e-2, 1e-1, d, 3, d, v, 1.32, 0.99, 0.90, 0.56, 0.07, 295.57, 295.89, 295.83, 295.23, 295.14, 204.47, 53.34, 3, 0.496965, -69.5, 0.894427, 0.447214, 0.23, 0.21, 0.25, 0.17, 0.2, 279, 2
!Han u
NAME=VDC_LeftBack
 drift, 498.62, 498.29, 498.30, 499.05, 499.15, -24.61, -24.52, -24.48, -24.69, -24.71, 7e-2, 9e-2, 1e-1, d, 3, d, u, 1.32, 0.99, 0.89, 0.56, 0.06, 294.52, 294.83, 294.78, 294.17, 294.08, 204.47, 53.34, 3, 0.496965, -69.5, 0.894427, -0.447214, 0.23, 0.21, 0.25, 0.17, 0.2, 279, 3
!Vader v
NAME=VDC_RightFront
 drift, 443.45, 443.83, 443.91, 443.16, 442.98, -24.33, -24.37, -24.24, -24.21, -24.2, 7e-2, 9e-2, 1e-1, u, 3, d, v, -0.08, -0.10, 0.45, -0.63, -0.60, 272.78, 272.44, 272.39, 273.11, 273.15, 204.47, 53.34, 3, 0.496965, -69.5, -0.894427, -0.447214, 0.16, 0.00, 0.0, 0.05, 0.15, 279, 4
!Vader u
NAME=VDC_RightFront
 drift, 445.77, 446.15, 446.23, 445.48, 445.29, -24.33, -24.37, -24.24, -24.21, -24.2, 7e-2, 9e-2, 1e-1, u, 3, d, u, -0.08, -0.09, 0.46, -0.62, -0.59, 271.73, 271.40, 271.34, 272.07, 272.11, 204.47, 53.34, 3, 0.496965, -69.5, -0.894427, 0.447214, 0.16, 0.00, 0.0, 0.05, 0.15, 279, 5
!Leia v
NAME=VDC_RightBack
 drift, 496.51, 496.90, 496.93, 496.17, 495.98, -24.41, -24.46, -24.33, -24.50, -24.28, 7e-2, 9e-2, 1e-1, u, 3, d, v, -0.02, -0.04, 0.58, -0.61, -0.57, 294.80, 294.43, 294.49, 295.24, 295.29, 204.47, 53.34, 3, 0.496965, -69.9, -0.894427, -0.447214, 0.16, 0.00, 0.0, 0.05, 0.15, 279, 6
!Leia u
NAME=VDC_RightBack
 drift, 498.82, 499.21, 499.24, 498.49, 498.29, -24.41, -24.46, -24.33, -24.50, -24.28, 7e-2, 9e-2, 1e-1, u, 3, d, u, -0.02, -0.04, 0.59, -0.61, -0.56, 293.75, 293.38, 293.45, 294.19, 294.25, 204.47, 53.34, 3, 0.496965, -69.9, -0.894427, 0.447214, 0.16, 0.00, 0.0, 0.05, 0.15, 279, 7


NAME=TS_Left
 scint, 539.74, 0, -9999, -9999, -9999, u, 4, t, y, 0, 317.34, 218.45, 30.48, 1.0, -9999, -9999, -9999, -9999, -9999, -9999, 8
NAME=TS_Right
 scint, 539.74, 0, -9999, -9999, -9999, d, 4, t, y, 0, -317.34, 218.45, 30.48, 1.0, -9999, -9999, -9999, -9999, -9999, -9999, 9

!NAME=MD1
! cerenkov, 581.665, 0, -9999, -9999, -9999, u, 5, c, y, 0, 334.724, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 10
!NAME=MD2
! cerenkov, 576.705, 0, -9999, -9999, -9999, d, 5, c, y, 0, 334.742, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 11
NAME=MD3
 cerenkov, 577.020, 0, -9999, -9999, -9999, u, 5, c, y, 0, 334.738, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 10
!NAME=MD4
! cerenkov, 577.425, 0, -9999, -9999, -9999, d, 5, c, y, 0, 335.036, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 11
!NAME=MD5
! cerenkov, 582.515, 0, -9999, -9999, -9999, u, 5, c, y, 0, 335.074, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 10
!NAME=MD6
! cerenkov, 577.955, 0, -9999, -9999, -9999, d, 5, c, y, 0, 335.252, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 11
NAME=MD7
 cerenkov, 577.885, 0, -9999, -9999, -9999, u, 5, c, y, 0, 335.224, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 10
!NAME=MD8
! cerenkov, 577.060, 0, -9999, -9999, -9999, d, 5, c, y, 0, 334.888, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -99, -9999, 11

NAME=HDC
 drift, -338.100, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 12
NAME=HDC
 drift, -336.122, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 13
NAME=HDC
 drift, -334.144, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 14
NAME=HDC
 drift, -332.166, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 15
NAME=HDC
 drift, -330.188, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 16
NAME=HDC
 drift, -328.210, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 17
NAME=HDC
 drift, -338.100, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 18
NAME=HDC
 drift, -336.122, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 19
NAME=HDC
 drift, -334.144, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 20
NAME=HDC
 drift, -332.166, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 21
NAME=HDC
 drift, -330.188, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6,-0.8, 0, 32, 22
NAME=HDC
 drift, -328.210, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 23
NAME=HDC
 drift, -295.400, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 24
NAME=HDC
 drift, -293.422, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 25
NAME=HDC
 drift, -291.444, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 26
NAME=HDC
 drift, -289.466, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 27
NAME=HDC
 drift, -287.488, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 28
NAME=HDC
 drift, -285.510, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 29
NAME=HDC
 drift, -295.400, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 30
NAME=HDC
 drift, -293.422, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 31
NAME=HDC
 drift, -291.444, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 32
NAME=HDC
 drift, -289.466, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 0, 32, 33
NAME=HDC
 drift, -287.488, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 0, 32, 34
NAME=HDC
 drift, -285.510, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 0, 32, 35

!GEM Z location: -558 cm (in actual Qweak), -542.2 cm (in MC simulation, front chamber), -534.0 cm (in MC simulation, back chamber), chamber thickness (unknown)
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, u, 1, g, r, 0, 19.19, 12.0, 24.42, 1.0, 0.477886, 68.1, -9999, -9999, 0, 512, 36
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, u, 1, g, y, 0, 19.19, 12.0, 15.585, 1.0, 0.31373, -40.0, -9999, -9999, 0, 256, 37
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, d, 1, g, r, 0, -19.19, 12.0, 24.42, 1.0, 0.477886, -68.1, -9999, -9999, 0, 512, 38
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, d, 1, g, y, 0, -19.19, 12.0, 15.585, 1.0, 0.31373, 40.0, -9999, -9999, 0, 256, 39

! sType, Zpos, Rot, Spatial Res., Track Res, slope matching, up/low, region, type, plane dir, detector originx, y, active width x, y, z, wire spacing, 1st wire pos., rcos of wire oirentation w.r.t x, rsin, no.of wires, ID

!END
