#include <Arduino.h>


const int switches_pinout [8] = {2,3,4,5,6,7,8,9};

int current_position = 0;
long current_step = 5000;


// put function declarations here:
void flip_switch(int);
bool check_switch();

int flip_closest_switch();
void flip_random_switch();
void flip_user_switches();

void return_to_zero();


bool open_close_box();
void open_box(bool);
int array_switches [8] = {0,0,0,0,0,0,0,0};
int stack_past_switches [9] = {0,0,0,0,0,0,0,0,0};



void setup() {
  for (int i = 0; i < 8; i++)
  {
    pinMode(switches_pinout[i],INPUT);  //pins for switches
  }
    pinMode(A0, INPUT);  //reset to 0 pin
  Serial.begin(9600); 
  return_to_zero();

}

void loop() {

  /*update_switch_array();                    // writes array with on/off switches
  open_box(check_open_close_box());       // opens or closes box depending on open switches
  flip_switches();*/
  



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
void flip_switch(int position)             //goes to specified switch and flips switch
{
  //go to position "postition"
  // flips switch
  // updates stack

}
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

void return_to_zero(){
  //move 400 steps fd
  current_step =+ 400;
  Serial.print("Current step: ");
  Serial.println(current_step);
  int back_to_zero = digitalRead(A0);
  Serial.println(back_to_zero);
  while (back_to_zero == 0)
  {
    back_to_zero = digitalRead(A0);
    current_step = current_step - 4;
    Serial.print("Current step: ");
    Serial.println(current_step);
    delay(10);
  }
  current_step = 0;
  Serial.print("Current step: ");
  Serial.println(current_step);


}





//-------------------MODE 1 (CLOSEST SWITCH)------------------------------------
int flip_closest_switch()                       // flips closest switch
{
  bool active_switch = false;
  for (int i = 0; i < 8; i++)             // checks wether there is a switch active or not
  {
    if (array_switches[i]==1)
    {
      active_switch = true;
    }
    
  }
  if (active_switch == false)             // if no switch is flipped nothing happens
  {
    return 0;
  }
  

  int back = 0;                                  // checks distance back and front
  int front = 0;
  
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
    flip_switch(current_position-back);
  }
  else if (front < back)
  {
    flip_switch(current_position + front);
  }
  else if (back < front)
  {
    flip_switch(current_position - back);
  }
}
//------------------MODE 2 (RANDOM SWITCH)-------------------------------------
void flip_random_switch(){
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
  flip_switch(random_switch_auswahl);
}
//-----------------MODE 3 (FOLLOW USER)-----------------------------------------
void flip_user_switch(){
flip_switch(stack_past_switches[0]);
  int temporary_array[9];
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

