 void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float curr_time;
  float read_time;
  float interval;
  float count=0;
  float rpm;
  float count_rotations;
  if(digitalRead(8)==LOW)
  { 
    curr_time = millis();
    count=1;
    count_rotations = count_rotations+1 ;
    Serial.println(curr_time);
    delay(10);
  }
 if(digitalRead(8)==LOW && count==1)
  {
    count_rotations++;
    count=0;
    read_time=millis();
    Serial.println(read_time);
    interval = read_time-curr_time;
    Serial.print("Interval:");
    Serial.println(interval);
    rpm = (float)(1/interval*6)*60;
    Serial.print("RPM:");
    Serial.println(rpm);
    Serial.print("Count_Rotations:");
    Serial.println(count_rotations/2);
  }
}
