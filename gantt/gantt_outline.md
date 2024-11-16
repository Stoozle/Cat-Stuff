Working out dependencies

# Physical structure/enclosure

## PCBA Enclosure
    • 3D CAD tutorial/learn basics
    • Make a box in CAD (something simple, but on your own) and printable
    • 3D print the box, for experience in the process
    • Make it more complex - add in spaces for wires to exit, mounting holes, a top that can be screwed on, etc. 
    NOTE: At this point is just needs to have the elements we need for an enclosure, but good enough that once we have our boards

## Bed Structure
    • Brainstorm material types, general requirements, sizing
    • Sketch out structure, all major components
    • Make prototype out of basic cheap material - cardboard or foam board is fine
    • Collect materials - go to hardware store to see what's available, buy

## Display of measurements
    • Research ways to display plot of data to user
    • Test methods of visual data representation
        ○ Screen mounted to bed housing
            • Research libraries for interpretation of user input
            • Implement interface for user input
            • Test if user input (WiFi name) is handled correctly
        ○ Web browser
            • Determine how it will receive information
            • Code interpretation of data received
            • Code data storage scheme
            • Code user output

## Wireless Communication
    • Research methods to transmit data wirelessly from ESP32
    • Test methods (contingent upon selection of web browser, mobile app - Wifi or bluetooth, or combination)
        ○ WiFi 
            • Research how to do http post on ESP32 using either wifi.h or httpclient.h arduino libraries
            • Setup client server scheme on ESP32
            • Test communication with outside server

## Load Cell/Strain Gauge - Sensor 
    • Research types of sensors that can be used for measuring weight
    • Get load cell and ADC, additional components ala datasheet
    • Get force reading from strain gauge and ADC on breakout board and ESP32 dev board
    • Transmit and receive force reading wirelessly 

## Force Sensor Circuit
    • Research force sensors
    • Select force sensors
    • Design force sensor circuit
    • Order force sense circuit parts
    • Test sensitivity with weights according to data sheet
    • Test if ESP32 responds to interpretation logic
    
## Power
    • Research what's required for power needs
    • Design power distribution circuit
    • Order parts
* Test circuit