#include <Arduino.h>


const int switches_pinout [8] = {2,3,4,5,6,7,8,9};
long switches_steps[8] = {400,3000,6000,9000,12000,15000,18000,21000};

int current_switching_mode = 0;
int next_switch = 0;

//MOVER
int current_position = 0;
long current_step = 5000;
//OPENER
long opener_current_step = 0;
//SWITCHER
long switcher_current_step = 0;



//int speed = xx;                    für geschwindigkeit der steps definieren

volatile bool button_changed = false;


//----------------------FUNCTIONS-------------------------
void flip_switch(int);
bool check_switch();
void make_step(bool forward_backward);
void move_arm_to_switch();

int set_closest_switch();
int set_random_switch();
int set_user_switch();

void set_next_switch();

void return_to_zero();


bool open_close_box();
void open_box(bool);
int array_switches [8] = {0,0,0,0,0,0,0,0};
int stack_past_switches [9] = {0,0,0,0,0,0,0,0,0};

//#todo
//implement move stepper function



void setup() {
  for (int i = 0; i < 8; i++)
  {
    pinMode(switches_pinout[i],INPUT_PULLUP);  //pins for switches
  }
  pinMode(A0, INPUT);  //reset_mover
  pinMode(A2,INPUT);   //mode selection potentiometer


//#todo
//implement pin setup for steppers


    // Enable Pin Change Interrupts for PCINT0 (D8-D13) and PCINT2 (D0-D7)
  PCICR |= (1 << PCIE0) | (1 << PCIE2);  

  // Enable PCINT for pins 2-9
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23); // D2-D7
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1); // D8-D9




  Serial.begin(9600); 
  return_to_zero();

}

//---------------------interrupt setup---------------------------
// ISR for pins D2-D7 (PCINT2 group)
ISR(PCINT2_vect) {
  // Check for any pin change in the range D2-D7
  for (int i = 2; i <= 7; i++) {
      if (digitalRead(i) == LOW) { // Button pressed (active LOW)
          button_changed = true;
          break;  // Exit loop once a button press is detected
      }
  }
}

// ISR for pins D8-D9 (PCINT0 group)
ISR(PCINT0_vect) {
  // Check for any pin change in the range D8-D9
  for (int i = 8; i <= 9; i++) {
      if (digitalRead(i) == LOW) { // Button pressed (active LOW)
          button_changed = true;
          break;  // Exit loop once a button press is detected
      }
  }
}





void loop() {
  if (button_changed) {
    set_next_switch();  // Call function when any button is pressed
    button_changed = false;  // Reset flag
}
  flip_switch(next_switch);
  
}


//-----------------SETUP FUNCTIONS--------------------------
bool check_open_close_box()               // gives back bool for an open or close box
{
  for (int i = 0; i < 8; i++)
  {
    if (array_switches[i]==1)
    {
      return true;
    }
  }
  return false;
}
void update_switch_array()
{
  for (int i = 0; i < 8; i++)
  {
    array_switches[i] = digitalRead(switches_pinout[i]);
  }
      for (int e=0; e < 9; e++){
        if (stack_past_switches[e] == 0){
            stack_past_switches[e] = e+1;
            break;
        }
      }
}
void return_to_zero(){
  //-----------------MOVER RESET---------------------
  //move 400 steps fd
  current_step =+ 400;
  Serial.print("Current step: ");
  Serial.println(current_step);
  int back_to_zero = digitalRead(A0);
  Serial.println(back_to_zero);
  while (back_to_zero == 0)
  {
    back_to_zero = digitalRead(A0); //readswitch
    current_step = current_step - 4; //move towards zero
    Serial.print("Current step: ");
    Serial.println(current_step);
    delay(10);
  }
  current_step = 0;               //set new 0
  Serial.print("Current step: ");
  Serial.println(current_step);
  //---------------RESET OPENER-------------------------
  //like above
  //--------------RESET SWITCH----------------------
  //jse one of the switches as reset button


}
int set_mode(){
  int mode_seletion = analogRead(A2);
  if (mode_seletion < 340)
  {
    return 0;
  }
  else if (mode_seletion > 340)
  {
    return 2;
  }
  else 
  {
    return 1;
  }
  

}
int findClosestIndex(long arr[], int arrSize, long targetValue) {
  long closestIndex = 0;            // Start with the first element
  long minDifference = abs(arr[0] - targetValue);  // Calculate the difference for the first element

  for (int i = 1; i < arrSize; i++) {  // Loop through the rest of the array
      long currentDifference = abs(arr[i] - targetValue); // Calculate the current difference

      // If the current difference is smaller, update the closest index
      if (currentDifference < minDifference) {
          minDifference = currentDifference;
          closestIndex = i;
      }
  }

  return closestIndex;  // Return the index of the closest element
}


