# Setup

- Specify the board used in your setup with a debug flag in `platformio.ini` file

```
-DBOARD_CJ1   // Collabjam Version 1 (GPIO)
-DBOARD_CJ2   // Collabjam Verison 2 (DA7280)
-DBOARD_CJ3   // DRV2605L based systems like the Navibration headband
	-TD_PCA     // Use PCA9547 multiplexer (otherwise TCA9548A)
-DBOARD_CJ4   // Collabjam Verison 4 (DA7282 + M5Stack Stamp S3)
```
