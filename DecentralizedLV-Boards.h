#ifndef DECENTRALIZED_LV_BOARDS_H
#define DECENTRALIZED_LV_BOARDS_H

#include "Particle.h"
#include <mcp_can.h>

//////////////////////////////////////////////////////////////////////////////////////////////////
// MACROS FOR SYSTEM OPERATION
#define DRIVE_MODE_PARK     0
#define DRIVE_MODE_FORWARD  1
#define DRIVE_MODE_SPORT    3
#define DRIVE_MODE_ECO      5
#define DRIVE_MODE_REVERSE  8
#define DRIVE_MODE_NEUTRAL  16
//////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
// MACROS FOR CAMRY CLUSTER

//LCD Power sequence prompts
#define LCD_POWER_GOOD                  0x00    //Value for LCD_PowerPrompt to clear all prompts
#define LCD_TURN_POWER_OFF              0x01    //Value for LCD_PowerPrompt to show "Turn Power Off Before Exiting Vehicle"
#define LCD_SHIFT_PARK_BEFORE_EXITING   0x02    //Value for LCD_PowerPrompt to show "Shift to Park Before Exiting Vehicle"
#define LCD_HYBRID_SYSTEM_STOPPED       0x04    //Value for LCD_PowerPrompt to show "Hybrid System Stopped"
#define LCD_SHIFT_TO_NEUTRAL_RESTART    0x08    //Value for LCD_PowerPrompt to show "Shift to Neutral and Push Engine Switch to Restart"
#define LCD_PRESS_BRAKE_PREDAL_AND_PTS  0x30    //Value for LCD_PowerPrompt to show the turn on instructions "Press Brake Pedal and Push Power Switch to Start"
#define LCD_KEY_DETECTED_IN_VEHICLE     0x40    //Value for LCD_PowerPrompt to show "Key Not Detected In Vehicle"
#define LCD_IGNITION_PROMPT             0x50    //Value for LCD_PowerPrompt to show that we're in ignition. "Not ready to drive" with icon for brake pedal and push to start

//LCD Brightness values
#define LCD_BRIGHTNESS_LOW              0xF0    //Value for LCD_Brightness to set brightness to low
#define LCD_BRIGHTNESS_HIGH             0xB0    //Value for LCD_Brightness to set brightness to high

// Engine Stopped LCD codes
#define LCD_ENGINE_NORMAL               0x00    //Value for LCD_EngineStoppedCode to not show engine prompt
#define LCD_ENGINE_STOPPED              0x10    //Value for LCD_EngineStoppedCode to show "Engine Stopped, stop in a safe place"
#define LCD_ENGINE_STOPPED_BEEP         0x1B    //Value for LCD_EngineStoppedCode to show "Engine Stopped, stop in a safe place" and make beeping noise.

//LCD Check Engine Prompts
#define LCD_CHECK_ENGINE_NONE           0x00    //Value for LCD_CheckEnginePrompt to show no check engine light or message
#define LCD_CHECK_ENGINE                0x10    //Value for LCD_CheckEnginePrompt to show "Check Engine" on the LCD
#define LCD_CHECK_ENGINE_REDUCED        0x30    //Value for LCD_CheckEnginePrompt to show "Reduced Engine Power" on the LCD
#define LCD_CHECK_ENGINE_MAINTENANCE    0x40    //Value for LCD_CheckEnginePrompt to show "Engine Maintenance Required" on the LCD

