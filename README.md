Akai APC key 25 midi controller based sequencer project with STM32F411+flash ,can be used with a similarly mapped controller though , uses USB midi to talk to pc then midi-ox links it to the midi controller (including lighting the buttons) also midi out on serial   . Has LCD for useful info like tempo ,  but not super essential.
 
lcd(16x2. I2c) shows cc options and operating modes 
use left and right arrows to step through zoom levels : notes , bars , bars*8,bars*64 


rec_arm on default screen , records midi notes played on keyboard , live or in pause , only way to enter notes now , 8 notes buffer 
 shift+rec_arm  = clears notes
playing will loop for a short time , can be saved to the currently selected recording slot (shift held) 
 
 
shift+pause = loops current bar , can be changed with bar selector (fourth row) 
row 5 = select sound, 1-3 keys rest drums 

pot 4 = bar scrub
pot 7+pause+device = 	sets midi channel for current sound 
pot8=		velocity/accent ,  now default and red is velocity:96
pot8 +shift+device=tempo

stop_all_clips = jumps to start

send = write to flash , 
send + shift = reload 
solo = solo single channel
mute = select any part to mute, turns red 
up arrow = select sounds 8-15, 
pause= loops current pattern
keyboard =  also play notes on selected sound 

clip stop= sends cc pot 1-8 cc 90-97, not saved

device =   shows current midi channel on top rows ,blinky yellow  

down arrow = calls up fx menu , pots set cc send on keys  , 3 channels * 4 settings , press shift and adjust pots to assign cc controller to particular pot ,saved 

Pot 3 =sets transpose pitch change advance rate on keys on main screen 

middle row = 8 patterns per midi channel/voice 
Forth row is bar position of 8 bars. Holding down one and pressing another will loop those bars.Not doing anything atm 



 
