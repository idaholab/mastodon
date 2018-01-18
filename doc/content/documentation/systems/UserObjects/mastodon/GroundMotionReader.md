# GroundMotionReader

!syntax description /UserObjects/GroundMotionReader

## Description

The `GroundMotionReader` UserObject reads a set of ground motion files, which are of .csv format. This UserObject assumes that the first column of the file contains time, and the next three columns contain acceleration data in the X, Y and Z directions. The listing below demonstrates the `GroundMotionReader` UserObject. The `pattern` parameter describes the pattern of the filenames of the ground motions that are to be read.

!listing test/tests/userobjects/ground_motion_reader/ground_motion_reader.i block=UserObjects

!syntax parameters /UserObjects/GroundMotionReader

!syntax inputs /UserObjects/GroundMotionReader

!syntax children /UserObjects/GroundMotionReader
