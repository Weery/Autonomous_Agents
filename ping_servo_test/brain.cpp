#include "Brain.h"

void Brain::InitializePins(byte pin_servo_ping, byte pin_sensor_ultrasonic_trig,byte pin_sensor_ultrasonic_echo, byte pin_sensor_ultrasonic_upper)
{
    _pin_servo_ping = pin_servo_ping;
    _servo_ping.attach(_pin_servo_ping);
    
    _pin_sensor_ultrasonic_trig=pin_sensor_ultrasonic_trig;
    _pin_sensor_ultrasonic_echo=pin_sensor_ultrasonic_echo;
    
    _pin_sensor_ultrasonic_upper=pin_sensor_ultrasonic_upper;
    
    pinMode(_pin_sensor_ultrasonic_echo,INPUT);
    pinMode(_pin_sensor_ultrasonic_trig,OUTPUT);
    
    _servo_ping_signal=90;
}

void Brain::Run()
{
    int readings_lower[38] = {};
    int readings_upper[38] = {};
    int min_reading = 1000;
    int min_angle = 90;
    _servo_ping.write(45);
    delay(60);
    
    for (int i = 0; i <= 18; i++)
    {
        int angle = 45+i*5;
        _servo_ping.write(angle);
        int ultra_upper= Brain::RCTime(_pin_sensor_ultrasonic_upper);
        int ultra_lower=Brain::ReadUltrasonic4Pin();
        readings_lower[i]=ultra_lower;
        readings_upper[i]=ultra_upper;
        if (ultra_lower<min_reading && abs(ultra_upper-ultra_lower)>5)
        {
            min_reading = ultra_lower;
            min_angle = angle;
        }
        delay(20);
    }
    for (int i = 19; i <=37 ; i++)
    {
        int angle = 135-(i-19)*5;
        _servo_ping.write(angle);
        int ultra_upper= Brain::RCTime(_pin_sensor_ultrasonic_upper);
        int ultra_lower=Brain::ReadUltrasonic4Pin();
        readings_lower[i]=ultra_lower;
        readings_upper[i]=ultra_upper;
        if (ultra_lower<min_reading && abs(ultra_upper-ultra_lower)>5)
        {
            min_reading = ultra_lower;
            min_angle = angle;
        }
        delay(20);
    }
    
    Serial.print("Best Length:");
    Serial.println(min_reading);
    
    Serial.print("Best angle:");
    Serial.println(min_angle);
    /*
    int ultra_upper= Brain::RCTime(_pin_sensor_ultrasonic_upper);
    int ultra_lower=Brain::ReadUltrasonic4Pin();
    
    Serial.print("Lower:");
    Serial.println(ultra_lower);
    
    Serial.print("Upper:");
    Serial.println(ultra_upper);*/
    
    _servo_ping.write(min_angle);
    
    delay(2000);
}


int Brain::ReadUltrasonic()
{
    int ultra_upper= Brain::RCTime(_pin_sensor_ultrasonic_upper);
}

int Brain::ReadUltrasonic4Pin()
{
    short maximumRange = 400;
    short minimumRange = 0;

     digitalWrite(_pin_sensor_ultrasonic_trig, LOW);
     delayMicroseconds(2);

     digitalWrite(_pin_sensor_ultrasonic_trig, HIGH);
     delayMicroseconds(10);

     digitalWrite(_pin_sensor_ultrasonic_trig,LOW);
     float duration = pulseIn(_pin_sensor_ultrasonic_echo, HIGH);
     float distance = duration/58.2;
     if (distance >= maximumRange || distance <= minimumRange){
        distance = 1000;
     }


     return floor(distance);
}
    
bool ReadIr(byte pin_reciever)
{
    bool ir = digitalRead(pin_reciever);
    return ir;
    
}   
 
byte Brain::ReadIrDistance(byte pin_reciever, byte pin_transmitter)
{
    unsigned short frequencies[5] = {38000,39000,40000,41000,42000};
    byte distances[5] = {40,35,30,25,20}; //TBD
    byte distance = 0;
    for(short i = 4; i >= 0; i--)
    {
        bool detection = Brain::ReadIrWithTransmitter(pin_transmitter, pin_reciever, frequencies[i]);
        if (!detection){
            distance = distances[i];
            break;
        }
    }
    return distance;
}

bool Brain::ReadIrWithTransmitter(byte pin_reciever, byte pin_transmitter, unsigned short frequency)
{
    tone(pin_transmitter, frequency, 8);
    delay(1);
    int ir = digitalRead(pin_reciever);
    return ir;
}