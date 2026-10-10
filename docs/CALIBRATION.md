# Detector calibration

[Back to the project overview](../README.md)

The [sketch](../firmware/MinesweeperMega.ino) sets its baseline from 100 readings at startup. It reports a detection when the absolute difference from that baseline reaches `DET_DELTA`, currently **80 ADC counts**.

1. Disconnect motor power and keep the coil away from metal targets. Power the controller and let startup calibration finish.
2. Watch the ADC readings on the LCD or Bluetooth terminal and record their normal variation.
3. Bring ordinary metal targets toward the coil. Record the ADC change, target distance, and repeatability.
4. Choose a `DET_DELTA` above normal noise that still detects the intended test targets. Update the constant in the sketch and upload it.
5. Restart away from metal and repeat the tests. The baseline is recalculated at each startup; it does not adapt continuously.
6. In a controlled drive test, check whether motor activity causes false alarms. Keep the physical power disconnect accessible.

The threshold depends on the detector and test conditions. These tests demonstrate metal detection, not explosive identification or clearance capability. Ultrasonic readings are telemetry only; the sketch does not use them for obstacle avoidance.
