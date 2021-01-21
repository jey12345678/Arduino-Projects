/*Ultrasonic_Distance_Meter
 *Program allows users to measure,hold,and review distances shown in cm or inches with ultrasonic sensor, and displays this onto a liquid crystal display.
 *Version: 04/03/2019
 *Created By:Jeyason Jeyaparan and Michael Lapshin
 */

// include liquid crystal library
#include <LiquidCrystal.h>

//Include the library for vectors.
#include <Vector.h>

//Declare the pins for echo and trig
#define ECHO A14
#define TRIG A15

//Declare the pins for the lcd
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Declare the variables used
float duration;
const int usTocm = 29;
String cmDisplay = "";
float cm, inches;
int analogButton, buttonIndex, mapped;
String buttonPressed, state = "Measure", unit = "cm";
const int measureSample = 50;
float q1, q3;
float goodCase = 0;
float sum = 0;

//Create an array called buttons, so that the buttonIndex could reference a value in the buttons array, to let us know what button it is that we are pressing.
String buttons[12] = {"Right", "Up", "Down", "", "Left","", "", "Select", "", "", "", "Null"};

//Create the vector called recorded.
Vector<float> recorded;

//Set index to zero.
int index = 0;

void setup() {
  // put your setup code here, to run once:
  
  //Declare the pinModes for the trig and Echo.
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  //Set the storage_array and have the recorded vector have that storage amount.
  float storage_array[10];
  recorded.setStorage(storage_array);

  //Have the serial.begin be 9600.
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Read the anolog pin of A0.
  analogButton = analogRead(A0);

  //Have the button Index be equalled to the mapping of the buttons.
  buttonIndex = map(analogButton, 0, 450, 0, 5);

  //Have the buttonPressed be 
  buttonPressed = buttons[buttonIndex];

  //if the buttonPressed is up.
  if(buttonPressed == "Up"){

      //if the state is equalled to Measure make state be Ultra Measure.
      if(state == "Measure") state = "Ultra Measure";

      //else if the state is Ultra Measure, make the state be hold.
      else if(state == "Ultra Measure") state = "Hold";
      
      //else if state is equalled to hold make state be Review.
      else if(state == "Hold") state = "Review";

      //else just make the state be measured.
      else state = "Measure";
  }
  
  //Measurement mode
  //if state is equalled to measure
  if(state == "Measure"){
    //Have the trigger be set from low to high and then back to low to send out a pulse.
      digitalWrite(TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG, HIGH);
      delayMicroseconds(5);
      digitalWrite(TRIG, LOW);
    
    //Caluclate the duration of the pulse.
    duration = pulseIn(ECHO, HIGH, 25000);
  
    //Calculate the cm from the duration of the pulse and by calculating cm we could find inches.
    cm = duration/usTocm/2;
    inches = cm/2.54;

    //else if the state is Ultra Measure
  } else if (state == "Ultra Measure"){

    //Create two arrays, data and temp to order the array of measurements.
    float data[measureSample], temp[measureSample];

    //for i equalled to 0 and i is less than the measured sample.
    for(int i =  0; i < measureSample; i++){
      
      //Have the trigger be set from low to high and then back to low to send out a pulse.
      digitalWrite(TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG, HIGH);
      delayMicroseconds(5);
      digitalWrite(TRIG, LOW);
    
      //Caluclate the duration of the pulse.
      temp[i] = pulseIn(ECHO, HIGH, 25000); 

      delay(20);
    }
    //Sort the data array.
    for(int i = 0; i < measureSample; i++){
      float minimum = 10000000;
      int index = 0;
      for(int j = 0; j < measureSample; j++){
        if(temp[j] < minimum){
          minimum = temp[j];
          index = j;  
        }  
      }  
      
      data[i] = minimum;
      temp[index] = 9999999;
    }

    q1 = data[measureSample/4];
    q3 = data[measureSample*3/4];

    for(int i = 0; i < measureSample; i++){
      
      //Set the limits in the data array, to find the outliers and to not include the outliers. 
      if(data[i] <= q3 + (q3 - q1)*1.5 && data[i] >= q1 - (q3 - q1) * 1.5){
        
          //Add the data numbers that is acceptable to the sum.
          sum += data[i];
          
          //Add one to goodCase variable.
          goodCase++;
      }  
    }

    //Calculate the cm by taking the sum and dividing it by the number of good cases in the array, and then dividing that by usTocm and then dividing that by 2 and you could find inches from this.
    cm = (sum/goodCase)/usTocm/2;
    inches = cm/2.54;
  }

  //Clear the liquid crystal display.
  lcd.clear();
  
  //Set the cursor at 0 columns and 1st row.
  lcd.setCursor(0, 1);

  //print the state
  lcd.print(state);
  
  //if state is equalled to Measure or Hold.
  if(state == "Measure" || state == "Hold" || state == "Ultra Measure"){
    // set up the LCD's number of columns and rows:
    lcd.setCursor(0, 0);
  
    //if the unit is cm, then print out the cm up to 3 decimal places.
    if(unit == "cm") lcd.print(cm, 3);

    //else if the unit is inches then print out the inches up to 3 decimal places.
    else if (unit == "in") lcd.print(inches, 3);

    //Print out the unit beside the number value.
    lcd.print(unit);

    //if the button that is pressed is Down.
    if(buttonPressed == "Down"){
      //Add the cm value to the vector.
      recorded.push_back(cm);

      //Set the cursor of lcd to 9th column, 2nd row.
      lcd.setCursor(9, 1);
      
      //Let user know that you stored the cm value.
      lcd.print("Stored!");
    }
  }
  //if the button that is pressed is Select
  if(buttonPressed == "Select"){

    //if the unit is cm toggle it to make it inches.
    if(unit == "cm") unit = "in";
    
    //else just make the unit cm.
    else unit = "cm";
  }
  //if the state of the button is Review.
  if(state == "Review"){

    //Set the cursor to be at row 0, and column 0.
    lcd.setCursor(0,0);
    //if the size of the recorded is less than or equal to 0, have it print out Empty on lcd, as nothing is recorded.
    if(recorded.size() <= 0){
      lcd.print("Empty");  
    }else{
      //Have the lcd display one stored value and its index out of total number of stored values.
      lcd.print((index + 1));
      lcd.print("/");
      lcd.print(recorded.size());
      lcd.print(" : ");

      //if the unit is cm, then print out the recorded index with 3 decimal places else convert the cm to inches and display it onto the lcd with its corresponding unit.
      if(unit == "cm") lcd.print(recorded[index], 3);
      else lcd.print(recorded[index]/2.54, 3);
      lcd.print(unit);
    }
    //if the recorded size is greater than 0.
    if(recorded.size() > 0)
      // if the buttonPressed is Right
      if(buttonPressed == "Right"){
        //Then increase the index
        index++;
        
        //if the index is equalled to the recorded size, then the index equals to 0 again.
        if(index == recorded.size()) index = 0;  
      }
      //if the button pressed is left.
      if(buttonPressed == "Left"){
        //then decrease the index
        index--;
        //if index is less than 0 then the index is equalled to the recorded size subtracted by 1.
        if(index < 0) index = recorded.size()-1;
      }

    // Clears the recorded values if the down button is pressed and lets users know that that the values are cleared.
    if(buttonPressed == "Down") {
      recorded.clear();
      lcd.setCursor(0,1);
      lcd.print("Cleared!");
    }  
  }
  delay(500);
}
