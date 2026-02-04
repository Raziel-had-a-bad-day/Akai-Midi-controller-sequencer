Akai APC key 25 midi controller based sequencer project with STM32F411+flash ,can be used with a similarly mapped controller though , uses USB midi to talk to pc then midi-ox links it to the midi controller (including lighting the buttons) also midi out on serial   . Has LCD for useful info like tempo ,  but not super essential.
 

use left and right arrows to step through zoom levels : notes , bars , bars*8,bars*64 
 
row 5 = select sound, 0-7 drums 8-15 keys


pot 7+shift+device = 	sets midi channel for current sound 
pot8=		velocity/accent ,  now default and red is velocity:96
pot8 +shift+device=tempo
pause =  shift + clip_stop : clear program changes on drums , 
select=	enables program change 1-8 on drums , shows selected pitch bank for keys

stop_all_clips = pause and jumps to start

send = write to flash , 
send + shift = reload 
solo = solo uses top 4 pots to select between 4 sounds each muting the rest
mute = select any part to mute, turns red 
up arrow = select sounds 8-15, 


keyboard =  also play notes on selected sound 
Stop all clips = stops and jumps to start 

device =   shows current midi channel on top rows ,blinky yellow  

down arrow = calls up fx menu , pots set cc send on 1-8 fx channels  , buttons set on/off per bar 1,8,64 ,0-3 level on , 4-7 level off, only single channel for now

Pot 3 =sets pitch change advance rate on keys 

Third row is accent on/off for avery 2 notes for 8 bars , can be recorded with shift+pot8 as well.
Forth row is bar position of 8 bars.

////pitch entry mode ///
clip stop on enables pitch screen
bars 1 and 2 shows pitch in current octave  0-11 
bar 4 shows pitch sequencer current step , adjust playback rate using pot 3 
in pause enter pitch for steps using and these bars, can use keyboard also
in pause + rec arm = step record mode using keyboard 

 
