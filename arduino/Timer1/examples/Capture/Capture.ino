/*
   Marcelo H Moraes
   marceloh220@hotmail.com

   A simple sketch Arduino using Marcelino cores.

   Get a width of pulse in CLKO pin (Digital pin 8), using the capture mode of the timer1.

   This application is made in interrupt without unnecessary delays in the code.

   For detail of this modules read the manual.

*/

#include <timer1.h>
Timer1  capture;                //Module Timer1 instantiate as capture (Guess what!)

double width;                   //Variable to guard the width pulse in milliseconds

byte aux;                       //this variable will be increassed by button in digital pin 2

word ovf_capture;               //if pulse is much large, it will cause the overflow of timer capture

void ovf() {
  ovf_capture++;                //have a overflow of pulse captured
}

void measure() {

  if(capture.rising()) {        //If detected rising edge

    //change capture to detect the falling edge
    capture.attach(CAPT,FALLING,measure);
    
    capture.timer(0);           //Clear the timer
    capture.prescale(8);        //Turn on the timer with a prescaler 1/8
    
  }
  
  else {                        //Else, detected falling edge

    //With a prescaler in 1/8 the timer will have an increase every:
    // tic = prescale / F_CPU
    // tic = 8 / 16e6 = 500ns
    // each unit in capture register means 500 nano seconds pass.
    // to milliseconds that be:
    // micros = capt() * 500e-3

    capture.prescale(OFF);        //Turn off the timer

    width = ( capture.capt() + ovf_capture * 65535 ) * 0.5;
    
    //Iniciate a new rising capture
    capture.attach(CAPT,RISING,measure);

    ovf_capture = 0;            //Clear overflows of capture
    
  }

  
}

void setup() {

  pinMode(5, OUTPUT);   //OC0B like output to generate the pwm signal (digital pin 5)
                                //For more details about this pins names read the Digital module manual

  pinMode(ICP, INPUT);     //ICP like input to read the sginal of pwm (digital pin 8)

  pinMode(2, INPUT_PULLUP);      //Button to increased the width of pwm signal
  pinMode(3, INPUT_PULLUP);      //Button to decreased the width of pwm signal
  
  //Attach the measure function in a capture interrupt of timer 1
  //This part start the capture to detect the rising edge of pulse
  capture.attach(CAPT,RISING,measure);
  capture.attach(OVF,ovf);
Serial.begin(9600);

}

void loop() {

  if(!digitalRead(2)&&(aux<255)) //if button in pin 2 is pressed (inverted logic) and aux not max
    aux++;                      //Increassed the pwm width

  if(!digitalRead(3)&&(aux>0) )  //if button in pin 3 is pressed (inverted logic) and aux not min
    aux--;                      //Decreased the pwm width

  analogWrite(5,aux);                ///generate the pwm pulse
  
  Serial.print(width);          //print the pwm pulse width in serial monitor
  Serial.println(" us");        //print unit

  delay(100);                //to debounce the button

  

}
