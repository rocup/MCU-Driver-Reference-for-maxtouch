import os
import re
import sys
import argparse
import numpy as np
import pandas as pd
from struct import *

def parse_args(args=None):
    parser = argparse.ArgumentParser(
        prog='hex to bin and htxt',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description='Examples: python h2bNh -f D:\\file.hex -s 0x3800')

    parser.add_argument('--version',
                        action='version', version='%(prog)s v1.0.0',
                        help='show version')
                        

    parser.add_argument('-f', '--filename', required=True,
                        nargs='?',
                        default='',
                        metavar='bin_hex',
                        help='where the \'hex\' file will be load')

    parser.add_argument('-t', '--type', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='bin|hex',
                        help='tbd')

    parser.add_argument('-r', '--range', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='NA',
                        help='tbd')

    parser.add_argument('-s', '--size', required=True,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='NA',
                        help='Fill with 0xFF if the bin smaller than the size')

    return parser

def crc24(crc,firstbyte,secondbyte):
    crcpoly = 0x80001B
    #print(type(firstbyte),type(secondbyte))
    data_word = (secondbyte << 8) | firstbyte
    result = ((crc << 1) ^ data_word)

    if result & 0x1000000:
        result ^= crcpoly

    return result
    
    #bin to htxt
def bin2htxt(inputfile,outputfile):
    file = open(inputfile,'rb')
    fsize = os.path.getsize(inputfile)
    val = file.read()
    file.close()
    
    with open(outputfile,'w') as nfile:
        #write head info
        nfile.write("#ifndef TINY1617_FW_H"+"\n"+"#define TINY1617_FW_H"+"\n"+"#define CRC_SIZE    3" + "\n"+"\n"+"const uint8_t tiny1617_fw[]={"+"\n")
        w_cnt = 0
        crc = 0
        b_cnt = 0
        nfile.write("\t\t")
        while w_cnt < fsize:            
            nfile.write(str(hex(val[w_cnt]))+",")
#            crc += val[w_cnt]
            b_cnt +=1
            if b_cnt == 2:
                b_cnt = 0
                crc = crc24(crc,val[w_cnt - 1],val[w_cnt])
            w_cnt += 1
            if (w_cnt%16) == 0:
                nfile.write("\n")
                nfile.write("\t\t")        
#        nfile.write(str(hex(crc)) + "};" + "\n")
        if b_cnt == 1:
            crc = crc24(crc, val[w_cnt -1],0)
        nfile.write(str(hex((crc >> 16)&0xff)) + "," + str(hex((crc >> 8)&0xff)) + "," + str(hex((crc >> 0)&0xff)) + "};" + "\n")
#        nfile.write(str(hex(crc&0xff0000) >> 16) + "," + str(hex(crc&0xff00) >> 8) + "," + str(hex(crc&0xff)) + "};" + "\n")
        nfile.write("\n" + "#endif"+"\n")
    print("htxt output: ", outputfile)
    return

    #hex to bin
def hex2bin(inputfile, outputfile):    
    fin = open(inputfile)
    fout = open(outputfile,'wb')
    cnt = 0
#    result =''
    for hexstr in fin.readlines():
        hexstr = hexstr.strip()
        size = int(hexstr[1:3],16)
        if int(hexstr[7:9],16) != 0:
            continue    
        #end if    
        for h in range( 0, size):
            b = int(hexstr[9+h*2:9+h*2+2],16)
#            result += chr(pack('B',b))
            fout.write(pack('B',b))
            cnt += 1
        #end if
#        fout.write(result)        
#        result=''
    while cnt < 0x3800:
        fout.write(pack('B',255))
        cnt += 1
    print("bin output to: ",outputfile)
    print("size: ",hex(cnt))
    fin.close()
    fout.close()
    return

def main(args=None):
    parser = parse_args(args)
    aargs = args if args is not None else sys.argv[1:]
    args = parser.parse_args(aargs)
    print(args)
    
#    if not args.filename and not args.type:
    if not args.filename:
        parser.print_help()
        return
        
    inputfile = args.filename
#    outputfile = inputfile + '.bin'
    
    hex2bin(inputfile,inputfile+'.bin')
    bin2htxt(inputfile+'.bin',inputfile+'.bin' + '.h')

if __name__ == '__main__':
    main()
    print("end")
    