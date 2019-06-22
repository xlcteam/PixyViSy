PixyViSy
========

An implementation of the [ViSy system/algorithm](http://xlc-team.info/visy) on
Arduino using Pixy (CMUcam 5).


Installation
------------

Download the PixyViSy library and put it into the `sketchbook/libraries` folder
of your Arduino installation.

In UNIX like environment you can do something like this, provided that you have
the `git` application installed:


        ~/sketchbook/libraries/$ git clone --depth 1 git@github.com:xlcteam/PixyViSy.git


You also need to download and install these Arduino libraries:
 * [Pixy](https://pixycam.com/downloads-pixy1/)
 * [XLCPixy](https://github.com/xlcteam/XLCPixy)


Model
-----

We use the pinhole camera model and two intrinsic parameters representing focal
length in terms of pixel for each axis. See
[Pinhole camera model](https://en.wikipedia.org/wiki/Pinhole_camera_model)
and [Camera resectioning](https://en.wikipedia.org/wiki/Camera_resectioning).
The *Calibration* example should be used to determine intrinsic parameters.


Algorithm
---------

Main function is divided into three independent parts:
* **Processing ball**
  The biggest blob of defined signature and minimal area is taken into account.
  Then distance and angle are computed using the model.
* **Processing goal**
  The biggest three blobs of defined signature and minimal area are taken
  into account. All blob's pixels on left/right side are sumed up apart.
  Distance to goal is calculated from goal's real and pixel height (mean of
  all blob's heights). The resulting action depends on whether kicked ball
  (straight kick is supposed) will fit to any blob. If not, the action is
  rotating to side with more sumed up goal pixels.
* **Processing teammate**
  The biggest blob of defined signature and minimal area is taken into account.
  Then distance, difference in the x axis and angle are computed using
  the model.
