
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

int kettledetect = D3;      //  {
int push = D2;              //
int ledr = D1;              //   Define pins
int ledj = D0;              //
int captemp = A0;           //
int relay = D4;             //  }

int kettleControl(String command);
void waittemp(int tempmax);
void overheat(void);
void stopoverheat();
void err();
void state();
int status, error, temp, compt, tempinit;


void setup() {
    Particle.function("kettleAPI", kettleControl); // Expose the kettle function to the Spark API
    
    pinMode(kettledetect, INPUT_PULLUP);           // sets the pins as input
    pinMode(push, INPUT_PULLUP);                    // sets the pins as input
    pinMode(ledr, OUTPUT);                  // sets the pins as output
    pinMode(ledj, OUTPUT);                  // sets the pins as output
    pinMode(relay, OUTPUT);                  // sets the pins as output
   
    attachInterrupt(kettledetect, kettlechange, CHANGE);            // call Kettlechange if kettledect change state
    attachInterrupt(push, pushChange, FALLING);                     // call pushChange if push change of low state
    error = 0;
    compt = 0;
    temp = analogRead(captemp);
    err();
    state();

    Particle.variable("error", error);
    Particle.variable("state", status);
    Particle.variable("temp", temp);
}

void loop() 
{
    if (status == 2){                       // overheat loop
        waittemp(3290);
    } 
    temp = analogRead(captemp);
    err();                                  // verify error
    state();                                // define status
    delay(250);
    Serial.println(temp);
    Serial.printlnf("status = %u", status);
    Serial.printlnf("error = %u", error);
    compt = 0;
}

void waittemp(int tempmax)                  // overheat loop
{
    while (status == 2)
    {
        err();
        if (error != 0){                    // if error, stop overheat
            break;
        }
        temp = analogRead(captemp);
        digitalWrite(ledr, HIGH);           // active the overheat led
        digitalWrite(relay, HIGH);          // active the overheat relay
        delay(50);
        Serial.println(temp);
        Serial.printlnf("compt = %u", compt);
        if (temp > tempmax){                // if watter is hot, stop overheat
            stopoverheat();
            break;
        }
        compt = compt +1;
    }
}

void kettlechange()                 // kettle on or off base
{
    if (digitalRead(kettledetect) == 0){        // on base
        state();
    }
            
    else{                                       // off base
        kettleoff();
        error = 0;
    }
}

void pushChange()                   // buton of overheat, off or on
{
    if (status == 2){               
        stopoverheat();
    }
    else if (status == 1){
        overheat();
    }
}

void kettleoff()                    // kettle off base
{ 
    digitalWrite(ledj, LOW);
    stopoverheat();
}

void overheat() 
{
    tempinit = analogRead(captemp);
    if (error == 0) {
        status = 2;
    }
}

void stopoverheat()                         // stop overheat
{
    digitalWrite(ledr, LOW);
    digitalWrite(relay, LOW);
    status = 1;
    state();
}

void state() {                                  // define the state of kettle
    if (error == 1 || digitalRead(kettledetect) != 0) {                 // kettle off base or there are error
        status = 0;
    }
    else if (digitalRead(kettledetect) == 0 & error != 1){              // kettle on base and there are not error
        if (status != 2) {
            status = 1;
            digitalWrite(ledj, HIGH);
        }
    }
}

void err() {                                    // error detect
    if (digitalRead(kettledetect) == 0) {
        if (temp < 100){
            error = 1;                                              // kettle detected but the thermistor is not detected
            Serial.printlnf("error 1 : Kettle ill-posed", temp);
            kettleoff();
        }
        else if (compt > 1350 & (temp - tempinit) < 85) {           
            error = 2;                                              // not water on kettle
            stopoverheat();
        }
        else if (error !=2 ) {
            error = 0;                          // not error
        }
        if (error == 2) {
            Serial.printlnf("error 2 : Not watter in kettle");
        }
    }
    else {
            error = 0;                          // not error
        }
}

int kettleControl(String command)                      // command wifi
{
    if(command == "POWEROFF")        // power off command
    {
        stopoverheat();
        return 1;
    }
    
    if(command == "POWERON")        // set power on overheat command
    {
        if (status == 0) {                          // if kettle is not on base, return 0
            return false;
        }
        else {
            overheat();                             // call overheat
            if (status != 2) {                      // if overheat is not activate, return 0
                return false;
            }
        }
        return 1;
    }
    else {
        return -1;
    }
}
