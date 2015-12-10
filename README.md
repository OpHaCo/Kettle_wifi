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
	curl https://api.spark.io/v1/devices/'SPARK_CORE_ID'/kettleAPI -d access_token='YOUR_TOKEN' -d "params=CMD_NAME"
 
 * CMD_NAME =
   * POWERON	(activate the overheat)
   * POWEROFF	(stop the overheat)
   * STATUS		(for get the status)
   * ERROR		(for get the error number)
   * TEMP		(for get the temperature)
     
* Example :

	curl https://api.spark.io/v1/devices/'SPARK_CORE_ID'/kettleAPI -d access_token='YOUR_TOKEN' -d "params=STATUS"
	{
 	"id": "ID",
 	"last_app": "",
 	"connected": true,
  	"return_value": status
	}

# References
 * https://docs.particle.io/guide/getting-started/intro/photon/
 * https://community.particle.io/t/getting-mac-address-of-unconnected-core/8473/5
 * http://blog.particle.io/2014/08/06/control-the-connection/
