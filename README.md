![image](https://github.com/user-attachments/assets/537c78b6-64ce-4eb9-b8a7-01ba5e4c9b47)


ASCOM connected DIY Automatic Flat Panel with Arduino and Electroluminescent panel 

This flat panel project was built following existing designs. Its' purpose is not to reinvent the device, but to bring a DIY solution for a flat panel.
I am aware, as you probably are, that there are multiple other DIY panels online, but I wanted to build a variant that does not use a servo motor. 

This functions (and even looks) like any other flat panels available (sans the dew heater available on some) and can be used in any imaging software that has ASCOM capabilities.
The BOM is very short: 
  - An Arduino Nano
  - MX1508 chip
  - GA12 N20 15rpm geared micromotor
  - a PCB mounted potentiometer anywhere between 1k and 20k (I've used a 20k one, but anything in this range is a drop-in replacement)
  - LR2905 N-MOSFET (SMD mounted)
  - two 100uF (min 10V) SMD mounted caps (6.3mm)
  - two 220uF (min 10V) TH mounted caps (6.3mm)
  - 10k , 100 and 220 ohms SMD 0805 resistors
  - Two SMD 0805 100nF caps
  - 1 SS545L SMD diode  
  - 1 BZV55 SMD diode
  - 1 470uH inductor.

Most of these components can be sourced from the modules here https://shorturl.at/lNhVN (inductor and SS45L diode) and here: https://shorturl.at/gths6 (MX1508 chip, 2 100uF SMD caps, BZV55 diode and 220 ohm resistor).


  The EL panel can be bought from AliExpress here: https://shorturl.at/BpqwX. Make sure to select the 5V variant, as it will draw power directly from the Arduino +5V pin (motor + EL panel power draw is low enough not to cause harm to the Arduino)

  To avoid, or, at least minimize the flickering, we will boost the PWM frequency of the PIN 3 on the Arduino to 31kHz. Since we are using pin number 3 to drive our buck convertor, it won't affect any delay times (we are changing the TCCR2B register. TCCR0B affects delay)
I've tested the panel and the Serial functionality is not affected by this frequency increase and, also, the flickering, or "waving of the panel is greatly reduced, compared to the standard frequency. 
  
  I've tested this with my phone camera and I can go down to 1/60 exposure time without noticing any flickering. This is with the camera on my S20 FE, so f/1.8. I suspect that the exposure times on a dedicated camera will need to be greater that 1/60 anyways.


  The light is quite blue, despite their claims that it is white, so I've used a piece transparent red binder cover to bring the color balance closer to white. It might not matter for mono, but I think it does make a difference for color cameras.
On top of the red plastic,I've also added a piece of 2mm plexiglass for rigidity and a matte white translucid binder cover.
Make sure to black out the edges of the plexiglass to avoid internal reflection and light bleeds from the outside. I've also taped the whole outside edge with a black textile tape.

After adding the red and white binder cover, the histogram looks like this, which is good enough.

![image](https://github.com/user-attachments/assets/7bedba7d-d8d7-404f-bc6c-3f016e0d6335)


The panel is held together with 6 screws and another 4 that hold it to the arms. 

The arms are specifically designed to go on the 3mm and 6mm shafts of the motor and potentiometer respectively. They also have a slot for installing a nut and a hole for a locking screw (M3 nut and screw, 14mm length). When mounting the motor, it's a good practice to try and get the shafts' flat portion to line up with the locking screw.
For the potentiometer, it matters less, since the screw might end up holding on the solid part of the shaft. If you can find a potentiometer that has a solid shaft, instead of a D shaft, it would be even better. The potentiometer also acts as a hinge for the panel, so it must be securely mounted in place on the endcap.
For this, we use, once again, epoxy. Stick the shaft through the hole in the endcap, push the potentiometer all the way inside the walls inside, then add epoxy around it. Make sure that 1: You are not gluing the shaft to the body and 2: that the shaft is not leaning in any direction. 
Once this cures, it will be a solid hinge point for the panel.


To fine tune the 0 position, using the Arduino IDE:
  - With no arms attached send the close command and manually turn the potentiometer by hand to the 0 position, until the motor stops spinning.
  - Put the arms on the motor and potentiometer, then mount the panel
  - Send >CLOSE# again and see where they stop.
  - If they stop before the desired position, undo the potentiometer locking screw and turn it 1 or 2 degrees clockwise.
  - Send >OPEN# and then >CLOSE# again. Repeat this until you get the desired 0 position. Make sure to tighten the locking screws well enough so that they won't slip on the shafts.

The pottentiometer I've used is this kind (what I had available at the time. I might change it in the future).

![image](https://github.com/user-attachments/assets/4de30ccc-df25-472a-b501-4cfc316ada51)

A better model would be a mini potentiometer that can be fixed using a nut to the end cap, like this one:

![image](https://github.com/user-attachments/assets/72f5f74d-272d-4478-8b77-2e5b590ae62b)



The motor I've used is a GA12 N20 15rpm geared micromotor (https://shorturl.at/ir1aB) 
Initially, I wanted to use a 28BYJ-48, modified to bipolar mode, to run at 12V, but I found that the gearing was too weak to hold the panel position at "half open" (the panel would easily backdrive the motor). 
This micromotor, as small as it is, has a 1000:1 gearing ratio (compared to 1:64 on the stepper motor). I did not manage to backdrive it by applying a safe amount of force (more than the panel would apply anyway), so it's safe to say this won't budge during the night.


The schematic is quite simple and the board has a pretty small footprint 
![image](https://github.com/user-attachments/assets/290eb6ae-2977-4938-9050-784aab798a8a)
![image](https://github.com/user-attachments/assets/71d06e24-8a3e-4633-805c-6a8808d2223a)
![image](https://github.com/user-attachments/assets/07c1bc4c-0736-4f68-be47-80da9ff294c6)




The case is made out of three parts:
  - The body + the end cap with the motor mounting point
  - The end cap with the Potentiometer mounting point and USB hole
  - The base that will mount on the telescope.
I did not print the base as part of the main body to avoid large support material usage. The body was designed to hold the PCB in place. The PCB will fit inside the channels and will go in as far as the end cap allows.
Basically, if you respect the dimensions, the PCB should go in all the way and be flush with the open end. If it's too sunken in, just add some bits of plastic to the end cap. If it's peaking out, remove some PCB.

![image](https://github.com/user-attachments/assets/b7680632-d96a-433d-b673-de995c7ce923)


I've glued the base to the body using some epoxy glue. Make sure to rough up the surfaces with a 40 grit sandpaper, so that the epoxy can adhere better. I've also added two pieces of tape to better hold the base to the body. 
Remember: all the force that will be exerted to the device will be conentrated here. If this joint is weak, it will break and fall off the telescope (not fun). 

You might also add some screws between the base and the body, but make sure that the screws don't touch anthing inside (basically why I've avoided using screws initially).

The last thing is to make way for the 5V to 160V converter that comes with the panel. EL panels run on high frequency high voltage AC (160V at around 1kHz) created by a choppper circuit inside the converter. 
This is the reason why we need to use a buck converter and cannot use PWM to drive and dim the panel. The converter needs DC to correctly function, so we need to vary the voltage that we send to its' power supply instead of the duty cycle. 

The converter will be mounted on the back wall, inside the body, using either epoxy or double sided tape (i recommend double sided foam tape, so that it can be removed easily if ever needed). 

![image](https://github.com/user-attachments/assets/368728ca-31bf-482c-a3e7-16403fa747f3)

TO get the power outside, drill a small hole on the back panel, close to the fixed endcap (the one with the motor), cut the connector, then feed the wire through that hole. I suggest skipping the connector between the panel and the converter and just soldering the wires together.
Since it runs on AC, polarity doesn't matter. Fix the wire with a ziptie around the motor arm.



The PCB can be printed at home using the laser printer + clothes iron + etching solution, or you can order it online. Whatever works best for you. 
You can load the code to the Arduino before or after mounting it. 
Make sure to wire the motor wires correctly so that it spins in the right direction. If you've wired it wrong, you can just reverse the 5 and 6 pins for OPEN and CLOSE in the arduino code: 

![image](https://github.com/user-attachments/assets/3a62564d-5ec1-468b-8ab2-19a06a03e4de)



The ASCOM setup window has three properties:
  - COM port. Select the right port for this device. 
  - Closed Position - Where should the panel stop when closing
  - Open Position - Where should the panel stop when opening
The open and close values are between 0 and 1023, however, I would suggest only using between 0 and 950 due to inacuracies in the potentiometers.

![image](https://github.com/user-attachments/assets/c4e7d72b-7d30-4cd0-ace7-a0611c7df51f)


To find out the right values, you can use the Serial Monitor inside the Arduino IDE.
Send >OPEN# to open the panel and >CLOSE# to close it. Send >HALT# to stop the motion where you want it to stop, then send >GETPOSITION# to get the current position. Use that position in either of the two properties to set the panel's limits. 

![image](https://github.com/user-attachments/assets/11eaeb8e-90e3-411f-a937-706cd0912002)

This is how the screen will look in NINA (I am giving this an an example, since I am only using NINA)

Toggle will turn the panel on or off. If there has been no value set to the intensity before, the panel will light up at max brightness (255).
Open and CLose buttons do what they say: they open and close the cover. 
Since it is ASCOM connected, the panel can be included in the sequencer to automate flat taking.

Also, I've added a watchdog to check for panel stalling. If the panel is set to open or close and the position does not change by at least 10 in about 2 seconds, the power to the motor will be cut off and "Error" state will be sent to driver.
This is in place to avoid damage to the device.




You can find the PCB copper file, EasyEDA project and the Fusion 360 project inside the "Construction" folder





