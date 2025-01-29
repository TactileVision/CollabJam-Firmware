# CollabJam: Firmware

This repository contains the firmware necessary to control and interact with the tactile display Printed Circuit Board (PCB). 


## Repositories

Below are links to the individual repositories associated with the project:

- ⭐️ [Meta Repository](https://github.com/TactileVision/CollabJam) - The main repository.
- 🖥️ [Client App Repository](https://github.com/TactileVision/CollabJam-Client) - The main client application used for interacting with and designing vibrotactile experiences collaboratively.
- ⚙️ [Server Repository](https://github.com/TactileVision/CollabJam-Server) - The server application used for collaborative designing, shared rendering of vibrotactile patterns, and storing patterns in a database.
- ⚙️ [Share Components Repository](https://github.com/TactileVision/CollabJam-Shared) - Components which are shared across the server and client application.
- 🕹️ [Hardware Repository](https://github.com/TactileVision/CollabJam-PCB) - Detailed schematics and designs for the tactile display.
- 📈 [Data Visualization Repository](https://github.com/TactileVision/CollabJam-Tacton-Table-Tool) - Tool for visualizing vibrotactile patterns collected during the study sessions.


## Citation

If you use this project in your research, please cite it as follows:

```bibtex
@inproceedings{Wittchen2025CollabJam,
    author = {Wittchen, Dennis and Ramian, Alexander and Sabnis, Nihar and Chlebowski, Christopher and Böhme, Richard and Freitag, Georg and Fruchard, Bruno and Degraen, Donald},
    title = {CollabJam: Studying Collaborative Haptic Experience Design for On-Body Vibrotactile Patterns},
    year = {2025},
    publisher = {Association for Computing Machinery},
    address = {New York, NY, USA},
    booktitle = {Proceedings of the 2025 CHI Conference on Human Factors in Computing Systems},
    location = {Yokohama, Japan},
    series = {CHI '25},
    doi = {10.1145/3706598.3713469}
}
```


## Setup

- Specify the board used in your setup with a debug flag in `platformio.ini` file

```
-DBOARD_CJ1   // Collabjam Version 1 (GPIO)
-DBOARD_CJ2   // Collabjam Verison 2 (DA7280)
-DBOARD_CJ3   // DRV2605L based systems like the Navibration headband
	-TD_PCA     // Use PCA9547 multiplexer (otherwise TCA9548A)
-DBOARD_CJ4   // Collabjam Verison 4 (DA7282 + M5Stack Stamp S3)
```