//LCD Parking Brake Prompts
#define LCD_PBRK_GOOD                   0x00    //Value for LCD_ParkingBrakePrompt to show no parking brake warning
#define LCD_PBRK_MAY_ROLL               0x10    //Value for LCD_ParkingBrakePrompt to show "Parking Brake May Roll" on the LCD
#define LCD_PBRK_AUTO_APPLY             0x20    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Automatically Applied" on the LCD
#define LCD_PBRK_BRAKE_ON               0x30    //Value for LCD_ParkingBrakePrompt to show "Parking Brake On" on the LCD
#define LCD_PBRK_NO_DISENGAGE           0x40    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Unable to Disengage" on the LCD
#define LCD_PBRK_SHIFT_INTERLOCK_EN     0x50    //Value for LCD_ParkingBrakePrompt to show "Shift Interlock Activated" on the LCD
#define LCD_PBRK_SHIFT_INTERLOCK_DIS    0x60    //Value for LCD_ParkingBrakePrompt to show "Shift Interlock Deactivated" on the LCD
#define LCD_PBRK_UNABLE_DISENGAGE       0x70    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Unable to Disengage" on the LCD
#define LCD_PBRK_UNAVAIL_ROLL           0x80    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Unavailable, May Roll" on the LCD
#define LCD_PBRK_UNAVAIL                0x90    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Unavailable" on the LCD
#define LCD_PBRK_TEMP_UNAVAIL           0xA0    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Temporarily Unavailable" on the LCD
#define LCD_PBRK_MALFUNCTION            0xB0    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Malfunction" on the LCD
#define LCD_PBRK_PBRK_UNAVAIL           0xC0    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Unavailable" on the LCD
#define LCD_PBRK_PBRK_MALFUNCTION       0xD0    //Value for LCD_ParkingBrakePrompt to show "Parking Brake Malfunction" on the LCD  

//LCD Take a Break Prompts
#define LCD_TAKE_BREAK_NONE             0x00    //Value for LCD_TakeBreak_Prompt to show no take a break message
#define LCD_PLEASE_TAKE_BREAK           0x40    //Value for LCD_TakeBreak_Prompt to show "Would you like to take a Break" on the LCD
#define LCD_TAKE_BREAK_WARN             0x80    //Value for LCD_TakeBreak_Prompt to show "Please take a Break" on the LCD

//Instrument cluster beep rate macros
#define BEEP_RATE_OFF                   0x00    //Value for clusterBeeps to turn off the beep
#define BEEP_RATE_CONTINUOUS            0x01    //Value for clusterBeeps to beep continuously
#define BEEP_RATE_1                     0x02    //Value for clusterBeeps to beep at rate 1 (Fastest)
#define BEEP_RATE_2                     0x03    //Value for clusterBeeps to beep at rate 2 (Faster)
#define BEEP_RATE_3                     0x04    //Value for clusterBeeps to beep at rate 3 (Fast)
#define BEEP_RATE_4                     0x05    //Value for clusterBeeps to beep at rate 4 (Medium)
#define BEEP_RATE_5                     0x06    //Value for clusterBeeps to beep at rate 5 (Slow) 
#define BEEP_RATE_6                     0x07    //Value for clusterBeeps to beep at rate 6 (Slower)
#define BEEP_RATE_7                     0x08    //Value for clusterBeeps to beep at rate 7 (Slowest)

//HUD Lane Departure Macros
#define HUD_LANE_OFF                    0x00    //Value for hudLeftLaneColor or hudRightLaneColor to turn off the lane departure indicators
#define HUD_LANE_CLEAR                  0x01    //Value for hudLeftLaneColor or hudRightLaneColor have the lane show a clear outline
#define HUD_LANE_WHITE                  0x02    //Value for hudLeftLaneColor or hudRightLaneColor have the lane be filled white
#define HUD_LANE_ORANGE                 0x03    //Value for hudLeftLaneColor or hudRightLaneColor have the lane flash orange

