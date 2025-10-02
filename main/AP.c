


unsigned long t1 = 0;
unsigned long t2 = 0;
const int led[] = {2, 3, 4, 5};
bool ledState = false;
int  ledNum = 0;
#define BTN 6


void ISR_BTN(){
    const float rez = 5.0f / 1023.0f;

    int val = analogRead(A0);
    Serial.print(val*rez);
    Serial.println(" V.");
}


void setup(){
    for(int i = 0; i < 4; i++){
        pinMode(led[i], OUTPUT);
    }
    pinMode(BTN, INPUT);
    pinMode(A0, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(BTN), ISR_BTN, RISING);

}

void loop(){
    if(millis() - t1 >= 300){
        t1 = millis();
        digitalWrite(led[ledNum], ledState);
        ledState = (ledState) ? false : true;

    }
    if(millis() - t2 >= 2000){
        t2 = millis();
        digitalWrite(led[ledNum], false);
        if(ledNum == 3) ledNum = 0;
        else ledNum++;
    }
}
