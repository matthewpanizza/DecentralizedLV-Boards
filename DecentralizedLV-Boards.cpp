
#include "DecentralizedLV-Boards.h"
#include "Particle.h"
#include <mcp_can.h>

#if PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION   //If we're not on a photon, assume we're using the MCP2515 library

    CANChannel can(CAN_D1_D2);

#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////       DASHBOARD CONTROLLER FUNCTIONS      //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Dashboard controller transmit example - i.e. code that runs on the Dashboard Controller
//CAN_Controller canController;   //Create an instance of the CAN Controller
//DashController_CAN dc(0x99);    //Create a Dash Controller that transmits on 0x99
//
//canController.begin(500000);        //Start CAN bus transmission at 500000kbps CAN on a Photon.
//dc.rightTurnPWM = 255;              //The right turn signal is on at full brightness (0-255)
//dc.headlight = true;                //The user has turned on the headlight
//dc.driveMode = DRIVE_MODE_FORWARD;  //The user has turned the gear shifter to forward
//radiatorFan = false;                //Turn off radiator fan
//dc.sendCANData(canController);      //Finally, send out the data to the rest of the boards in the system



//Dashboard controller receive example - i.e. code that runs on a LPDRV, Power Controller, etc
//CAN_Controller canController;   //Create an instance of the CAN Controller
//LV_CANMessage canMessage;
//DashController_CAN dc(0x99);    //Create a Dash Controller that transmits on 0x99
//
//void setup(){
//    Serial.begin(9600);                 //Start a serial session so you can see the output
//    canController.begin(500000);        //Start CAN bus transmission at 500000kbps CAN on a Photon.
//}
//
//void loop(){
//    if(canController.receive(canMessage)){  //Check if we received a message
//        dc.receiveCANData(canMessage);      //Process the message
//        //Now that we've received the CAN bus data from the Dashboard Controller, you can just access the fields to get the data!
//        Serial.printlnf("Received the right turn signal value of %d from the Dash Controller!", dc.rightTurnPWM);
//    }    
//}    


/// @brief Creates an instance of the controller to either send or receive CAN frames for this board. Takes agreed upon address for this board. Example: 'DashboardController_CAN dc(0x99);' would create a Dashboard Controller that transmits on 0x99.
/// @param boardAddr The 32-bit CAN Bus address the Dashboard Controller transmits on.
DashController_CAN::DashController_CAN(uint32_t boardAddr){
    boardAddress = boardAddr;
}

/// @brief Initializes the control fields of the Dashboard Controller to a default value. 
void DashController_CAN::initialize(){
    rightTurnPWM = 0;
    leftTurnPWM = 0;
    batteryFanPWM = 0;
    headlight = false;
    highbeam = false;
    reversePress = false;
    driveMode = 0;
    radiatorFan = false;
    radiatorPump = false;
    bmsFaultDetected = false;
    rmsFaultDetected = false;
    boardDetected = false;
}

/// @brief Takes the variables that you've previously updated and sends them out in the agreed CAN bus format for this board.
/// @param controller The CAN bus controller attached to this microcontroller.
void DashController_CAN::sendCANData(CAN_Controller &controller){
    byte tx2 = 0;
    byte tx4 = headlight + (highbeam << 1) + (reversePress << 5);
    byte tx5 = 0;
    byte tx6 = driveMode;
    byte tx7 = radiatorFan + (radiatorPump << 1);
    controller.CANSend(boardAddress, rightTurnPWM,leftTurnPWM,tx2,batteryFanPWM,tx4,tx5,tx6,tx7);   //Send out the main message to the corner boards
}

