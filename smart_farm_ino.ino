#include <Wire.h>
#include <DHT.h>  

//soil
#define soilPin       A0      
//cds
#define cdsPin        A1      
//dht
#define DHTTYPE       DHT11   
#define DHTPIN        3                  
//r1
#define lightingPin   4
//r2
#define lightingPin2  5        
//pump 
#define pumpPin       6

#define motor 7

DHT dht(DHTPIN, DHTTYPE); //온습도 센서 모듈
unsigned long previousMillis = 0;
const long interval = 2000;

int temp_value = 0;
int humi_value = 0;
int soil_value = 0;
int light_value =0;

int prev_tV = 99;
int prev_hV = 99;
int prev_sV = 1;
int prev_lV = 99;

void setup() 
{
  Serial.begin(9600);

  pinMode(DHTPIN, INPUT);
  pinMode(cdsPin, INPUT);
  pinMode(soilPin, INPUT);
  pinMode(lightingPin, OUTPUT);
  pinMode(lightingPin2, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(pumpPin, OUTPUT);
 
  dht.begin();
}

void loop() 
{
  unsigned long currentMillis = millis();
  
  // 2초마다 온도 값을 갱신
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    //센서값 측정
    int cdsValue = analogRead(cdsPin);               
    //int cds = map(cdsValue, 0, 1023, 2, 98); // 어두울수록 값이 높아지고 밝을수록 값이 낮아집니다.
    int cds = map(cdsValue, 0, 1023, 98, 2);
    
    int soilValue = analogRead(soilPin);
    int soil = map(soilValue, 0, 1023, 99, 2); // 수분이 많을수록 값이 낮아지고, 수분이 적을수록 값이 높아아집니다.
    
    int tempValue = dht.readTemperature();
    int temp = map(tempValue, 0, 100, 1, 99);
    int humi = dht.readHumidity();
        
    Serial.print("Temperature:");
    Serial.print(temp);
    Serial.print(" Humidity:");
    Serial.print(humi);
    Serial.print(" soil_moisture:");
    Serial.print(soil);
    Serial.print(" Light_intensity:");
    Serial.print(cds);

    //Serial.print(" >>cdsValue:");
    //Serial.print(cdsValue);
  
    Serial.println();

    if (Serial.available() > 0) 
    {      
      String data = Serial.readStringUntil(','); // 시리얼로부터 데이터를 읽음
      
      if (data.toInt() == 99 || data.toInt() == 1 || data.toInt() == "unknown"){
        
        if (data == "unknown") 
        {
          //이전 값을 사용하여 변수 설정
          temp_value = prev_tV;
        }
        else 
        {
          temp_value = data.toInt();
          prev_tV = temp_value; //새로운 값을 이전 값으로 업데이트
        }
      
        data = Serial.readStringUntil(',');
      
        if (data == "unknown") 
        {
          humi_value = prev_hV;
        }
        else 
        {
          humi_value = data.toInt();
          prev_hV = humi_value;
        }
      
        data = Serial.readStringUntil(',');
      
        if (data == "unknown") 
        {
          soil_value = prev_sV;
        }
        else 
        {
          soil_value = data.toInt();
          prev_sV = soil_value;
        }
        
        data = Serial.readStringUntil('\n');
        
        if (data == "unknown") 
        {
          light_value = prev_lV;
        }
        else 
        {
          light_value = data.toInt();
          prev_lV = light_value;
        }
      
        control(temp_value, humi_value, soil_value, light_value, temp, humi, soil, cds);   
      }
      else {
        autoControl(temp_value, humi_value, soil_value, light_value, temp, humi, soil, cds);
      }
    }    
  }
}

void autoControl(int temp_value, int humi_value, int soil_value, int light_value, int temp, int humi, int soil, int cds)
{      
  //값안넣고 실행누른경우
  if(temp_value == 98 && humi_value == 99 && soil_value == 1 && light_value == 99)
  {   
    if (temp > 27)
    {
     digitalWrite(motor, HIGH);
    }
    else {
      digitalWrite(motor, LOW);
    }

    if (65 > humi){
      digitalWrite(pumpPin, HIGH);
    }
    else {
      digitalWrite(pumpPin, LOW);
    }

    if (75 > soil){
      digitalWrite(pumpPin, HIGH);
    }
    else {
      digitalWrite(pumpPin, LOW);
    }

    if (30 > cds){
      digitalWrite(lightingPin, HIGH);
      digitalWrite(lightingPin2, HIGH);      
    }
    else {
      digitalWrite(lightingPin, LOW);
      digitalWrite(lightingPin2, LOW);
    }
  }
  else { //값넣고 실행 눌렀을때  
    if (humi > humi_value) {
      digitalWrite(motor, HIGH);
    }
    else {
      digitalWrite(motor, LOW);
    }

    if (temp > temp_value) {
      digitalWrite(motor, HIGH);
    } 
    else {
      digitalWrite(motor, LOW);
    }

    if (cds > light_value) {
      digitalWrite(lightingPin, HIGH);
      digitalWrite(lightingPin2, HIGH);
    }
    else {
      digitalWrite(lightingPin, LOW);
      digitalWrite(lightingPin2, LOW);
    }
    
    if (soil < soil_value) {
      digitalWrite(pumpPin, HIGH);
    }
    else {
      digitalWrite(pumpPin, LOW);
    }
  }
}

void control(int temp_value, int humi_value, int soil_value,int light_value, int temp, int humi, int soil, int cds) {
  
  if (humi > humi_value) {
    digitalWrite(motor, HIGH);
  } 
  else {
    digitalWrite(motor, LOW);
  }

  if (temp > temp_value) {
    digitalWrite(motor, HIGH);
  } 
  else {
    digitalWrite(motor, LOW);
  }
  
  if (soil < soil_value) {
    digitalWrite(pumpPin, HIGH);
  }
  else {
    digitalWrite(pumpPin, LOW);
  }

  if (cds > light_value) {
    digitalWrite(lightingPin, HIGH);
    digitalWrite(lightingPin2, HIGH);
  }
  else {
    digitalWrite(lightingPin, LOW);
    digitalWrite(lightingPin2, LOW);
  }
}
