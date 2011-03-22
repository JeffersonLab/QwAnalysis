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
!   type, local origin in global z, rotation around local y,
!   spatial resolution, tracking resolution, slope matching,
!   package, region, type, direction,
!   local origin in global x, y,
!   active width in local x, y, z,
!   wire spacing, position of first wire,
!   cos of x/u/v axis angle with respect to local x and y,
!   sin of x/u/v axis angle with respect to local x and y,
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

NAME=VDC_LeftFront
 drift, 444.062, -24.451, 7e-2, 9e-2, 1e-1, d, 3, d, v, 0, 275.236, 204.47, 53.34, 2.54, 0.496965, -69.5,  0.89442759, 0.447212797, 279, 0
NAME=VDC_LeftFront
 drift, 446.377, -24.451, 7e-2, 9e-2, 1e-1, d, 3, d, u, 0, 274.191, 204.47, 53.34, 2.54, 0.496965, -69.5, 0.89442759, -0.447212797, 279, 1
NAME=VDC_LeftBack
 drift, 496.683, -24.588, 7e-2, 9e-2, 1e-1, d, 3, d, v, 0, 296.281, 204.47, 53.34, 2.54, 0.496965, -69.5,  0.89442759, 0.447212797, 279, 2
NAME=VDC_LeftBack
 drift, 498.997, -24.588, 7e-2, 9e-2, 1e-1, d, 3, d, u, 0, 295.236, 204.47, 53.34, 2.54, 0.496965, -69.5, 0.89442759, -0.447212797, 279, 3
NAME=VDC_RightFront
 drift, 445.199, -24.305, 7e-2, 9e-2, 1e-1, u, 3, d, v, 0, 275.045, 204.47, 53.34, 2.54, 0.496965, -69.5, -0.89442759, -0.447212797, 279, 4
NAME=VDC_RightFront
 drift, 447.514, -24.305, 7e-2, 9e-2, 1e-1, u, 3, d, u, 0, 273.999, 204.47, 53.34, 2.54, 0.496965, -69.5,  -0.89442759, 0.447212797, 279, 5
NAME=VDC_RightBack
 drift, 497.329, -24.502, 7e-2, 9e-2, 1e-1, u, 3, d, v, 0, 296.032, 204.47, 53.34, 2.54, 0.496965, -69.5, -0.89442759, -0.447212797, 279, 6
NAME=VDC_RightBack
 drift, 499.643, -24.502, 7e-2, 9e-2, 1e-1, u, 3, d, u, 0, 294.987, 204.47, 53.34, 2.54, 0.496965, -69.5,  -0.89442759, 0.447212797, 279, 7
// NAME=VDC_RightBack
// drift, 504.582, -24.502, 1e-2, 1e-1, 4e-1, u, 3, d, u, 0, 291.983, 204.47, 53.34, 2.54, 0.496965, -74,  -0.89442759, 0.447212797, 279, 7


NAME=TS_Left
 scint, 539.74, 0, -9999, -9999, -9999, u, 4, t, y, 0, 317.34, 218.45, 30.48, 1.0, -9999, -9999, -9999, -9999, -9999, 8
NAME=TS_Right
 scint, 539.74, 0, -9999, -9999, -9999, d, 4, t, y, 0, -317.34, 218.45, 30.48, 1.0, -9999, -9999, -9999, -9999, -9999, 9

!NAME=MD1
! cerenkov, 581.665, 0, -9999, -9999, -9999, u, 5, c, y, 0, 334.724, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 10
!NAME=MD2
! cerenkov, 576.705, 0, -9999, -9999, -9999, d, 5, c, y, 0, 334.742, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 11
NAME=MD3
 cerenkov, 577.020, 0, -9999, -9999, -9999, u, 5, c, y, 0, 334.738, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 10
!NAME=MD4
! cerenkov, 577.425, 0, -9999, -9999, -9999, d, 5, c, y, 0, 335.036, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 11
!NAME=MD5
! cerenkov, 582.515, 0, -9999, -9999, -9999, u, 5, c, y, 0, 335.074, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 10
!NAME=MD6
! cerenkov, 577.955, 0, -9999, -9999, -9999, d, 5, c, y, 0, 335.252, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 11
NAME=MD7
 cerenkov, 577.885, 0, -9999, -9999, -9999, u, 5, c, y, 0, 335.224, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 10
!NAME=MD8
! cerenkov, 577.060, 0, -9999, -9999, -9999, d, 5, c, y, 0, 334.888, 200.0, 18.0, 1.25, -9999, -9999, -9999, -9999, -9999, 11

NAME=HDC
 drift, -338.100, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 12
NAME=HDC
 drift, -336.122, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 13
NAME=HDC
 drift, -334.144, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 14
NAME=HDC
 drift, -332.166, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 15
NAME=HDC
 drift, -330.188, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 16
NAME=HDC
 drift, -328.210, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 17
NAME=HDC
 drift, -338.100, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 18
NAME=HDC
 drift, -336.122, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 19
NAME=HDC
 drift, -334.144, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 20
NAME=HDC
 drift, -332.166, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 51.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 21
NAME=HDC
 drift, -330.188, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6,- 0.8, 32, 22
NAME=HDC
 drift, -328.210, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 23
NAME=HDC
 drift, -295.400, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 24
NAME=HDC
 drift, -293.422, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 25
NAME=HDC
 drift, -291.444, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 26
NAME=HDC
 drift, -289.466, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 27
NAME=HDC
 drift, -287.488, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 28
NAME=HDC
 drift, -285.510, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 29
NAME=HDC
 drift, -295.400, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 30
NAME=HDC
 drift, -293.422, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 31
NAME=HDC
 drift, -291.444, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 32
NAME=HDC
 drift, -289.466, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 55.013, 28.575, 37.39, -9999, 1.1684, -19.2786, 1, 0, 32, 33
NAME=HDC
 drift, -287.488, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.711, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, -0.8, 32, 34
NAME=HDC
 drift, -285.510, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.955, 28.575, 37.39, -9999, 1.1684, -19.2786, 0.6, 0.8, 32, 35

!GEM Z location: -558 cm (in actual Qweak), -542.2 cm (in MC simulation, front chamber), -534.0 cm (in MC simulation, back chamber), chamber thickness (unknown)
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, u, 1, g, r, 0, 19.19, 12.0, 24.42, 1.0, 0.477886, 68.1, -9999, -9999, 512, 36
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, u, 1, g, y, 0, 19.19, 12.0, 15.585, 1.0, 0.31373, -40.0, -9999, -9999, 256, 37
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, d, 1, g, r, 0, -19.19, 12.0, 24.42, 1.0, 0.477886, -68.1, -9999, -9999, 512, 38
NAME=GEM
 drift, -542.2, 0, 0.4, 0.4, -9999, d, 1, g, y, 0, -19.19, 12.0, 15.585, 1.0, 0.31373, 40.0, -9999, -9999, 256, 39

! sType, Zpos, Rot, Spatial Res., Track Res, slope matching, up/low, region, type, plane dir, detector originx, y, active width x, y, z, wire spacing, 1st wire pos., rcos of wire oirentation w.r.t x, rsin, no.of wires, ID

!END
