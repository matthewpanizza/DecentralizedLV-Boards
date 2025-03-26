#include "DecentralizedLV-HVBoards.h"
#include "vector"
#include "map"


std::vector<float> cell_telemetry(105);
unsigned int cell_telemetry_counter = 0;

OrionBMS::OrionBMS(uint32_t packStatsAddress, uint32_t cellStatsDTCAddress, uint32_t currentLimitTempAddress, uint32_t j1772Address)
{
  packStatsAddr = packStatsAddress;
  cellStatsDTCAddr = cellStatsDTCAddress;
  currentLimitTempAddr = currentLimitTempAddress;
  j1772Addr = j1772Address;
}

void OrionBMS::initialize()
{
  packCurrentAmps = 0.0;                     //Current number of amps being charged/discharged from the pack
  packInstantaneousVoltage = 0.0;            //Raw voltage reading of the full pack
  inputSupplyVoltage = 0.0;                  //12V voltage the BMS is getting
  avgCellVoltage = 0.0;                      //Average voltage of all the cells in the pack (calculated by the BMS)
  highestCellVoltage = 0.0;                  //Highest voltage of any cell in the pack (calculated by the BMS)
  lowestCellVoltage = 0.0;                   //Lowest voltage of any cell in the pack (calculated by the BMS)
  packAmpHours = 0.0;                        //Amp hours of the pack (estimated by the BMS)
  packResistanceOhms = 0.0;                  //Resistance estimated by the BMS for the pack (estimated by the BMS)
  lowestCellResistanceOhms = 0.0;            //Resistance of the lowest cell in the pack (calculated by the BMS)
  dtcFlags1 = 0;                             //Bit masks for error code type 1. See the Orion BMS manual for which bits represent which errors.
  dtcFlags2 = 0;                             //Bit masks for error code type 2. See the Orion BMS manual for which bits represent which errors.
  dischargeCurrentLimit = 0;                 //Discharge current limit in amps, set by the Orion BMS. This is the maximum discharge current that can be sent from the pack.
  chargeCurrentLimit = 0;                    //Charge current limit in amps, set by the Orion BMS. This is the maximum charge current that can be sent to the pack.
  bmsAverageTempC = 0;                       //Average temperature of the thermistors on the BMS itself (not expansion)
  bmsInternalTempC = 0;                      //Internal thermistor on the BMS
  thermistorHighTempC = 0;                   //Highest temperature read of all thermistors in the thermistor expansion module
  thermistorLowTempC = 0;                    //Lowest temperature read of all thermistors in the thermistor expansion module
  j1772PlugState = false;                    //True if the J1772 plug is connected to the BMS, false if not. This is used to determine if the car is charging or not.
  j1772ACCurrentLimit = 0;                   //AC current limit set by the J1772 plug, in amps.
  j1772ACVoltage = 0;                        //AC voltage from the J1772 plug, in volts.
  packStatsReceived = false;       
  cellStatsDTCReceived = false;    
  currentLimitTempReceived = false;
  j1772Received = false;           
}

void OrionBMS::sendPackStats(CAN_Controller &controller){
  uint16_t packCurrentTemp = (uint16_t)(packCurrentAmps * 10);                  //Convert to 0.1A increments
  uint16_t packVoltageTemp = (uint16_t)(packInstantaneousVoltage * 10);         //Convert to 0.1V increments
  uint8_t packAmpHoursTemp = (uint8_t)(packAmpHours * 10);                      //Convert to 0.1Ah increments
  uint8_t packResistanceTemp = (uint8_t)(packResistanceOhms * 1000);            //Convert to 1mOhm increments
  uint8_t inputSupplyVoltageTemp = (uint8_t)(inputSupplyVoltage * 10);          //Convert to 0.1V increments
  uint8_t packSOCTemp = (uint8_t)packSOC;                                       //State of charge is already in 0-100% increments

  controller.CANSend(packStatsAddr, 
    (uint8_t)(packCurrentTemp >> 8), (uint8_t)(packCurrentTemp & 0xFF),
    (uint8_t)(packVoltageTemp >> 8), (uint8_t)(packVoltageTemp & 0xFF),
    packAmpHoursTemp, packResistanceTemp, packSOCTemp, inputSupplyVoltageTemp);
}

