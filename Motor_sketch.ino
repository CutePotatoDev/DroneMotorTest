// Created by Dominykas P. 2019-02-21 (Create idea of work flow and write initial code.)
// Modified by Saulius V. 2019-03-19 (Code refactoring and cleaning.)

#include <ESC.h>
#include <HX711.h>

// ###### Configuration.
#define DEBUG

// ADC
#define NUM_SAMPLES 10
#define ADC_CURRENT_PIN A0
#define ADC_VOLTAGE_PIN A2

// Scale
#define calibration_factor -420400.0    // This value is obtained using the SparkFun_HX711_Calibration sketch.
#define DOUT 4
#define CLK 3

// ESC
#define ESC_PIN 9
#define ESC_ARM_VAL 1000

// ######

HX711 scale(DOUT, CLK);
ESC myESC (ESC_PIN, 1100, 2000, ESC_ARM_VAL);   // ESC_Name (ESC PIN, Minimum Value, Maximum Value, Arm Value).


// Variables list.
int prewtime = 0;
int rpm = 0;
float rev = 0;

double amps = 0.0;
float voltage = 0.0;


int delayms = 200;
int sp = 1200;
bool top = true;

int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int ACSoffset = 2500; 

 
void isr() {    //interrupt service routine
    rev++;
}

void setup() {   
    Serial.begin(9600); 
  
    myESC.arm();                            // Send the Arm value so the ESC will be ready to take commands.
  
    scale.set_scale(calibration_factor);    // This value is obtained by using the SparkFun_HX711_Calibration sketch.
    scale.tare();                           // Assuming there is no weight on the scale at start up, reset the scale to 0.

    delay(5000);                            // Wait for a while.        
} 

void calcRPM() {
    attachInterrupt(0, isr, RISING); 
    delay(delayms);
    detachInterrupt(0);             // Detaches the interrupt.

    int time = millis() - prewtime; // Finds the time. 
    rpm = (rev / time) * 30000;     // Calculates RPM.
    prewtime = millis();            // Saves the current time.
    rev = 0;
}

void calcADC() {
    int val = analogRead(ADC_CURRENT_PIN);
    double volt = (val / 1024.0) * 5000; // Gets you mV.
    amps = ((volt - ACSoffset) / mVperAmp);

    val = 0;    // Zero value.

    // Take a number of analog samples and add them up.
    int sample_count = 0;
    while (sample_count < NUM_SAMPLES) {
        val += analogRead(ADC_VOLTAGE_PIN);
        sample_count++;
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    voltage = ((float)val / (float)NUM_SAMPLES * 5.015) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
}

void dataOut() {
    // Print formatted data to serial if debug is enabled.
    #ifdef DEBUG
        Serial.print("speeed ");
        Serial.print(sp);
        Serial.print(" RPM ");
        Serial.print(rpm);

        Serial.print(" kg "); 
        Serial.print(abs(scale.get_units()), 3);    // scale.get_units() returns a float.
        // You can change this to kg but you'll need to refactor the calibration_factor.

        Serial.print(" amps "); // Shows the voltage measured.
        Serial.print(amps, 3);   // the '3' after voltage allows you to display 3 digits after decimal point.

        Serial.print(" V ");
        Serial.println(voltage * 5.779);
    #else
        // TODO: Create python friendly data output format.
    #endif 
}


void loop() {
    if (top) {
         // Max 1960!
        if (sp <= 1800) {
            myESC.speed(sp);              // Set the speed to a testing value between 1000 and 2000
            
            calcRPM();
            calcADC();
            dataOut();
     
            sp += 10;
        } else {
            top = false;
        }

    } else {
        if (sp >= 1000) {
            myESC.speed(sp);              // Set the speed to a testing value between 1000 and 2000
            
            calcRPM();
            calcADC();
            dataOut();
    
            sp -= 10;
        } else {
            exit(0);
        }
    }
}
