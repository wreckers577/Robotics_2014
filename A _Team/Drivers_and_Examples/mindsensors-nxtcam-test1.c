#pragma config(Sensor, S1,     cam,                 sensorI2CCustomFastSkipStates)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * $Id: mindsensors-nxtcam-test1.c 133 2013-03-10 15:15:38Z xander $
 */

/**
 * mindsensors-nxtcam.h provides an API for the Mindsensors NXTCam.  This program
 * demonstrates how to use that API.
 *
 * Changelog:
 * - 0.1: Initial release
 * - 0.2: More comments
 *
 * Credits:
 * - Gordon Wyeth for writing the original driver and cam_display program
 *
 * License: You may use this code as you wish, provided you give credit where it's due.
 *
 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 3.59 AND HIGHER. 

 * Xander Soldaat (xander_at_botbench.com)
 * 28 November 2009
 * version 0.2
 */

#include "drivers/mindsensors-nxtcam.h"

// int xscale(int x) - Scales x values from camera coordinates to screen coordinates.
int xscale(int x) {
  return ((x - 12) * 99) / 175;
}

// int yscale(int y) - Scales y values from camera coordinates to screen coordinates.
int yscale(int y) {
  return ((143 - y) * 63) / 143;
}

/*
 Main task
 */
task main () {
  blob_array _blobs;
  // combine all colliding blobs into one
  bool _condensed = true;

  //blob_array _blobs;
  int _l, _t, _r, _b;
  int _nblobs;
  eraseDisplay();

  // Initialise the camera
  NXTCAMinit(cam);
  while(true) {
    eraseDisplay();
    // Fetch all the blobs, have the driver combine all
    // the colliding blobs.
    _nblobs = NXTCAMgetBlobs(cam, _blobs, _condensed);
    for (int i = 0; i < _nblobs; i++) {
      // Draw the scaled blobs
      _l = xscale(_blobs[i].x1);
      _t = yscale(_blobs[i].y1);
      _r = xscale(_blobs[i].x2);
      _b = yscale(_blobs[i].y2);
      nxtFillRect(_l, _t, _r, _b);
    }

    nxtDisplayTextLine(1, "%d", _nblobs);
    wait1Msec(100);
  }
}

/*
 * $Id: mindsensors-nxtcam-test1.c 133 2013-03-10 15:15:38Z xander $
 */