void OrionBMS::sendCellStatsDTC(CAN_Controller &controller){
  uint8_t avgCellVoltageTemp = (uint8_t)(avgCellVoltage * 10);                  //Convert to 0.1V increments
  uint8_t highestCellVoltageTemp = (uint8_t)(highestCellVoltage * 10);          //Convert to 0.1V increments
  uint8_t lowestCellVoltageTemp = (uint8_t)(lowestCellVoltage * 10);            //Convert to 0.1V increments
  uint8_t lowestCellResistanceTemp = (uint8_t)(lowestCellResistanceOhms * 10);  //Convert to 0.1mOhm increments

  controller.CANSend(cellStatsDTCAddr, 
    avgCellVoltageTemp, highestCellVoltageTemp, lowestCellVoltageTemp, lowestCellResistanceTemp,
    (uint8_t)(dtcFlags1 >> 8), (uint8_t)(dtcFlags1 & 0xFF),
    (uint8_t)(dtcFlags2 >> 8), (uint8_t)(dtcFlags2 & 0xFF));
}

void OrionBMS::sendCurrentLimitAndTemp(CAN_Controller &controller){
  uint16_t dischargeCurrentLimitTemp = (uint16_t)(dischargeCurrentLimit);       //Convert to 1A increments
  uint16_t chargeCurrentLimitTemp = (uint16_t)(chargeCurrentLimit);             //Convert to 1A increments

  controller.CANSend(currentLimitTempAddr, 
    (uint8_t)(dischargeCurrentLimitTemp >> 8), (uint8_t)(dischargeCurrentLimitTemp & 0xFF),
    (uint8_t)(chargeCurrentLimitTemp >> 8), (uint8_t)(chargeCurrentLimitTemp & 0xFF),
    bmsAverageTempC, bmsInternalTempC, thermistorHighTempC, thermistorLowTempC);
}

void OrionBMS::sendJ1772Stats(CAN_Controller &controller){
  controller.CANSend(j1772Addr, 
    (uint8_t)j1772PlugState, j1772ACCurrentLimit, j1772ACVoltage, 0, 0, 0, 0, 0);
}

void OrionBMS::sendCANData(CAN_Controller &controller)
{
  sendPackStats(controller);            //Sends the pack statistics to the LV CAN Bus
  sendCellStatsDTC(controller);         //Sends the cell statistics and DTC error codes to the LV CAN Bus
  sendCurrentLimitAndTemp(controller);  //Sends the current limits and temperatures to the LV CAN Bus
  sendJ1772Stats(controller);           //Sends the J1772 charger status to the LV CAN Bus
}

void OrionBMS::receivePackStats(LV_CANMessage msg)
{
  if(msg.addr != packStatsAddr) return; //Ignore messages not meant for this address

  uint16_t packCurrentTemp = (uint16_t)(msg.byte0 << 8 | msg.byte1);                  //Convert to 0.1A increments 
  uint16_t packVoltageTemp = (uint16_t)(msg.byte2 << 8 | msg.byte3);                 //Convert to 0.1V increments

  packCurrentAmps = (float)(packCurrentTemp / 10.0);                                     //Convert to amps
  packInstantaneousVoltage = (float)(packVoltageTemp / 10.0);                            //Convert to volts
  packAmpHours = (float)(msg.byte4 / 10.0);                                              //Convert to amp hours
  packResistanceOhms = (float)(msg.byte5 / 1000.0);                                      //Convert to ohms
  packSOC = (uint8_t)msg.byte6;                                                          //State of charge is already in 0-100%
  inputSupplyVoltage = (float)(msg.byte7 / 10.0);                                        //Convert to volts

  packStatsReceived = true;                                                              //Set the flag to true to indicate that pack stats have been received
}

void OrionBMS::receiveCellStatsDTC(LV_CANMessage msg)
{
  if(msg.addr != cellStatsDTCAddr) return; //Ignore messages not meant for this address

  avgCellVoltage = (float)(msg.byte0 / 10.0);                                            //Convert to volts
  highestCellVoltage = (float)(msg.byte1 / 10.0);                                        //Convert to volts
  lowestCellVoltage = (float)(msg.byte2 / 10.0);                                         //Convert to volts
  lowestCellResistanceOhms = (float)(msg.byte3 / 10.0);                                  //Convert to ohms
  dtcFlags1 = (uint16_t)(msg.byte4 << 8 | msg.byte5);                                   //Bit masks for error code type 1
  dtcFlags2 = (uint16_t)(msg.byte6 << 8 | msg.byte7);                                   //Bit masks for error code type 2

  cellStatsDTCReceived = true;                                                           //Set the flag to true to indicate that cell stats and DTC have been received
}

