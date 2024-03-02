# Raspberry-tm1637
TM1637 Driver for RaspberryPi.   
I based it on [this](https://github.com/petrows/esp-32-tm1637).   

![tm1637-ip](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/15cb623d-2298-4344-a9c7-f573cb0255cd)
![tm1637-play](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/bb98913e-f5e5-4f17-8d12-4cd9f16518f7)
![tm1637-stop](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/b1928da8-c461-4772-8d33-8f82175c5f54)

# Changes from the original
- Applied to WiringPi.   
- Added display of ascii characters.   
- Added text scroll display function.   
- Added text animation display function.   

# Software requirement
WiringPi Library   
This project uses the wiringPiSetupGpio() function to initialize GPIO.   
If you use it on a board other than the RPI board, you may need to change the GPIO number.   
As far as I know, there are these libraries.   
- WiringPi for OrangePi   
- WiringPi for BananaPi   
- WiringPi for NanoPi   
- WiringPi for Pine-64   

# Hardware requirement
TM1637 8 segment 0.36INCH Digital Display Tube 4 digit LED module.   
There are several products with different segments.   
- Product 1   
 8 segments with dots + 8 segments with dots + 8 segments with dots + 8 segments with dots   
- Product 2   
 7 segments + 8 segments with colon + 7 segments + 7 segments   

![TM1637-1](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/59e95bf5-01bb-4c4a-9f96-4d36869be2e5)
![TM1637-2](https://github.com/nopnop2002/esp-idf-tm1637/assets/6020549/e3430e3a-34ae-455a-8891-555ae89f2e0f)

# Build
```
git clone https://github.com/nopnop2002/Raspberry-tm1637
cd Raspberry-tm1637
cc -o tm1637 main.c tm1637.c -lwiringPi -lpthread
```

# Wirering

|TM1637||RPi||
|:-:|:-:|:-:|:-:|
|CLK|--|GPIO3|(*1)|
|DIO|--|GPIO2|(*1)|
|GND|--|GND||
|VCC|--|3.3V||

(*1)   
The TM1637's interface is similar to I2C, but it is not I2C.   
Therefore, you can change it to any pin using arguments.   

# How to use
```
$ ./tm1637 --help
./tm1637: invalid option -- '-'
usage: tm1637  [-c CLK] [-d DIO] [-t TEXT]

optional arguments:
  -c CLK    CLK GPIO
  -d DIO    DIO GPIO
  -t TEXT   Display Text
```

### Start Auto Demo using default GPIO   
```
./tm1637
```

### Start Auto Demo using non-default GPIO   
Example of CLK=GPIO14/DIO=GPIO15.   
```
./tm1637 -c 14 -d 15
```

### Display any text
```
./tm1637 -t "12345 67890"
```