//CAN Message Addresses
#define SPEED_CAN_ADDR                  0xB4    //Address to send to the instrument cluster to fake the speedometer
#define ABS_CAN_ADDR                    0x3B7   //Address to send to the instrument cluster to fake the anti-lock brakes
#define AIRBAG_CAN_ADDR                 0x3B1   //Address to send to the instrument cluster to fake the airbag system
#define ENGINE_CONTROL_CAN_ADDR         0x3BB   //Address to send to the instrument cluster to fake the engine controller
#define POWER_STEER_CAN_ADDR            0x394   //Address to send to the instrument cluster to fake the power steering system
#define PARKING_BRAKE_CAN_ADDR          0x32C   //Address to send to the instrument cluster to fake the parking brake
#define PARK_ASSIST_CAN_ADDR            0x378   //Address to send to the instrument cluster to fake the power steering system
#define LANE_DEPART_CAN_ADDR            0x412   //Address to send to the instrument cluster to fake the lane departure system
#define PRECOLLISION_CAN_ADDR           0x411   //Address to send to the instrument cluster to fake the precollision system
#define PARKING_CAN_ADDR                0x43A   //Address to send to the instrument cluster to fake the parking assist system
#define SMART_KEY_CAN_ADDR              0x633   //Address to send to the instrument cluster to fake the smart key system
#define MOTOR_SPOOF_CAN_ADDR            0x1EA   //Address to send to the instrument cluster to fake the motor controller
#define TRANSMISSION_CAN_ADDR           0x3BC   //Address to send to the instrument cluster to fake the transmission controller
#define ANIMATIONS_CAN_ADDR             0x620   //Address to send to the instrument cluster to fake the animations
#define FUEL_ECONOMY_CAN_ADDR           0x1C4   //Address to send to the instrument cluster to fake the fuel economy system
#define LIGHTING_CAN_ADDR               0x622   //Address to send to the instrument cluster to fake the lighting controller
#define ENGINE_PROMPTS_CAN_ADDR         0x400   //Address to send to the instrument cluster to fake the engine unit
#define OUTDOOR_TEMP_CAN_ADDR           0x3B0   //Address to send to the instrument cluster to fake the outdoor temperature