void OrionBMS::receiveCurrentLimitAndTemp(LV_CANMessage msg)
{
  if(msg.addr != currentLimitTempAddr) return; //Ignore messages not meant for this address

  dischargeCurrentLimit = (uint16_t)(msg.byte0 << 8 | msg.byte1);                       //Convert to amps
  chargeCurrentLimit = (uint16_t)(msg.byte2 << 8 | msg.byte3);                          //Convert to amps
  bmsAverageTempC = (uint8_t)msg.byte4;                                                 //Average temperature of the BMS
  bmsInternalTempC = (uint8_t)msg.byte5;                                                //Internal temperature of the BMS
  thermistorHighTempC = (uint8_t)msg.byte6;                                             //Highest temperature of the thermistor expansion module
  thermistorLowTempC = (uint8_t)msg.byte7;                                              //Lowest temperature of the thermistor expansion module

  currentLimitTempReceived = true;                                                       //Set the flag to true to indicate that current limits and temperatures have been received
}

void OrionBMS::receiveJ1772Stats(LV_CANMessage msg)
{
  if(msg.addr != j1772Addr) return; //Ignore messages not meant for this address

  j1772PlugState = (bool)msg.byte0;                                                    //True if the J1772 plug is connected to the BMS
  j1772ACCurrentLimit = (uint8_t)msg.byte1;                                            //AC current limit set by the J1772 plug, in amps
  j1772ACVoltage = (uint8_t)msg.byte2;                                                 //AC voltage from the J1772 plug, in volts

  j1772Received = true;                                                                //Set the flag to true to indicate that J1772 stats have been received
}

void OrionBMS::receiveCANData(LV_CANMessage msg)
{
  receivePackStats(msg);            //Receives the pack statistics from the board translating from the HV Bus and parses it into this object
  receiveCellStatsDTC(msg);         //Receives the cell statistics and DTC error codes from the board translating from the HV Bus and parses it into this object
  receiveCurrentLimitAndTemp(msg);  //Receives the current limits and temperatures from the board translating from the HV Bus and parses it into this object
  receiveJ1772Stats(msg);           //Receives the J1772 charger status from the board translating from the HV Bus and parses it into this object
}

void OrionBMS::receiveHVCANData(LV_CANMessage msg)
{
  auto bms = bmscanmap.find(msg.addr);

  if (bms != bmscanmap.end()) {
    // Found the ID in the BMS CAN Map
    // The unpack will automatically feed the message into the appropriate struct for parsing the data
    // Conversion from LV_CANMessage to uint8_t array for unpacking
    //Serial.printlnf("Found BMS ID: %X", msg.addr);
    uint8_t data[8] = {msg.byte0, msg.byte1, msg.byte2, msg.byte3, msg.byte4, msg.byte5, msg.byte6, msg.byte7};
    bms->second->unpack(data, msg.addr);
    return;
  }

  // BMS pack statistics
  packCurrentAmps = (float)dbc_bms_msgid_0_x6_b0.pack_current_decode();                 //2 bytes
  packInstantaneousVoltage = (float)dbc_bms_msgid_0_x6_b0.pack_inst_voltage_decode();   //2 bytes
  packAmpHours = (float)dbc_bms_msgid_0_x6_b2.pack_amphours_decode();                   //1 byte
  packResistanceOhms = (float)dbc_bms_msgid_0_x6_b2.pack_resistance_decode();           //1 byte
  packSOC = (uint8_t)dbc_bms_msgid_0_x6_b0.pack_soc_decode();                           //1 byte          
  //teleP->bms_pack_soc = float_map(teleP->bms_pack_inst_voltage / 104, 2.55, 4, 0, 100);
  inputSupplyVoltage = (float)dbc_bms_msgid_0_x6_b5.input_supply_voltage_decode();      //1 byte

  //Cell voltages and resistance
  avgCellVoltage = (float)dbc_bms_msgid_0_x6_b6.avg_cell_voltage_decode();              //1 byte
  highestCellVoltage = (float)dbc_bms_msgid_0_x6_b6.high_cell_voltage_decode();         //1 byte
  lowestCellVoltage = (float)dbc_bms_msgid_0_x6_b3.low_cell_voltage_decode();           //1 byte
  lowestCellResistanceOhms = (float)dbc_bms_msgid_0_x6_b4.low_cell_resistance_decode(); //1 byte

  //DTC (Error) Codes
  dtcFlags1 = (uint16_t)dbc_bms_msgid_0_x6_b6.dtc_flags_1_decode();                     //2 bytes
  dtcFlags2 = (uint16_t)dbc_bms_msgid_0_x6_b6.dtc_flags_2_decode();                     //2 bytes

  //Charge current limits
  dischargeCurrentLimit = (uint16_t)dbc_bms_msgid_0_x6_b1.pack_dcl_decode();            //2 bytes
  chargeCurrentLimit = (uint16_t)dbc_bms_msgid_0_x6_b1.pack_ccl_decode();               //2 bytes

  //BMS Temperatures
  bmsAverageTempC = (uint8_t)dbc_bms_msgid_0_x6_b3.average_temperature_decode();        //1 byte
  bmsInternalTempC = (uint8_t)dbc_bms_msgid_0_x6_b3.internal_temperature_decode();      //1 byte
  thermistorHighTempC = (uint8_t)dbc_bms_msgid_0_x6_b1.high_temperature_decode();       //1 byte
  thermistorLowTempC = (uint8_t)dbc_bms_msgid_0_x6_b1.low_temperature_decode();         //1 byte

  j1772PlugState = (bool)dbc_bms_msgid_0_x6_b3.j1772_plug_state_decode();               //1 bit
  j1772ACCurrentLimit = (uint8_t)dbc_bms_msgid_0_x6_b3.j1772_ac_current_limit_decode(); //1 byte
  j1772ACVoltage = (uint8_t)dbc_bms_msgid_0_x6_b5.j1772_ac_voltage_decode();            //1 byte

}

