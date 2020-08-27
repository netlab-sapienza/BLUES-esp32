BE Mesh: A Bluetooth Low Energy Mesh Network
===================================

<img align="left" src="https://www.uniroma1.it/sites/default/files/images/logo/sapienza-big.png"/>
<img align="right" width="25%" src="http://www.cs.ucc.ie/wowmom2020/WoWMoM%202016_files/logo-wowmom.png"/>
<br/><br/><br/><br/><br/>

***Emanuele Giacomini∗, Francesco D’Alterio∗, Andrea Lacava∗, Francesca Cuomo∗***

***∗University of Rome “La Sapienza”, 00184, ITALY***

*Abstract* - The introduction of new key features into the core specification of Bluetooth Low Energy (BLE) increased its potentialities, thus paving the way to the development of new networking paradigms.
The ability for a single node to participate in more than a piconet and to assume both the role of master and slave makes it possible the formation of multihop networks that can be used in several emerging application scenarios.
Additionally, the inherent low power consumption at the cost of contained throughput makes this technology appealing for Internet of Things (IoT), where power memory and capacity constrained devices exchange messages at relatively low data rates.
In this paper, we devise a two layers BLE mesh-based networking paradigm obtained by generalizing Android BEMESH for hardware-independent sensor networks.
Each node enforces a plug-and-play architecture which makes it able to autonomously switch between client and server role, discover and connect to existing scatternets and relay messages through them, making the network able to extend and self re-organize in a distributed fashion.
To have our implementation ready for IoT systems we based it on the ESP32 off-the-shelf board.
We describe both the implemented functions as well as some practical results proving the effectiveness of the framework in some tested configurations.

# Create a BLUES mesh network!

Clone the __BLUES__ Repository first:

__In Linux Terminal__
```
    git clone https://github.com/BE-Mesh/BLUES-esp32.git
    cd BLUES-esp32/
```
Compile the project:

__In Linux Terminal__
```
    idf.py build
```
Once the compilation process ends, you can upload the compiled code inside the ESP32 board.

__In Linux Terminal__
```
    idf.py -p /dev/ttyUSBx flash
```
Replace _ttyUSBx_ with the device on which the ESP32 is connected.
Repeat the same process on all the required devices, then open the serial monitor of the connected ESP32 to display its routing table

__In Linux Terminal__
```
    idf.py -p /dev/ttyUSBx monitor
```
Replace _ttyUSBx_ with the device on which the ESP32 is connected.

## Pre-requisites

- Two or more ESP32 boards
- Download the ESP-IDF Official Development framework [here](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html#step-2-get-esp-idf)
