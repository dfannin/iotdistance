#!/usr/bin/env python

from __future__ import print_function


import requests
import json
import datetime 
import signal
import sys
import time
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("-d", "--daemon", help="run in daemon mode", action="store_true")
parser.add_argument("-c", "--count", help="run C times, default 1 ", type=int, default=1)
parser.add_argument("-t", "--time", help="num of secs between queries, default 60 secs ", type=int, default=60)
parser.add_argument("-H", "--host", help="host name or ip", required=True)

args = parser.parse_args()

count =  1 if  args.count == None  else  args.count
dtime =  60 if  args.time == None  else  args.time



def signal_handler(signal, frame):
    print("exiting") 
    sys.exit(0) 


def poll():
    try: 
        cdatetime = datetime.datetime.now() 
        eburl = 'http://' + args.host + '/distance/get'
        response = requests.get(eburl,timeout=5.0) 
        sensor =  json.loads(response.text) 
        print ("%s action: get MAC: %s distance: %4d" % ( cdatetime, sensor['mac'],sensor['distance']) ) 
    except requests.exceptions.RequestException as e:
        print ( cdatetime,e) 
    except KeyError as e:
        print ( cdatetime, "key error", e) 


signal.signal(signal.SIGINT, signal_handler) 

if args.daemon:
    while True:
        poll() 
        time.sleep(dtime) 
else:
    while count > 0:
        poll() 
        if count == 1:
            break 
        time.sleep(dtime) 
        count -= 1 