RMSController::RMSController(uint32_t powerStatAddress, uint32_t motorTempAddress, uint32_t faultsAddress)
{
  powerStatAddr = powerStatAddress;
  motorTempAddr = motorTempAddress;
  faultsAddr = faultsAddress;
}

void RMSController::initialize()
{
  accessoryVoltage = 0.0;            //12V reference voltage
  busVoltage = 0.0;                   //DC bus voltage
  busCurrent = 0.0;                   //DC bus current
  rmsPhaseACurrent = 0.0;             //Phase A current
  motorRPM = 0;                       //Motor RPM
  commandedTorque = 0.0;              //Commanded torque
  motorTemperatureC = 0.0;            //Motor temperature in degrees C
  inverterTemperatureC = 0.0;         //Inverter temperature in degrees C
  postFaultHigh = 0;                  //Post fault high code
  postFaultLow = 0;                   //Post fault low code
  runFaultHigh = 0;                   //Run fault high code
  runFaultLow = 0;                    //Run fault low code
  faultActive = false;                //Flag indicating if the RMS is in a fault state
  powerStatsReceived = false;         //Flag indicating if power statistics have been received
  motorTempReceived = false;         //Flag indicating if motor temperature has been received
  faultsReceived = false;            //Flag indicating if fault codes have been received
}

void RMSController::sendPowerStats(CAN_Controller &controller)
{
  uint16_t busVoltageTemp = (uint16_t)(busVoltage * 10);                    //Convert to 0.1V increments
  uint16_t busCurrentTemp = (uint16_t)(busCurrent * 10);                    //Convert to 0.1A increments
  uint16_t accessoryVoltageTemp = (uint16_t)(accessoryVoltage * 100);       //Convert to 0.01V increments
  uint16_t phACurrentTemp = (uint16_t)(rmsPhaseACurrent * 10);              //Convert to 0.1A increments

  controller.CANSend(powerStatAddr, 
    (uint8_t)(accessoryVoltageTemp >> 8), (uint8_t)(accessoryVoltageTemp & 0xFF),
    (uint8_t)(busVoltageTemp >> 8), (uint8_t)(busVoltageTemp & 0xFF),
    (uint8_t)(busCurrentTemp >> 8), (uint8_t)(busCurrentTemp & 0xFF), 
    (uint8_t)(phACurrentTemp >> 8), (uint8_t)(phACurrentTemp & 0xFF));       //Phase A current is already in 0.1A increments
}