//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Dashboard Controller CAN Message Format. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define DASH_CONTROL_ADDR   0x99
// byte 0: Right Turn PWM 0-255
// byte 1: Left Turn PWM 0-255
// byte 2: 
// byte 3: HV Battery Fan PWM 0-255
// byte 4: b0:headlight b1:highbeam b5:reversePress
// byte 5: 
// byte 6: Drive Mode: b0: Drive, b1: Sport, b2: Eco, b3: Reverse, b4: Neutral (BPS fault)
// byte 7: b0: Radiator Fan, b1: Radiator pump
// EXAMPLE FRAME: CANSend(0x99, 0xFF, 0xFF, 0x00, 0xFF, 0x03, 0x00, 0x01, 0x03);
// - Right and Left turn signal, headlight, and highbeam are on (at full brightness for L and R signal)
// - Car is not in low power mode, not doing startup animations
// - Car is in Drive Forward mode
// - Radiator Fan and Pump are both on
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Power Controller CAN Message Format. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define POWER_CONTROL_ADDR   0x120
// byte 0: 
// byte 1: 
// byte 2: 
// byte 3: 
// byte 4:
// byte 5:
// byte 6:
// byte 7: 
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Orion BMS Message Forwarding Format for the HV pack statistics. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define ORION_PACK_STAT_ADDR   0x112
// byte 0: pack current (upper 8 bits) (0.1A increments)
// byte 1: pack current (lower 8 bits) (0.1A increments)
// byte 2: pack voltage (upper 8 bits) (0.1V increments)
// byte 3: pack voltage (lower 8 bits) (0.1V increments)
// byte 4: pack amp-hours (0.1Ah increments)
// byte 5: pack resistance (1mOhm increments)
// byte 6: pack state of charge (0-100%)
// byte 7: 12V voltage (0.1V increments)
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Orion BMS Message Forwarding Format for cell stats and DTC error codes. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define ORION_DTC_CELLV_ADDR   0x113
// byte 0: average cell voltage (0.1V increments)
// byte 1: highest cell voltage (0.1V increments)
// byte 2: lowest cell voltage (0.1V increments)
// byte 3: lowest cell resistance (0.1mOhm increments)
// byte 4: DTC error flags 1 (upper 8 bits)
// byte 5: DTC error flags 1 (lower 8 bits)
// byte 6: DTC error flags 2 (upper 8 bits)
// byte 7: DTC error flags 2 (lower 8 bits)
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Orion BMS Message Forwarding Format for the current limits and cell temperatures. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define ORION_CUR_LMT_TEMP_ADDR   0x114
// byte 0: discharge current limit (upper 8 bits) (1A increments)
// byte 1: discharge current limit (lower 8 bits) (1A increments)
// byte 2: charge current limit (upper 8 bits) (1A increments)
// byte 3: charge current limit (lower 8 bits) (1A increments)
// byte 4: BMS average temperature (degrees C)
// byte 5: BMS internal temperature (degrees C)
// byte 6: Highest thermistor temp (degrees C)
// byte 7: Lowest thermistor temp (degrees C)
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Orion BMS Message Forwarding Format for the J1772 charger. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define ORION_J1772_STATS_ADDR   0x115
// byte 0: J1772 charger connected
// byte 1: J1772 Charger Current Limit (1V increments)
// byte 2: J1772 AC Charger Voltage (1V increments)
// byte 3: 
// byte 4:
// byte 5: 
// byte 6:
// byte 7:
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//RMS Motor Controller Message Forwarding Format for voltages and currents. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define RMS_POWER_STAT_ADDR   0x116
// byte 0: 12V accessory Voltage (upper 8 bits) (0.01V increments)
// byte 1: 12V accessory Voltage (lower 8 bits) (0.01V increments)
// byte 2: HV Bus Voltage (upper 8 bits) (0.1V increments)
// byte 3: HV Bus Voltage (lower 8 bits) (0.1V increments)
// byte 4: HV Bus Current (upper 8 bits) (0.1A increments)
// byte 5: HV Bus Current (lower 8 bits) (0.1A increments)
// byte 6: Motor Phase A Current (upper 8 bits) (0.1A increments)
// byte 7: Motor Phase A Current (lower 8 bits) (0.1A increments)
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//RMS Motor Controller Message Forwarding Format for motor RPM, torque and temperature and inverter temperature. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define RMS_MTR_TEMP_ADDR   0x117
// byte 0: Motor RPM (upper 8 bits) (RPM increments, 1 RPM = 1 increment)
// byte 1: Motor RPM (lower 8 bits) (RPM increments, 1 RPM = 1 increment)
// byte 2: Commanded Torque (upper 8 bits) (0.1Nm increments, 1 Nm = 10 increments)
// byte 3: Commanded Torque (lower 8 bits) (0.1Nm increments, 1 Nm = 10 increments)
// byte 4: Motor Temperature C (upper 8 bits) (degrees C, signed, 0.1C increments)
// byte 5: Motor Temperature C (lower 8 bits) (degrees C, signed, 0.1C increments)
// byte 6: Inverter Temperature C (upper 8 bits) (degrees C, signed, 0.1C increments)
// byte 7: Inverter Temperature C (lower 8 bits) (degrees C, signed, 0.1C increments)
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//RMS Motor Controller Message Forwarding Format for error codes. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define RMS_POST_FAULTS_ADDR   0x118
// byte 0: Post Faults (High) (upper 8 bits) (see RMS documentation for fault codes)
// byte 1: Post Faults (High) (lower 8 bits) (see RMS documentation for fault codes)
// byte 2: Post Faults (Low) (upper 8 bits) (see RMS documentation for fault codes)
// byte 3: Post Faults (Low) (lower 8 bits) (see RMS documentation for fault codes)
// byte 4: Run Faults (High) (upper 8 bits) (see RMS documentation for fault codes)
// byte 5: Run Faults (High) (lower 8 bits) (see RMS documentation for fault codes) 
// byte 6: Run Faults (Low) (upper 8 bits) (see RMS documentation for fault codes)
// byte 7: Run Faults (Low) (lower 8 bits) (see RMS documentation for fault codes) 
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//Power Controller CAN Message Format. UPDATE THIS WHEN YOU ADD FIELDS OR ADDITIONAL CAN DATA!
#define REAR_LEFT_DRIVER   0x95
// byte 0: 
// byte 1: 
// byte 2: 
// byte 3: 
// byte 4:
// byte 5:
// byte 6:
// byte 7: 
///////////////////////////////////////////////////////////////////////////////////////////////////





