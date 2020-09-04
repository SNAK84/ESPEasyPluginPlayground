//#######################################################################################################
//################### Plugin 247 PZEM-004Tv30 AC Current and Voltage measurement sensor ####################
//#######################################################################################################
//
// This plugin is interfacing with PZEM-004Tv30 Sesor with softserial communication as the sensor
// has an UART pinout (TX/RX/VCC/GND)
//
#define PLUGIN_BUILD_TESTING

#ifdef PLUGIN_BUILD_TESTING

#include <SoftwareSerial.h>
#include <PZEM004Tv30.h>
PZEM004Tv30 *Plugin_247_pzem;
//IPAddress pzemIP(192,168,1,1);    // required by the library but not used (dummy value)
//uint8_t pzemIP = 0x01;

#define PLUGIN_247
#define PLUGIN_ID_247 247
#define PLUGIN_247_DEBUG false //activate extra log info in the debug
#define PLUGIN_NAME_247 "Voltage & Current (AC) - PZEM-004Tv30 [TESTING]"
#define PLUGIN_VALUENAME1_247 "Voltage"
#define PLUGIN_VALUENAME2_247 "Current"
#define PLUGIN_VALUENAME3_247 "Power"
#define PLUGIN_VALUENAME4_247 "Energy"
#define PLUGIN_VALUENAME5_247 "Frequency"
#define PLUGIN_VALUENAME6_247 "PF"

// local parameter for this plugin
#define PZEM_MAX_ATTEMPT 3

boolean Plugin_247(byte function, struct EventStruct *event, String &string)
{
  boolean success = false;

  switch (function)
  {
  case PLUGIN_DEVICE_ADD:
  {
    Device[++deviceCount].Number = PLUGIN_ID_247;
    Device[deviceCount].Type = DEVICE_TYPE_DUAL;
    Device[deviceCount].VType = SENSOR_TYPE_QUAD;
    Device[deviceCount].Ports = 0;
    Device[deviceCount].PullUpOption = false;
    Device[deviceCount].InverseLogicOption = false;
    Device[deviceCount].FormulaOption = true;
    Device[deviceCount].ValueCount = 6;
    Device[deviceCount].SendDataOption = true;
    Device[deviceCount].TimerOption = true;
    Device[deviceCount].GlobalSyncOption = false;
    break;
  }

  case PLUGIN_GET_DEVICENAME:
  {
    string = F(PLUGIN_NAME_247);
    break;
  }

  case PLUGIN_GET_DEVICEVALUENAMES:
  {
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_247));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_247));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_247));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[3], PSTR(PLUGIN_VALUENAME4_247));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[4], PSTR(PLUGIN_VALUENAME5_247));
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[5], PSTR(PLUGIN_VALUENAME6_247));
    break;
  }

  case PLUGIN_WEBFORM_LOAD:
  {
    addFormNote(string, F("SoftSerial: 1st=RX-Pin, 2nd=TX-Pin"));
    success = true;
    break;
  }

  case PLUGIN_WEBFORM_SAVE:
  {
    success = true;
    break;
  }

  case PLUGIN_READ:
  {
    if (PLUGIN_247_DEBUG)
    {
      String log = F("PZEM004Tv30: Reading started.");
      addLog(LOG_LEVEL_INFO, log);
    }
    float pzVoltage = Plugin247_ReadVoltage();
    float pzCurrent = Plugin247_ReadCurrent();
    float pzPower = Plugin247_ReadPower();
    float pzEnergy = Plugin247_ReadEnergy();
    float pzFrequency = Plugin247_ReadFrequency();
    float pzPF = Plugin247_ReadPF();
    //-------------------------------------------------------------------
    // readings can be ZERO if there's no AC input on the module.
    // in this case V A and W are reported correctly as ZERO but
    // the accumulated Energy paramenter will not be saved so to
    // preserve previous value
    //-------------------------------------------------------------------
    UserVar[event->BaseVarIndex] = pzVoltage;
    UserVar[event->BaseVarIndex + 1] = pzCurrent;
    UserVar[event->BaseVarIndex + 2] = pzPower;
    if (pzEnergy >= 0)
      UserVar[event->BaseVarIndex + 3] = pzEnergy;
    UserVar[event->BaseVarIndex + 4] = pzFrequency;
    UserVar[event->BaseVarIndex + 5] = pzPF;
    if (PLUGIN_247_DEBUG)
    {
      String log = F("PZEM004Tv30: Reading completed.");
      addLog(LOG_LEVEL_INFO, log);
    }
    success = true;
    break;
  }

  case PLUGIN_INIT:
  {
    if (!Plugin_247_pzem)
    {
      int pzemRXpin = Settings.TaskDevicePin1[event->TaskIndex];
      int pzemTXpin = Settings.TaskDevicePin2[event->TaskIndex];
      Plugin_247_pzem = new PZEM004Tv30(pzemRXpin, pzemTXpin);
      if (PLUGIN_247_DEBUG)
      {
        String log = F("PZEM004Tv30: Object Initialized");
        log += F(" - RX-Pin=");
        log += pzemRXpin;
        log += F(" - TX-Pin=");
        log += pzemTXpin;
        addLog(LOG_LEVEL_INFO, log);
      }
    }
    success = true;
    break;
  }
  }
  return success;
}

//************************************//
//***** reading values functions *****//
//************************************//

// NOTE: readings are attempted only PZEM_AMX_ATTEMPT times

float Plugin247_ReadVoltage()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->voltage();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  if (reading == -1)
    reading = 0;
  return reading;
}

float Plugin247_ReadCurrent()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->current();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  if (reading == -1)
    reading = 0;
  return reading;
}

float Plugin247_ReadPower()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->power();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  if (reading == -1)
    reading = 0;
  return reading;
}

float Plugin247_ReadEnergy()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->energy();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  return reading;
}

float Plugin247_ReadFrequency()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->frequency();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  return reading;
}

float Plugin247_ReadPF()
{
  int counter = 0;
  float reading = -1.0;
  do
  {
    reading = Plugin_247_pzem->pf();
    wdt_reset();
    counter++;
  } while (counter < PZEM_MAX_ATTEMPT && reading < 0.0);
  return reading;
}
#endif
