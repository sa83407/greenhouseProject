# Greenhouse Project
The team project consists of MATLAB, Processing, and Arduino interface. With the use of an Arduino Mega Microcontroller, data is obtained and displayed using the MATLAB and Processing software. The Arduino project is implemented at the Biology Greenhouse Complex at California State University, Fullerton. There are 10 greenhouses and some require different temperature and humidity parameters to ensure there is no damage or loss of plant life. The Arduino project is programmed to send an SMS alert when the temperature and humidity falls below or rises above the desired threshold. It will also alert the manager if the main campus power is shut down. MATLAB will be used to analyze data collected. It will display desired data from each sensor such as the data collected every 15 minutes, as well as the high and low temperatures for the selected date. The real-time data is displayed on a monitor with the use of a raspberry pi 3 and Processing software.

# Background:
An Arduino Mega 2560 microcontroller is used for the project. Ten HTU21D-F
temperature and humidity sensors are used to collect data. Ten OLED displays are used to display the data in each greenhouse. Twenty SparkFun Differential I2C Breakout boards and 2 SparkFun Qwiic Mux Breakout Boards are used to connect ten CAT5 cables of different length in order deploy the sensors in the different greenhouses. One FONA is used to send the SMS notifications. A Raspberry Pi 3 is used to display the text data with the help of the software Processing. An SD card and clock module are also connected to the Arduino to keep record of data collected. MATLAB is used to graphically display temperature and humidity data for a specific day chosen by the user as well as the high and low of that day.


![greenhouses](https://i.ibb.co/3WJp4j2/Screenshot-from-2019-07-08-19-21-33.png)

# Matlab GUI Screenshots:

## Initial GUI Screen:
![s1](https://i.ibb.co/sCgMcXj/s1.png)

## Select Greenhouse:
![s2](https://i.ibb.co/Ybj1TbV/s2.png)

## Data displayed for selected Greenhouse:
![s3](https://i.ibb.co/0FmJX4m/s3.png)
