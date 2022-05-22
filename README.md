# Druid-Plant-Monitoring

## Project Description

  Druid is a plant monitoring system that monitors and displays useful information about the surrounding environment, such as soil moisture, weather, and air quality, to help you plan to manage your garden. Our project uses Bluetooth that connects to an application on your mobile phone. It will send you notifications if the environment suddenly changes or its value becomes overwhelming in order to make you aware and prevent a disaster like a rainstorm, tornado, or drought.


## Diagram

![diagram](https://user-images.githubusercontent.com/79465272/169698275-361143cc-04fa-4c03-97ea-a29d967ef314.png)


## Components and Pins

Components | PINs used 
:------------ | :------------- 
1 x Arduino UNO R3 | -
1 x Soil Moisture Sensor Module | 5V, GND, A0
1 x Air Quality Sensor Module | 5V, GND, A1 
1 x Photoresistor | 5V, GND, A3
1 x I2C 1602 LCD | 5V, GND, A4, A5
1 x HM-10 Bluetooth Module | 5V, GND, D10, D11
1 x Digital Temperature and Humidity Sensor | 5V, GND, D2
1 x Switch | GND, D3



## Arduino Libraries

[LiquidCrystal I2C Library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library)

[Temperature and Humidity Library](https://github.com/adafruit/DHT-sensor-library)

[Adafruit Library (Driver for Sensors)](https://github.com/adafruit/Adafruit_Sensor) 

[Software Serial Library (Changing Rx Tx)](https://github.com/PaulStoffregen/SoftwareSerial)