/// @brief Generic CAN bus message with address and data fields.
class LV_CANMessage{
  public:
  uint32_t addr = 0;        //CAN bus address of this message
  uint8_t byte0 = 0;
  uint8_t byte1 = 0;
  uint8_t byte2 = 0;
  uint8_t byte3 = 0;
  uint8_t byte4 = 0;
  uint8_t byte5 = 0;
  uint8_t byte6 = 0;
  uint8_t byte7 = 0;
  void update(uint32_t Can_addr, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7);
};

unsigned long convertBaudRateToParticle(unsigned long baudRate);
unsigned long convertBaudRateToMCP(unsigned long baudRate);

/// @brief Class to handle CAN bus controllers (either onboard on Photon or using the MCP2515 on P2/other microcontrollers).
class CAN_Controller{
    public:
    void addFilter(uint32_t address);
    bool receive(LV_CANMessage &outputMessage);
    void CANSend(uint16_t Can_addr, byte data0, byte data1, byte data2, byte data3, byte data4, byte data5, byte data6, byte data7);
    void CANSend(LV_CANMessage inputMessage);
    void changeCANSpeed(uint32_t newCanSpeed);
    uint32_t CurrentBaudRate();
    #if PLATFORM_ID == PLATFORM_PHOTON_PRODUCTION   //When running on a board with a photon, we'll use the internal controller, no need to specify chip select pin
    void begin(unsigned long baudRate);
    #else                                           //When running on a P2 or other, we need the MCP2515, which has a chip select pin you must specify.
    void begin(unsigned long baudRate, uint8_t chipSelectPin);
    #endif
    private:
    MCP_CAN *CAN0;
    uint8_t filterIndex;
    uint8_t csPin;
    uint32_t currentBaudRate;
};

/// @brief Class to send data from Dash Controller OR to receive CAN data from the Dash Controller on other boards.
class DashController_CAN{
    public:
    uint32_t boardAddress;      //The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
    byte rightTurnPWM;          //Brightness of the right turn signal. Value ranges from 0 (fully off) to 255 (fully on).
    byte leftTurnPWM;           //Brightness of the left turn signal. Value ranges from 0 (fully off) to 255 (fully on).
    byte batteryFanPWM;         //Fan percentage for the battery box fan. Value ranges from 0 (fully off) to 255 (max speed).
    bool headlight;             //Toggle switch for the car headlights. True turns on headlights, false turns off headlights.
    bool highbeam;              //Toggle switch for the car highbeams. True turns on highbeams, false turns off highbeams.
    bool reversePress;          //Toggle switch for being in reverse mode. Use to turn on/off reverse lights, backup camera, etc.
    byte driveMode;             //The gear that the user has requested (Park, Reverse, Forward, ...). Use the macros like DRIVE_MODE_PARK, DRIVE_MODE_NORMAL, etc.
    bool radiatorFan;           //Toggle to control the cooling fan for the motor controller.
    bool radiatorPump;          //Toggle to control the cooling pump for the motor controller.
    bool bmsFaultDetected;      //Flag that is set true if a Battery Management System fault has been detected.
    bool rmsFaultDetected;      //Flag that is set true if a Motor Controller fault has been detected.
    bool boardDetected;         //Flag set true in receiveCANData when a message from the Dash Controller has been received. Use this on other boards to check if you're hearing from the Dash Controller.

