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
 drift, 450.188, -24.451, 1e-2, 1e-1, 10e-1, d, 3, d, v, 0, 270.105, 204.47, 53.34, 2.54, 0.496965, -69.5751,  0.89442759, 0.447212797, 279, 0
NAME=VDC_LeftFront
 drift, 451.098, -24.451, 1e-2, 1e-1, 10e-1, d, 3, d, u, 0, 270.519, 204.47, 53.34, 2.54, 0.496965, -69.5751, 0.89442759, -0.447212797, 279, 1
NAME=VDC_LeftBack
 drift, 503.359, -24.588, 1e-2, 1e-1, 10e-1, d, 3, d, v, 0, 291.932, 204.47, 53.34, 2.54, 0.496965, -69.5751,  0.89442759, 0.447212797, 279, 2
NAME=VDC_LeftBack
 drift, 504.269, -24.588, 1e-2, 1e-1, 10e-1, d, 3, d, u, 0, 292.346, 204.47, 53.34, 2.54, 0.496965, -69.5751, 0.89442759, -0.447212797, 279, 3
NAME=VDC_RightFront
 drift, 450.627, -24.305, 1e-2, 1e-1, 10e-1, u, 3, d, v, 0, 269.171, 204.47, 53.34, 2.54, 0.496965, -69.5751, -0.89442759, -0.447212797, 279, 4
NAME=VDC_RightFront
 drift, 451.537, -24.305, 1e-2, 1e-1, 10e-1, u, 3, d, u, 0, 269.585, 204.47, 53.34, 2.54, 0.496965, -69.5751,  -0.89442759, 0.447212797, 279, 5
NAME=VDC_RightBack
 drift, 503.672, -24.502, 1e-2, 1e-1, 10e-1, u, 3, d, v, 0, 291.183, 204.47, 53.34, 2.54, 0.496965, -69.5751, -0.89442759, -0.447212797, 279, 6
NAME=VDC_RightBack
 drift, 504.582, -24.502, 1e-2, 1e-1, 10e-1, u, 3, d, u, 0, 291.597, 204.47, 53.34, 2.54, 0.496965, -69.5751,  -0.89442759, 0.447212797, 279, 7

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
 drift, -341.749, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, 1, 0, 32, 12
NAME=HDC
 drift, -339.844, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, 0.6, -0.8, 32, 13
NAME=HDC
 drift, -337.939, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.1299, 0.6, 0.8, 32, 14
NAME=HDC
 drift, -336.034, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, 1, 0, 32, 15
NAME=HDC
 drift, -334.129, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, 0.6, -0.8, 32, 16
NAME=HDC
 drift, -332.224, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.131, 0.6, 0.8, 32, 17
NAME=HDC
 drift, -341.749, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, -1, 0, 32, 18
NAME=HDC
 drift, -339.844, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, -0.6, 0.8, 32, 19
NAME=HDC
 drift, -337.939, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.123, -0.6, -0.8, 32, 20
NAME=HDC
 drift, -336.034, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, -1, 0, 32, 21
NAME=HDC
 drift, -334.129, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, -0.6, 0.8, 32, 22
NAME=HDC
 drift, -332.224, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 50.8216, 28.575, 38.10, -9999, 1.1684, -18.123, -0.6, 0.8, 32, 23
NAME=HDC
 drift, -299.435, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, 1, 0, 32, 24
NAME=HDC
 drift, -297.530, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, 0.6, -0.8, 32, 25
NAME=HDC
 drift, -295.625, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.123, 0.6, 0.8, 32, 26
NAME=HDC
 drift, -293.720, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, x, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, 1, 0, 32, 27
NAME=HDC
 drift, -291.815, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, u, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, 0.6, -0.8, 32, 28
NAME=HDC
 drift, -289.910, 0, 1e-2, 0.5e-1, 1e-2, u, 2, d, v, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.123, 0.6, 0.8, 32, 29
NAME=HDC
 drift, -299.435, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, -1, 0, 32, 30
NAME=HDC
 drift, -297.530, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, -0.6, 0.8, 32, 31
NAME=HDC
 drift, -295.625, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.123, -0.6, -0.8, 32, 32
NAME=HDC
 drift, -293.720, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, x, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.19988, -1, 0, 32, 33
NAME=HDC
 drift, -291.815, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, u, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.1273, -0.6, 0.8, 32, 34
NAME=HDC
 drift, -289.910, 0, 1e-2, 0.5e-1, 1e-2, d, 2, d, v, 0, 54.8216, 28.575, 38.10, -9999, 1.1684, -18.123, -0.6, -0.8, 32, 35

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
