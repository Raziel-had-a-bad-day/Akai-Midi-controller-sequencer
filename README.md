Akai APC key 25 midi controller based sequencer project with STM32F411+flash ,can be used with a similarly mapped controller though , uses USB midi to talk to pc then midi-ox links it to the midi controller (including lighting the buttons) also midi out on serial   . Has LCD for useful info like tempo ,  but not super essential.
 



row 1 and 2 =input notes 
row 3= setting pitch for 8 Presets per sound on first page of drums , sends NRPN 
row 3 + select= program_change for drums(0-7) or select alt_pots pitch bank (0-7) for keys 12-16
row 4= select a bar to play 
row 5 = select sound, 0-11 drums 12-16 keys

Pot 1 + Shift =	program change only for non drum channels
Pot 2 = play notes only on every X bar 
Pot 3= set pitch for drums during pause , select preset first on row 3 
Pot 4 = 	set musical scale type for notes,
pot5=		LFO rate  sends NRPN only for filter, also modifies velocity for keys for now 
pot6=		 LFO level
pot 7+shift+device = 	sets mid channel for current sound 
pot8=		velocity/accent ,  now default and red is velocity:96

pause =  shift + clip_stop : clear program changes on drums , also enter pitch info for bars on selected sound 
select=	enables program change 1-8 on drums , shows selected pitch bank for keys
arm_rec = 	records pitch change selections on drums
stop_all_clips = pause and jumps to start

Hold Shift to enter notes with velocity=96 .
Down arrow = copy current part in bar
Left arrow= 	select another bar  than paste

send = write to flash , 
send + shift = reload 
solo = solo uses top 4 pots to select between 4 sounds each muting the rest
mute = select any part to mute, turns red 
up arrow = select sounds 8-15, 
record = allows overwriting program change for now

keyboard =  also play notes on selected sound ,useful to set pitch on drums 
Stop all clips = stops and jumps to start 
clip_stop = enables all_pots to set pitch for 8 sequentual notes  on keys
device =   shows current midi channel on top rows ,blinky yellow  
pan =  ///pot 8 to set load/save position ,disabled/ 
 
 