    DashController_CAN(uint32_t boardAddr);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);
    
};

/// @brief Class to send data from Power Controller OR to receive CAN data from the Power Controller on other boards.
class PowerController_CAN{
    public:
    uint32_t boardAddress;      //The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
    bool BrakeSense;            //Flag indicating if the brake pedal is being pressed.
    bool PushToStart;           //Flag indicating if the push to start button is being pressed.
    bool ACCharge;              //Flag indicating if the car is being charged from the wall.
    bool SolarCharge;           //Flag indicating if the car is in solar charge mode.
    bool Horn;                  //Flag indicating if the horn is being pressed.
    bool Acc;                   //Flag indicating if the car has its Accessory busbar active.
    bool Ign;                   //Flag indicating if the car has its Ignition busbar active.
    bool FullStart;
    bool CarOn;
    bool StartUp;               
    bool LowPowerMode;          //Flag indicating to the rest of the system that we are operating in Low Power Mode. Use this to update controls of other boards!
    bool LowACCBattery;         //Flag indicating that the 12V accessory is low (true) or normal (false).
    bool boardDetected;         //Flag set true in receiveCANData when a message from the Power Controller has been received. Use this on other boards to check if you're hearing from the Power Controller.

    PowerController_CAN(uint32_t boardAddr);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);

};

class LPDRV_RearLeft_CAN{
    public:
    uint32_t boardAddress;      //The CAN Bus address that this controller runs at, should be defined by DASH_CONTROL_ADDR
    bool bmsFaultInput;         //This board reads in the Battery Management System fault line and tells the rest of the system if we have a fault.
    bool switchFaultInput;      //This board reads in the manual kill switch fault line and tells the rest of the system if we have a fault.
    bool boardDetected;         //Flag set true in receiveCANData when a message from the Power Controller has been received. Use this on other boards to check if you're hearing from the Power Controller.
    LPDRV_RearLeft_CAN(uint32_t boardAddr);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);
};

/// @brief Class to send data from Dash Controller to Camry Instrument Cluster.
class CamryCluster_CAN{
    private:
    bool last_headlight;                //Internal flag to check last state of headlight before sending again to cluster
    bool last_highbeam;                 //Internal flag to check last state of highbeam before sending again to cluster
    uint32_t timer25ms;                 //Internal timer to keep track of 25ms packets
    uint32_t timer250ms;                //Internal timer to keep track of 250ms packets
    uint32_t timer1000ms;               //Internal timer to keep track of 1000ms packets
    void send25msPackets(CAN_Controller &controller);       //Send all the packets needed every 25ms to the instrument cluster
    void send250msPackets(CAN_Controller &controller);      //Send all the packets needed every 250ms to the instrument cluster
    void send1000msPackets(CAN_Controller &controller);     //Send all the packets needed every 1000ms to the instrument cluster