void RMSController::sendMotorTemp(CAN_Controller &controller)
{
  uint16_t motorRPMTemp = (uint16_t)motorRPM;                                //Motor RPM is already in 1 RPM increments
  uint16_t motorTemperatureTemp = (uint16_t)(motorTemperatureC * 10);        //Convert to 0.1C increments
  uint16_t inverterTemperatureTemp = (uint16_t)(inverterTemperatureC * 10);  //Convert to 0.1C increments
  uint16_t commandedTorqueTemp = (uint16_t)(commandedTorque * 10);           //Convert to 0.1Nm increments

  controller.CANSend(motorTempAddr, 
    (uint8_t)(motorRPMTemp >> 8), (uint8_t)(motorRPMTemp & 0xFF),
    (uint8_t)(motorTemperatureTemp >> 8), (uint8_t)(motorTemperatureTemp & 0xFF),
    (uint8_t)(inverterTemperatureTemp >> 8), (uint8_t)(inverterTemperatureTemp & 0xFF), 
    (uint8_t)(commandedTorqueTemp >> 8), (uint8_t)(commandedTorqueTemp & 0xFF));    //Commanded torque is already in Nm increments
}

void RMSController::sendFaults(CAN_Controller &controller)
{
  controller.CANSend(faultsAddr, 
    (uint8_t)(postFaultHigh >> 8), (uint8_t)(postFaultHigh & 0xFF),
    (uint8_t)(postFaultLow >> 8), (uint8_t)(postFaultLow & 0xFF),
    (uint8_t)(runFaultHigh >> 8), (uint8_t)(runFaultHigh & 0xFF), 
    (uint8_t)(runFaultLow >> 8), (uint8_t)(runFaultLow & 0xFF));       //Faults already in 1 increments
}

void RMSController::sendCANData(CAN_Controller &controller)
{
  sendPowerStats(controller);            //Sends the power statistics to the LV CAN Bus
  sendMotorTemp(controller);              //Sends the motor statistics and inverter temperature to the LV CAN Bus
  sendFaults(controller);                 //Sends the fault codes to the LV CAN Bus
}

void RMSController::receivePowerStats(LV_CANMessage msg)
{
  if (msg.addr != powerStatAddr) return; //Ignore messages not meant for this address

  uint16_t accessoryVoltageTemp = (uint16_t)(msg.byte0 << 8 | msg.byte1);            //Convert to 0.01V increments
  uint16_t busVoltageTemp = (uint16_t)(msg.byte2 << 8 | msg.byte3);                  //Convert to 0.1V increments
  uint16_t busCurrentTemp = (uint16_t)(msg.byte4 << 8 | msg.byte5);                  //Convert to 0.1A increments
  uint16_t phACurrentTemp = (uint16_t)(msg.byte6 << 8 | msg.byte7);                  //Convert to 0.1A increments

  accessoryVoltage = (float)(accessoryVoltageTemp / 100.0);                          //Convert to volts
  busVoltage = (float)(busVoltageTemp / 10.0);                                        //Convert to volts
  busCurrent = (float)(busCurrentTemp / 10.0);                                        //Convert to amps
  rmsPhaseACurrent = (float)(phACurrentTemp / 10.0);                                  //Convert to amps

  powerStatsReceived = true;                                                          //Set the flag to true to indicate that power stats have been received
}

void RMSController::receiveMotorTemp(LV_CANMessage msg)
{
  if (msg.addr != motorTempAddr) return; //Ignore messages not meant for this address

  uint16_t motorRPMTemp = (uint16_t)(msg.byte0 << 8 | msg.byte1);                    //Motor RPM is already in 1 RPM increments
  uint16_t motorTemperatureTemp = (uint16_t)(msg.byte2 << 8 | msg.byte3);            //Convert to 0.1C increments
  uint16_t inverterTemperatureTemp = (uint16_t)(msg.byte4 << 8 | msg.byte5);          //Convert to 0.1C increments
  uint16_t commandedTorqueTemp = (uint16_t)(msg.byte6 << 8 | msg.byte7);             //Convert to 0.1Nm increments

  motorRPM = (uint16_t)motorRPMTemp;                                                 //Motor RPM is already in 1 RPM increments
  motorTemperatureC = (float)(motorTemperatureTemp / 10.0);                          //Convert to degrees C
  inverterTemperatureC = (float)(inverterTemperatureTemp / 10.0);                    //Convert to degrees C
  commandedTorque = (float)(commandedTorqueTemp / 10.0);                             //Convert to Nm

  motorTempReceived = true;                                                           //Set the flag to true to indicate that motor temp has been received
}

