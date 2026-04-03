from PIL import Image
import math
import argparse

parser = argparse.ArgumentParser(
                    prog = 'Image Edit Script',
                    description = 'Manipulate or extract information from an image file',
                    epilog = '')

parser.add_argument('filename')           # positional argument
parser.add_argument('-o', '--output')      # option that takes a value
parser.add_argument('-f', '--format', choices=['firmware','nodered','nodered_int'], default='firmware', help='Output format')      # option that takes a value
parser.add_argument('-n', '--variablename', action='store_true')  # use outputfilename as variable name (for firmats nodered and nodered_int
parser.add_argument('-v', '--verbose', action='store_true')  # on/off flag
parser.add_argument('-i', '--invert', action='store_true')
parser.add_argument('-y', '--ymirror', action='store_true')
parser.add_argument('-x', '--xmirror', action='store_true')

args = parser.parse_args()
print(args.filename, args.output, args.format,args.verbose,args.invert,args.xmirror,args.ymirror)

outputfilename="result.txt"
if args.output is not None:
    outputfilename=args.output

im = Image.open(args.filename) # Can be many different formats.
pix = im.load()
print(im.size)  # Get the width and hight of the image for iterating over
#print(pix[10,10])  # Get the RGBA Value of the a pixel of an image

variablename="msg.payload"
if (args.variablename):
    variablename="var "+outputfilename.split('.')[0]
    print("Using variablename="+str(variablename))

output_pre=""
output_col_preL=""
output_col_writeCol_flag=False
output_col_preR=""
output_col_write_binary_flag=False
output_col_write_int_flag=False
output_col_post=""
output_post=""

if args.format=='firmware':
    output_pre=""
    output_col_preL="backBuffer["
    output_col_writeCol_flag=True
    output_col_preR="]=0b"
    output_col_write_binary_flag=True
    output_col_post=";\n"
elif args.format=='nodered': #Nodered Binary
    #output_pre="msg.payload=\"\\\n"
    output_pre=variablename+"=\"\\\n"
    output_col_preL=""
    output_col_writeCol_flag=False
    output_col_preR=""
    output_col_write_binary_flag=True
    output_col_post="\\\n"
    if (args.variablename):
        output_post="\";"
    else:
        output_post="\";\nreturn msg;"
elif args.format=='nodered_int': #Nodered Int
    output_pre=variablename+"=\"\\\n"
    output_col_preL=""
    output_col_writeCol_flag=False
    output_col_preR=""
    output_col_write_int_flag=True
    output_col_post=","
    if (args.variablename):
        output_post="\";"
    else:
        output_post="\";\nreturn msg;"



xrange = range(im.size[0])
yrange = range(im.size[1])

    
def calculateDistance(x1,y1,x2,y2):  
    dist = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)  
    return dist  
    
with open(outputfilename, 'w') as f:
    f.write(output_pre)
    _xrange=xrange
    if args.xmirror:
        _xrange=reversed(xrange)
    for ix,x in enumerate(_xrange):
        if args.verbose:
                print("ix="+str(ix)+" x="+str(x))
        f.write(output_col_preL)
        if output_col_writeCol_flag:
            f.write(str(x))
        f.write(output_col_preR)
        
        columnValue=0
                    
        _yrange=yrange
        if args.ymirror:
            _yrange=reversed(yrange)
        for iy,y in enumerate(_yrange):
            if args.verbose:
                print("iy="+str(iy)+" y="+str(y))
            c = pix[x,y] #get pixel
            if args.verbose:
                print("get pixel "+str(x)+","+str(y)+": color="+str(c))
            if args.invert:
                c=255-c
                
            if (c[0]>127):
                if output_col_write_binary_flag:
                    f.write("1")
                columnValue+=pow(2,iy)
            else:
                if output_col_write_binary_flag:
                    f.write("0")
        
        if output_col_write_int_flag:
            f.write(str(columnValue))

	    
        f.write(output_col_post)
        
    f.write(output_post)



