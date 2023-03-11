# EE303 - Mobile Robotics Group Project

This repository contains the code for the EE303 module group project which involves the development of an automatic robot that can follow a white line, make HTTP calls to a server to get the next checkpoint position and move there. Additionally, the team implemented an innovative feature where the microcontroller acts as a server after completing the automatic journey to be controlled from a front-end display website.

## Group Members

- Adelin Diac (me)
- Cian Foy
- Thomas Carrigan
- Sean Connaghan

## Robot Design

Our team designed a small robot using a MSP432 microcontroller and 5 IR sensors that allows the robot to follow a white line. The robot also has a Wi-Fi module (CC3000 Wifi Booster) to connect to a server and receive the next checkpoint position through HTTP calls.

## Project Structure

```javascript
├── frontend_display
| └── index.html
├── http_control_as_server
| └── http_control_as_server.ino
├── robot
| └── robot.ino
├── nodered_robot_server
└── README.md
```

## Frontend Display

The frontend_display directory contains the index.html file, which serves as the frontend website to control the robot.

## HTTP Control as Server

The http_control_as_server directory contains the http_control_as_server.ino file, which is the firmware for the microcontroller. This firmware allows the robot to act as a server and receive HTTP requests from the frontend website. This is not necessary to be installed, it was more for testing the code before adding to the final file.

## Robot

The robot directory contains the robot.ino file, which has the main moving, control, and server functionality and is the final draft of the code used on race day. The track along with the visible checkpoints is below. The tricky part about this track is that there are two positions which are not checkpoints, however there is an intersection of lines. Checkpoint 5 was always the final position where the robot had to park as close as possible to a wall using a distance IR sensor.

![EE303 Path](https://raw.githubusercontent.com/adelin-diac/Automated-Guided-Robot/master/Path.svg)

## Node-RED Robot Server

The nodered_robot_server file was initially used to connect the device controlling the robot but is not essential in the final draft. This is a node-red server which could be ran on any device on the same network as the robot.

## Installation and Setup

In order for the root to work, a couple of pre-requisits have to be filled in. Firstly, the SSID and password of the wifi need to be filled in so the robot can be connected to the internet. The server string is the server to which the HTTP calls would be made so that the next position on the track can be retrieved.

For the remote control, there is a "control" vairable which is stored in the node-red server mentioned above, however this can be removed if necessary. It is only there so that control can be taken off and robot can't be controlled using the buttons on the "Frontend Display". To control the robot, various HTTP GET requests are made from the "Frontend Display" to tell the robot which action to perform.

Inside the frontend display index.html, the IP of the robot can be set so that communication between the robot and the controlling device can happen. The node-red server can also be ran however it is not necessary.

## Conclusion

Overall, our team was able to successfully develop an automatic robot that can follow a white line and communicate with a server to retrieve checkpoint positions. Additionally, our innovative feature of making the robot act as a server after completing the automatic journey allowed us to control the robot from a front-end website.
