
/******************************************************************************
 * @file    kettle spark firmware
 * @author  Pierre Barralon - INRIA
 * @date    8 dec. 2015   
 *
 * @brief Spark core firmware controlling a kettle over WIFI
 * 
 * Project : kettle_wifi - https://github.com/OpHaCo/kettle_wifi
 * Contact:  Rémi Pincent - remi.pincent@inria.fr
 * 
 * Revision History:
 * Refer https://github.com/OpHaCo/kettle_wifi
 * 
 * LICENSING
 * kettle_wifi (c) by Pierre Barralon
 * 
 * kettle_wifi is licensed under a
 * Creative Commons Attribution 3.0 Unported License.
 * 
 * You should have received a copy of the license along with this
 * work.  If not, see <http://creativecommons.org/licenses/by/3.0/>.
 *****************************************************************************/
typedef enum{
    NO_ERROR = 0,
    BAD_POSITION = 1, /** The kettle was badly put on its base */
    NO_WATER = 2
}EKettleError;

int kettleControl(String command);
void waittemp(int tempmax);
int overheat(void);
void stopoverheat();
void checkError();
void updateKettleDockStatus();
void updateTemp();
bool checkStatus(int pin, bool status);
void onAlreadyHotAnim();
void onHeatButtonPressIT();
void onHeatButtonPress();
static const int KETTLE_ON_BASE_IN_PIN  = D3;
static const int HEATING_BUTTON_PIN     = D2;
static const int HEATING_IND_LED_PIN    = D1;
static const int KETTLE_DOCKED_LED_PIN  = D0;
static const int TEMP_SENSOR_PIN        = A0;
static const int RELAY_PIN              = D4;

static const int BOILING_VALUE = 3290;

inline bool isOnBase(void){return !digitalRead(KETTLE_ON_BASE_IN_PIN);}
inline bool isTempSensorConnected(void){return analogRead(TEMP_SENSOR_PIN) > 100;}
inline bool isDocked(void){return isOnBase() && isTempSensorConnected();}

 /*** DOCKER = kettle on base and temperature sensor connected */
bool isKettleDocked, isHeating, heatButtonPressed;
int error, temp, tempinit;
long heatTime;

void setup() {
    Particle.function("kettleAPI", kettleControl); 
    
    pinMode(KETTLE_ON_BASE_IN_PIN, INPUT_PULLUP);
    pinMode(HEATING_BUTTON_PIN, INPUT_PULLUP);
    pinMode(HEATING_IND_LED_PIN, OUTPUT);
    pinMode(KETTLE_DOCKED_LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(TEMP_SENSOR_PIN, INPUT);
   
    attachInterrupt(HEATING_BUTTON_PIN, onHeatButtonPressIT, FALLING);       // call onHeatButtonPress if push change of low state
    
    error = NO_ERROR;
    temp = analogRead(TEMP_SENSOR_PIN);
    tempinit = 0;
    isKettleDocked = false;
    isHeating = false;
    heatButtonPressed = false;
    heatTime = 0;
    
    checkError();
    updateKettleDockStatus();
    updateTemp();

    Particle.variable("error", error);
    Particle.variable("isheating", isHeating);
    Particle.variable("isdocked", isKettleDocked);
    Particle.variable("temp", temp);
}

void loop() 
{
    if(heatButtonPressed)
    {
        onHeatButtonPress();
    }
    if (isHeating){                       // overheat loop
        waittemp(BOILING_VALUE);
    }

    checkError();
    updateKettleDockStatus();
    updateTemp();
    Serial.println(temp);
    Serial.printlnf("isHeating = %d", isHeating);
    Serial.printlnf("error = %u", error);

    delay(10);
}

void waittemp(int tempmax)                  // overheat loop
{
    if (error != NO_ERROR || !isDocked){
        stopoverheat();
        return;
    }
    else
    {
        if (temp > tempmax){                // if water is hot, stop overheat
            stopoverheat();
            onAlreadyHotAnim();
            return;
        }
    }
}

void onHeatButtonPress()                   // buton of overheat, off or on
{   
    if (isHeating){               
        stopoverheat();
    }
    else{
        overheat();
    }
    heatButtonPressed = false;
}

void onHeatButtonPressIT()                   // buton of overheat, off or on
{   
    if(!checkStatus(HEATING_BUTTON_PIN, LOW)) return;
    heatButtonPressed = true;
}

void kettleOffBase()                    // kettle off base
{ 
    digitalWrite(KETTLE_DOCKED_LED_PIN, LOW);
    if(isHeating)
        stopoverheat();
}

int overheat() 
{
    if (isKettleDocked && error == NO_ERROR) {
        if(temp < BOILING_VALUE)
        {
            digitalWrite(HEATING_IND_LED_PIN, HIGH);           // active the overheat led
            digitalWrite(RELAY_PIN, HIGH);          // active the overheat relay
            isHeating = true;
            tempinit = temp;
            heatTime = millis();
        }
        else
        {
            onAlreadyHotAnim();
        }
    }
    return error;
}

void stopoverheat()                         // stop overheat
{
    digitalWrite(HEATING_IND_LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    isHeating = false;
}

void updateKettleDockStatus() {                                  // define the state of kettle
    if(isDocked())
    {
        isKettleDocked = true;
        digitalWrite(KETTLE_DOCKED_LED_PIN, HIGH);
    }
    else
    {
        isKettleDocked = false;
        kettleOffBase();
    }
}

void checkError() {                                    // error detect
    if(isOnBase())
    {
        if(!isTempSensorConnected())
        {
            error = BAD_POSITION;                // kettle detected but the thermistor is not detected
            Serial.printlnf("error 1 : Kettle ill-posed", temp);
            kettleOffBase();
        }
        else if (isHeating && abs(millis() - heatTime) > 3000 && (temp - tempinit) < 85) {           
            error = NO_WATER;                                              // not water on kettle
            stopoverheat();
        }
        else if (error == BAD_POSITION ) {
            /** no more error */
            error = NO_ERROR;
        }

        if (error == 2) {
            Serial.printlnf("error 2 : Not watter in kettle");
        }
    }
    else {
        error = NO_ERROR;                          // not error
    }
}

int kettleControl(String command)                      // command wifi
{
    if(command == "POWEROFF")        // power off command
    {
        if(isHeating) stopoverheat();
        return 1;
    }
    
    if(command == "POWERON")        // set power on overheat command
    {
        return (overheat() == NO_ERROR) ? 1 : 0;
    }
    else {
        return -1;
    }
}

void updateTemp()
{
    temp = analogRead(TEMP_SENSOR_PIN);
}

bool checkStatus(int pin, bool status)
{
    int it = 0;
    while(it++ < 40)
    {
        if(digitalRead(pin) != status)
            return false;
        delayMicroseconds(20);
    }
    return true;
}

void onAlreadyHotAnim(void){
    int val = 0xFF;
    analogWrite(HEATING_IND_LED_PIN, val);
    delay(100);
    while(val-- != 0)
    {
        analogWrite(HEATING_IND_LED_PIN, val);
        delay(2);
    }
}
