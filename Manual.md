# Aerial Matrix
A Lux Protocol product

## WebApp

### Attachments
Currently supported attachments: 
- LightModule       V1.0      (ID: 11)
- GroveAIModule     V1.0      (ID: 21)
- PortentaAIModule  V1.0      (ID: 22)


## HTTP APIs

### Base URL
Accessing `http://{AerialMatrixIP}/` will respond with the Aerial Matrix WebApp

For more information on using the Aerial Matrix WebApp, navigate to [WebApp](#webapp).


### Vision APIs
#### Vision Health
Each AI module will periodically poll `http://{AerialMatrixIP}/acc/ai/health` with its name and IP. 

#### Vision Data
Each AI module will poll `http://{AerialMatrixIP}/acc/ai/data` with its current data (position adjustments, etc) 

#### Vision Stream
Since the main module has limited memory, streaming will occur from AI module straight to the client(s). Clients will poll `http://{VisionModuleIP}/vision/img`


### Accessory APIs
#### Light Module
By accessing `http://{AerialMatrixIP}/acc/light/toggle` while a supported light module is installed, you can cycle though several light modes.

This can also be done through the WebApp as mentioned in [Attachments](#attachments) under the WebApp section.

#### Environmental Module
By accessing `http://{AerialMatrixIP}/acc/env/data` while a supported environmental module is installed, you can view the current measurements by the environmental 

The Environmental Module will periodically poll `http://{AerialMatrixIP}/acc/env/health` with its name and IP.

### Drone Control API
#### ESC Power
By accessing `http://{AerialMatrixIP}/esc/arm` or `http://{AerialMatrixIP}/esc/disarm`, you can control the status of the ESCs, allowing or preventing flight.

#### System Reboot
If you access `http://{AerialMatrixIP}/reboot`, the drone controller will start a reboot sequence. MAKE SURE THE DRONE IS NON-MOBILE WHEN REBOOTING!!!