    public:
    bool brakeIcon;                     //Set true to turn on red BRAKE text on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool parkingBrakeCircle;            //Set true to turn on yellow circle with exclamation mark, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool seatBeltIcon;                  //Set true to turn on red seat belt icon on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool checkEngineOn;                 //Set true to turn on check engine indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool clusterBacklight;              //Set true to turn on backlight on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool oilPressureLow;                //Set true to turn on low oil pressure prompt on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool chargingSystemMalfunction;     //Set true to turn on low accessory battery indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint16_t motorTempDegC;             //Set to the motor temperature in degrees Celcius. Changes position of temperature dial on instrument cluster. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool powerSteeringIcon;             //Set true to turn on power steering icon on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t powerSteeringPrompt;        //Set value to show power steering prompt on LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_PowerPrompt;            //Set value to show powerup prompt on LCD. Use the macros such as LCD_HYBRID_SYSTEM_STOPPED and LCD_IGNITION_PROMPT to set the value. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_Brightness;             //Set the brightness of the cluster (low or high). Use macros LCD_BRIGHTNESS_HIGH and LCD_BRIGHTNESS_LOW. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool trunkOpen;                     //Set true to show the trunk being open on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool frontLeftDoor;                 //Set true to show the front left door being open on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool frontRightDoor;                //Set true to show the front right door being open on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool rearLeftDoor;                  //Set true to show the rear left door being open on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool rearRightDoor;                 //Set true to show the rear right door being open on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool animateStartup;                //Set true to show a fancy animation on the LCD when powering on. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_EngineStoppedCode;      //Set value to show engine error codes. Use macros LCD_ENGINE_NORMAL, LCD_ENGINE_STOPPED, LCD_ENGINE_STOPPED_BEEP. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_CheckEnginePrompt;      //Set value to show check engine codes. Use macros LCD_CHECK_ENGINE_NONE, LCD_CHECK_ENGINE, LCD_CHECK_ENGINE_REDUCED, LCD_CHECK_ENGINE_MAINTENANCE. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_ParkingBrakePrompt;     //Set value to show parking brake error codes. Use macros LCD_PBRK_GOOD, LCD_PBRK_BRAKE_ON, etc. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint16_t rpmGauge;                  //Set value to the motor RPM. Changes the instrument cluster gauge.
    uint16_t speedGauge;                //Set value to the vehicle speed. Changes the instrument cluster gauge.
    uint8_t ecoGauge;                   //Percentage of bars on the economy display. Ranges from 0 to 100.
    bool ecoLeaf;                       //Set the ECO leaf to show on the bottom right of the LCD. 
    bool fogLightOrange;                //Set true to turn on orange fog light indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool fogLightGreen;                 //Set true to turn on green fog light indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool headlight;                     //Set true to turn on headlight indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool highbeam;                      //Set true to turn on high beam indicator on instrument cluster, false to turn off. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t driveMode;                  //Set to the drive mode the Dashboard Controller is in such as DRIVE_MODE_PARK, DRIVE_MODE_FORWARD, DRIVE_MODE_REVERSE, etc. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t gearNumber;                 //Set this to a sport gear (1-10) if you're feeling adventurous. Shows sport gear next to drive mode on LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool sportMode;                     //Turns on sport mode on the instrument cluster (makes top banner red and shows sport text on bottom). See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool ecoMode;                       //Turns on eco mode on the instrument cluster (makes top banner blue and shows ECO Mode text on bottom). See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool readyToDrive;                  //Set true if the car is ready to move forward/backwards. Allows shifting to occur. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool crashBrakePrompt;              //Set true to show the crash brake prompt on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t clusterBeeps;               //Set if the cluster should be making noise. Use the BEEP_RATE macros to set the value. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    bool hudBlueLeftLane;               //Show the blue left lane departure on the HUD. Can be shown at same time as the regular left lane departure.
    bool hudBlueRightLane;              //Show the blue right lane departure on the HUD. Can be shown at same time as the regular right lane departure.
    uint8_t hudLeftLaneColor;           //Set the color of the left lane departure on the HUD. Use the macros HUD_LANE_OFF, HUD_LANE_CLEAR, HUD_LANE_WHITE, HUD_LANE_ORANGE. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t hudRightLaneColor;          //Set the color of the left lane departure on the HUD. Use the macros HUD_LANE_OFF, HUD_LANE_CLEAR, HUD_LANE_WHITE, HUD_LANE_ORANGE. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    uint8_t LCD_TakeBreak_Prompt;       //Set the value to show the "Take a Break" prompt on the LCD. See spreadsheet linked in CamryCluster_CAN::sendCANData for details.
    int outsideTemperatureF;            //Set the outside temperature in degrees F. Shows on the LCD.

    void initialize();
    void sendCANData(CAN_Controller &controller);
    //void receiveCANData(LV_CANMessage msg);
};

#endif
