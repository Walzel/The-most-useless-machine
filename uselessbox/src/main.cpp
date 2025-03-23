#include <Arduino.h>



const int switches_pinout [8] = {2,3,4,5,6,7};
long switches_steps[6] = {20500,17000,13500,10000,6500,3500};
int switcher_steps[3] = {0,800,1100}; //[zero point, active, switch action]
int opener_steps[2] = {0,40}; //[closed, opened box]
int array_switches[6] = {0,0,0,0,0,0};

int current_switching_mode = 0;
int next_switch = 0;

//MOVER
int current_position = 0;
long current_step = 20500;
//OPENER
int opener_current_step = 0;
//SWITCHER
int switcher_current_step = 0;


//Stepper speeds #todo set speeds properly
int speed_mover_delay = 100;
int speed_opener_delay = 5000;
int speed_switcher_delay = 700;

int queue_counter = 0;

volatile bool button_changed = false;


//----------------------FUNCTIONS-------------------------
int main_movement_control(int next_switch);
bool check_switch();
void make_step(bool fd_or_bk, int pin_PUL, int pin_DIR, int delay_time);
void move_arm_to_switch(int switch_select);

int set_closest_switch();
int set_random_switch();
int set_user_switch();

void set_next_switch();

void return_to_zero();


bool open_close_box();
bool check_open_close_box();
void read_switches();
void open_box(int open_close);
int stack_past_switches [6] = {-1,-1,-1,-1,-1,-1};


unsigned long lastCheckTime = 0;
unsigned long currentTime = 0;


void setup() {
  for (int i = 0; i < 8; i++)
  {
    pinMode(switches_pinout[i],INPUT_PULLUP);  //pins for switches
  }
  pinMode(A0, INPUT_PULLUP);  //reset_mover
  pinMode(A1,INPUT_PULLUP);   //reset opener
  pinMode(A2,INPUT);   //mode selection potentiometer
  pinMode(A3,INPUT_PULLUP);   //reset all


  //STEPPER MOVER
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //STEPPER SWITCHER
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  //STEPPER OPENER
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);



    // Enable Pin Change Interrupts for PCINT0 (D8-D13) and PCINT2 (D0-D7)
  PCICR |= (1 << PCIE0) | (1 << PCIE2);  

  // Enable PCINT for pins 2-9
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23); // D2-D7
  PCMSK0 |= (1 << PCINT0) | (1 << PCINT1); // D8-D9




  Serial.begin(9600); 
  return_to_zero();
  set_next_switch();
  Serial.println(next_switch);

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
  currentTime = millis();  // Get the current time

    if ((currentTime - lastCheckTime) >= 200) {  // Check if 200ms have passed
        lastCheckTime = currentTime;  // Update the last check time
        if (button_changed) {
            set_next_switch();  // Call function when any button is pressed
            Serial.print("changed");
            button_changed = false;  // Reset flag
        }
    }


  
  main_movement_control(next_switch);
  

  //open_box(check_open_close_box());


  //open_box(digitalRead(4));

}


