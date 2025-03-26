/* RMS Frame ids. */
#define DBC_RMS_M172_TORQUE_AND_TIMER_INFO_FRAME_ID (0xacu)
#define DBC_RMS_M192_COMMAND_MESSAGE_FRAME_ID (0xc0u)
#define DBC_RMS_M171_FAULT_CODES_FRAME_ID (0xabu)
#define DBC_RMS_M170_INTERNAL_STATES_FRAME_ID (0xaau)
#define DBC_RMS_M169_INTERNAL_VOLTAGES_FRAME_ID (0xa9u)
#define DBC_RMS_M167_VOLTAGE_INFO_FRAME_ID (0xa7u)
#define DBC_RMS_M166_CURRENT_INFO_FRAME_ID (0xa6u)
#define DBC_RMS_M165_MOTOR_POSITION_INFO_FRAME_ID (0xa5u)
#define DBC_RMS_M162_TEMPERATURE_SET_3_FRAME_ID (0xa2u)
#define DBC_RMS_M161_TEMPERATURE_SET_2_FRAME_ID (0xa1u)
#define DBC_RMS_M188_U2_C_MESSAGE_RXD_FRAME_ID (0x1d5u)
#define DBC_RMS_BMS_CURRENT_LIMIT_FRAME_ID (0x202u)
#define DBC_RMS_M176_FAST_INFO_FRAME_ID (0xb0u)

/* RMS Unused Frame ids. */
#define DBC_RMS_M173_MODULATION_AND_FLUX_INFO_FRAME_ID (0xadu)
#define DBC_RMS_M194_READ_WRITE_PARAM_RESPONSE_FRAME_ID (0xc2u)
#define DBC_RMS_M193_READ_WRITE_PARAM_COMMAND_FRAME_ID (0xc1u)
#define DBC_RMS_M168_FLUX_ID_IQ_INFO_FRAME_ID (0xa8u)
#define DBC_RMS_M164_DIGITAL_INPUT_STATUS_FRAME_ID (0xa4u)
#define DBC_RMS_M163_ANALOG_INPUT_VOLTAGES_FRAME_ID (0xa3u)
#define DBC_RMS_M160_TEMPERATURE_SET_1_FRAME_ID (0xa0u)
#define DBC_RMS_M174_FIRMWARE_INFO_FRAME_ID (0xaeu)
#define DBC_RMS_M175_DIAG_DATA_FRAME_ID (0xafu)
#define DBC_RMS_M187_U2_C_COMMAND_TXD_FRAME_ID (0x1d7u)

/* BMS Frame ids. */
#define DBC_BMS_MSGID_0_X6_B0_FRAME_ID (0x6b0u)
#define DBC_BMS_MSGID_0_X6_B1_FRAME_ID (0x6b1u)
#define DBC_BMS_MSGID_0_X6_B2_FRAME_ID (0x6b2u)
#define DBC_BMS_MSGID_0_X6_B3_FRAME_ID (0x6b3u)
#define DBC_BMS_MSGID_0_X6_B4_FRAME_ID (0x6b4u)
#define DBC_BMS_MSGID_0_X6_B5_FRAME_ID (0x6b5u)
#define DBC_BMS_MSGID_0_X6_B6_FRAME_ID (0x6b6u)
#define DBC_BMS_MSGID_0_X36_CELLBCAST_FRAME_ID (0x36u)

#include "Particle.h"
#include <mcp_can.h>
#include "DecentralizedLV-Boards/DecentralizedLV-Boards.h"
#include "DecentralizedLV-Boards/HVBoards/dbc_rms.h"
#include "DecentralizedLV-Boards/HVBoards/dbc_bms.h"
#include "DecentralizedLV-Boards/HVBoards/canstruct.h"
#include "vector"
#include "map"

//Class to represent the Orion BMS on the Low Voltage CAN Bus. This class contains only necessary info that will be parsed from the HV CAN Bus
class OrionBMS {
    private:

