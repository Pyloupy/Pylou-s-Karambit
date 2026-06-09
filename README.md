# Pylou-s-Karambit
![Hardware](https://img.shields.io/badge/Hardware-Seeed_XIAO_nRF52840-green)

![Firmware](https://img.shields.io/badge/Firmware-C%2B%2B%20%2F%20Arduino-blue)

![Machine Learning](https://img.shields.io/badge/AI-Edge_Impulse-orange)

![CAD](https://img.shields.io/badge/CAD-Blender%20%2F%20SolidWorks-red)

Hey there ! This is the GitHub Repo where I'll show my creation based of the Reaver Karambit from Valorant (cf photo down below) that I wanted to bring to life!

<img src=./Pictures/ReaverKarambit.jpg width=300>

I tried to make it as realistic as possible with LEDs A smoke generator and even movement and voice recognition!  You should see everything here that will make you able to recreate my work, GL :)
# Why ?
Well, you should first know that my work is completly useless and is purely aesthetic. It'll be able to cut butter at his peak or maybe set my house in fire if I did my job wrong ;-;

I wanted to make it as a fan of the game Valorant. A lot of recreation has been made because it is a very popular weapon skin in the game, but none tried to push the reality to the maximum, thats why I'm here !

At first, I created a prototype with not a lot of brain work behind it and it didnt work at all (as you can see in the photo) and I didn't even know why, so I gave up.

<img src=./Pictures/Prototype.jpg width=500>

My friends made a little fun of me and that's why when I hear of the Fallout project I decided to take my revenge and make a clean work that will work! That's enough story telling, let's get in the details !
# Functionalities
Here's an exhaustive list of the functionnalities I implemented into my Karambit that maybe I'll extend later when completing the build!
- A rechargeable Smoke generator based on a E-cig
- RGB Show! With the help of an SK6812MINI and an acrylic tube to make special effects!
- Movement detection : A list of pre-trained movement to make different effects base on which movement you execute! 
- Edge AI Speech recogntion : A list of pre-trained word-orders to activate/desactivate certain functionalities using a custom-trained TinyML model via the PDM microphone
- Magnetic sensor : to detect if it is on his stand (more and that down below)
- Ultra-Low Power Management : Hardware-level Deep Sleep (SYSTEM OFF) implementation with QSPI flash memory shutdown and Wake-on-Motion via I2C intterupts, to save battery life.
- Hardware Fail-Safe: Native Watchdog Timer (WDT) implementation directly addressing the ARM Cortex-M4 to prevent software failure / thermal runaway

I really wanted to make this as complete as possible while keeping it the size of a real karambit that can fit in you hands, it was quite a challenge.
# Zine page
But an image is worth thousands of words! I made the CAD of my model in blender because the karambit is a really complex shape and I'm a lot more comfortable with blender even thought it's not really made for this job and I had to deal with some issues due to blender. 

But I made a little render with an exploded view effect and I think it turned out well:

<img src=./Pictures/ZinePage.png wdith=900>

The design is around the exploded view where you can see every components linked to the explanation of his role. I think it's a cool way to show off everything you have to fit in this tiny space to make it work!
For the text name, I tried to make an effect with the smoke comming out of the resistor, although I think the idea is great, I'm not completly happy with the effect it gives. But overall I'm happy with my zine page!  
If you want, In the CAD folder you can find the "render.blend" file where I worked on the exploded view if you want to play with it :)

<img src=./Pictures/Render.png width=400>

# CAD
### the Karambit
Talking about CAD, as I said I did almost everything in Blender to work around the complex shape that the karambit is:

<img src=./Pictures/CADBlender.png width=600>

But I did use SOLIDWORKS when I had to model i couldn't find on the internet or to make a special adapter and then imported it in Blender.   
I think it is really more easy to work with it for placing components the way you want, and since I had so little space to work with it was perfect for me.  
I had to think about a way to:
- insert the pcb inside the handle
- fit the resistor inside the handle close the blade side and a good way to fix it
- make each part (2 handle part, 2 blade side) fit together with screw for a strong grip since you have to spin the karambit
- make a way to conduct light to the blade with no LEDs inside of it because the pcb couldn't go into the blade
- place usb-c port, reed sensor and cork hole for the tank in a way that is comfortable and make sense with the way it is held in the stand
Everything have his place and it's really not made randomely, I, in fact, made the pcb in the same time as the CAD files to rearange everything as I wanted to for optimal placement
### Stand
In the first place I wanted only to build the Karambit, but when I worked on the firmware, I realised that the board I selected didn't have a way to wake up itself throught the IMU.  
That's why I wanted a backup option for the wakeup system and I thought about a magnetic option with a stand. The idea is that when the karambit detects a magnet placed in the stand, It goes to sleep to save the battery, and when take it off it wakes up with special effects!

I wanted a dark and stylized way to show off the reaver karambit. With a bit of research I found this picture reference of the reaver woods from the game wayfinder that i didnt't even know existed:

<image src=./Pictures/ReaverWoods.png width=400>
  
I thought it completly matched the theme and made a model based on this reference image. It holds the knife with different branches and goes around like a protection.

<image src=./Pictures/Stand.png width=600>
  
I made it really big and I will have to print it in 5 different parts and glue it together, I really hope it will be strong enough to hold everything, but it should do the job.
### Adapters 

I also made some adapter in SOLIDWORKS to:
- Fix the resistor inside the handle
- Fix the pvc tube to the resistor
- Convert the airflow from the fan to the inside holes of the resistor

Thats is pretty much all for the CAD design

# PCB
Now for the PCB part! I did everything in Kicad which is a free software you can find [here](https://www.kicad.org/download/)
### Schematic
Overall, the schematic and electrical design is not really complicated as it consist only of the xiao nrf52840, pins for the battery, pins for the resistor, pins for the fan, 2 IRLZ44N mosfets and a couples of resistors and diode for protection.  
Here's the schematic:

<image src=./Pictures/Schematics.png width=800>
  
As you can see the mosfets control both the fan and resistor as the current could kill the microcontroller if connected directly to it.  
There are 10k pull-down resistors linked to the mosfets to prevent any unwanted turn on when the board is in deep sleep. The schottky 1N5819 diode is connected to the fan to prevent any back current that could fry the xiao board.

That is pretty much it, as I said nothing really complicated for the schematic, the hard part was to fit everything -->
### PCB design
Since the xiao boards include the charging system, IMU and microphone on itself, it really made the job easier.  
But the pcb itself is a bit small and in a strange shape since it had to fit in the curved form karambit.  
I think I couldn't really more include more components with the way I shaped the pcb, with the large trace I had to place for the current to flow easily into the heat resistor.
Here's how it looks:

<img src=./Pictures/PCB.png width=800>

I also added holes to scew the pcb to the handle of the karambit, and I used the mosfets holes to fix the battery beneath the pcb.  
The SK6812MINI LEDs is place as far away as I could to place it near the blade of the karambit, that is why the pcb has this strange L shape.  
You could see that there is two times pins for the battery. The first one on the left side is to connect it to the BT2.0 connector that my battery use for a proper connection. The second one inside the xiao nrf52 footprint is placed this way because the pins for the battery module inside the xiao board is beneath it, so it will be easier for the solder job to place it this way instead of directly connect it to the BT2.0 connector.  

I have to mention I choosed to use the DIP version of the board and not the SMD one because I'm still a beginner with soldering electronics and it is way more easier to do it this way. But a way of impromevement could be to use the SMD version to not have two little wires hanging between the pcb and the microcontroller.

I think there is some details I could improve on the pcb but I didn't want to make it to complex for a V1 of this project as I really want to make it work like a charm !
# Firmware
## Installation
### Arduino
You will have to get the [Arduino IDE](https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE) to flash the xiao board. Then you will have to install the Seeed Xiao board inside the Arduino IDE (tutorial [here](https://wiki.seeedstudio.com/XIAO_BLE/#getting-started)) and make sure you select the mbed boards and not the classic one!  
After that you will have to install a couples of arduino libraries that I worked with, I made a folder called "Librairies" that you can copy-paste into your local Arduino folder in what I believe is ```/username/Documents/Arduino/librairies``` or simply get your own with the library manager inside arduino. There is a txt file with links to all of them.

Then you'll just have to open the ReaverFirmware.ino file select the xiao nrf52840 board (mbed version) and flash the firmware, this can take a while with the speech recognition compilation.
### Custom Speech Recognition
I already trained a model with my voice, but I'm not sure it'll respond to yours. That's why I made a tool to make your own dataset and train your own speech recognition model! First you'll have to flash the ```AudioRecordPC.ino```file to the xiao board.   
Then there is a python script to record directly from the xiao nrf52 microphone! You will have to install [python](https://www.python.org/downloads/) and the keyboard librarie first with this simple command: ```py -m pip install keyboard``` .  
Then modify in the script the name of your folder you want to get the dataset in, and the name of the first order you are recording. Record you first word press "r" and repeat. The more you do the better, I'll say minimum of 100 of them. Then press "q" to quit and save every file. 

Now you just have to follow this [tutorial](https://wiki.seeedstudio.com/XIAO-BLE-PDM-EI/) with you own dataset. Here's the parameters that I personnaly used and that is working great for me: <image src=>
Don't forget to change in the Firmware code the import of the library to your own !  
⚠️ I recommend you record you files after build (see below) to have more realistic conditions
### Movement Recognition
Haven't done that yet since i'm waiting to build it to record the different movement I want the microcontroller to recognize
## How does it work ?
For this part, I really wanted to do an advanced job and use all the potential that the xiao nrf52840 as to offer.  
I choosed to use Arduino code for this project as it is efficient and well more documented than the other programming language options.  

There is a couple of files that is in the firmware folder, and I tried to comment everything on the way they work if you want to take a closer look!
Most of these folders are the firmware divided in multiples part to test every functionality of the firmware separetely:
- Testing the Deep Sleep mode and the different way to wake it up (IMU or reed sensor)
- Testing the Voice recognition model and the different way to use it (Continuous or no)
- Testing the watchdog (natively implemented inside the nrf52 chip from Nordic Semiconductor, this was a bit complicated to make it work) to prevent any errors crashing with the voice AI model
- Battery reading test
- IMU testing
- Fan/Led/mosfets testing
These scripts could help you when building the Karambit to check every components and maybe find what isn't working (if it's not working) especially with voice and movement recognition and maybe calibrate the battery reading, Deep Sleep and watchdog should work perfectly.

Have fun creating new effects/movement/orders to you karambit to make it personal!
#BOM
Now here's the BOM or aka how to get your components.  
There is link for everything mainly using Aliexpress. I personaly took the resistor with a French reseller so maybe you'll have to look somewhere else, (just search the model name).  
For paint, I took mine in a local store, be free to pick any colour you want to make your favorite variant of the reaver karambit!  
I ordered my pcb on Jlcpcb, it is great and cheap. For filament I used what I already had, but take whatever you want, it's best if it is strong and heat resistant.  
Careful for prices they may vary over time, Aliexpress is very volatile with there prices

Here's the BOM (you can also find it in CSV and xlsx format in the files):
| Item | Specs | Quantity | Buying Price(€) | Unit Price| Total Price (€) | Link | Tips |
| ---- | ----- | -------- | --------------- | --------- | --------------- | ---- | ---- |
| 18x14mm fan | 0.2 cfm, blow the smoke as it is in a closed case | 1 | 4.29 | 4.29 | 4.29 | [Link](https://fr.aliexpress.com/item/1005010476941631.html) | |
| 1N5819 Diode | Reverse polarity protection for the fan | 1 | 1.33 | 0.0248 | 0.0248 | [Link](https://fr.aliexpress.com/item/1005001552094086.html) | Others diodes will work, check the specs and measurement |     
| 100x2mm acrylic tube | Conduct the light into the blade | 1 | 1.8 | 0.171 | 0.171 | [Link](https://fr.aliexpress.com/item/1005005536829247.html) |  |
| M2xL2xOD3.2 insert | to fix the blade the the handle | 5 | 1.9 | 0.019 | 0.095 | [Link](https://fr.aliexpress.com/item/1005006472702418.html) | It'll be cheaper to just buy a kit of different type of inserts |
| M2xL4xOD3,2 insert | to fix the 2 parts of the knife together | 6 | 2.25 | 0.0225 | 0.135 | [Link](https://fr.aliexpress.com/item/1005006472702418.html) |  |
| M3xL5xOD4,2 insert | to fix the pcb via the transistors holes | 3 | 1.82 | 0.0364 | 0.1092 | [Link](https://fr.aliexpress.com/item/1005006472702418.html) |  |
| Battery BETAFPV LAVA II 1S 480mah | LiHv with 95C rating to ensure enough current | 1 | 15.29 | 3.54 | 3.54 | [Link](https://fr.aliexpress.com/item/1005010775720329.html) | Other batteries could work just be sure the C rating is high enough (hard to check on most Aliexpress battery where only fpv drones C rating are precised) |
| Reed switch NC  | Low voltage GPS-23 (act as a sensor to detect if it's in the stand) | 1 | 5.79 | 5.79 | 5.79 | [Link](https://fr.aliexpress.com/item/1005006926125113.html) |  |
| Seeed Studio Xiao nRF52840 Sense | 64 Mhz processor, IMU and microphone integrated | 1 | 18.19 | 18.19 | 18.19 | [Link](https://fr.aliexpress.com/item/1005006988944671.html) |  |
| BT 2.0 male cable | to cleanly connect the battery to the pcb | 1 | 3.59 | 1.945 | 1.945 | [Link](https://fr.aliexpress.com/item/1005009623841974.html) |  |
| SK6812MINI LED | 35 35 ARGB Led  | 1 | 10.89 | 0.1089 | 0.1089 | [Link](https://fr.aliexpress.com/item/1005004931260882.html) | You should modify the pcb and take SK6812MINI-E (the E is important) It's a lot more easier to solder, I already have those so i will use them (in pain) |
| GS Air Eleaf Clearomizer | 2.5 ml tank / 1,5 ohm coil (included) | 1 | 7.5 | 7.5 | 7.5 | [Link](https://www.oliquide.com/Clearomiseurs-Eleaf-Eleaf-GS-Air.) |  |
| Transistor IRLZ44N | 47A max, Vgs=2V, to control both fan and heat coil | 2 | 2.06 | 0.206 | 0.412 | [Link](https://fr.aliexpress.com/item/1005009876208141.html) | Should be overkill, I took them coz I already had them |
| 6mm diameter PVC Tube | 6mm exterior diameter 4mm interior diameter, to conduct the smoke to the blade | 1 | 2.09 | 2.09 | 2.09 | [Link](https://fr.aliexpress.com/item/1005006173546510.html) |  |
| M2 Screws | Screws different lengths kit | 1 | 5.09 | 5.09 | 5.09 | [Link](https://fr.aliexpress.com/item/1005007278965396.html) | Might have to cut them, make sure to put a bolt before to not break the thread |
| Magnet | 8mm diameter | 1 | 3.59 | 0.07 | 0.07 | [Link](https://fr.aliexpress.com/item/1005011969218794.html) |  |
| Anti-slip pads | for the stand | 7 | 1.8 | 0.018 | 0.12 | [Link](https://fr.aliexpress.com/item/1005008980666002.html) |  |
| 200/1K Ω resistors | for transistor signal and pull-down | 2 each |  |  |  | Take any kit of resistors on Aliexpress it'll be useful in the future |  |
| Hyper CF PLA Filament | For printing the handles, blades, stand and adaptaters | 1 | 34.99 | 34.99 | 34.99 | [Link](https://store.creality.com/fr/products/hyper-pla-cf-carbone-fibre-1-75mm-filament-dimpression-3d-1kg) | It's just what I will be using coz i have it in stock, but anything can do the job ABS will be better for heat coil |
| Fake Leather | Auto-adhesive layer | 1 | 4.99 | 4.99 | 4.99 | [Link](https://fr.aliexpress.com/item/1005008381916334.html) |  |
| Mettalic Paint | Shiny paint weeee | 1 | 5 | 5 | 5 | Bought from local store |  |
| Red Metallic Paint | Shiny paint weeee | 1 | 5 | 5 | 5 | Bought from local store |  |
| PCB | 1.6mm two layers | 1 | 3 | 0.6 | 0.6 | Got it from jlcpcb but use whats cheaper in your region (jlcpcb is a great choice tho) |  |
| Shipping Price |  | ~10-15€ |  |  |  |  |  |
| Total Price |  | 109.55€ | 128.88$ |  |  |  |  |
| Total Buying Price (Because you'll buying multiples at once) |  | 150.45€ | 177$  |  |  |  |  |


# Build time !
It's now time to make the Karambit! If you're confused any time, be free to check the blender file to understand where everything is placed! Everything is named and (more or less) organized, I hope you'll understand everything :)

### **Step 1:** Printing

First print everything from the STLs folder with your filament, it should be around 800g gram worth of filament, (I hope for you that everything fits in your printer, for reference mine is an ender 3 V3 KE).  

⚠️ Be careful when printing the stand, you will have to stop the print at a certain height to insert the magnet inside the branches and then resume print.
Then glue everything together for the stand, sand it and paint it if you think that is necessary. I recommend you to do it at least for the Karambit parts to make it cleaner!
Don't forget to place the anti-slip pads below the stand for a stable structure!
Place the fake leather into both handles, mines auto-adhesive if you take the one linked in the BOM

### **Step 2** soldering:

Beforehand be careful with the reed sensor, you will have to cut a platic part from it : 

<img src=./Pictures/reed.jpg width=200>

without that it won't fit.  
Solder the xiao nrf52840 (dont forget connection for the battery), 200/1k ohms resistor, both mosfets, battery cable, fan, Schottky diode, SK6812MINI Led and Reed switch (let enough cable to play with) into the PCB and test everything with the different scripts in the Firmware folder.  

With a soldering Iron place the differents inserts in there places.  

Drill a 6mm hole into the Resistor's tank (check alignement with the hole made in the blade) and glue the tube to the tank with a waterproof glue like epoxy (will add a picture on that when i'll do it)
Solder + and - 18awg or less cable to the resistor as it shows on this image(then solder it to the PCB with enough length to play with):  (will add a picture on that when i'll do it)

### Step 3: assembling:
Fix the battery below the PCB with the adpater and screws/bolt.  
Fix the PCB into the handle with the scews.  
Fix the Resistor with the adapter printed and screws. 

Take the joint from the part you removed earlier from the clearomizer and place it on the adapter, then put it with the 6mm tube into the resistor. 

<img src=./Pictures/removejoint.jpg width=600>

Then block the left side of the base to force the airflow to go throught the coil : 

<img src=./Pictures/Airflow.png width=500>

Fix the first blade to the handle.

Cut and insert the acrylic tube into the handle and place it above the LED, if it doesn't transmit light good enough, try sanding the sides of the acrylic tube.  
Fix the reed switch to the handle.

Finally fix the other blade to the other handle and fix both handles together, upload the firmware when you are sure everything works and you're good to go!

# Special Thanks !

Thanks to my boy Carabouille for participating with me and qualify before me (bravo!)

Thanks to the Hack Club community for this incredible opurtinty that represent fallout!

Thanks to the UwU GanG to exist and be the best RL team imaginable :).

Thanks to Seeed Studio for the xiao boards (cyu in Shenzhen, plzzzzzz make more exemples code on your board ty)

Thanks to Emmanuel Macron for giving me so much money in scolarship, the goat
<br/>
<br/>
<br/>

See you in Shenzhen ;).


<br/>
<br/>
<br/>



\- Pylou







