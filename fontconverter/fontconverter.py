import pygame

pygame.init()
screen = pygame.display.set_mode((640, 480))
clock = pygame.time.Clock()
done = False

persistance=False
fps=10
color_checkerboardA=(250,250,250)
color_checkerboardB=(200,200,200)
color_bg=(255,255,255)
color_text=(0,0,0)

exportImages=True
exportImageFolder="imageexport/"

pixelsize=32


if exportImages:
    pixelsize=1
    color_checkerboardA=(0,0,0)
    color_checkerboardB=(0,0,0)
    color_bg=(0,0,0)
    color_text=(255,255,255)

#fontname="font_normal"
#font = pygame.font.Font("Minecraftia-Regular.ttf", fontsize*pixelsize);
#fontsize=8
#pixelsW=6
#pixelsH=8KeyboardInterrupt

#offset=(0,0)

#fontname="font_large"
#fontsize=16
#pixelsW=9
#pixelsH=8*2
#offset=(0,1)
#font = pygame.font.Font("Sweet16mono.ttf", fontsize*pixelsize); # https://github.com/kmar/Sweet16Font


fontname="font_small"
fontsize=10
pixelsW=3
pixelsH=8
offset=(0,2)
font = pygame.font.Font("pzim3x5.ttf", fontsize*pixelsize); # https://www.fonts4free.net/pixelzim-3x5-font.html



asciioffset=33
asciiend=126
ascii = asciioffset






file = open("output.txt", "w")  # write mode


file.write("const uint8_t "+fontname+"_width="+str(pixelsW)+";\n");
file.write("const uint8_t "+fontname+"_offset="+str(asciioffset)+";\n");

#file.write("uint8_t "+fontname+"["+str(asciiend-asciioffset+1)+"]["+str(pixelsW)+"];\n")
file.write("const uint"+str(pixelsH)+"_t "+fontname+"["+str(asciiend-asciioffset+1)+"]["+str(pixelsW)+"]= {\n")

screen.fill(color_bg)

screencleared=False

while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            done = True
    

    if not screencleared or not persistance:
        screencleared=True
        screen.fill(color_bg)

        for y in range(pixelsH):
            for x in range(pixelsW):
                color=color_checkerboardA
                if (x+y)%2==0:
                    color=color_checkerboardA
                pygame.draw.rect(screen, color, pygame.Rect(pixelsize*x, pixelsize*y, pixelsize, pixelsize))
    
    text = font.render(chr(ascii), True, color_text)
    screen.blit(text,(0+offset[0]*pixelsize,-pixelsize*2+offset[1]*pixelsize))
    print("ascii="+str(ascii)+" char="+chr(ascii))

    '''
    uint16_t font[COLUMNS];
    font[ascii][column]=0b00000000;

    int a[2][3] = {  
   {0, 2, 1} ,   /*  row at index 0 */
   {4, 3, 7} ,   /*  row at index 1 */
   };
    '''
    
    file.write("{")
    for x in range(pixelsW):
        int8number=0
        string8=""
        for y in range(pixelsH):    
            readcolor = screen.get_at(((int)(pixelsize*x+pixelsize/2),(int)(pixelsize*y+pixelsize/2)))
            if (readcolor==color_text):
                int8number+=pow(2,y)
                string8+='1'
            else:
                string8+='0'

        #file.write(fontname+"["+str(ascii-asciioffset)+"]["+str(x)+"]=0b"+string8+";\n")
        file.write(str(int8number))
        if (x<pixelsW-1):
            file.write(",")


    comment_character=chr(ascii)
    if comment_character=='\\':
        comment_character="Backslash"
    
    file.write("}, // "+str(ascii)+"="+comment_character+"\n")
    
            

    pygame.display.flip()

    if exportImages:
        exportrect=pygame.Rect(0,0,pixelsW,pixelsH)
        sub = screen.subsurface(exportrect)
        pygame.image.save(sub, exportImageFolder+str(ascii)+".png")

    clock.tick(fps)

    
    ascii+=1
    if (ascii>asciiend):
        done=True

file.write("};")
file.close()
