![front](https://github.com/user-attachments/assets/61d67608-b8cb-4ab6-ad03-d83aa7dcdf60)



ASCOM connected DIY Automatic Flat Panel with Arduino and LED Strip.

While a number of DIY flat panel projects exist online already, I wanted to make my own.
This is a flat panel with a spin: The motor and circuit board are on the panel itself, not on the telescope mounted base.
This allows the whole thing to be more low profile than existing designs. One advantage is the fact that no fragile wires will flex during operation. While there still are some wires present (the USB and Power cables), these wires can be swapped out at any time easily and without the need to open up the device.

This functions like any other flat panel (sans the dew heater available on some) and can be used in any imaging software that has ASCOM capabilities.


The BOM is fairly short: 
  - An Arduino Nano
  - a small buck converter to get 5V from 12V https://vi.aliexpress.com/item/1005007092498838.html?spm=a2g0o.productlist.main.5.588f6bd5KwwhlR&algo_pvid=3e0ed6be-7084-4139-8f32-94e96cfbea35&algo_exp_id=3e0ed6be-7084-4139-8f32-94e96cfbea35-2&pdp_ext_f=%7B%22order%22%3A%22767%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21RON%216.09%216.09%21%21%219.15%219.15%21%40210384b917411158943222182e5c98%2112000039378058524%21sea%21RO%211948324624%21X&curPageLogUid=93Xb3F4eaNjm&utparam-url=scene%3Asearch%7Cquery_from%3A
  - TD-7120MG 270 degrees stepper motor https://vi.aliexpress.com/item/1005007475952814.html?spm=a2g0o.productlist.main.1.19c660c7mPopOI&algo_pvid=f957a072-991a-4a65-8213-ea865c47b00c&algo_exp_id=f957a072-991a-4a65-8213-ea865c47b00c-1&pdp_ext_f=%7B%22order%22%3A%221%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21RON%2154.48%2143.56%21%21%2111.23%218.98%21%402103956a17411159195922612e85f0%2112000040906975075%21sea%21RO%211948324624%21X&curPageLogUid=SEp3C8e8rwn3&utparam-url=scene%3Asearch%7Cquery_from%3A
  - LR2905 N-MOSFET (SMD mounted)
  - a through-hole ferrite bead
  - 10k and 100 ohms SMD 0805 resistors
  - 1 SS545L SMD diode 
  - 1 through hole current limiting resistor for the LED strip (I went with 330 ohms, pick your own value based on your needs.
  - a length of 8mm wide warm/natural white COB led strip (links below)
  - a 12W round LED light fixture (links below). We will use this for the internal diffuser: https://vi.aliexpress.com/item/33054747819.html?spm=a2g0o.productlist.main.25.2e8e2a0ccoLWNZ&algo_pvid=72f3e5f8-c4d3-4181-bed1-150988b04c45&algo_exp_id=72f3e5f8-c4d3-4181-bed1-150988b04c45-12&pdp_ext_f=%7B%22order%22%3A%224%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21RON%2133.76%2127.02%21%21%216.96%215.57%21%40211b813f17411153875014758e86c5%2167512424867%21sea%21RO%211948324624%21X&curPageLogUid=cfqfcEqeoubA&utparam-url=scene%3Asearch%7Cquery_from%3A

  To avoid flickering, the frequency of pin 3 has been increased to 31kHz and a ferite bead has been introduced in the circuit between the switching MOSFET and the LED strip. A flyback diode has been added to avoid any spikes created by the ferrite bead.


  The light panel is based around the 12W LED light diffuser. Once you get the light fixture, unscrew the 6 screws on the back and remove the round metal cover. Inside, you should find a round piece of white foam, a white plastic reflector, a disk of transparent acrylic and a white plastic translucent diffuser (the outside part). This is all we need from this light fixture.

  

The arms and base are printed in 3 parts: the two arms and the cross-brace that holds them in place with screws. Once you print the parts, align the two arms and the crossbrace, then drill 2mm holes in the brace through the already modeled holes in the arms. Then, insert either M3 screws and tighten down. Avoid overtightnening to not strip out the plastic. If you want, you can drill out bigger holes and use heat-set inserts. This base will be held in place on the telescope using metalic hose clamps. Make sure to stick some self-adhesive velvet on the dew shield to avoid scratching the metal. 


![image](https://github.com/user-attachments/assets/8c22c011-faad-4d7d-acfe-cd579086f3c3)



Print the four parts of the panel (BASE,RING,SPACER and SUPPORT), then assemble as follows:

- use 6 x 8mm M3 screws and nuts and connect the BASE to the SUPPORT using pre-modeled holes in the BASE. You can add some epoxy on the screw heads to stick them to the base for ease of later adjustment.
- solder two wires to the LED strip, the stick it around the inside of the BASE. You will need to overlap the last 2-3 mm of the strip after cutting it on the closest cutting point. To do that strip back the 2-3mm off the silicone cover from the LED strip and stick the end with the wires on top of that. Thread the wires thorugh the hole in the center of the BASE (it comes out through the SUPPORT into the electronics chamber)
- Put the foam piece in the center. You might want to cut a channel for the wires into the foam.
- Add the thin white reflector
- add the acrylic disk in the center
- add the 2.2mm 3d printed spacer ring
- put the translucent white diffuser on top
- add the 3d printed ring, align the screw holes with the ones in the base and screw in 6 x 4mm M3 bolts. You can use the bolts that came with the servo motor, since we will be using different bolts to secure the arm to the servo.

This concludes building the panel itself.


Next, cut, etch, drill and assemble the PCB, then put it inside the electronics case (PCB is in the Construction folder.
![Schematic](https://github.com/user-attachments/assets/b4beb28e-5662-41a8-ae67-3254c906eca2)

I suggest cutting a 40mm x 60mm board so it fits snuggly against the walls of the SUPPORT. Solder the servo motor wires and the LED strip wires to the PCB. 
Then, drill four M3 holes in the PCB and the plastic risers, then screw the PCB down, avoiding overtightnening the screws. Make sure you print the risers with 100% infil.
Secure the SERVO in the SUPPORT bracket using the plastic screws that came supplied with the motor. 
Print the COVER, then secure it using M3 bolts, two on top and two on the sides.

At this point, the flat panel can be used as is. If you intend to build and use the panel as a manual panel, instead of a motorised one, you can follow the same procedure, but skip adding the motor. Skip the next step if you want this.



Mounting everything together:

Add the two metal wheels to the servo. Secure the one on the driven axle of the servo using the included M3 bolt. Use another M3 bolt for the support axle (the black plastic one that does not rotate). 
Next, loosely mount the ARMS and CROSS-BRACE to the telescope, then slide the SERVO motor shafts between the two arms. The arms have slotted holes, so the 0 position can be adjusted (the splined shaft holes do not end up perfectly aligned at 90 degrees).
DO NOT INSERT THE SCREWS MOUNTING THE ARMS TO THE SERVO YET! We need to make sure where the servo is currently positioned (0 to 270 degrees).
Print the cable clips and glue them to the ARM you want you cables to be attached to (use superglue or epoxy). 


Programing the board and driver setup:

Open Arduino IDE on your computer and connect the Arduino. Download the INO file from the release page and open it in the IDE. Select the correct COM port and board type, then upload the code. 
Download and install the ASCOM driver from the release page. 
Once installed, open N.I.N.A (or whatever aquisition software you are using), then select PopUp AFP. Warning: due to some reasons unknown to me, the ASCOM driver installer creates two entries in the dropdown selection menu. One of them works, but one of them is not registered with ASCOM, so it won't work. Check to see whcih one works and use that one. Open the properties, select the right COM port and set the Closed and Open positions (0 to 270 degrees), then hit Ok, then Connect to the device. 

Once connected, it's time for testing. Plug in a 12V power source and toggle the light panel on and off, setting different light levels (0 - 255). 
If you've used a jumper instead of a limiting resistor, the 255 value will be extremely bright. Now it's time to check and chose the right current limiting resistor for your situation (I've used 330 ohms and it works fine).
Then, click Open and Close and watch for movement of the stepper motor (remember, we do not have any screws mounted, so only the stepper flanges will move, but not the whole panel). 

Setting the Closed and Open Positions (angles) and the physical 0 position.

Earlier, when setting up the driver in NINA, we had to select an Open and Closed position for the panel. The Closed position should always be 0, but you can alter that in the unlikely eventuality you actually need to change it. 
Now, open the panel manually and check how much can it open. Measure the angle starting from the 0 position, then set that as Open position.
When there's no obstruction to the panel, it can fully open. Set the Open position to 270. This will open the panel fully, then set it parralel to the telescope
![image](https://github.com/user-attachments/assets/182fdfe3-1eff-4d75-9b9f-a09fd87e9433)

If theres any obstruction or, for any other reason, you need to open the panel lees than the full 270 degrees (which, with the TD7120MG is never really 270, more like 250-260), set the desired angle as Open position.

![image](https://github.com/user-attachments/assets/fc809829-f3bd-4a35-a91a-78e5acb836a1)




![image](https://github.com/user-attachments/assets/11eaeb8e-90e3-411f-a937-706cd0912002)

Set the two values, then connect to the device. Once connected, click the Close button.

Point your telescope at zenith, then close the panel manually. You might also need to adjust the position of the telescope-side mounting so that the panel can be flush with the dew shield's opening. 
At this point, you can insert the 8  M3 x 10mm bolts throught he slots in the arms and tighten them down. You can use some lock washers between the screw head and the arms, so it will bite into the plastic and not move. 

Once you have tightened the screws, click the Open button. The panel should now move to its' set max position. 

And..... that's it! You're ready to use the flat panel. 

Note on LED strips: 
I have used a warm white LED strip. While it looks very yellow-ish, the emissions in Ha and S2 are more intense than with cool/natural white. You can test and go with whichever colour temperature you might find fitting for your situation.

You can find the PCB copper file, EasyEDA project, Fusion 360 project and STL files inside the "Construction" folder





