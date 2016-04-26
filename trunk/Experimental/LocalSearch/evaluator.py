# NOTE I modified this FOR THE LOLZ, it's just a test to replace every minion with Leeroy Jenkins. DON'T USE THIS.

# Simple Python script to check that a deck is correct, and then launch the evaluation
# by Alberto Tonda, 2016 <alberto.tonda@gmail.com>

import os
import sys
import numpy as np
import signal
import errno
import subprocess, threading
import time
import shlex
from threading import Timer


# horrible hard-coded variable for debugging purposes
DEBUG = False

# other hard-coded values: for example, the list of legendary minions
outputDeckFileName = "evolvedDeck.json"
fitnessFileName = "fitness.out"
legendaryMinions = [
	"minion_alexstrasza",
	"minion_baron_geddon",
	"minion_baron_rivendare",
	"minion_blingtron_3000",
	"minion_bloodmage_thalnos",
	"minion_bolf_ramshield",
	"minion_cairne_bloodhoof",
	"minion_captain_greenskin",
	"minion_chillmaw",
	"minion_chromaggus",
	"minion_deathwing",
	"minion_dr_boom",
	"minion_elite_tauren_chieftain",
	"minion_emperor_thaurissan",
	"minion_eydis_darkbane",
	"minion_feugen",
	"minion_fjola_lightbane",
	"minion_foe_reaper_4000",
	"minion_gazlowe",
	"minion_gelbin_mekkatorque",
	"minion_gormok_the_impaler",
	"minion_gruul",
	"minion_harrison_jones",
	"minion_hemet_nesingwary",
	"minion_hogger",
	"minion_icehowl",
	"minion_illidan_stormrage",
	"minion_justicar_trueheart",
	"minion_kelthuzad",
	"minion_king_mukla",
#	"minion_leeroy_jenkins",
	"minion_loatheb",
	"minion_lorewalker_cho",
	"minion_maexxna",
	"minion_majordomo_executus",
	"minion_malygos",
	"minion_mekgineer_thermaplugg",
	"minion_millhouse_manastorm",
	"minion_mimirons_head",
	"minion_mogor_the_ogre",
	"minion_nat_pagle",
	"minion_nefarian",
	"minion_nexus-champion_saraad",
	"minion_nozdormu",
	"minion_old_murk-eye",
	"minion_onyxia",
	"minion_ragnaros_the_firelord",
	"minion_rend_blackhand",
	"minion_skycapn_kragg",
	"minion_sneeds_old_shredder",
	"minion_stalagg",
	"minion_sylvanas_windrunner",
	"minion_the_beast",
	"minion_the_black_knight",
	"minion_the_skeleton_knight",
	"minion_tinkmaster_overspark",
	"minion_toshley",
	"minion_troggzor_the_earthinator",
	"minion_ysera",
# class-specific legendary minions (for Hunter)
	"minion_",
# class-specific legendary minions (for Mage)
	"minion_archmage_antonidas",
	"minion_flame_leviathan",
	"minion_rhonin"
# class-specific legendary minions (for Hunter)
	"minion_acidmaw",
	"minion_dreadscale",
	"minion_gahzrilla",
	"minion_king_krush"
]

######TIMEOUT 



class Command(object):
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None

    def run(self, timeout):
        def target():
            print 'Thread started'
            self.process = subprocess.Popen(self.cmd, shell=True,  preexec_fn=os.setsid)
            self.process.communicate()
            print 'Thread finished'

        thread = threading.Thread(target=target)
        thread.start()

        thread.join(timeout)
        print "EXIT WITH CODE "+str(self.process.returncode)
        if thread.is_alive():
            print 'Terminating process'

            #self.process.terminate()
            os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
            thread.join()
            print "Game lasted a lot :/"
            return False
        return True


######METASTONE STUFF
java_bin = "/home/pgarcia/jdk1.8.0_45/bin/java -jar metastone.jar"
#java_bin = "java -jar metastone.jar"
aiDecks = ["Midrange Druid Season 18",
#"Midrange Hunter Season 18", #CHANGE THIS WHEN COMPARING WITH MAGE!!!!!!
"Mage Tempo Season 18",
"Aggro Paladin Season 18",
"Shadow Madness Priest Season 18",
"Oil Rogue Season 18",
"Mech Shaman Season 18",
"Warlock MalyLock Season 18",
"Control Warrior Season 18"]