/// @brief Extracts CAN frame data into the object's variables so you can use them for controlling other things
/// @param msg The CAN frame that was received by can.receive(). Need to convert from CANMessage to LV_CANMessage by copying address and byte.
void DashController_CAN::receiveCANData(LV_CANMessage msg){
    if(msg.addr == boardAddress){   //Our message that we received was from this board. Go ahead and import the data to the packets.
        boardDetected = true;
        rightTurnPWM = msg.byte0;
        leftTurnPWM = msg.byte1;
        batteryFanPWM = msg.byte3;
        headlight = msg.byte4 & 1;
        highbeam = (msg.byte4 >> 1) & 1;
        reversePress = (msg.byte4 >> 5) & 1;
        driveMode = msg.byte6;
        radiatorFan = msg.byte7 & 1;
        radiatorPump = (msg.byte7 >> 1) & 1;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////         POWER CONTROLLER FUNCTIONS        //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Creates an instance of the controller to either send or receive CAN frames for this board. Takes agreed upon address for this board. Example: 'PowerController_CAN pc(0x120);' would create a Power Controller that transmits on 0x120.
/// @param boardAddr The 32-bit CAN Bus address the Power Controller transmits on.
PowerController_CAN::PowerController_CAN(uint32_t boardAddr){
    boardAddress = boardAddr;
}

/// @brief Initializes the control fields of the Power Controller to a default value. 
void PowerController_CAN::initialize(){
    BrakeSense = false;
    PushToStart = false;
    ACCharge = false;
    SolarCharge = false;
    Horn = false;
    Acc = false;
    Ign = false;
    FullStart = false;
    CarOn = false;
    StartUp = false;
    LowPowerMode = false;
    LowACCBattery = false;
    boardDetected = false;
}

/// @brief Takes the variables that you've previously updated and sends them out in the agreed CAN bus format for this board.
/// @param controller The CAN bus controller attached to this microcontroller.
void PowerController_CAN::sendCANData(CAN_Controller &controller){
    byte tx0 = BrakeSense + (PushToStart << 1) + (ACCharge << 2) + (SolarCharge << 3) + (Horn << 4);
    byte tx1 = Acc + (Ign << 1) + (FullStart << 2) + (CarOn << 3) + (StartUp << 4);
    byte tx2 = LowPowerMode + (LowACCBattery << 1) + (boardDetected << 2);
    controller.CANSend(boardAddress, tx0, tx1, tx2, 0, 0, 0, 0, 0);
}
/// @brief Extracts CAN frame data into the object's variables so you can use them for controlling other things
/// @param msg The CAN frame that was received by can.receive(). Need to convert from CANMessage to LV_CANMessage by copying address and byte.
void PowerController_CAN::receiveCANData(LV_CANMessage msg){
    if(msg.addr == boardAddress){
        boardDetected = true;
        //do something with the power controller data
        BrakeSense = (msg.byte0) & 1;
        PushToStart = (msg.byte0 >> 1) & 1;
        ACCharge = (msg.byte0 >> 2) & 1;
        SolarCharge = (msg.byte0 >> 3) & 1;
        Horn = (msg.byte0 >> 4) & 1;
        Acc = (msg.byte1) & 1;
        Ign = (msg.byte1 >> 1) & 1;
        FullStart = (msg.byte1 >> 2) & 1;
        CarOn = (msg.byte1 >> 3) & 1;
        StartUp = (msg.byte1 >> 4) & 1;
        LowPowerMode = (msg.byte2) & 1;
        LowACCBattery = (msg.byte2 >> 1) & 1;
        boardDetected = (msg.byte2 >> 2) & 1;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////         REAR LEFT DRIVER FUNCTIONS        //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Creates an instance of the controller to either send or receive CAN frames for this board. Takes agreed upon address for this board. Example: 'LPDRV_RearLeft_CAN pc(0x95);' would create a Rear Left Driver that transmits on 0x95.
/// @param boardAddr The 32-bit CAN Bus address the Rear Left Driver transmits on.
LPDRV_RearLeft_CAN::LPDRV_RearLeft_CAN(uint32_t boardAddr){
    boardAddress = boardAddr;
}

/// @brief Initializes the control fields of the Rear Left Driver to a default value. 
void LPDRV_RearLeft_CAN::initialize(){
    bmsFaultInput = false;
    switchFaultInput = false;
    boardDetected = false;
}

/// @brief Takes the variables that you've previously updated and sends them out in the agreed CAN bus format for this board.
/// @param controller The CAN bus controller attached to this microcontroller.
void LPDRV_RearLeft_CAN::sendCANData(CAN_Controller &controller){
    controller.CANSend(boardAddress, bmsFaultInput, switchFaultInput, 0, 0, 0, 0, 0, 0);
}

/// @brief Extracts CAN frame data into the object's variables so you can use them for controlling other things
/// @param msg The CAN frame that was received by can.receive(). Need to convert from CANMessage to LV_CANMessage by copying address and byte.
void LPDRV_RearLeft_CAN::receiveCANData(LV_CANMessage msg){
    if(msg.addr == boardAddress){
        boardDetected = true;
        bmsFaultInput = msg.byte0 & 1;  //Extract BMS fault from the first bit of byte 0
        bmsFaultInput = msg.byte1 & 1;  //Extract BMS fault from the first bit of byte 1
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////         POWER CONTROLLER FUNCTIONS        //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Initializes the control fields of the cluster to a default value. Must call sendCANData for the cluster to actually update.
void CamryCluster_CAN::initialize(){
    brakeIcon = false;
    seatBeltIcon = false;
    checkEngineOn = false;
    clusterBacklight = true;
    oilPressureLow = false;
    chargingSystemMalfunction = false;
    motorTempDegC = 25;
    powerSteeringIcon = false;
    powerSteeringPrompt = 0x00;
    LCD_PowerPrompt = LCD_POWER_GOOD;
    LCD_Brightness = LCD_BRIGHTNESS_HIGH;
    trunkOpen = false;
    frontLeftDoor = false;
    frontRightDoor = false;
    rearLeftDoor = false;
    rearRightDoor = false;
    animateStartup = false;
    LCD_EngineStoppedCode = LCD_ENGINE_NORMAL;
    LCD_CheckEnginePrompt = LCD_CHECK_ENGINE_NONE;
    rpmGauge = 0;
    speedGauge = 0;
    ecoGauge = 0x3C;
    fogLightOrange = false;
    fogLightGreen = false;
    headlight = false;
    last_headlight = !headlight;
    highbeam = false;
    last_highbeam = !highbeam;
    driveMode = DRIVE_MODE_PARK;
    gearNumber = 0;
    sportMode = false;
    ecoMode = false;
    readyToDrive = false;
}

/// @brief Taskes the locally populated fields and generates the CAN bus frames needed to spoof the Camry Cluster components.
/// @param controller The CAN bus controller attached to this microcontroller.
void CamryCluster_CAN::sendCANData(CAN_Controller &controller){
    //SEE THIS SHEET FOR HOW THE SPOOF WORKS: https://docs.google.com/spreadsheets/d/1bL61UoguuONFQnytRpy7xj2nyJdYXmgT9HQCQns6Ij0/edit?usp=sharing
    uint8_t lowACC = 0;                                         //Default to not a low accessory battery
    if(chargingSystemMalfunction) lowACC = 0x04;                //If ACC battery is low, display prompt on LCD
    else if(oilPressureLow) lowACC = 0x03;                      //Show oil pressure low if battery is OK

    uint8_t powerSteerState = powerSteeringIcon ? 0x38:0x00;    //Clear steering wheel icon if no power steering error

    uint8_t engineFault = checkEngineOn ? 0x00:0x40;                  //0x40 turns off check engine light from instrument cluster
    if(!clusterBacklight) engineFault = checkEngineOn ? 0xB0:0xC0;    //Turn off backlight if needed

    uint8_t dashAnimationMask = animateStartup ? 0x00:0x40;
    dashAnimationMask += trunkOpen + (rearLeftDoor << 2) + (rearRightDoor << 3) + (frontRightDoor << 4) + (frontLeftDoor << 5);

    uint8_t otherGear = 0;                                      //Variable to check if in neutral or reverse, default to no gear
    switch (driveMode){
        case DRIVE_MODE_PARK:
            otherGear = readyToDrive ? 0x20:0x00;
            break;
        case DRIVE_MODE_REVERSE:
            otherGear = 0x10;
            break;
        case DRIVE_MODE_SPORT:
            otherGear = 0x09;
            break;
        case DRIVE_MODE_NEUTRAL:
            otherGear = 0x08;
            break;
    }

    uint8_t driveSet = 0;                                       //Drive setting variable, default of 0 (no gear)
    if(driveMode != DRIVE_MODE_PARK && driveMode != DRIVE_MODE_REVERSE) driveSet = 0x80;           //If not in park mode, set this to 0x80

    uint8_t driveModifier = 0;                                  //Default normal drive mode (not eco or sport)      
    if(sportMode) driveModifier = 0x10;                         //If bit 1 is set, then we are in sport mode (0x10 to instrument cluster signals sport)
    if(ecoMode) driveModifier = 0x30;                           //If bit 2 is set, then we are in eco mode (0x30 to instrument cluster signals eco)
    
    controller.CANSend(ABS_CAN_ADDR, (brakeIcon ? 0x40:0x00), 0, 0, 0, 0, 0, 0, 0);                         //Spoof Anti-Lock brakes (All 0's clears errors)
    controller.CANSend(AIRBAG_CAN_ADDR, 0, 0, 0, (seatBeltIcon ? 0x05:0x00), 0, 0, 0, 0);                   //Spoof SRS Airbag system (All 0's clears errors)
    controller.CANSend(ENGINE_CONTROL_CAN_ADDR, engineFault,lowACC,(motorTempDegC*1.59)+65,0,0,0,0,0);      //Spoof for engine controller. Takes a flag that sets check engine, alternator failure and motor temperature
    controller.CANSend(POWER_STEER_CAN_ADDR_1, 0, powerSteerState, powerSteeringPrompt, 0, 0, 0, 0, 0);     //Spoof for Power Steering, byte 1 controls steering wheel icon on cluster
    controller.CANSend(POWER_STEER_CAN_ADDR_2, 0, 0, 0, 0, 0, 0, 0, 0);
    controller.CANSend(POWER_STEER_CAN_ADDR_3, 0, 0, 0, 0, 0, 0, 0, 0);
    controller.CANSend(LANE_DEPART_CAN_ADDR, 0, 0, 0, 0, 0, 0, 0, 0);                                       //Lane departure spoof
    controller.CANSend(PRECOLLISION_CAN_ADDR, 0, 0, 0, 0, 0, 0, 0, 0);                                      //Precollision spoof
    controller.CANSend(PARKING_CAN_ADDR, 1, 1, 1, 1, 0, 0, 0, 0);                                           //Parking sonar spoof
    controller.CANSend(SMART_KEY_CAN_ADDR, 0x81, 0, 0, 0, 0, 0, LCD_PowerPrompt, LCD_PowerPrompt ? 0x0D:0); //Smart Key and Push to Start instructions
    controller.CANSend(ANIMATIONS_CAN_ADDR, 0x10, 0, 0, 0, LCD_Brightness, dashAnimationMask, 0x08, seatBeltIcon ? 0x50:0x00);    //Spoof for instrument cluster animations and backlight dimming
    controller.CANSend(MOTOR_SPOOF_CAN_ADDR, 0, 0, 0, 0, 0, 0, rpmGauge/200, rpmGauge%200);                 //Motor spoof for RPM dial
    controller.CANSend(TRANSMISSION_CAN_ADDR, 0, otherGear, 0, 0, (gearNumber << 4), driveSet, 0, driveModifier);           //Transmission controller spoof, sets drive gear and sport/eco/normal modes
    controller.CANSend(ENGINE_PROMPTS_CAN_ADDR, 0, 0 , 0, 0, 0, 0, LCD_EngineStoppedCode, LCD_CheckEnginePrompt);
    if(driveMode != DRIVE_MODE_PARK) controller.CANSend(FUEL_ECONOMY_CAN_ADDR, 0, 0, 0, 0, 0, 1, 0, 0);                 //Spoof for fuel economy meter
    if(headlight != last_headlight || highbeam != last_highbeam){       //This CAN id only needs to get sent if the value has changed
        controller.CANSend(LIGHTING_CAN_ADDR, 0x12, 0, 0xE8,(headlight << 5) + (highbeam << 6), 0, 0, 0, 0); //Send out spoof for headlights/high beam system when headlight switches have changed
        last_headlight = headlight;
        last_highbeam = highbeam;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////         CAN BUS CONTROLLER SUBMODULE         ///////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Converts baud rate from MCP_CAN_RK library macro to true Particle speed
/// @param baudRate Baud rate in bits per second.
unsigned long convertBaudRateToParticle(unsigned long baudRate){
    if(baudRate <= CAN_1000KBPS){   //Check if this is a MCP_CAN_RK baud rate     
        switch (baudRate){
        case CAN_1000KBPS:
            return 1000000;
        case CAN_500KBPS:
            return 500000;
        case CAN_250KBPS:
            return 250000;
        case CAN_200KBPS:
            return 200000;
        case CAN_125KBPS:
            return 125000;
        case CAN_100KBPS:
            return 100000;
        case CAN_50KBPS:
            return 50000;
        default:
            return 500000;  //Use 500kbps CAN if using unrecognized macro
        }
    }
    return baudRate;    //If baudRate > CAN_1000KBPS, then assumes we're already in Particle format
}

/// @brief Converts baud rate from MCP_CAN_RK library macro to true Particle speed
/// @param baudRate Baud rate in bits per second.
unsigned long convertBaudRateToMCP(unsigned long baudRate){
    if(baudRate > CAN_1000KBPS){   //Check if this is a MCP_CAN_RK baud rate     
        switch (baudRate){
        case 1000000:
            return CAN_1000KBPS;
        case 500000:
            return CAN_500KBPS;
        case 250000:
            return CAN_250KBPS;
        case 200000:
            return CAN_200KBPS;
        case 125000:
            return CAN_125KBPS;
        case 100000:
            return CAN_100KBPS;
        case 50000:
            return CAN_50KBPS;
        default:
            return CAN_500KBPS;  //Use 500kbps CAN if using unrecognized macro
        }
    }
    return baudRate;            //If baudRate <= CAN_1000KBPS, then assumes we're already in MCP format
}

//Update function which allows new data values to be passed in all at once
void LV_CANMessage::update(uint32_t Can_addr, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7){
    addr = Can_addr;
    byte0 = data0; byte1 = data1; byte2 = data2; byte3 = data3; byte4 = data4; byte5 = data5; byte6 = data6; byte7 = data7;
}

#if PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION   //When running on a board with a photon, use the integrated CAN bus controller

/// @brief Initializes the CAN bus controller on the photon with the specified speed.
/// @param baudRate Baud rate in bits per second.
void CAN_Controller::begin(unsigned long baudRate){
    currentBaudRate = convertBaudRateToParticle(baudRate);
    can.begin(currentBaudRate);
}

/// @brief Adds a filter to the CAN bus receiving function to only allow messages with a specified address.
/// @param address Baud rate in bits per second.
/// @param mask [Advanced] Bit mask for CAN address. Most cases will be 0x7FF
void CAN_Controller::addFilter(uint32_t address){
    can.addFilter(address, 0x7FF);
}

/// @brief Checks if a message was received on CAN bus and populates the LV_CANMessage with the received data. Do 'LV_CANMessage message;' then 'controller.receive(message);'.
/// @param outputMessage CAN bus message that will be returned by the CAN controller (returns reference). 
/// @return Boolean indicating whether or not a message was received from the CAN bus
bool CAN_Controller::receive(LV_CANMessage &outputMessage){
    CANMessage inputMessage;
    bool receivedMessage = can.receive(inputMessage);
    if(!receivedMessage) return receivedMessage;    //Didn't receive anything from CAN, don't bother processing input message data.
    if(inputMessage.id == 0) return false;
    outputMessage.addr = inputMessage.id;
    outputMessage.byte0 = inputMessage.data[0];
    outputMessage.byte1 = inputMessage.data[1];
    outputMessage.byte2 = inputMessage.data[2];
    outputMessage.byte3 = inputMessage.data[3];
    outputMessage.byte4 = inputMessage.data[4];
    outputMessage.byte5 = inputMessage.data[5];
    outputMessage.byte6 = inputMessage.data[6];
    outputMessage.byte7 = inputMessage.data[7];
    return true;
}

/// @brief Checks if a message was received on CAN bus and populates the LV_CANMessage with the received data. Do 'LV_CANMessage message;' then 'controller.receive(message);'.
/// @param outputMessage CAN bus message that will be returned by the CAN controller (returns reference). 
/// @return Boolean indicating whether or not a message was received from the CAN bus
void CAN_Controller::changeCANSpeed(uint32_t newCanSpeed){
    can.end();
    currentBaudRate = convertBaudRateToMCP(newCanSpeed);
    can.begin(currentBaudRate);
}

/// @brief Returns the current baud rate of the CAN controller
/// @return The current baud rate in bps. 
uint32_t CAN_Controller::CurrentBaudRate(){
    return currentBaudRate;
}

/// @brief [Internal Function] Manually sends a CAN bus packet using the CAN bus controller on the address specified with the inputted data. Example: 'CANSend(0x100, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);' transmits on address 0x100 with the data [1,2,3,4,5,6,7,8]
/// @param Can_addr CAN Bus address to transmit on
/// @param data0 CAN Bus Data 0 field (8 bits)
/// @param data1 CAN Bus Data 1 field (8 bits)
/// @param data2 CAN Bus Data 2 field (8 bits)
/// @param data3 CAN Bus Data 3 field (8 bits)
/// @param data4 CAN Bus Data 4 field (8 bits)
/// @param data5 CAN Bus Data 5 field (8 bits)
/// @param data6 CAN Bus Data 6 field (8 bits)
/// @param data7 CAN Bus Data 7 field (8 bits)
void CAN_Controller::CANSend(uint16_t Can_addr, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7){
    CANMessage txMessage;
    txMessage.id = Can_addr;
    txMessage.len = 8;
    txMessage.data[0] = data0;
    txMessage.data[1] = data1;
    txMessage.data[2] = data2;
    txMessage.data[3] = data3;
    txMessage.data[4] = data4;
    txMessage.data[5] = data5;
    txMessage.data[6] = data6;
    txMessage.data[7] = data7;
    
    can.transmit(txMessage);
}

/// @brief [Internal Function] Manually sends a CAN bus packet using the CAN bus controller on the address specified with the inputted data. Example: 'CANSend(0x100, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);' transmits on address 0x100 with the data [1,2,3,4,5,6,7,8]
/// @param Can_addr CAN Bus address to transmit on
/// @param data0 CAN Bus Data 0 field (8 bits)
/// @param data1 CAN Bus Data 1 field (8 bits)
/// @param data2 CAN Bus Data 2 field (8 bits)
/// @param data3 CAN Bus Data 3 field (8 bits)
/// @param data4 CAN Bus Data 4 field (8 bits)
/// @param data5 CAN Bus Data 5 field (8 bits)
/// @param data6 CAN Bus Data 6 field (8 bits)
/// @param data7 CAN Bus Data 7 field (8 bits)
void CAN_Controller::CANSend(LV_CANMessage inputMessage){
    CANMessage txMessage;
    txMessage.id = inputMessage.addr;
    txMessage.len = 8;
    txMessage.data[0] = inputMessage.byte0;
    txMessage.data[1] = inputMessage.byte1;
    txMessage.data[2] = inputMessage.byte2;
    txMessage.data[3] = inputMessage.byte3;
    txMessage.data[4] = inputMessage.byte4;
    txMessage.data[5] = inputMessage.byte5;
    txMessage.data[6] = inputMessage.byte6;
    txMessage.data[7] = inputMessage.byte7;
    
    can.transmit(txMessage);
}

#else

/// @brief Initializes the MCP2515 CAN bus controller on the P2/other with the specified speed and chip select pin.
/// @param baudRate Baud rate in bits per second.
void CAN_Controller::begin(unsigned long baudRate, uint8_t chipSelectPin){
    csPin = chipSelectPin;
    currentBaudRate = convertBaudRateToMCP(baudRate);
    CAN0 = new MCP_CAN(chipSelectPin);
    Serial.printlnf("Begin at %d", currentBaudRate);
    CAN0->begin(MCP_STDEXT, currentBaudRate, MCP_8MHZ);
    CAN0->setMode(MCP_NORMAL);
    SPI.setClockSpeed(8000000);
    filterIndex = 0;
}

/// @brief Adds a filter to the CAN bus receiving function to only allow messages with a specified address.
/// @param address Baud rate in bits per second.
/// @param mask [Advanced] Bit mask for CAN address. Most cases will be 0x7FF
void CAN_Controller::addFilter(uint32_t address){
    if(filterIndex == 0){
        CAN0->init_Mask(0, 0x01FFC000); //this number is 0x7FF bit shifted left by 14. This only allows standard CANBUS IDs
        CAN0->init_Mask(1, 0x01FFC000); //the first number is the choise of mask (0-5)
    }
    if(filterIndex < 6){                              //the MCP only has 6 total filters (6 hex IDs it can accept messages from)
        CAN0->init_Filt(filterIndex, 0, address<<16); //this tells the MCP to only accept messages matching an ID (address bit shifted by 16)
        filterIndex++;
    }
}

/// @brief Checks if a message was received on CAN bus and populates the LV_CANMessage with the received data. Do 'LV_CANMessage message;' then 'controller.receive(message);'.
/// @param outputMessage CAN bus message that will be returned by the CAN controller (returns reference). 
/// @return Boolean indicating whether or not a message was received from the CAN bus
bool CAN_Controller::receive(LV_CANMessage &outputMessage){
    bool receivedMessage = CAN0->checkReceive();
    if(!receivedMessage) return receivedMessage;
    uint32_t rxId = 0;
    unsigned char len = 0;
    unsigned char rxBuf[8];
    CAN0->readMsgBuf(&rxId, &len, rxBuf);
    if(rxId == 0) return false;
    outputMessage.addr = rxId;
    outputMessage.byte0 = rxBuf[0];
    outputMessage.byte1 = rxBuf[1];
    outputMessage.byte2 = rxBuf[2];
    outputMessage.byte3 = rxBuf[3];
    outputMessage.byte4 = rxBuf[4];
    outputMessage.byte5 = rxBuf[5];
    outputMessage.byte6 = rxBuf[6];
    outputMessage.byte7 = rxBuf[7];
    return true;
}

/// @brief Reinitializes the CAN controller with a new CAN Bus baud rate.
/// @param newCanSpeed New CAN Bus speed in bps. 
void CAN_Controller::changeCANSpeed(uint32_t newCanSpeed){
    currentBaudRate = convertBaudRateToMCP(newCanSpeed);
    CAN0->begin(MCP_STDEXT, currentBaudRate, csPin);
}

/// @brief Returns the current baud rate of the CAN controller
/// @return The current baud rate in bps. 
uint32_t CAN_Controller::CurrentBaudRate(){
    return currentBaudRate;
}

/// @brief [Internal Function] Manually sends a CAN bus packet using the CAN bus controller on the address specified with the inputted data. Example: 'CANSend(0x100, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);' transmits on address 0x100 with the data [1,2,3,4,5,6,7,8]
/// @param Can_addr CAN Bus address to transmit on
/// @param data0 CAN Bus Data 0 field (8 bits)
/// @param data1 CAN Bus Data 1 field (8 bits)
/// @param data2 CAN Bus Data 2 field (8 bits)
/// @param data3 CAN Bus Data 3 field (8 bits)
/// @param data4 CAN Bus Data 4 field (8 bits)
/// @param data5 CAN Bus Data 5 field (8 bits)
/// @param data6 CAN Bus Data 6 field (8 bits)
/// @param data7 CAN Bus Data 7 field (8 bits)
void CAN_Controller::CANSend(uint16_t Can_addr, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7){    //Implementation of CANSend on boards 
    byte data[8] = {data0, data1, data2, data3, data4, data5, data6, data7};
    CAN0->sendMsgBuf(Can_addr, 0, 8, data);
}

/// @brief [Internal Function] Manually sends a CAN bus packet using the CAN bus controller on the address specified with the inputted data. Example: 'CANSend(0x100, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08);' transmits on address 0x100 with the data [1,2,3,4,5,6,7,8]
/// @param Can_addr CAN Bus address to transmit on
/// @param inMsg CAN Bus message class
void CAN_Controller::CANSend(LV_CANMessage inMsg){    //Implementation of CANSend on boards 
    byte data[8] = {inMsg.byte0, inMsg.byte1, inMsg.byte2, inMsg.byte3, inMsg.byte4, inMsg.byte5, inMsg.byte6, inMsg.byte7};
    CAN0->sendMsgBuf(inMsg.addr, 0, 8, data);
}

#endif
