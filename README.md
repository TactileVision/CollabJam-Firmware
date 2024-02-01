# Setup

- Specify the board used in your setup with a debug flag in `platformio.ini` file

```
-DBOARD_RIA   // Octopus (GPIO)
-DBOARD_ZSOLT // PCB from Linguwerk (GPIO)
-DBOARD_LNDW  // Headband construction used in LNDW 2022 (GPIO)
-DBOARD_WHWS  // Collabjam Version 1 (GPIO)
-DBOARD_CJ2   // Collabjam Verison 2 (DA7280)
-DDRVS        // DRV2605L based systems like the Navibration headband
	-TD_PCA   // Use PCA9547 multiplexer (otherwise TCA9548A)
``` 