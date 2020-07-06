## Industrial IoT meter

The Data is displayed on a Dashboard using Grafana. Data Analytics are give in AWS cloud. 

## What will you Need ?

1. Raspberry Pi 3/4 ( or any variant of these)
2. mcro-SD card and micro-SD card Reader/Adapter.
3. RS485 to USB converter
4. AWS account
5. female to female Du-point cables  
6. laptop or server installed with influxdb and grafana.

## Wiring Raspberry Pi to Meter

Use RS485 to USB converter and connect RS485 pins of converter to RS485 of meter and USB to Raspberry Pi.

![RS485](/RS485.jpeg)

## API for reading Modbus RTU

> modbusRTURead() /*this help to read data from coil address */
  Cur_avg=modbusRTURead("001",19200,3057);
  
## API for reading user data

User data is take input has Jason file and it is readed with rapidjason API

> document["json object name"].Getstring()

For more referance look in to -[Link](https://rapidjson.org/md_doc_tutorial.html)

## API for Connecting to AWS cloud

To this `awsConnectMqtt()` API set is structure that contain AWS credentials is given has input. 

> awsConnectMqtt(set)

## API for Publishing data to AWS cloud

> awsPublishMqtt("topic", "Send this message");

## API for sending data to dash board 

> writeDbInflux (set, "measurement,tag1,tag2 value1=%d,value2=%d %ld", temp, pressure, unix_timestamp);

The code is written with Help of this API's

## Summary Code is in - [Link](https://github.com/KART-HIKEYAREKULAPALLI/METER/blob/master/Meter.cpp)
