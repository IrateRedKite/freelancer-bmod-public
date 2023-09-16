# -*- coding: utf-8 -*-
"""
Created on Sat Sep 16 15:15:14 2023

@author: Katzenkebap
"""

import json
import os
from sys import exit

### usage
"""
This should grab the xml by itself and let you work through it.
Any checkpoints or the revised xml structure (in the form of a
json file containing old and new ids, among other things) should
pop out into the enclosing folder.

If the block offset (configurable below) is too small, new_id
may overflow into the next block and overwrite things or some
other unwanted behavior. Just use large blocks, m'kay.
"""

### exitcodes
"""
0: Script ran successfully.
1: Script terminated normally, without saving.
2: Script terminated normally, with save.
3: Wanker.
If you get any OSErrors, you're probably executing from the wrong place.
"""

### config
blocks = {
    "0": {"name": "Equipment", 
          "offset": 0
          },
    "1": {"name": "Commodities", 
          "offset": 32768
          },
    "2": {"name": "NPCs", 
          "offset": 32768*2
          },
    "3": {"name": "UI", 
          "offset": 32768*3
          },
    "4": {"name": "Solars/Map", 
          "offset": 32768*4
          },
    "5": {"name": "News", 
          "offset": 32768*5
          },
    "6": {"name": "Rumors", 
          "offset": 32768*6
          },
    "7": {"name": "Missions", 
          "offset": 32768*7
          },
    }

min_id = 524288 # 2**19

q = "Which category does the following object belong to?"

with open("../../infocard-import.xml", "r") as reader:
    
    lines = reader.readlines()


# Check saved state availability
if not os.path.exists("./revised_xml_saved_state.json"):
    user_input = "f"
else: 
    user_input = input(f"A saved state has been found, would you like to use this saved state? (c: continue with saved state, f: fresh start, fd: fresh start and delete old checkpoint)\n")
if user_input == "fd":
    # Fresh start, delete checkpoint
    print("Removing old checkpoint ...")
    os.remove("./revised_xml_saved_state.json")
    user_input = "f"
if user_input == "f":
    # Fresh start
    revised_xml = {key: {} for key in blocks}
    current_line = 0
    revised_xml["current_line"] = current_line
elif user_input == "c":
    # Load saved state
    with open("./revised_xml_saved_state.json", "r") as reader:
        revised_xml = json.load(reader)
        current_line = revised_xml["current_line"]
else:
    print("How about you enter one of the options instead of probing the robustness of this script?")
    exit(3)

# Read lines until you run out of lines
while current_line < len(lines):
    
    # Ask for the assigned block. If its legal, add it to the dictionary under the assigned block.
    while True:
        
        # User input
        assigned_block = input(f"[{current_line}/{len(lines)}, {current_line*100/len(lines):.1f}%] {q} Type h to see available blocks. Type q to quit. Type sq to save and quit.\n{lines[current_line]}{lines[current_line+1]}{lines[current_line+2]}\n")
        
        # Assigned block is legal
        if assigned_block in list(blocks.keys()):
            old_id = lines[current_line].strip()
            entry_type = lines[current_line + 1].strip()
            entry = lines[current_line + 2].strip()
            revised_xml[assigned_block][old_id] = {
                "new_id": min_id + blocks[assigned_block]["offset"] + len(revised_xml[assigned_block]),
                "entry_type": entry_type,
                "entry": entry,
                }
            # This can technically overflow into the next block, but I don't think we have 32768 custom strings
            break
        
        # Get help
        elif assigned_block in ["h", "H", "help"]:
            for key in blocks:
                print(f"{key}: {blocks[key]['name']}")
        
        # Quit out, no saving
        elif assigned_block in ["q", "Q", "quit"]:
            if input("Are you sure you want to quit? Progress will not be saved!") in ["", "y", "Y", "q", "Q"]:
                print("Exiting ...")
                exit(1)
            else:
                print("Not exiting ...")
                continue
            
        # Quit out, save first
        elif assigned_block in ["s", "S", "sq", "SQ", "save"]:
            print("Writing progress to file ...")
            with open("./revised_xml_saved_state.json", "w") as writer:
                json.dump(revised_xml, writer)
            exit(2)
            
        # Illegal choice
        else:
            print(f"Please make a legal choice (h for help, q for quit, s for saving and quitting, or a positive integer in {[key for key in blocks]}.")
    
    # Skip lines until you find the next ID or reach the end of the document
    while True:
        current_line += 1
        if current_line >= len(lines) or lines[current_line].strip().isnumeric():
            revised_xml["current_line"] = current_line
            break

# Write the finished xml and the current mapping to files, done
with open("./revised_xml_completed.json", "w") as writer:
    json.dump(revised_xml, writer)
print("Writing to xml ...")
with open("./revised_xml.xml", "w") as xw, open("./id_mapping.json", "w") as mw:
    mapping = {}
    for block in revised_xml:
        if block == "current_line":
            continue
        xw.writelines([f"<!--BLOCK: {blocks[block]['name']}-->\n", "\n"])
        for old_id in revised_xml[block]:
            new_id = revised_xml[block][old_id]["new_id"]
            entry_type = revised_xml[block][old_id]["entry_type"]
            entry = revised_xml[block][old_id]["entry"]
            xw.writelines([str(new_id), "\n", entry_type, "\n", entry, "\n", "\n"])
            mapping[old_id] = new_id
    print("Writing mapping ...")
    json.dump(mapping, mw)
exit(0)