ais = ["FlatMonteCarlo",
"GreedyOptimizeTurn",
"GreedyOptimizeMove",
"PlayRandomBehaviour"
]

numberOfGames = 16

def average(s): return sum(s) * 1.0 / len(s)

def removeComma(filename):
	f = open(filename,'r')
	filedata = f.read()
	f.close()

	newdata = filedata.replace(",\n]","\n]")

	f = open(filename,'w')
	f.write(newdata)
	f.close()

def launchAgainstAI(deckf, enemy, tmp):
	deck1 = "Evolutionary Deck"
	deck2 = enemy
	os.system("rm decks/evodeck.json")
	removeComma(deckf)
	os.system("cp "+deckf+" decks/evodeck.json")

	AI1 = "GreedyOptimizeTurn"
	AI2 = "GreedyOptimizeTurn"
	params = "\""+deck1+"\" \""+deck2+"\" "+AI1+" "+AI2
	launchline = "("+java_bin+" "+params+" "+str(numberOfGames) +")>"+tmp
	print launchline
	com = Command(launchline)
	attempts = 0
	finished = False
	while finished == False:
		print "Launching attempt "+str(attempts)
		os.system("rm "+tmp) 
		finished = com.run(1200)#1200
		attempts = attempts+1
		if attempts == 3:
			finished = True

	print "Parsing file"
	p1,p2 = parseFile(tmp)
	print "Parsed"
	print p1
	return p1


######END METASTONE STUFF
# this script has exactly one command-line argument (at the moment): the name of the deck file
deckFileName = sys.argv[1]

# read all the lines in the deck; we cannot use a JSON reader, because the files produced by
# ugp3 have an extra comma (",") on the last card, making them unreadable
lines = []
with open(deckFileName, "r") as fp : lines = fp.readlines()

# parse the cards
if DEBUG : print "Parsing cards..."

# we use some knowledge of the file structure here, so BEWARE! should you change the structure, the file might change
cards = []
for i in range(2, len(lines)-4) :
	if DEBUG : print "Now reading line:", lines[i]
	
	# super-simple split to get the card name
	tokens = lines[i].rstrip().lstrip().split("\"")
	# remove empty tokens
	tokens = [x for x in tokens if x]
	# the card is the first token
	cardName = tokens[0]
	if DEBUG : print "Found card \"" + cardName + "\"!"
	
	cards.append(cardName)
	
# now that we have all the cards, let's check the number of legendary minions
extraLegendaryMinions = 0
for minion in legendaryMinions :
	if cards.count( minion ) > 1 :
		if DEBUG : print "Found " + str(cards.count(minion)) + " copies of \"" + minion + "\"!"
		extraLegendaryMinions += cards.count( minion ) - 1
if DEBUG : print "The deck contains " + str(extraLegendaryMinions) + " legendary minions in excess of 1x"

# and the number of repeated cards
repeatedCards = 0
for card in set(cards) :
	if cards.count( card ) > 2 and card not in legendaryMinions and card != "minion_leeroy_jenkins" : # LEEEEROYYY JEENKIIINNSSS
		if DEBUG : print "Found " + str(cards.count(card)) + " copies of \"" + card + "\"!"
		repeatedCards += cards.count( card ) - 2
if DEBUG : print "The deck contains " + str(repeatedCards) + " cards in excess of 2x."

# finally, here is the fitness! (to be maximized)
fitness = []
fitness.append( 1.0 / (float(extraLegendaryMinions + repeatedCards)+1) )
# LEEROY JEEEEENNKIIIIINNSSSS
fitness.append( cards.count( "minion_leeroy_jenkins" ) )

if DEBUG : print "Final fitness:", fitness

# this is just for me, to debug stuff
if DEBUG :
	for card in sorted(cards) :
		print "- \"" + card + "\""

with open(fitnessFileName, "w") as fp :
	for f in fitness :
		fp.write(str(f) + " ")
if DEBUG : print "Done"
