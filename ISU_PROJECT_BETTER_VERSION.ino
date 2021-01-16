/*ISU_PROJECT_BETTER_VERSION
 *Displays a rocket that moves or scrolls up,down, and side to side the screen by the pushing buttons
 *June 12th 2019
 *Jeyason Jeyaparan
 */

//------------------------------------------------------------------------------
// Defines Shift Register Pins
#define DS 22
#define STcp 24
#define MR 26
#define SHcp 28

//define button pins
#define buttonDown 45
#define buttonUp 44
#define buttonRight 43
#define buttonLeft 42
//--------------------------------------------------------------------------
//declaring variables
int columnNum=0;

//My rocket picture
byte rocket[8]={
  B11111111,
  B11110111,
  B01101011,
  B10011101,
  B01101011,
  B11110111,
  B11111111,
  B11111111,
};

//-----------------------------------------------------------------------
void setup() {

  DDRC = B11111111; //make the led grounds outputs
  
  //setting the shift register pins as outputs
  pinMode(DS,OUTPUT);
  pinMode(STcp,OUTPUT);
  pinMode(MR,OUTPUT);
  pinMode(SHcp,OUTPUT);

  //setting the buttons as inputs.
  pinMode(buttonDown,INPUT_PULLUP);
  pinMode(buttonUp,INPUT_PULLUP);
  pinMode(buttonRight,INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  
  cli();                                // disable all interrupts
 
  TCCR1A = 0;                           // clear the two Timer/Counter Control Registers (TCR) for Timer1
  TCCR1B = 0;                           // 
  TCCR1B |= (1 << WGM12);               // turn on Clear Timer on Compare (CTC) mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // set CS12 and CS10 bits for prescaler_value = 1024
  
  TCNT1  = 0;                           // initialize the Timer/Counter value to 0
  OCR1A = 38;                        // set OCR = [ 16,000,000/1024 * 0.02 ] - 1 

  TIMSK1 |= (1 << OCIE1A);              // enable interrupts from Timer1 when TCNT1 reaches OCR1A value
  sei(); 
  
  //clear the shift register
  digitalWrite(MR,LOW);
  digitalWrite(MR,HIGH);
  
  Serial.begin(9600);
}
//-------------------------------------------------------------------------------------------------------------------------

ISR(TIMER1_COMPA_vect){ // Interrup Service Routine for Timer1 OCRA event
  
  //It's a shift register, so we have to power first column and then that will be shifted by the shift register to the rest of the columns, so for us it will look like all the columns are powered.
  boolean poweringColumns;
  if (columnNum == 0){
    poweringColumns = 1;
  }else{
    poweringColumns = 0;
  }
  
  //displaying the line
  digitalWrite(STcp,LOW);
  digitalWrite(SHcp,LOW);
  digitalWrite(DS,poweringColumns);
  PORTC = rocket[columnNum];
  digitalWrite(STcp,HIGH);
  digitalWrite(SHcp,HIGH);
  
  //if the column num is equalled to 7, make it be equalled to 0 again or else continue the column number counting.
  if (columnNum==7){
    columnNum=0;
  }else{
  columnNum = columnNum + 1;
  }

}
//-----------------------------------------------------------------------
void loop() {
     //If the button down button is being pressed, the rocket goes down.
     if(digitalRead(buttonDown)== 0){
     //Shift all bits up.
     for(byte i =0; i <= 7; i++){
      rocket[i] = rotateBitDown(rocket[i],1);
      }
     }
     //or if the button up is being pressed, the rocket goes up.
     else if(digitalRead(buttonUp) == 0){
      for(byte i =0; i <= 7; i++){
      rocket[i] = rotateBitUp(rocket[i],1);
      }
     }
     //or if the buttonLeft button is pressed then the rocket goes to the left.
     else if(digitalRead(buttonLeft) == 0)
     {
       for(int i = 0; i<=7;i++)
       {
        byte newLeftIndex = rotateByteLeftValue(i);
        //Serial.println(newLeftIndex);
        rocket[i] = rocket[newLeftIndex];
        Serial.println(rocket[i]);
       }
     }
     //or if the buttonRight button is pressed then the rocket goes to the right.
     else if(digitalRead(buttonRight) == 0)
     {
       for(int i = 7; i>=0 ; i--)
       {
         int newRightIndex = rotateByteRightValue(i);
         rocket[i] = rocket[newRightIndex];
         Serial.println(newRightIndex);
         Serial.println(rocket[i]);
       }
     }
     delay(200);
         
}
//-----------------------------------------------------------------------------
/*
* rotateBitUp
* rotates bits in a byte of data up.
* @param value is the value of the byte.
* @param shift is the amount that I want to shift my bits by.
* @return shiftedByte, which is the shifted byte.
*/
byte rotateBitUp(byte value, int shift) {
    byte shiftedByte =(value >> shift) | (value << (8 - shift));
    return shiftedByte;
}
/*
* rotateBitDown
* rotates bits in a byte of data  down.
* @param value is the value of the byte.
* @param shift is the amount that I want to shift my bits by.
* @return shiftedByte, which is the shifted byte.
*/
byte rotateBitDown(byte value, int shift){
    byte shiftedByte = (value << shift) | (value >>(8-shift));
    return shiftedByte;
}
/*
*rotateBitLeftValue
*finds value for rotating byte to the left.
*@param value, is the value of i.
*@return newIndex, which is the new index that I will be using for the rocket index.
*/
byte rotateByteLeftValue(byte value){
  byte newIndex = value+1; //newIndex is value plus 1.
  if(newIndex == 8)  // if the newIndex is equalled to 8 then make it equal to 0 again.
  {
    newIndex= 0;
  }
  return newIndex;
}
/*
 *rotateBitRightValue
 *finds value for rotating byte to the right.
 *@param value, is the value of i.
 *@return newIndex is the new index that I will be using for the rocket index.
 */
int rotateByteRightValue(int value){
  int newIndex = value - 1;//newIndex is value minus 1.
  if (newIndex == -1)// if the newIndex is equalled to -1 then make it equal to 7 again.
  {
    newIndex = 7;
  }

  return newIndex;
}