    // BMS CANBUS Structs
    dbc_bms_msgid_0_x6_b0_t dbc_bms_msgid_0_x6_b0;
    dbc_bms_msgid_0_x6_b1_t dbc_bms_msgid_0_x6_b1;
    dbc_bms_msgid_0_x6_b2_t dbc_bms_msgid_0_x6_b2;
    dbc_bms_msgid_0_x6_b3_t dbc_bms_msgid_0_x6_b3;
    dbc_bms_msgid_0_x6_b4_t dbc_bms_msgid_0_x6_b4;
    dbc_bms_msgid_0_x6_b5_t dbc_bms_msgid_0_x6_b5;
    dbc_bms_msgid_0_x6_b6_t dbc_bms_msgid_0_x6_b6;

    // Missing the CELLBCAST message since that re-uses the same struct memebers and will get rewriten for each cellid.
    std::map<int, CAN_STRUCT*> bmscanmap =
    {
      {DBC_BMS_MSGID_0_X6_B0_FRAME_ID, &dbc_bms_msgid_0_x6_b0},
      {DBC_BMS_MSGID_0_X6_B1_FRAME_ID, &dbc_bms_msgid_0_x6_b1},
      {DBC_BMS_MSGID_0_X6_B2_FRAME_ID, &dbc_bms_msgid_0_x6_b2},
      {DBC_BMS_MSGID_0_X6_B3_FRAME_ID, &dbc_bms_msgid_0_x6_b3},
      {DBC_BMS_MSGID_0_X6_B4_FRAME_ID, &dbc_bms_msgid_0_x6_b4},
      {DBC_BMS_MSGID_0_X6_B5_FRAME_ID, &dbc_bms_msgid_0_x6_b5},
      {DBC_BMS_MSGID_0_X6_B6_FRAME_ID, &dbc_bms_msgid_0_x6_b6}
    };

    uint32_t packStatsAddr;             //CAN address for the pack statistics
    uint32_t cellStatsDTCAddr;          //CAN address for the cell statistics and DTC error codes
    uint32_t currentLimitTempAddr;      //CAN address for the current limits and temperatures
    uint32_t j1772Addr;                 //CAN address for the J1772 charger status

    void sendPackStats(CAN_Controller &controller);             //Sends the pack statistics to the LV CAN Bus
    void sendCellStatsDTC(CAN_Controller &controller);          //Sends the cell statistics and DTC error codes to the LV CAN Bus
    void sendCurrentLimitAndTemp(CAN_Controller &controller);   //Sends the current limits and temperatures to the LV CAN Bus
    void sendJ1772Stats(CAN_Controller &controller);            //Sends the J1772 charger status to the LV CAN Bus

    void receivePackStats(LV_CANMessage msg);                   //Receives the pack statistics from the board translating from the HV Bus and parses it into this object
    void receiveCellStatsDTC(LV_CANMessage msg);                //Receives the cell statistics and DTC error codes from the board translating from the HV Bus and parses it into this object
    void receiveCurrentLimitAndTemp(LV_CANMessage msg);         //Receives the current limits and temperatures from the board translating from the HV Bus and parses it into this object
    void receiveJ1772Stats(LV_CANMessage msg);                  //Receives the J1772 charger status from the board translating from the HV Bus and parses it into this object

    public:
    float packCurrentAmps;              //Current number of amps being charged/discharged from the pack
    float packInstantaneousVoltage;     //Raw voltage reading of the full pack
    float inputSupplyVoltage;           //12V voltage the BMS is getting
    float avgCellVoltage;               //Average voltage of all the cells in the pack (calculated by the BMS)
    float highestCellVoltage;           //Highest voltage of any cell in the pack (calculated by the BMS)
    float packAmpHours;                 //Amp hours of the pack (estimated by the BMS)
    float packResistanceOhms;           //Resistance estimated by the BMS for the pack (estimated by the BMS)
    float lowestCellVoltage;            //Lowest voltage of any cell in the pack (calculated by the BMS)
    float lowestCellResistanceOhms;     //Resistance of the lowest cell in the pack (calculated by the BMS)

    uint8_t bmsAverageTempC;              //Average temperature of the thermistors on the BMS itself (not expansion)
    uint8_t bmsInternalTempC;             //Internal thermistor on the BMS

    uint8_t thermistorHighTempC;          //Highest temperature read of all thermistors in the thermistor expansion module
    uint8_t thermistorLowTempC;           //Lowest temperature read of all thermistors in the thermistor expansion module
    
