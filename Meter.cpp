/** @file meter.cpp
 *  @brief  This file contains Reading Data From ModbusRTU Protocol and transfer to AWS cloud
 *          user input is given from User Input JSON file
 *
 *  @author     Karthikeya Rekulapalli
 *  @bug        No known bugs.
 */

/*
 *#####################################################################
 *  Initialization block
 *  ---------------------
 *  This block contains initialization code for this particular file.
 *  It typically contains Includes, constants or global variables used
 *  throughout the file.
 *#####################################################################
 */
#define INIT_FUNC 1
/* --- Standard Includes --- */
#include <functions.h>
#include <cstdio>
#include <iostream>
#include <curl/curl.h>
#include <json-c/json.h>
/* --- Project Includes --- */
#include <plc_modbus.h>
#include <aws.h>
#include <shunyaInterfaces.h>
#include <si/influxdb.h>
/* RapidJSON Includes */
#include "rapidjson/document.h"




/*
 *#####################################################################
 *  Process block
 *  -------------
 *  This block contains all the process code for the particular layer.
 *  It typically contains internal functions and external functions
 *  which implement the functionalities of the respective layer.
 *#####################################################################
 */
 
using namespace rapidjson;

int main(void) {

  debugLogOn();

  initLib();
   
  /* Open the User_Input.json file in read mode */
  FILE* fp = fopen("User_Input.json", "r"); 
  
  /* Declare read buffer */
  char readBuffer[65536];
  
  /* Declare stream for reading the example stream */
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));


  /* Declare a JSON document. JSON document parsed will be stored in this variable */
  Document document;

  
  /* Parse User_Input.json and store it in `document` */
  document.Parse(json);
  
  /* Close the User_Input.json file*/
  fclose(fp);

  
  /*using InFluxdb to set database url and Id */
  struct InfluxdbSettings set = { 
        /* Dashboard URL */
        .dbURL=d["dashboardURL"].GetString(),
        /* Dashboard Name */
        .dbName=d["dashboardURL"].GetString() 
    };

  struct AWSSettings set = {

      /* MQTT HOST URL */
      .hostURL =document["hostURL"].GetString() ,
      /* MQTT port */
      .port =document["port"].GetInt() ,
      /* Cert location */
      .certDir =document["certDir"].GetString(),
      /* Client Root CA file name */
      .rootCA = document["rootCA"].GetString(),
      /* Client certificate file name */
      .certName = document["certName"].GetString(),
      /* Client Private Key file name */
      .privKey = document["privKey"].GetString(),
      /* Quality Of Service 0 or 1 */
      .QOS =document["QOS"].GetInt() ,
      /* Client ID */
      .clientID =document["clientID"].GetString()
    };

  int32_t rc;
  rc = awsConnectMqtt(set);
  /* Check if error occured */
  if (rc < 0) {
  
    printf("Connection Error");
    exit(-1);

  }

char Topic[]="Enter your Topic";

/*Average Current is Cur_avg */
 float  Cur_avg;
/*Current in 1 phase is Cur_1Pha */
 float Cur_1Pha;
/*Current in 2 phase is Cur_2Pha */
 float Cur_2Pha;
/*Current in 3 phase is Cur_3Pha */
 float Cur_3Pha;
/*Voltage (LN) average is V_LNavg*/
 float V_LNavg;
/*Voltage (LL) average is V_LLavg*/
 float V_LLavg;
/*Active power total is Act_Pow*/
 float Act_Pow;
/*Reactive power total is ReAct_Pow*/
 float ReAct_Pow;
/*Apparent power total is App_Pow*/
 float App_Pow;
/*Forward apparent energy is ForApp_Eng*/
 float ForApp_Eng;
/*Power factor total is Pow_Fac*/
 float Pow_fac;

  
  while (1) {

  /*Default Device Address is 001,baund rate is 19200 and coil address of Average current is 3057*/
     Cur_avg=modbusRTURead("001",19200,3057);
  /*Default Device Address is 001,baund rate is 19200 and coil address of Current in 1 phase is 3009*/
     Cur_1Pha=modbusRTURead("001",19200,3009);
     
  /*Default Device Address is 001,baund rate is 19200 and coil address of Current in 2 phase is 3025*/
     Cur_2Pha=modbusRTURead("001",19200,3025);
    
  /*Default Device Address is 001,baund rate is 19200 and coil address of Current in 3 phase is 3041*/
     Cur_3Pha=modbusRTURead("001",19200,3041);
     
  /*Coil Address of Average Line to neutral Voltage 3061 */
     V_LNavg=modbusRTURead("001",19200,3061);
    
  /*Coil Address of Average Line to Line Voltage 3059 */
     V_LLavg=modbusRTURead("001",19200,3059);
     
  /*Coil Address of Active Power Total is 3051 */
     Act_Pow=modbusRTURead("001",19200,3051);
   
  /*Coil Address of ReActive Power Total is 3053 */
     ReAct_Pow=modbusRTURead("001",19200,3053);
    
  /*Coil Address of Apparent Power Total is 3049 */
     App_Pow=modbusRTURead("001",19200,3049);
   
  /*Coil Address of Forward apparent energy is 3201 */
     ForApp_Eng=modbusRTURead("001",19200,3201);
   
  /*Coil Address of Power factor total is 3055 */
     Pow_fac=modbusRTURead("001",19200,3055);
     
  
     
    rc = awsPublishMqtt(Topic, "{ \"Average Current\": %f, \"Current in Phase 1\": %f ,\"Current in Phase 2\": %f, \"Current in Phase 3\": %f,\"Average Voltage(LN)\": %f,\"Average Voltage(LL)\": %f,\"Active Power is \": %f,\"ReActive Power is \": %f ,\"Apparent Power is \": %f,\"Forward apparent energy is \": %f,\"Power factor total is \": %f }",Cur_avg,Cur_1Pha,Cur_2Pha,Cur_3Pha,V_LNavg,V_LLavg,Act_Pow,ReAct_Pow,App_Pow,ForApp_Eng,Pow_fac);
    printf("Error code = %d\n", rc);
    
    
    writeDbInflux (set, "Average Current: %f,Current in Phase 1:%f ,Current in Phase 2: %f,Current in Phase 3: %f,Average Voltage(LN): %f,Average Voltage(LL): %f,Active Power is: %f,ReActive Power is: %f ,Apparent Power is: %f,Forward apparent energy is: %f,Power factor total is: %f",Cur_avg,Cur_1Pha,Cur_2Pha,Cur_3Pha,V_LNavg,V_LLavg,Act_Pow,ReAct_Pow,App_Pow,ForApp_Eng,Pow_fac);
    
    delay(10000);
  }
}
