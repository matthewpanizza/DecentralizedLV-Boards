# DecentralizedLV_Boards

This repository contains a submodule designed to facilitate communication between various hardware components in a Decentralized Low Voltage (DecentralizedLV) system using CAN Bus. Each class in this submodule represents a specific hardware component and its capabilities, allowing for structured and efficient data exchange between different PCBs (Printed Circuit Boards).

## About this Module

This software is part of the Decentralized Low Voltage system and is meant to be an abstraction method to make communication between boards in the system simpler. It also has a class for a platform-agnostic CAN Bus controller (at least for Particle based boards). 

### Benefits of using this Module
- **Platform-independence**: the platform-agnostic CAN controller allows for portability across hardware platforms (Photon, P2, etc)
- **Code Maintenance**: updates to the DecentralizedLV CAN Bus encoding on one board can be pulled down on the other boards for easy updating. For example, if a change was made to add a ```uint8_t temperature;``` field on the Dashboard Controller and the Power Controller needed to update to use this new field, a ```git pull``` could be done on the submodule in the Power Controller software to get the latest version of the message formatting.
- **Ease of Use**: Encoding of the fields (```rightTurnPWM```, ```headlight```, ```highbeam```, etc) is handled under the hood in the classes. The main PCB software (for Dashboard Controller, Power Controller, LPDRV) only needs to read or write to the fields and then call ```transmit()```.

### Fundamental Philosophy

