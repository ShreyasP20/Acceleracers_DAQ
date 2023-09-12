 void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}
int prev_value=1;
int current_value;
float curr_time;
float read_time;
float interval;
float count=0;
float rpm;
float rpm2;
int count_rotations;
  
void loop() {
  // put your main code here, to run repeatedly:
  current_value=digitalRead(8);
  
//  if(digitalRead(8)==LOW)
//  { 
//    curr_time = millis();
//    count=1;
//    count_rotations = count_rotations+1 ;
//    Serial.println(curr_time);
//    delay(10);
////  }
// if(current_value == 0 && prev_value == 1 && count==1)
//  {
//    count_rotations = count_rotations + 1;
//    count=0;
//    read_time=millis();
//    Serial.println(read_time);
//    interval = read_time-curr_time;
//    Serial.print("Interval:");
//    Serial.println(interval);
//    rpm = (float)((1/interval)*60000);
//    Serial.print("RPM:");
//    Serial.println(rpm);
//    Serial.print("Count_Rotations:");
//    Serial.println(count_rotations);
//  }
//  
  if(current_value == 0 && prev_value == 1)
  {
    if(count_rotations==0)
    {
     curr_time = millis();
     Serial.println(curr_time);
    }   
     count_rotations = count_rotations+1 ;
     //Serial.println(curr_time);
         
  }
  if(count_rotations%6==0)
  {
   read_time = millis();
   interval = read_time-curr_time;
   Serial.print("Interval:");
   Serial.println(interval);
   rpm = (float)((1/interval)*60000);
   count_rotations=0;
   rpm2 = (float)((1/interval)*360000);
   Serial.print("RPM:");
   Serial.println(rpm);
   Serial.print("RPM_2:");
   Serial.println(rpm2);
   
  }
  prev_value = current_value;
}