    uint8_t packSOC;                    //State of charge of the pack, 0-100%
    uint16_t chargeCurrentLimit;        //Charge current limit in amps, set by the Orion BMS. This is the maximum charge current that can be sent to the pack.
    uint16_t dischargeCurrentLimit;     //Discharge current limit in amps, set by the Orion BMS. This is the maximum discharge current that can be sent from the pack.

    bool j1772PlugState;                //True if the J1772 plug is connected to the BMS, false if not. This is used to determine if the car is charging or not.
    uint8_t j1772ACCurrentLimit;          //AC current limit set by the J1772 plug, in amps.
    uint8_t j1772ACVoltage;               //AC voltage from the J1772 plug, in volts.

    //https://www.orionbms.com/manuals/utility_o2/bms_param_dtc_status_1.html
    uint16_t dtcFlags1;         //Bit masks for error code type 1. See the Orion BMS manual for which bits represent which errors.
    uint16_t dtcFlags2;         //Bit masks for error code type 2. See the Orion BMS manual for which bits represent which errors.

    bool packStatsReceived;         //Flag set true in receiveCANData when a message from the Orion has been received. Use this on other boards to check if you're hearing from the Orion.
    bool cellStatsDTCReceived;      //Flag set true in receiveCANData when a message from the Orion has been received. Use this on other boards to check if you're hearing from the Orion.
    bool currentLimitTempReceived;  //Flag set true in receiveCANData when a message from the Orion has been received. Use this on other boards to check if you're hearing from the Orion.
    bool j1772Received;             //Flag set true in receiveCANData when a message from the Orion has been received. Use this on other boards to check if you're hearing from the Orion.

    OrionBMS(uint32_t packStatsAddress, uint32_t cellStatsDTCAddress, uint32_t currentLimitTempAddress, uint32_t j1772Address);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);     //Receives data from the HV Controller (or whichever board is translating the HV CAN Bus to the LV CAN Bus) and parses it into this object
    void receiveHVCANData(LV_CANMessage msg);   //Takes messages from the HV CAN Bus and parses them into this object which can then be sent on the LV CAN Bus
};

//Class to represent the Orion BMS on the Low Voltage CAN Bus. This class contains only necessary info that will be parsed from the HV CAN Bus
class RMSController {
    private:

    // RMS CANBUS Structs. These structs parse and decode CAN Bus messages.
    dbc_rms_m192_command_message_t dbc_rms_m192_command_message;
    dbc_rms_m171_fault_codes_t dbc_rms_m171_fault_codes;
    dbc_rms_m170_internal_states_t dbc_rms_m170_internal_states;
    dbc_rms_m169_internal_voltages_t dbc_rms_m169_internal_voltages;
    dbc_rms_m167_voltage_info_t dbc_rms_m167_voltage_info;
    dbc_rms_m166_current_info_t dbc_rms_m166_current_info;
    dbc_rms_m165_motor_position_info_t dbc_rms_m165_motor_position_info;
    dbc_rms_m162_temperature_set_3_t dbc_rms_m162_temperature_set_3;
    dbc_rms_m161_temperature_set_2_t dbc_rms_m161_temperature_set_2;
    dbc_rms_m188_u2_c_message_rxd_t dbc_rms_m188_u2_c_message_rxd;
    dbc_rms_bms_current_limit_t dbc_rms_bms_current_limit;
    dbc_rms_m176_fast_info_t dbc_rms_m176_fast_info;
    dbc_rms_m172_torque_and_timer_info_t dbc_rms_m172_torque_and_timer_info;

