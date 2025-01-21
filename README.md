![image](https://github.com/user-attachments/assets/537c78b6-64ce-4eb9-b8a7-01ba5e4c9b47)


ASCOM connected DIY Automatic Flat Panel with Arduino and Electroluminescent panel 

This flat panel project was built following existing designs. Its' purpose is not to reinvent the device, but to bring a DIY solution for a flat panel.
I am aware, as you probably are, that there are multiple other DIY panels online, but I wanted to build a variant that does not use a servo motor. 

This functions (and even looks) like any other flat panels available (sans the dew heater available on some) and can be used in any imaging software that has ASCOM capabilities.
The BOM is very short: 
  > An Arduino Nano
  > MX1508 chip
  > GA12 N20 15rpm geared micromotor
  > a PCB mounted potentiometer anywhere between 1k and 20k (I've used a 20k one, but anything in this range is a drop-in replacement)
  > LR2905 N-MOSFET (SMD mounted)
  > two 100uF (min 10V) SMD mounted caps (6.3mm)
  > two 220uF (min 10V) TH mounted caps (6.3mm)
  > 10k , 100 and 220 ohms SMD 0805 resistors
  > Two SMD 0805 100nF caps
  > 1 SS545L SMD diode  
  > 1 BZV55 SMD diode
  > 1 470uH inductor.

Most of these components can be sourced from the modules here https://shorturl.at/lNhVN (inductor and SS45L diode) and here: https://shorturl.at/gths6 (MX1508 chip, 2 100uF SMD caps, BZV55 diode and 220 ohm resistor).


The EL panel can be bought from AliExpress here: https://shorturl.at/BpqwX. Make sure to select the 5V variant, as it will draw power directly from the Arduino +5V pin (motor + EL panel power draw is low enough not to cause harm to the Arduino)
To avoid, or, at least minimize the flickering, we will boost the PWM frequency of the PIN 3 on the Arduino to 31kHz. Since we are using pin number 3 to drive our buck convertor, it won't affect any delay times (we are changing the TCCR2B register. TCCR0B affects delay)
I've tested the panel and the Serial functionality is not affected by this frequency increase and, also, the flickering, or "waving of the panel is greatly reduced, compared to the standard frequency. 
I've tested this with my phone camera and I can go down to 1/60 exposure time without noticing any flickering. This is with the camera on my S20 FE, so f/1.8. I suspect that the exposure times on a dedicated camera will need to be greater that 1/60 anyways.
The light is quite blue, despite their claims that it is white, so I've used a piece transparent red binder cover to bring the color balance closer to white. It might not matter for mono, but I think it does make a difference for color cameras.
On top of the red plastic, I've also added a translucent white binder cover, a piece of 2mm plexiglass for rigiditi and another matte translucid binder cover. I might remove the white piece, since it dims the light too much and does not help with the colour balance.

The motor I've used is a GA12 N20 15rpm geared micromotor (https://shorturl.at/ir1aB) 
Initially, I wanted to use a 28BYJ-48, modified to bipolar mode, to run at 12V, but I found that the gearing was too weak to hold the panel position at "half open" (the panel would easily backdrive the motor). 
This micromotor, as small as it is, has a 1000:1 gearing ratio (compared to 1:64 on the stepper motor). I did not manage to backdrive it by applying a safe amount of force (more than the panel would apply anyway), so it's safe to say this won't budge during the night.


