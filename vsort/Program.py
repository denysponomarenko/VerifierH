#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, os
import csv
import operator
from optparse import OptionParser
from shutil import copy, move

__version = '0.1'

def rsort(logfile, dest, bMove):
    counts = {}
  
    reader = csv.reader(open(logfile, "rb"))

    try:
        os.makedirs(dest)
    except OSError:
        pass # Folder exists
    
    for row in reader:
        if len(row) != 6:
            print 'Invalid Format, skipping Line:'
            print row
            continue    
   
        dest_path = dest + '/' + row[1]
    
        if bMove != 0:
            move(row[0], dest_path)
        else :
            copy(row[0], dest_path)
    
def main():
    parser = OptionParser(
                          usage='usage: %prog [options]',
                          version=sys.argv[0] + ' ' + __version
                          )
    parser.add_option('-l', '--log', dest='log', help='logfile')
    parser.add_option('-d', '--destination', dest='dest', help='destination directory')
    parser.add_option('-m', action="store_true", dest='move', help='move files') 
    
    (options, args) = parser.parse_args()

    if (not options.log or not options.dest):
        parser.print_help()
        return

    if(options.move):
       rsort(options.log, options.dest, 1)
    else:
        rsort(options.log, options.dest, 0)
 

if __name__ == '__main__':
    main()
