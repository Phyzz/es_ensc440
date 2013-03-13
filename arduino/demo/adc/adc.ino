int analogPin = 0;

int samples[512];
int sample_index = 0;
int sample_rate = 1000; // in microseconds


void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Begining");
  Serial.println(samples[0]);
  
  for(sample_index = 0; sample_index < 512; ++sample_index) {
    samples[sample_index] = analogRead(analogPin);
    delayMicroseconds(sample_rate);
  }
  delay(1000);

  Serial.println("standing");
  while(1) {
    if (Serial.read() > 0) {
      for(sample_index = 0; sample_index < 512; ++sample_index) {
        Serial.println(samples[sample_index]);
        
      }
      Serial.println("Done");    
    }
  }
}

void loop() {
}