//-----------------SETUP FUNCTIONS--------------------------
bool check_open_close_box()               // gives back bool for an open or close box
{
  read_switches();
  for (int i = 0; i < 6; i++)
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
  for (int i = 0; i < 6; i++)
  {
    array_switches[i] = digitalRead(switches_pinout[i]);
  }
}
void return_to_zero(){
  while(digitalRead(2)==0)                //required to switch first switch on to activate process
  {
    delay(1);
  }


//RESET OPENER
  for (int i = 0; i < 20; i++)   //#todo check right direction
  {
    make_step(1,12,13,speed_opener_delay*2);
  }
  int back_to_zero = digitalRead(A1);
  while (back_to_zero == 1)
  {
    make_step(0,12,13,speed_opener_delay*2);
    back_to_zero = digitalRead(A1); //readswitch
  }
  for (int i = 0; i < 85; i++)
  {
    make_step(1,12,13,speed_opener_delay);
    opener_current_step =  opener_current_step + 1;

  }
  opener_current_step = 0;               //set new 0
  

  //MOVER RESET
  for (int i = 1; i < 400; i++)
  {
    make_step(1,8,9,speed_mover_delay);
  }
  back_to_zero = digitalRead(A0);
  while (back_to_zero == 1)
  {
    make_step(0,8,9,speed_mover_delay);
    back_to_zero = digitalRead(A0); //readswitch
  }
  current_step = 0;               //set new 0
  move_arm_to_switch(0);

    //RESET SWITCHER
  for (int i = 0; i < 30; i++)   //#todo check right direction 
  {
    make_step(0,10,11,speed_switcher_delay);
  }
  back_to_zero = digitalRead(2);
  while (back_to_zero == 1)      
  {
    make_step(1,10,11,speed_switcher_delay);
    back_to_zero = digitalRead(2); //readswitch
  }
  switcher_current_step = switcher_steps[0];
  // go to rest position
  for (int i = 0; i < 1100; i++)
  {
    make_step(0,10,11,speed_switcher_delay);
  }
  switcher_current_step = switcher_steps[0];
    


  





}
int set_mode(){
  int mode_seletion = analogRead(A2);
  if (mode_seletion < 340)
  {
    return 0;
  }
  else if (mode_seletion > 680)
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
void read_switches() {
  for (int i = 2; i < 8; i++) {
      array_switches[i-2] = digitalRead(i);  // Read pin state into array
  }
}

//----------------MAIN FUNCTIONS--------------------------------------------
int main_movement_control(int position)             //goes to specified switch and flips switch
{
  if (position == -1)
  {
    //open_box(0);
    return 0;
  }
  //open_box(1);

  if (current_step < switches_steps[next_switch])
  {
    make_step(1,8,9,speed_mover_delay); //move 1step forward
    current_step = current_step + 1;
  }
  else if (current_step > switches_steps[next_switch])
  {
    make_step(0,8,9,speed_mover_delay); //move 1step backwards
    current_step = current_step - 1;
  }
  else if (current_step == switches_steps[next_switch])
  {
    for (int i = 2; i > 0; i--)
    {    
      long steps_to_move_switcher = switcher_steps[i] -switcher_current_step;
      if (steps_to_move_switcher > 0)
      {
        for (long i = 0; i < steps_to_move_switcher; i++)
        {
          make_step(1,10,11,speed_switcher_delay);
          switcher_current_step = switcher_current_step + 1;
        }
        
      }   
      else
      {
        for (long i = 0; i < -steps_to_move_switcher; i++)
        {
          make_step(0,10,11,speed_switcher_delay);
          switcher_current_step = switcher_current_step - 1;
        }
      }
    }
    
  return 0;
  }
}
void set_next_switch()
{
  update_switch_array();
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
  Serial.println(current_switching_mode);
  
  
}

//----------------ACTION FUNCTIONS
void open_box(int open_close)             // opens or closes box depnding on input
{  

  if (open_close == 1)
  {
    if (opener_current_step < 40)
    {
      for (int i = 0; i < 40; i++)
      {
        make_step(1,12,13,speed_opener_delay);
        opener_current_step = opener_current_step + 1;
      }
       
    }
    
    if (switcher_current_step > switcher_steps[1])
    {
      while (switcher_current_step != switcher_steps[1])
      {
        make_step(0,10,11,speed_switcher_delay);
        switcher_current_step = switcher_current_step - 1;
      }
      
      
    }
    else
    {
      while (switcher_current_step != switcher_steps[1])
      {
        make_step(1,10,11,speed_switcher_delay);
        switcher_current_step = switcher_current_step + 1;
      }
      
    }
  }
  else if (open_close == 0)
  {
    if (opener_current_step > 0)
    {
      for (int i = 0; i < 40; i++)
      {
        make_step(0,12,13,speed_opener_delay);
        opener_current_step = opener_current_step - 1;
      }
      
    }
    if (switcher_current_step > switcher_steps[0])
    {
      while (switcher_current_step != switcher_steps[0])
      {
        make_step(0,10,11,speed_switcher_delay);
        switcher_current_step = switcher_current_step - 1;
      }
      
      
    }
    else
    {
      while (switcher_current_step != switcher_steps[0])
      {
        make_step(1,10,11,speed_switcher_delay);
        switcher_current_step = switcher_current_step + 1;
      }
      
    }
  }
}
void make_step(bool forward_backward, int pin_PUL, int pin_DIR,int delay_time)
{
  if (forward_backward)
  {
    digitalWrite(pin_PUL,HIGH);
    digitalWrite(pin_DIR,LOW);
    digitalWrite(pin_PUL,LOW);
    digitalWrite(pin_DIR,HIGH);
  }
  else
  {
    digitalWrite(pin_PUL,LOW);
    digitalWrite(pin_DIR,LOW);
    digitalWrite(pin_PUL,HIGH);
    digitalWrite(pin_DIR,HIGH);
  }
  delayMicroseconds(delay_time);
  
}
void switches_switch(int switch_select)
{
  make_step(1,10,11,speed_switcher_delay);
}
void move_arm_to_switch(int switch_select)
{
  long steps_to_move = switches_steps[switch_select]-current_step;
  if (steps_to_move > 0)
  {
    for (long i = 0; i < steps_to_move; i++)
    {
      make_step(1,8,9,speed_mover_delay);
      current_step = current_step + 1; 
    }
    
  }
  else
  {
    for (long i = 0; i < -steps_to_move; i++)
    {
      make_step(0,8,9,speed_mover_delay);
      current_position = current_position -1;
    }
  }
  
}

//-------------------MODE 1 (CLOSEST SWITCH)------------------------------------
int set_closest_switch()                       // flips closest switch
{

  current_position = findClosestIndex(switches_steps, 6, current_step);

  for (int offset = 0; offset < 6; offset++) {
    int left = current_position - offset;
    int right = current_position + offset;

    if (left >= 0 && array_switches[left] == 1) {
        return left;  // Closest active element on the left
    }
    if (right < 6 && array_switches[right] == 1) {
        return right;  // Closest active element on the right
    }
}
return -1;
}
//------------------MODE 2 (RANDOM SWITCH)-------------------------------------
int set_random_switch(){
  int counter_active_switches = 0;
  int offset_array_active_switches = 0;
  int array_active_switches[6] ={0,0,0,0,0,0};
  for (int i = 0; i < 6; i++)
  {
      if (array_switches[i]==1)
      {
        array_active_switches[i-offset_array_active_switches] = i;
        counter_active_switches += 1;
      }
      else
      {
        offset_array_active_switches = offset_array_active_switches + 1;
      }
  }
if (counter_active_switches==0)
{
  return -1;
}


  int random_switch_auswahl = random(0,counter_active_switches);
  return array_active_switches[random_switch_auswahl];
}
//-----------------MODE 3 (FOLLOW USER)-----------------------------------------
int set_user_switch(){
  for (int i = 5; i >= 0; i--)
  {
    if (stack_past_switches[i] != -1)
    {
      return stack_past_switches [i];
    }
    
  }
  return 0;
}