    std::map<int, CAN_STRUCT*> rmscanmap =
    {
      {DBC_RMS_M192_COMMAND_MESSAGE_FRAME_ID, &dbc_rms_m192_command_message},
      {DBC_RMS_M171_FAULT_CODES_FRAME_ID, &dbc_rms_m171_fault_codes},
      {DBC_RMS_M170_INTERNAL_STATES_FRAME_ID, &dbc_rms_m170_internal_states},
      {DBC_RMS_M169_INTERNAL_VOLTAGES_FRAME_ID, &dbc_rms_m169_internal_voltages},
      {DBC_RMS_M167_VOLTAGE_INFO_FRAME_ID, &dbc_rms_m167_voltage_info},
      {DBC_RMS_M166_CURRENT_INFO_FRAME_ID, &dbc_rms_m166_current_info},
      {DBC_RMS_M165_MOTOR_POSITION_INFO_FRAME_ID, &dbc_rms_m165_motor_position_info},
      {DBC_RMS_M162_TEMPERATURE_SET_3_FRAME_ID, &dbc_rms_m162_temperature_set_3},
      {DBC_RMS_M161_TEMPERATURE_SET_2_FRAME_ID, &dbc_rms_m161_temperature_set_2},
      {DBC_RMS_M188_U2_C_MESSAGE_RXD_FRAME_ID, &dbc_rms_m188_u2_c_message_rxd},
      {DBC_RMS_BMS_CURRENT_LIMIT_FRAME_ID, &dbc_rms_bms_current_limit},
      {DBC_RMS_M176_FAST_INFO_FRAME_ID, &dbc_rms_m176_fast_info},
      {DBC_RMS_M172_TORQUE_AND_TIMER_INFO_FRAME_ID, &dbc_rms_m172_torque_and_timer_info}
    };

    uint32_t powerStatAddr;             //CAN address for the power statistics (accessory voltage, bus voltage, bus current, etc.)
    uint32_t motorTempAddr;             //CAN address for the motor statistics and inverter temperature
    uint32_t faultsAddr;                //CAN address for the fault codes

    void sendPowerStats(CAN_Controller &controller);             //Sends the power statistics to the LV CAN Bus
    void sendMotorTemp(CAN_Controller &controller);              //Sends the motor statistics and inverter temperature to the LV CAN Bus
    void sendFaults(CAN_Controller &controller);                 //Sends the fault codes to the LV CAN Bus
    
    void receivePowerStats(LV_CANMessage msg);                   //Receives the power statistics from the board translating from the HV Bus and parses it into this object
    void receiveMotorTemp(LV_CANMessage msg);                    //Receives the motor statistics and inverter temperature from the board translating from the HV Bus and parses it into this object
    void receiveFaults(LV_CANMessage msg);                       //Receives the fault codes from the board translating from the HV Bus and parses it into this object

    public:

    //https://wiki.neweagle.net/docs/Rinehart/PM100_User_Manual_3_2011.pdf
    uint16_t postFaultHigh;             //Upper bits of the post fault code. Check page 47 of the doc linked above for info.
    uint16_t postFaultLow;              //Lower bits of the post fault code. Check page 47 of the doc linked above for info.
    uint16_t runFaultHigh;              //Upper bits of the run fault code. Check page 47 of the doc linked above for info.
    uint16_t runFaultLow;               //Lower bits of the run fault code. Check page 47 of the doc linked above for info.

    float accessoryVoltage;             //Reading of the 12V bus from the RMS.
    float busVoltage;                   //Reading of the high voltage bus from the RMS.
    float busCurrent;                   //Reading of the high voltage bus current from the RMS.
    float commandedTorque;              //Pedal commanded torque (Nm).
    float rmsPhaseACurrent;             //Reading of the phase A current from the RMS.
    float motorTemperatureC;            //Reading of the motor temperature from the RMS.
    float inverterTemperatureC;         //Reading of the control board temperature from the RMS.

    uint16_t motorRPM;           //Reading of the motor RPM from the RMS. THIS IS NOT ALWAYS ACCURATE. Returns 0 when pedal is released.

    bool faultActive;           //Flag indicating if the RMS is in a fault state. This is set true if any of the fault codes are non-zero.

    bool powerStatsReceived;         //Flag set true in receiveCANData when a message from the RMS has been received. Use this on other boards to check if you're hearing from the RMS.
    bool motorTempReceived;         //Flag set true in receiveCANData when a message from the RMS has been received. Use this on other boards to check if you're hearing from the RMS.
    bool faultsReceived;            //Flag set true in receiveCANData when a message from the RMS has been received. Use this on other boards to check if you're hearing from the RMS.

    RMSController(uint32_t powerStatAddress, uint32_t motorTempAddress, uint32_t faultsAddress);
    void initialize();
    void sendCANData(CAN_Controller &controller);
    void receiveCANData(LV_CANMessage msg);     //Receives data from the HV Controller (or whichever board is translating the HV CAN Bus to the LV CAN Bus) and parses it into this object
    void receiveHVCANData(LV_CANMessage msg);   //Takes messages from the HV CAN Bus and parses them into this object which can then be sent on the LV CAN Bus
};