In the software, there will be a class to represent each PCB and its capabilities (i.e. a Low Power Output, an ADC reading, the power state such as Acc, Ign, etc). This class will then have a method for encoding and decoding the data into the 64 bits provided by a CAN Bus message. In the software of the board that the class represents (i.e. the ```DashController_CAN``` class in the Dashboard Controller software), the software will populate the variables, such as the ```DashController_CAN.headlight``` flag from the sense pin, and then transmit out the message on the CAN Bus with the agreed-upon format. Then, any board that wishes to receive from the Dashboard Controller would create an instance of the ```DashController_CAN``` class and pass in the received message. On the consumer, the ```DashController_CAN.headlight``` flag would then represent the value the Dash Controller transmitted. See the [Board Specific Classes](#board-specific-classes) section for information about what each board has for capabilities. The [Examples](#example-usage) section also has a snippet program for illustrating how to instantiate and use the various classes in this program.

### Platform-Agnostic CAN Controller

The boards in the Decentralized Low Voltage system use different hardware for communicating using CAN Bus. On the Photon-equipped platforms (Dashboard Controller, LPDRV, Sense), the integrated CAN controller in the STM32 is used. P2-equipped platforms (Power Controller, HV Controller, ULPDRV) do not have this, so they need an MCP2515 CAN controller. To overcome this difference, this submodule has an included ```CAN_Controller``` class which automatically calls the CAN functions based on the platform you are running on. The [Examples](#example-usage) section shows how a ```CAN_Controller``` can be instantiated for both flavors of hardware.

## Using this Module

Before you can install this module, ensure that you have your Particle development environment set up in Visual Studio Code, you have the project you wish to install it on open, and that you have logged into Particle in Visual Studio Code.

Once your project is open, make sure it has the MCP_CAN_RK library installed, as this contains the necessary drivers for decoding the CAN Bus speed and controlling the MCP2515. This can be done by pressing Ctrl+Shift+P and choosing ```Particle: Install Library```.

<image src="Pictures/Particle install library.png" width="50%">

It will ask for the library name. Type ```MCP_CAN_RK```.

<image src="Pictures/MCP_CAN_RK.png" width="50%">

After pressing enter, the library will begin install. After installation is complete, it should show up under the ```lib``` folder in the project explorer.

With the MCP_CAN_RK library installed, you will now need to install the submodule to your existing Particle project using git. [TortoiseGit](https://tortoisegit.org/) is a handy graphical tool for managing Git repositories and has great functionality for this. I will be using this and [Visual Studio Code](https://code.visualstudio.com/) for demonstrating installation. To start, open your file explorer and navigate to where you have the project downloaded and go into the ```src``` folder. It is important that you install it in ```src```, as this is where Particle compiles files from. Right click (and show more options on Windows 11) and choose "Submodule Add" from the list.

<image src="Pictures/TortoiseGit Submodule Add.png" width="75%">

Then another window will pop up and ask you where you want to clone the submodule from. You will need to point it at the repository where you cloned this program from.

<image src="Pictures/TortoiseGit Submodule Interface.png" width="50%">

Press "OK" to add this submodule, and Git will clone down a copy into ```src```. Going back into VS Code, your project should look something like this:

<image src="Pictures/MCP Library Installed.png" width="100%">

The submodule will show up  with the blue highlighting in the file explorer as a folder under ```src```. Now with this submodule added, we need to push up to the repository of your existing project (DecentralizedLV-Template in my instance) to tell it that you have linked a submodule. In VS Code, go to the Git management tab on the left bar. It should show a staged change for adding the submodule. Type a basic comment like ```Added DecentralizeLV-Boards submodule``` and commit and sync the changes. 

<image src="Pictures/Submodule initial commit in VS.png" width="100%">

At this point, your project on GitHub should reference the submodule and have its changes tracked. Notice how there are now two projects shown in the Git pane, one for the submodule and one for the main project. If you make changes to the submodule, then you can commit them up to the submodule repository and then ```git pull``` the changes down on other projects that use the submodule. Making changes to the submodule will also require a commit on the main project, as a submodule is a _reference_ to a particular commit in Git's eyes. Push changes to the submodule first before doing commits to your main project.

## Generic CAN Bus Classes

Below is an explanation of the classes in this submodule meant for handling CAN Bus communication using the platform-agnostic CAN_Controller class. In the [Adding Boards to the API](#adding-boards-to-the-api) section I have example code for creating these new classes in the source files.

### `LV_CANMessage`
A generic CAN bus message class with address and data fields. This is used to transmit and receive from a `CAN_Controller` object.

### `CAN_Controller`
Class to represent a hardware CAN Bus controller which can transmit/receive CAN Bus messages. This class has support for both the integrated CAN Bus controller on the Particle Photon or using a MCP2515 attached using SPI. Since the MCP uses SPI for communication, you will need to specify which pin is uses for Chip Select (CS). On non-Photon platforms, this class can also be instantiated multiple times with multiple CAN Controllers. 

#### Functions
- ```addFilter```: Sets the CAN Bus controller to only receive on certain addresses. Call this multiple times for each address you wish to receive from. There are maximums for the number of filters you can have. Check the [Particle Photon](https://docs.particle.io/reference/device-os/firmware/#can-canbus-) and [MCP2515](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf) datasheets for this.
- ```receive```: Receives a message from the CAN Bus if there is one present. Pass in a LV_CANMessage to this function. This will then be updated to the values of the received messages. Returns a boolean indicating if a message was received.
- ```CANSend```: Sends a message on the CAN Bus. Can either send a ```LV_CANMessage``` or manually specify the address and data bytes.
- ```changeCANSpeed```: Reinitializes the CAN Bus controller at the specified speed.
- ```CurrentBaudRate```: Returns the current baud rate of the CAN Bus controller.
- ```begin```: Initializes the CAN Bus controller at the given speed. When using the MCP2515, this function also takes the Chip Select pin.

#### Instantiating on a Photon
```cpp
CAN_Controller canController;       // Create an instance of the CAN Controller
canController.begin(500000);        // Start CAN bus transmission at 500000kbps CAN on a Photon.

LV_CANMessage lv_message;           // Create a new CAN Message to be sent
lv_message.address = 0x100;         // Set the address to 0x100
lv_message.byte0 = 0x55;            // Set the data byte 0 to be 0x55

canController.CANSend(lv_message);  // Send out message on address 0x100 with data 0x55 0x00 0x00 0x00 0x00 0x00 0x00 0x00
```

<image src="Pictures/Analyzer Photon.png" width="75%">

#### Instantiating on a P2/Boron/Argon/Xenon/Other

```cpp
CAN_Controller canController;       // Create an instance of the CAN Controller
canController.begin(500000, A2);    // Start CAN bus transmission at 500000kbps CAN on a MCP2515.

LV_CANMessage lv_message;           // Create a new CAN Message to be sent
lv_message.address = 0x100;         // Set the address to 0x100
lv_message.byte0 = 0x55;            // Set the data byte 0 to be 0x55

canController.CANSend(lv_message);  // Send out message on address 0x100 with data 0x55 0x00 0x00 0x00 0x00 0x00 0x00 0x00
```

<image src="Pictures/Analyzer MCP.png" width="75%">

#### Dual CAN Controller

```cpp
CAN_Controller canControllerA;       // Create an instance of the CAN Controller
canControllerA.begin(500000, A2);    // Start CAN bus transmission at 500000kbps CAN on a MCP2515.

CAN_Controller canControllerB;       // Create an instance of the CAN Controller
canControllerB.begin(500000, A3);    // Start CAN bus transmission at 500000kbps CAN on a MCP2515.

LV_CANMessage lv_message;           // Create a new CAN Message to be sent
lv_message.address = 0x100;         // Set the address to 0x100
lv_message.byte0 = 0x55;            // Set the data byte 0 to be 0x55

canControllerA.CANSend(lv_message); // Send out message on address 0x100 with data 0x55 0x00 0x00 0x00 0x00 0x00 0x00 0x00 on CAN Bus A

lv_message.address = 0x200;         //Change the address to 0x200 for CAN Bus B

canControllerB.CANSend(lv_message); // Send out message on address 0x100 with data 0x55 0x00 0x00 0x00 0x00 0x00 0x00 0x00 on CAN Bus B
```

## Board-Specific Classes

Below is documentation about the different boards this submodule currently supports and what their fields do.

### `DashController_CAN`
Manages data transmission and reception for the [Dashboard Controller](https://github.com/matthewpanizza/DecentralizedLV-DashController). The Dashboard Controller takes input from the driver for items such as the headlight, highbeam, and drive gear. It still retains some of the functionality from the [DecentralizedLV Sense](https://github.com/matthewpanizza/DecentralizedLV-Sense) board for controlling the fault states and radiator.

#### Fields (Note: this may not be up to date with what is in the file!)

- **uint32_t boardAddress**      : The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
- **byte rightTurnPWM**          : Brightness of the right turn signal. Value ranges from 0 (fully off) to 255 (fully on).
- **byte leftTurnPWM**           : Brightness of the left turn signal. Value ranges from 0 (fully off) to 255 (fully on).
- **byte batteryFanPWM**         : Fan percentage for the battery box fan. Value ranges from 0 (fully off) to 255 (max speed).
- **bool headlight**             : Toggle switch for the car headlights. True turns on headlights, false turns off headlights.
- **bool highbeam**              : Toggle switch for the car highbeams. True turns on highbeams, false turns off highbeams.
- **bool reversePress**          : Toggle switch for being in reverse mode. Use to turn on/off reverse lights, backup camera, etc.
- **byte driveMode**             : The gear that the user has requested (Park, Reverse, Forward, ...). Use the macros like DRIVE_MODE_PARK, DRIVE_MODE_NORMAL, etc.
- **bool radiatorFan**           : Toggle to control the cooling fan for the motor controller.
- **bool radiatorPump**          : Toggle to control the cooling pump for the motor controller.
- **bool bmsFaultDetected**      : Flag that is set true if a Battery Management System fault has been detected.
- **bool rmsFaultDetected**      : Flag that is set true if a Motor Controller fault has been detected.
- **bool boardDetected**         : Flag set true in receiveCANData when a message from the Dash Controller has been received. Use this on other boards to check if you're hearing from the Dash Controller.

#### CAN Bus Encoding (Note: this may not be up to date with what is in the file!)
```
Address   byte 7    byte 6    byte 5    byte 4    byte 3    byte 2    byte 1    byte 0    
0x99     000000xy  000vwxyz  xxxxxxxx  00x000yz  xxxxxxxx  00000000  xxxxxxxx  xxxxxxxx

byte 0: xxxxxxxx = 8-bit representation of right turn signal PWM. 11111111 (0xFF) is full brightness, 00000000 (0x00) is off.
byte 1: xxxxxxxx = 8-bit representation of left turn signal PWM. 11111111 (0xFF) is full brightness, 00000000 (0x00) is off.
byte 2: Not used
byte 3: xxxxxxxx = 8-bit representation of battery fan PWM. 11111111 (0xFF) is full speed, 00000000 (0x00) is off.
byte 4: x        = 1-bit boolean indicating if reverse lights should be on. 1 = On, 0 = Off
        y        = 1-bit boolean indicating if highbeam should be on. 1 = On, 0 = Off
        z        = 1-bit boolean indicating if headlight should be on. 1 = On, 0 = Off
byte 5: Not used
byte 6: v        = 1-bit boolean indicating if in Neutral drive mode
        w        = 1-bit boolean indicating if in Reverse drive mode
        x        = 1-bit boolean indicating Eco modifier
        y        = 1-bit boolean indicating Sport modifier
        z        = 1-bit boolean indicating if in Forward drive mode
byte 7: x        = 1-bit boolean indicating if the radiator pump should be on
        x        = 1-bit boolean indicating if the radiator fan should be on
```

### `PowerController_CAN`
Manages data transmission and reception for the Power Controller. The Power Controller orchestrates a Push-To-Start key switch system and is responsible for determining the power state of the car. Like a traditional key switch car, there is an Accessory, Ignition and Start state of the vehicle, however this is controlled in software and transmitted over CAN instead of using busbars. Use the ```Acc```, ```Ign``` and ```FullStart``` signals to create Virtual Power Rails instead of running wires.

#### Fields (Note: this may not be up to date with what is in the file!)

- **uint32_t boardAddress**      : The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
- **bool BrakeSense**            : Flag indicating if the brake pedal is being pressed.
- **bool PushToStart**           : Flag indicating if the push to start button is being pressed.
- **bool ACCharge**              : Flag indicating if the car is being charged from the wall.
- **bool SolarCharge**           : Flag indicating if the car is in solar charge mode.
- **bool Horn**                  : Flag indicating if the horn is being pressed.
- **bool Acc**                   : Flag indicating if the car has its Accessory busbar active.
- **bool Ign**                   : Flag indicating if the car has its Ignition busbar active.
- **bool FullStart**             : Flag indicating if the car is fully started (Ign + Start)
- **bool CarOn**                 : Flag indicating if the car is in some kind of drive state
- **bool StartUp**               : Flag indicating if the Power Controller is doing a soft-start
- **bool LowPowerMode**          : Flag indicating to the rest of the system that we are operating in Low Power Mode. Use this to update controls of other boards!
- **bool LowACCBattery**         : Flag indicating that the 12V accessory is low (true) or normal (false).
- **bool boardDetected**         : Flag set true in receiveCANData when a message from the Power Controller has been received. Use this on other boards to check if you're hearing from the Power Controller.

#### CAN Bus Encoding (Note: this may not be up to date with what is in the file!)
```
Address   byte 7    byte 6    byte 5    byte 4    byte 3    byte 2    byte 1    byte 0    
0x120    00000000  00000000  00000000  00000000  00000000  000000xy  000vwxyz  000vwxyz
byte 0: v        = 1-bit boolean indicating if the Horn is pressed
        w        = 1-bit boolean indicating if the Solar Charge switch is on
        x        = 1-bit boolean indicating if the AC Charger is connected
        y        = 1-bit boolean indicating if the Push-To-Start button is pressed
        z        = 1-bit boolean indicating if the Brake pedal is pressed
byte 1: v        = 1-bit boolean indicating if the Power Controller is doing a soft-start
        w        = 1-bit boolean indicating if the car is in some kind of drive state
        x        = 1-bit boolean indicating if the car is fully started (Ign + Start)
        y        = 1-bit boolean indicating if in the Ignition State
        z        = 1-bit boolean indicating if in the Accessory State
byte 2: x        = 1-bit boolean indicating if the accessory battery is low
        y        = 1-bit boolean indicating if the car should operate in low power mode
byte 3: Not used
byte 4: Not used
byte 5: Not used
byte 6: Not used
byte 7: Not used
```


### `LPDRV_RearLeft_CAN`
Manages data transmission and reception for the [Rear Left Driver](https://github.com/matthewpanizza/DecentralizedLV-LPDRV).

#### Fields (Note: this may not be up to date with what is in the file!)

- **uint32_t boardAddress**      : The CAN Bus address that this controller runs at, should be defined by REAR_LEFT_DRIVER
- **bool bmsFaultInput**         : This board reads in the Battery Management System fault line and tells the rest of the system if we have a fault.
- **bool switchFaultInput**      : This board reads in the manual kill switch fault line and tells the rest of the system if we have a fault.
- **bool boardDetected**         : Flag set true in receiveCANData when a message from the Power Controller has been received. Use this on other boards to check if you're hearing from the Power Controller.

#### CAN Bus Encoding (Note: this may not be up to date with what is in the file!)
controller.CANSend(boardAddress, bmsFaultInput, switchFaultInput, 0, 0, 0, 0, 0, 0);
```
Address   byte 7    byte 6    byte 5    byte 4    byte 3    byte 2    byte 1    byte 0    
0x120    00000000  00000000  00000000  00000000  00000000  00000000  0000000x  0000000x
byte 0: x        = 1-bit boolean indicating if the BMS fault line was set high
byte 1: x        = 1-bit boolean indicating if the kill switch was pressed
byte 2: Not used
byte 3: Not used
byte 4: Not used
byte 5: Not used
byte 6: Not used
byte 7: Not used
```

### `CamryCluster_CAN`
Handles data transmission for the spoof of the [Camry Instrument Cluster](https://github.com/matthewpanizza/CANAnalyzer). Only use this class to transmit from the Dashboard Controller! If you need to retrieve information from other boards in the system, first relay it to the Dashboard Controller. Note: this class needs to have ```CamryCluster_CAN::transmit()``` function called frequently (at least every 50ms) by the Dashboard Controller or the errors will not be cleared.

#### Fields

- **bool brakeIcon**                    : Set true to turn on red BRAKE text on instrument cluster, false to turn off.  
- **bool seatBeltIcon**                 : Set true to turn on red seat belt icon on instrument cluster, false to turn off.  
- **bool checkEngineOn**                : Set true to turn on check engine indicator on instrument cluster, false to turn off.  
- **bool clusterBacklight**             : Set true to turn on backlight on instrument cluster, false to turn off.  
- **bool oilPressureLow**               : Set true to turn on low oil pressure prompt on instrument cluster, false to turn off.  
- **bool chargingSystemMalfunction**    : Set true to turn on low accessory battery indicator on instrument cluster, false to turn off.  
- **uint16_t motorTempDegC**            : Set to the motor temperature in degrees Celcius. Changes position of temperature dial on instrument cluster.  
- **bool powerSteeringIcon**            : Set true to turn on power steering icon on instrument cluster, false to turn off.  
- **uint8_t powerSteeringPrompt**       : Set value to show power steering prompt on LCD.  
- **uint8_t LCD_PowerPrompt**           : Set value to show powerup prompt on LCD. Use the macros such as LCD_HYBRID_SYSTEM_STOPPED and LCD_IGNITION_PROMPT to set the value.  
- **uint8_t LCD_Brightness**            : Set the brightness of the cluster (low or high). Use macros LCD_BRIGHTNESS_HIGH and LCD_BRIGHTNESS_LOW.  
- **bool trunkOpen**                    : Set true to show the trunk being open on the LCD.  
- **bool frontLeftDoor**                : Set true to show the front left door being open on the LCD.  
- **bool frontRightDoor**               : Set true to show the front right door being open on the LCD.  
- **bool rearLeftDoor**                 : Set true to show the rear left door being open on the LCD.  
- **bool rearRightDoor**                : Set true to show the rear right door being open on the LCD.  
- **bool animateStartup**               : Set true to show a fancy animation on the LCD when powering on.  
- **uint8_t LCD_EngineStoppedCode**     : Set value to show engine error codes. Use macros LCD_ENGINE_NORMAL, LCD_ENGINE_STOPPED, LCD_ENGINE_STOPPED_BEEP.  
- **uint8_t LCD_CheckEnginePrompt**     : Set value to show check engine codes. Use macros LCD_CHECK_ENGINE_NONE, LCD_CHECK_ENGINE, LCD_CHECK_ENGINE_REDUCED, LCD_CHECK_ENGINE_MAINTENANCE.  
- **uint16_t rpmGauge**                 : Set value to the motor RPM. Changes the instrument cluster gauge.
- **uint16_t speedGauge**               : Set value to the vehicle speed. Changes the instrument cluster gauge.
- **uint8_t ecoGauge**                  : Set value based on fuel (power) economy. Ranges from 0x00 to 0x3C.
- **bool fogLightOrange**               : Set true to turn on orange fog light indicator on instrument cluster, false to turn off.  
- **bool fogLightGreen**                : Set true to turn on green fog light indicator on instrument cluster, false to turn off.  
- **bool headlight**                    : Set true to turn on headlight indicator on instrument cluster, false to turn off.  
- **bool highbeam**                     : Set true to turn on high beam indicator on instrument cluster, false to turn off.  
- **uint8_t driveMode**                 : Set to the drive mode the Dashboard Controller is in such as DRIVE_MODE_PARK, DRIVE_MODE_FORWARD, DRIVE_MODE_REVERSE, etc.  
- **uint8_t gearNumber**                : Set this to a sport gear (1-10) if you're feeling adventurous. Shows sport gear next to drive mode on LCD.  
- **bool sportMode**                    : Turns on sport mode on the instrument cluster (makes top banner red and shows sport text on bottom).  
- **bool ecoMode**                      : Turns on eco mode on the instrument cluster (makes top banner blue and shows ECO leaf on bottom).  
- **bool readyToDrive**                 : Set true if the car is ready to move forward/backwards. Allows shifting to occur.  

#### CAN Bus Encoding
There was a LOT of reverse-engineering work done to figure out the (incomplete) CAN Bus structure of the Camry instrument cluster. If you want to see the structure, check out the [CAN Analyzer](https://github.com/matthewpanizza/CANAnalyzer) project which has information about the CAN IDs and their respective data.

## Example Usage

### Dashboard Controller Transmit Example

Below is example code that would be run on the Dashboard Controller. An instance of the ```DashController_CAN``` class is created which will encode the value from the fields (```rightTurnPWM```, ```headlight```, ```highbeam```, etc) into a 64-bit CAN Bus message and transmit them out to the other boards on the CAN Bus. You first populate the fields with values, and then use the ```canController``` class to transmit out the encoded message.

```cpp
CAN_Controller canController;       // Create an instance of the CAN Controller
DashController_CAN dc(0x99);        // Create a Dash Controller that transmits on 0x99

canController.begin(500000);        // Start CAN bus transmission at 500000kbps CAN on a Photon.
dc.rightTurnPWM = 255;              // The right turn signal is on at full brightness (0-255)
dc.headlight = true;                // The user has turned on the headlight
dc.driveMode = DRIVE_MODE_FORWARD;  // The user has turned the gear shifter to forward
dc.radiatorFan = false;             // Turn off radiator fan
dc.sendCANData(canController);      // Finally, send out the data to the rest of the boards in the system
```

### Dashboard Controller Receive Example

Below is example code that would be run on a board that would receive information from the Dashboard Controller, such as the Power Controller, LPDRV boards, and HV Controller. An instance of the ```DashController_CAN``` class is created which will decode messages from the actual Dashboard Controller. Calling ```dc.receiveCANData(canMessage)``` will populate the fields (```rightTurnPWM```, ```headlight```, ```highbeam```, etc) with the values set by the Dashboard Controller, which you can then use to turn stuff on and off. When running the code from these two examples, the below snippet would print out ```"Received the right turn signal value of 255 from the Dash Controller!"```.

```cpp
CAN_Controller canController;           // Create an instance of the CAN Controller
LV_CANMessage canMessage;               // Create an instance of the LV_CANMessage (populated by canController.receive())
DashController_CAN dc(0x99);            // Create a representation of the Dash Controller which will receive on address 0x99

void setup(){
    Serial.begin(9600);                 // Start a serial session so you can see the output
    canController.begin(500000, A2);    // Start CAN bus transmission at 500000kbps CAN on a MCP2515 with Chip Select on A2
}

void loop(){
    if(canController.receive(canMessage)){  // Check if we received a message
        dc.receiveCANData(canMessage);      // Process the message
        // Now that we've received the CAN bus data from the Dashboard Controller, you can just access the fields to get the data!
        Serial.printlnf("Received the right turn signal value of %d from the Dash Controller!", dc.rightTurnPWM);
        //dc.headlight would be true
        //dc.driveMode would be DRIVE_MODE_FORWARD
        //dc.radiatorFan would be false
    }    
}
```

## Adding Boards to the API

Below are stub functions for the code segments needed to make a new board work (at least for CAN transmission). In your ```transmit()``` and ```receive()``` functions, you will need to come up with a CAN Bus message encoding based on the data you are attempting to send. Change ```SomeBoardName_CAN``` to be the name of your board

### Header File Code (DecentralizedLV-Boards.h)
```cpp
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////         SomeBoardName_CAN FUNCTIONS        /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Creates an instance of the controller to either send or receive CAN frames for this board. Takes agreed upon address for this board. Example: 'SomeBoardName_CAN sbn(0x150);' would create a SomeBoardName_CAN that transmits on 0x150.
/// @param boardAddr The 32-bit CAN Bus address the SomeBoardName_CAN transmits on.
SomeBoardName_CAN::SomeBoardName_CAN(uint32_t boardAddr){
    boardAddress = boardAddr;
}

/// @brief Initializes the control fields of the SomeBoardName_CAN to a default value. 
void SomeBoardName_CAN::initialize(){
    //Initialize variables to a "safe" value. Don't turn on high voltage stuff by default... bad things likely to happen!
    //Examples from the Power Controller
    //Horn = false;
    //Acc = false;
    //Ign = false;
    //FullStart = false;
    //CarOn = false;
    //StartUp = false;
}

/// @brief Takes the variables that you've previously updated and sends them out in the agreed CAN bus format for this board.
/// @param controller The CAN bus controller attached to this microcontroller.
void SomeBoardName_CAN::sendCANData(CAN_Controller &controller){
    byte tx0 = //Some value between 0 and 255. Can use bit masking to hold multiple booleans
    byte tx1 = //Some value between 0 and 255. Example From Power Controller: Acc + (Ign << 1) + (FullStart << 2) + (CarOn << 3) + (StartUp << 4);
    byte tx2 = //Some value between 0 and 255. Where Acc, Ign, FullStart, CarOn, and StartUp are booleans declared in the SomeBoardName_CAN class.
    controller.CANSend(boardAddress, tx0, tx1, tx2, 0, 0, 0, 0, 0);     //Replace the 0's with some byte variables if you need more bytes
}
/// @brief Extracts CAN frame data into the object's variables so you can use them for controlling other things
/// @param msg The CAN frame that was received by can.receive(). Need to convert from CANMessage to LV_CANMessage by copying address and byte.
void SomeBoardName_CAN::receiveCANData(LV_CANMessage msg){
    if(msg.addr == boardAddress){
        boardDetected = true;
        boardDetected = (msg.byte2 >> 2) & 1;
        //do something with the SomeBoardName_CAN data
        //This decoding must be the reverse of the encoding done in the sendCANData function!
        //Example from Byte 1 of the Power Controller
        //Acc = (msg.byte1) & 1;
        //Ign = (msg.byte1 >> 1) & 1;
        //FullStart = (msg.byte1 >> 2) & 1;
        //CarOn = (msg.byte1 >> 3) & 1;
        //StartUp = (msg.byte1 >> 4) & 1;
    }
}

```

### Source File Code (DecentralizedLV-Boards.cpp)
```cpp
class SomeBoardName_CAN{
    public:
    uint32_t boardAddress;      //The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
    //Declare any variables/information you wish to transmit to other boards here! i.e. uint8_t temperature;
    bool boardDetected;         //Flag set true in receiveCANData when a message from this board has been received. Use this on other boards to check if you're hearing from this board.
    //Examples from the Power Controller:
    //bool Acc;                   //Flag indicating if the car has its Accessory busbar active.
    //bool Ign;                   //Flag indicating if the car has its Ignition busbar active.
    //bool FullStart;
    //bool CarOn;
    //bool StartUp;               

    SomeBoardName_CAN(uint32_t boardAddr);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);

};
```
