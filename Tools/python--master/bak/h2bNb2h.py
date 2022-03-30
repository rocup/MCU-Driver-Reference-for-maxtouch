import os
import sys
import struct
from struct import *

#hex to bin
def hex_bin(hexfile,binfile):    
    fin = open(hexfile)
    fout = open(binfile,'wb')
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
    print(cnt)
    fin.close()
    fout.close()

# bin to hex
def bin_hex(binfile,hexfile):
    fbin = open(binfile,'rb')
    fhex = open(hexfile,'w')
    offset = 0
    seg_addr = 0
    while 1:
        checksum=0 
        result = ':'
        bindata = fbin.read(0x10)
        if len(bindata) == 0 :
            break
        #end if
        result += '%02X' % len(bindata)
        result += '%04X' % offset
        result += '00'
        checksum = len(bindata)
        checksum += (offset & 0xff) + (offset >> 8)

        for i in range(0,len(bindata)):
            byte = unpack('B',bindata[i])
            result+='%02X' % byte
            checksum += byte[0]
        #end for    
        checksum = 0x01 + ~checksum
        checksum = checksum & 0xff
        result += '%02X/n' % checksum 
        fhex.write(result)
        offset += len(bindata)
        if offset == 0x10000:
            offset = 0            
            seg_addr += 1
            result = ':02000004'
            result += '%02X%02X' % ((seg_addr>>8) & 0xff,seg_addr & 0xff)
            checksum = 0x02 + 0x04 + (seg_addr>>8) + seg_addr & 0xff            
            checksum = -checksum
            result+='%02X' % (checksum & 0xff)
            result += '/n'
            fhex.write(result)
        #end if    
        if len(bindata) < 0x10:
            break
        #end if    
    #end while    
    fhex.write(':00000001FF')
    fbin.close()
    fhex.close()
#end for

if len(sys.argv) != 4 or (sys.argv[1] != '-h' and sys.argv[1] != '-b'):
    print ("usage:")
    print ("convert binary format to hexadecimal format: ")
    print (" hexbin.py -h binfile hexfile")
    print ("convert hexadecimal format to binary format:")
    print (" hexbin.py -b hexfile binfile")
    exit(0)

#if sys.argv[1] == '-h':
#    bin_hex(sys.argv[2],sys.argv[3])
#else:
#    hex_bin(sys.argv[2],sys.argv[3])
    
if __name__ == '__main__':
    hex_bin("d:\\1.hex","d:\\1.bin")
    print("end")
    