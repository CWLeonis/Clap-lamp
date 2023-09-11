# Clap-lamp-
Smart switch that can control the lamp and electronic appliances with your clap, app and webpage
# Introduction
in this iot project created a smart switch using Wemos D1 mini and KY-038 module. it control devices and lamps with clap. also it can power off and on using Android app and Webpage.
In the webpage, there is report tab that can monitor sounds detected by the module. also monitoring supports real time zone where ever you are because the code was written unix time stamp, so in other words it can provides exact date and time for reporting.
The app using MQTT Protocol to connect to the bord. make sure you have stable connection هn order to use the app. also you can change the MQTT server.
Webpage has html , css and js code. in order to use the webpage read the "HOW TO USE IT" section.

# Requirements
ESP8266(wemos d1 mini)
LEDs
KY-038 (mic. module)

# How to use it
**DO NOT REOMVE THE data FOLDER OR ITS CONTENT**
you download or clone the repository and in arduino ide from the tools menu select "sketch data upload" to upload the webpage code on the connected esp8266 module then run the "final-proj.ino" in arduino ide. 
after uploading the code on the board type the ip address of the board in browser and then you can control the lamps using your browser.
on the Android app just click on connect and then you're good to go.

# At the end...

I hope you have fun with this project,also if you have any issue running the codes feel free to ask :)
AND
don't forget to give me a star that shines just like you ;)