void RMSController::receiveFaults(LV_CANMessage msg)
{
  if (msg.addr != faultsAddr) return; //Ignore messages not meant for this address

  uint16_t postFaultHighTemp = (uint16_t)(msg.byte0 << 8 | msg.byte1);               //Post fault high code
  uint16_t postFaultLowTemp = (uint16_t)(msg.byte2 << 8 | msg.byte3);                //Post fault low code
  uint16_t runFaultHighTemp = (uint16_t)(msg.byte4 << 8 | msg.byte5);                 //Run fault high code
  uint16_t runFaultLowTemp = (uint16_t)(msg.byte6 << 8 | msg.byte7);                  //Run fault low code

  postFaultHigh = (uint16_t)postFaultHighTemp;                                        //Post fault high code
  postFaultLow = (uint16_t)postFaultLowTemp;                                          //Post fault low code
  runFaultHigh = (uint16_t)runFaultHighTemp;                                          //Run fault high code
  runFaultLow = (uint16_t)runFaultLowTemp;                                            //Run fault low code

  faultsReceived = true;                                                              //Set the flag to true to indicate that faults have been received
}

void RMSController::receiveCANData(LV_CANMessage msg)
{
  receivePowerStats(msg);            //Receives the power statistics from the board translating from the HV Bus and parses it into this object
  receiveMotorTemp(msg);              //Receives the motor statistics and inverter temperature from the board translating from the HV Bus and parses it into this object
  receiveFaults(msg);                 //Receives the fault codes from the board translating from the HV Bus and parses it into this object
}

void RMSController::receiveHVCANData(LV_CANMessage msg)
{

  auto rms = rmscanmap.find(msg.addr);
  if (rms != rmscanmap.end())
  {
    // Found the ID in the RMS CAN Map
    // The unpack will automatically feed the message into the appropriate struct for parsing the data
    // Conversion from LV_CANMessage to uint8_t array for unpacking
    uint8_t data[8] = {msg.byte0, msg.byte1, msg.byte2, msg.byte3, msg.byte4, msg.byte5, msg.byte6, msg.byte7};
    rms->second->unpack(data, msg.addr);
    return;
  }

  //RMS Voltages and Currents
  accessoryVoltage = (float)dbc_rms_m169_internal_voltages.d4_reference_voltage_12_0_decode();            // 2 bytes
  busVoltage = (float)dbc_rms_m167_voltage_info.d1_dc_bus_voltage_decode();		                            // 2 bytes
  busCurrent = (float)dbc_rms_m166_current_info.d4_dc_bus_current_decode();                               // 2 bytes
  rmsPhaseACurrent = (float)dbc_rms_m166_current_info.d1_phase_a_current_decode();                        // 2 bytes

  //Motor and Temperature Info
  motorRPM = (uint16_t)dbc_rms_m165_motor_position_info.d2_motor_speed_decode();                          // 2 bytes
  commandedTorque = dbc_rms_m172_torque_and_timer_info.d1_commanded_torque_decode();                      // 2 bytes
  motorTemperatureC = (float)dbc_rms_m162_temperature_set_3.d3_motor_temperature_decode();                // 2 bytes
  inverterTemperatureC = (float)dbc_rms_m161_temperature_set_2.d1_control_board_temperature_decode();     // 2 bytes

  // RMS Run Faults
  postFaultHigh = (uint16_t)dbc_rms_m171_fault_codes.d2_post_fault_hi_decode();                           // 2 bytes
  postFaultLow = (uint16_t)dbc_rms_m171_fault_codes.d1_post_fault_lo_decode();                            // 2 bytes
  runFaultHigh = (uint16_t)dbc_rms_m171_fault_codes.d4_run_fault_hi_decode();                             // 2 bytes
  runFaultLow = (uint16_t)dbc_rms_m171_fault_codes.d3_run_fault_lo_decode();                              // 2 bytes

  //teleP->rms_motor_speed = (float)dbc_rms_m176_fast_info.fast_motor_speed_decode();
  //Serial.println(teleP->rms_motor_speed);

  faultActive = (runFaultLow > 0) || (postFaultLow > 0) || (postFaultHigh > 0); //Set the faultActive flag if any of the fault codes are non-zero
}
