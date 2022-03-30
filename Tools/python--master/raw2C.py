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
                        help='where the \'raw\' file will be load')

    parser.add_argument('-t', '--object', required=False,
                        nargs='?',
                        default='',
                        const='.',
                        metavar='object #',
                        help='generate array from a specified object. Eg: -t 38')

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

    
def crc24(crc,firstbyte,secondbyte):
    crcpoly = 0x80001B
    #print(type(firstbyte),type(secondbyte),type(crc))
    data_word = (secondbyte << 8) | firstbyte
    result = ((crc << 1) ^ data_word)

    if result & 0x1000000:
        result ^= crcpoly
        
    return result&0xFFFFFF
        
def raw2c(args=None):
    parser = parse_args(args)
    aargs = args if args is not None else sys.argv[1:]
    args = parser.parse_args(aargs)
    print(args)

    if not args.filename:
        parser.print_help()
        return

    #format = args.type
#    if not DebugViewLog.supported_format(format):
#        print("Unsupported type", format)
#        return
    
    path = args.filename
    if args.object:
        object_mark = int(args.object,10)
        object_mark_t = args.object
    else:
        object_mark = 0;
    
    file = open(path,'r')
    output = path + ".h"
    line_cnt = 0
    crc_fg = False
    cfg_crc = 0
    cal_crc = 0
    crc_data = []
    raw_fg = False
    raw_data = []
    with open(output,'w') as nfile:
        for line in file.readlines():
            n_line = line.replace('\n','')
            if line_cnt == 0:
                print(n_line)
                nfile.write("const uint8_t file_magic_code[] = {\"" + n_line + "\"}; \n")
            elif line_cnt == 1:
                n_line = n_line.replace(' ',',0x')
                nfile.write("const uint8_t file_device_info[] = {0x" + n_line + "}; \n")
            elif line_cnt == 2:
                nfile.write("const uint32_t file_block_info_crc = 0x" + n_line + "; \n")
            elif line_cnt == 3:
                nfile.write("const uint32_t file_cfg_crc = 0x" + n_line + "; \n")
                cfg_crc = int(n_line, 16)
                print("config crc = ", hex(cfg_crc))
            else:
                #write array head
                if  line_cnt == 4:
                    nfile.write("const uint8_t file_cfg_data[] = {\n")
                raw_data_buf = n_line.split(" ")
                #del raw_data_buf[0:3]
                #raw_data += raw_data_buf
                #raw_data_buf = n_line.split(" ")
                #del raw_data_buf[0:3]
                #raw_data += raw_data_buf
                n_line = n_line.replace(' ',',0x')
                nfile.write("\t0x" + n_line + ",\n")
                object_num = int(line[0:4],16)
                if not crc_fg:
                    if object_num == 14 or object_num == 71:
                        crc_fg = True
                        print("cal crc start at:", object_num)
                if crc_fg:
                    line = line.replace('\n','')
                    line = line.split(" ")
                    length = len(line)
                    if length < 4:
                        print("file error!!", length)
                        return
                    crc_data += line[3:]
                if not raw_fg:
                    if object_num == object_mark:
                        raw_fg = True
                        print("found specified object: T",object_mark)
                if raw_fg:
                    del raw_data_buf[0:3]
                    raw_data += raw_data_buf
            line_cnt += 1
        nfile.write("\t};\n")
        #print(raw_data)
        
        #cal the crc
        offset = 0
        b_cnt = 0
        length = len(crc_data)
        while offset < length:            
            b_cnt += 1
            if b_cnt == 2:
                b_cnt = 0
                cal_crc = crc24(cal_crc, int(crc_data[offset - 1],16), int(crc_data[offset],16))
            offset += 1            
        if b_cnt == 1:
            cal_crc = crc24(cal_crc, int(crc_data[offset - 1]), 0)
        if cal_crc != cfg_crc:
            print("crc mismatched!", hex(cal_crc), "can not generate file")
            nfile.close()
            os.remove(output)
        print("cal crc marched")
        
        #write active program data start from specified object       
        if object_mark:
            length = len(raw_data)
            nfile.write("\n")
            nfile.write("const uint8_t file_start_obj_num = "+ object_mark_t + "; \n\t")
            nfile.write("\n")
            nfile.write("const uint8_t file_program_data[] = { \n\t")
            for offset in range(0,length):
                nfile.write("0x" + raw_data[offset] + ",")
                offset += 1
                if not offset%16:
                    nfile.write("\n\t")                   
            nfile.write("\n\t};\n")
        

if __name__ == '__main__':
    raw2c()
#    hex_bin(1.hex 1.bin)
    print("end")
    