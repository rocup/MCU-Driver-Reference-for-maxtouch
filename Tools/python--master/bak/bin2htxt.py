import os
import re
import sys
import argparse
import numpy as np
import pandas as pd

def parse_args(args=None):
    parser = argparse.ArgumentParser(
        prog='convert(bin,txt,hex)',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description='bin to .h(txt) hex to bin(tbd)')

    parser.add_argument('--version',
                        action='version', version='%(prog)s v1.0.0',
                        help='show version')

    parser.add_argument('-f', '--filename', required=True,
                        nargs='?',
                        default='',
                        metavar='bin_hex',
                        help='where the \'bin|hex\' file will be load')

    parser.add_argument('-t', '--type', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='bin|hex',
                        help='format of of file data content')

    parser.add_argument('-r', '--range', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='NA',
                        help='NA')

    parser.add_argument('-s', '--size', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='NA',
                        help='NA')

    return parser

def bin2h(args=None):
    parser = parse_args(args)
    aargs = args if args is not None else sys.argv[1:]
    args = parser.parse_args(aargs)
    print(args)

    if not args.filename and not args.type:
        parser.print_help()
        return

    format = args.type
#    if not DebugViewLog.supported_format(format):
#        print("Unsupported type", format)
#        return
    
    path = args.filename
    
    file = open(path,'rb')
    fsize = os.path.getsize(path)
    
    #file = open("d:\\1.bin",'rb')
    #fsize = os.path.getsize("d:\\1.bin")
    val = file.read()
    file.close()
    print(fsize)
    
    output = path + ".h"
    with open(output,'w') as nfile:
    
        #write head info
        nfile.write("#ifndef TINY1617_FW_H"+"\n"+"#define TINY1617_FW_H"+"\n"+"\n"+"\n"+"uint8_t tiny1617_fw[]={"+"\n")
        w_cnt = 0
        crc = 0
        nfile.write("\t\t")
        while w_cnt < fsize:            
            nfile.write(str(hex(val[w_cnt]))+",")
            crc += val[w_cnt]
            w_cnt += 1
            if (w_cnt%16) == 0:
                nfile.write("\n")
                nfile.write("\t\t")        
        nfile.write(str(hex(crc)) + "};" + "\n")
        nfile.write("#endif"+"\n")
#    nfile = open("d:\\1.h",'w')
#set point to 0
#    nfile.seek(0)
#    cnt = 0
#    while 1
#    nfile.write('%s=%s'%('val[0]',val[0])
#        cnt++
#    nfile.close()
#    print ("file output")
#    print(val[0])
#    print ("file load")
#    return

if __name__ == '__main__':
    bin2h()
#    hex_bin(1.hex 1.bin)
    print("end")
    