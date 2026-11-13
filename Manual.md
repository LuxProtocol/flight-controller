# Aerial Matrix
A Lux Protocol product

## WebApp

### Attachments
Currently supported attachments: 
| Module:               | Version:  | Address:          |
| --------------------- | --------- | ----------------- |
| LightModule           |   V1.0    |   192.168.4.1     |
| GroveAIModule         |   V1.0    |   192.168.4.10    |
| PortentaAIModule      |   V1.0    |   192.168.4.11    |
| EnvironmentalModule   |   V1.0    |   192.168.4.20    |    


## HTTP APIs

### Base URL
Accessing `http://192.168.4.1/` will respond with the Aerial Matrix WebApp

For more information on using the Aerial Matrix WebApp, navigate to [WebApp](#webapp).


### Vision APIs
#### Vision Health
Each AI module will periodically poll `http://192.168.4.1/acc/ai/health` for connection status. 

#### Vision Data
Each AI module will poll `http://192.168.4.1/acc/ai/data` with its current data (position adjustments, etc) 

#### Vision Stream
Since the main module has limited memory, streaming will occur from AI module straight to the client(s). Clients will poll `http://192.168.4.10/vision/img` (for GroveAIModule) or `http://192.168.4.11/vision/img` (for PortentaAIModule).


### Accessory APIs
#### Light Module
By accessing `http://192.168.4.1/acc/light/toggle` while a supported light module is installed, you can cycle though several light modes.

This can also be done through the WebApp as mentioned in [Attachments](#attachments) under the WebApp section.

#### Environmental Module
By accessing `http://192.168.4.20/acc/env/data` while a supported environmental module is installed, you can view the current measurements by the environmental 

The Environmental Module will periodically poll `http://192.168.4.1/acc/env/health` with its name and IP.

### Drone Control API
#### ESC Power
By accessing `http://192.168.4.1/esc/arm` or `http://192.168.4.1/esc/disarm`, you can control the status of the ESCs, allowing or preventing flight.

#### System Reboot
If you access `http://192.168.4.1/reboot`, the drone controller will start a reboot sequence. MAKE SURE THE DRONE IS NON-MOBILE WHEN REBOOTING!!!
