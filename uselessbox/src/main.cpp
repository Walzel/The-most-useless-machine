#include <Arduino.h>

const int switch1 = 2;
const int switch2 = 3;
const int switch3 = 4;
const int switch4 = 5;
const int switch5 = 6;
const int switch6 = 7;
const int switch7 = 8;
const int switch8 = 9;

int current_position = 0;


// put function declarations here:
int flip_switch(int);
int flip_switches();
int read_switches();
bool check_switch();

bool open_close_box();
int open_box(bool);
int array_switches [8] = {0,0,0,0,0,0,0,0};


void setup() {

  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT);
  pinMode(switch5, INPUT);
  pinMode(switch6, INPUT);
  pinMode(switch7, INPUT);
  pinMode(switch8, INPUT);

  Serial.begin(9600); 

}

void loop() {

  read_switches();                        // writes array with on/off switches
  open_box(check_open_close_box());       // opens or closes box depending on open switches
  flip_switches();

}

int flip_switch(int position)             //goes to specified switch and flips switch
{
  //go to position "postition"
  // flips switch

}

int open_box(bool open_close)             // opens or closes box depnding on input
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

int flip_switches()                       // flips closest switch
{
  bool active_switch = false
  for (int i = 0; i < 8; i++)             // checks wether there is a switch active or not
  {
    if (array_switches[i]==1)
    {
      active_switch = true;
    }
    
  }
  if (active_switch == false)
  {
    return 0;
  }
  

  int back = 0                                    // checks distance back and front
  int front = 0
  
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

  if (front = back)                       //flips closest switch
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

int read_switches()                       // updates array on open switches
{
  for (int i = 0; i < 8; i++)
  {
    int switch_on_off = digitalRead(i+2);
    if ((switch_on_off)== 1)
    {
      array_switches[i]=1;
    }
  }

}

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


