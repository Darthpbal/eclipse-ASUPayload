
int light2FreqPin = 3;

void setup() {
    Serial.begin(9600);
    pinMode(light2FreqPin, INPUT);
}



void loop() {
    int lightPulse = pulseIn(light2FreqPin, HIGH);
    Serial.println(lightPulse);
    Serial.println();
    delay(100);
}
