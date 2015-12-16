# Description
Resurrect your old kettle! Add Wifi connectivity and control it from anyway! 

Project has been done in Amiqual4Home Equipex Creativity Lab - https://amiqual4home.inria.fr/

# Prerequisities
 * Photon with latest firmware updates :
     
    ```
	sudo particle flash --usb cc3000
	sudo particle flash --usb deep_update_2014_06
    ``` 
 * 1 Relay
 * 1 transformer 220V to 5V
 * 2 led and 1 buton
 * 1 thermistor
 * A electric kettle

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/kettle.jpg" width="500">

# Setup
## hardware 
* For this model, there are a buttons for the overheat (ON and OFF) that we will keep as a security.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/kettle_buton.jpg" width="500">

* Adding a button for heating on the pins D2 and 2 LED , a yellow to display the detection of the kettle on its base and a red for the Heatin.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/base.jpg" width="500">

* The relay is connected to the photons , as in the diagrams below. Connect the pins relay on the live wire (brown 220v) for close or open the current on the kettle, The relay is connected to the output photon in D4 and Vin(5v), GND.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/relay.JPG" width="500">

* The thermistor is placed in the handle of the kettle.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/thermistor.jpg" width="500">

* For connect the pins of the sensor to the kettle at the base, placing copper plate on the kettle and springs on the base, as below.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/copperplate.jpg" width="500">
<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/spring.jpg" width="500">

* To detect that the kettle is on the base , use the old switches the kettle placed on the base.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/kettledetect.jpg" width="500">

* for the sparkcore and the relay is supplied with 5V, install a portable charger transformer.

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/transformer.jpg" width="500">

* The Photon :

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/photon.jpg" width="500">

* The Base :

<img src="https://github.com/OpHaCo/kettle_wifi/blob/master/img/base2.jpg" width="500">

# Commands
## Control commands - as string

 * CMD_NAME =
   * "POWERON"
   * "POWEROFF"

## Fan control over particle API

	curl https://api.spark.io/v1/devices/'PHOTON_ID'/kettleAPI -d access_token='YOUR_TOKEN' -d "params=CMD_NAME"

## Kettle control over MQTT
 * a valid MQTT broker must be defined in Photon code mqttserver variable 

### topics
#### "kettle/kettleCmds" : kettle control commands
In this topic, payload containing command name must be sent, 

e.g :

    mosquitto_pub -h BROKER_IP -t kettle/kettleCmds -m CMD_NAME
 
#### "kettle/particleCloud" : kettle cloud connection
To enable cloud connection
    
    mosquitto_pub -h BROKER_IP -t kettle/particleCloud -m ENABLE
    
To disable cloud connection
    
    mosquitto_pub -h BROKER_IP -t kettle/particleCloud -m DISABLE

## Input commands - as strings
Different variables can be read : 

 * VARIABLE_NAME =
   * "error" : error value, possible values are : 
      * 0 : No error
      * 1 : The kettle was badly put on its base
      * 2 : No water in the kettle
   * "isHeating" : is kettle heating?
      * 0 : The kettle does not heat
      * 1 : The kettle heats
   * "temp" : current kettle temperature in °C
   * "isdocked" 
      * 0 : Kettle not docked to base, either kettle removed from base, kettle on base but sensor not connected
      * 1 : Kettle is docjed the base

## Read variable over particle API

    curl https://api.spark.io/v1/devices/'PHOTON_ID'/'VARIABLE'?access_token='YOUR_TOKEN'

Example :

    curl https://api.spark.io/v1/devices/'PHOTON_ID'/temp?access_token='YOUR_TOKEN'

## Subscribe variables over MQTT
 * a valid MQTT broker must be defined in Photon code mqttserver variable

### Variables topics
 * "error" - 1 byte unsigned integer - "kettle/error" topic
 * "isHeating" - 1 byte unsigned integer - "kettle/heating" topic
 * "temp" - 8 bytes double , temp in °C - "kettle/temp" topic
 * "isdocked" - 1 byte unsigned integer - "kettle/docked" topic

e.g :

    mosquitto_sub -h BROKER_IP -t kettle/temp

# References
 * https://docs.particle.io/guide/getting-started/intro/photon/
 * https://community.particle.io/t/getting-mac-address-of-unconnected-core/8473/5
 * http://blog.particle.io/2014/08/06/control-the-connection/
