#include <ESC.h>          //ESC biblioteka 
#include <Servo.h>        //Variklis valdomas kaip servo variklis, servo biblioteka
#include <HX711.h>

#define NUM_SAMPLES 10
#define calibration_factor -420400.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define DOUT  4
#define CLK  3

HX711 scale(DOUT, CLK);

Servo motor;

int delayms = 200;

ESC myESC (9, 1100, 2000, 1000);   // ESC_Name (ESC PIN, Minimum Value, Maximum Value, Arm Value)
int sp = 1200;
bool top = true;

float value=0;
float rev=0;
int rpm;
int oldtime=0;
int time;

const int analogIn = A0;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue= 0;
int ACSoffset = 2500; 
double Voltage = 0;
double Amps = 0;

void isr() //interrupt service routine
{
  rev++;
}

int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage

 
void setup()
{   
  Serial.begin(9600); 
  
  myESC.arm();                    // Send the Arm value so the ESC will be ready to take commands
  
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

 
  delay(5000);                    // Wait for a while;
        
} 

void loop()
{
  if (top)
  {
    if (sp <= 1800) // Max 1960!
    
    {
      myESC.speed(sp);              // Set the speed to a testing value between 1000 and 2000
      attachInterrupt(0,isr,RISING); 
      delay(delayms);
      detachInterrupt(0);           //detaches the interrupt
      time=millis()-oldtime;        //finds the time 
      rpm=(rev/time)*30000;         //calculates rpm
      oldtime=millis();             //saves the current time
//      Serial.print("rev "); 
//      Serial.print(rev); 
//      Serial.println("   "); 
      rev=0;
      Serial.print("speeed "); 
      Serial.print(sp); 
      Serial.print(" RPM "); 
      Serial.print(rpm); 
      //Serial.println("   ");                      // Wait for a while 
     
      //Serial.print("Reading: ");
       Serial.print(" kg "); 
      float trauka = abs(scale.get_units());
      Serial.print(trauka, 3); //scale.get_units() returns a float
     //You can change this to kg but you'll need to refactor the calibration_factor
      //Serial.println();


      
      
      RawValue = analogRead(analogIn);
 Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
 Amps = ((Voltage - ACSoffset) / mVperAmp);
 
 
// Serial.print("Raw Value = " ); // shows pre-scaled value 
// Serial.print(RawValue); 
// Serial.print("\t mV = "); // shows the voltage measured 
// Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
 Serial.print(" Amps "); // shows the voltage measured 
 Serial.print(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point


    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A2);
        sample_count++;
        
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    voltage = ((float)sum / (float)NUM_SAMPLES * 5.015) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
    Serial.print (" V ");
    Serial.println(voltage * 5.779);
    sample_count = 0;
    sum = 0;
     
      sp += 10;
    }
    else
    {
      top = false;
    }
  }
  else
  {
    if (sp >= 1000)
    {
      myESC.speed(sp);              // Set the speed to a testing value between 1000 and 2000
      attachInterrupt(0,isr,RISING); 
      delay(delayms);
      detachInterrupt(0);           //detaches the interrupt
      time=millis()-oldtime;        //finds the time 
      rpm=(rev/time)*30000;         //calculates rpm
      oldtime=millis();             //saves the current time
//       Serial.print("rev "); 
//      Serial.print(rev); 
//      Serial.println("   "); 
      rev=0;
      Serial.print("speeed "); 
      Serial.print(sp); 
      Serial.print(" RPM "); 
      Serial.print(rpm); 
      //Serial.println("   ");    ;                     // Wait for a while 
     
      //Serial.print("Reading: ");
      Serial.print(" kg ");
      float trauka = abs(scale.get_units());
      Serial.print(trauka, 3); //scale.get_units() returns a float
      //You can change this to kg but you'll need to refactor the calibration_factor
      //Serial.println();


     RawValue = analogRead(analogIn);
 Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
 Amps = ((Voltage - ACSoffset) / mVperAmp);
 
 
// Serial.print("Raw Value = " ); // shows pre-scaled value 
// Serial.print(RawValue); 
// Serial.print("\t mV = "); // shows the voltage measured 
// Serial.print(Voltage,3); // the '3' after voltage allows you to display 3 digits after decimal point
 Serial.print(" Amps "); // shows the voltage measured 
 Serial.print(Amps,3); // the '3' after voltage allows you to display 3 digits after decimal point

    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(A2);
        sample_count++;
        
    }
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    voltage = ((float)sum / (float)NUM_SAMPLES * 5.015) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
    Serial.print(" V ");
    Serial.println(voltage*5.779 );
    
    sample_count = 0;
    sum = 0;
      
      sp -= 10;
    }
    else
    {
      exit(0);
    }
  }               
}


