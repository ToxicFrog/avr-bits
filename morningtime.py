#!/usr/bin/env python3

from __future__ import print_function

import sys
import usb.core
import usb.util

dev = usb.core.find(idVendor=0x16C0, idProduct=0x05DC)

if dev is None:
  raise ValueError("Couldn't find the Digispark.")

for cfg in dev:
  print('cfg:', cfg.bConfigurationValue)
  for intf in cfg:
    print('  if:', intf.bInterfaceNumber, intf.bAlternateSetting)
    for ep in intf:
      print('    ep:', ep.bEndpointAddress)

if sys.argv[1] == "on":
  dev.ctrl_transfer(0x40, 1, 0, 0, None)
elif sys.argv[1] == "off":
  dev.ctrl_transfer(0x40, 0, 0, 0, None)
else:
  print("Unknown command. Try 'on' or 'off'.")
