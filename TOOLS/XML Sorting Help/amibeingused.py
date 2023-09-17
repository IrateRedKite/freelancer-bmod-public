# -*- coding: utf-8 -*-
"""
Created on Sun Sep 17 15:43:51 2023

@author: Katzenkebap
"""

### usage
"""
Run in TOOLS/XML Sorting Help or adjust base_dir below.
Recursively searches for all files of a type (default ".ini")
and asks for keywords. Then prints out any of the files
and the lines in those files where the expression
f"= {keyword}" shows up, as that is typically what the
assignment as some sort of parameter looks like.
"""

import os

def getFileList(path: str, fileType: str = '.ini'):
    """
    Compiles a list of path/to/files by recursively checking the input path a list of all directories
    and sub-directories for existence of a specific string at the end. Much slower than a shallow
    search. Use a shallow search if you know that recursiveness is not necessary.
    
    Inputs:
    
    path - Path-like or string, must point to the directory you want to check for files.
    fileType - String, all files that end with this string are selected.
    
    Outputs:
    
    fileList - A list of all paths/to/files
    """
    
    fileList = []
    if os.path.isfile(path) and path.lower().endswith(fileType):
        return [path]
    elif os.path.isdir(path):
        for d in sorted(os.listdir(path)):
            new_path = os.path.join(path, d)
            fileList += getFileList(new_path, fileType)
    else:
        # Should never be called
        return []

    return sorted(fileList)

# First, find and cache all .ini files
base_dir = "../../"
all_inis = getFileList(base_dir, ".ini")
print(f"Found {len(all_inis)} relevant files. Caching ...")
cache = {ini: [] for ini in all_inis}
for ini in all_inis:
    with open(ini, 'r', encoding="utf-8") as reader:
        try:
            cache[ini] = reader.readlines()
        except:
            cache[ini] = ["binary file has been excluded :v)"]
print("Cached.")

# Enter nickname (or arbitrary skin) and search cache for f"= {keyword}"
while True:
    
    keyword = input("Enter a keyword to search for (or enter to leave).\n")
    
    if keyword == "":
        break
    
    locations = {ini: [] for ini in all_inis}
    for ini in cache:
        for i, line in enumerate(cache[ini]):
            if f"= {keyword}" in line:
                locations[ini].append(i)
    for ini, lines in locations.items():
        if len(locations[ini]) != 0:
            print(f"{keyword} is assigned in file {ini} at line(s) {lines}!")