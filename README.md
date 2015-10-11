# Tempino, the Arduino sensors display
Software for the Tempino Arduino project, that allows you to monitor temperatures and fan speed in 
a nicely displayed way, using Adafruit and clones LCD screens. You'll need Adafruit's GFX Library 
https://github.com/adafruit/TFTLCD-Library for the arduino project in client/tempino and the
python script requirements are in it's own requirements.txt file for you to create a virtualenv.

# How it works
We just open the serial port (over the USB) and send a string with the information, that the Arduino
then shows up on its screen. The "server" can be closed with Ctrl-C.

You can see it in action here: https://www.youtube.com/watch?v=UGLoJ1REsmY

# License

All this code is GPL'd.