//----------------MAIN FUNCTIONS--------------------------------------------
void flip_switch(int position)             //goes to specified switch and flips switch
{
  
  open_box(check_open_close_box());
  if (current_step < switches_steps[next_switch])
  {
    make_step(1); //move 1step forward
  }
  else if (current_step > switches_steps[next_switch])
  {
    make_step(0); //move 1step backwards
  }
  else if (current_step == switches_steps[next_switch])
  {
    move_arm_to_switch(); //switches the switch
    if (current_switching_mode==2)
    {
      int temporary_array[9];                                  //FIFO on array
      for (int i = 1; i < 9; i++)
      {
            temporary_array[i-1]=stack_past_switches[i];
        }  
        temporary_array[8]=0;
      for (int i = 0; i < 8; i++)
        {
            stack_past_switches[i]=temporary_array[i];
        }
    }
  }
    

}
void set_next_switch()
{
  //read all the switch_pins
  update_switch_array();
  //is posibility to switch
  if (check_open_close_box()==true)
  {
    open_box(check_open_close_box());
  }
  //set mode
  current_switching_mode = set_mode();
  //define next swich
  if (current_switching_mode == 0)
  {
    next_switch = set_random_switch();
  }
  else if (current_switching_mode == 1)
  {
    next_switch = set_closest_switch();
  }
  else if (current_switching_mode ==2)
  {
    next_switch = set_user_switch();
  }
  
  
  
}

//----------------ACTION FUNCTIONS
void open_box(bool open_close)             // opens or closes box depnding on input
{  
  if (open_close==true)
  {
    //open lid
  }
  else
  {
    //close lid
  }
  
}
void make_step(bool forward_backward)
{
  //1 steps auf einmal
}
void move_arm_to_switch()
{
  //move arm to switch and back
}


//-------------------MODE 1 (CLOSEST SWITCH)------------------------------------
int set_closest_switch()                       // flips closest switch
{

  int back = 0;                                  // checks distance back and front
  int front = 0;
  current_position = findClosestIndex(switches_steps, 8, current_step);

  for (int i = current_position; i <= 8; i++)
  {
    if (array_switches[i]==1)
    {
      break;
    }
    else
    {
      front = front + 1;
    }

  }

  for (int i = current_position; i >= 0; i--)
  {
    if (array_switches[i]==1)
    {
      break;
    }
    else
    {
      back = back + 1;
    }
  }

  if (front == back)                       //flips closest switch
  {
    return (current_position-back);
  }
  else if (front < back)
  {
    return (current_position + front);
  }
  else if (back < front)
  {
    return (current_position - back);
  }
}
//------------------MODE 2 (RANDOM SWITCH)-------------------------------------
int set_random_switch(){
  int counter_active_switches = 0;
  int array_active_switches[8] ={0,0,0,0,0,0,0,0};
  for (int i = 0; i < 8; i++)
  {
      if (array_switches[i]==1)
      {
        array_active_switches[i] = i;
        counter_active_switches += 1;
      }
  }
  int random_switch_auswahl = random(counter_active_switches);
  return array_active_switches[random_switch_auswahl];
}
//-----------------MODE 3 (FOLLOW USER)-----------------------------------------
int set_user_switch(){
  return (stack_past_switches[0]);

}

