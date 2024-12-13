# Fast fragment parser for Android written in C++ 20

## How to compile and run 

```sh
zig c++ -std=c++2a -O3 -g0 fragmentdumper.cpp && a "adb -s 127.0.0.1:5565 shell dumpsys activity top -a -c
```

##  How to use it in Python
```py

import pandas as pd
from cythondfprint import add_printer
import cv2


# a "adb -s 127.0.0.1:5565 shell dumpsys activity top -a -c --checkin" > c:\forpandascsv.csv
def cropimage(img, coords):
    return img[coords[1] : coords[3], coords[0] : coords[2]]


# adb -s 127.0.0.1:5565 shell screencap -p /sdcard/mypngfile.png
# adb -s 127.0.0.1:5565 pull /sdcard/mypngfile.png

imx = cv2.imread(r"C:\ProgramData\anaconda3\envs\a0\mypngfile.png")
add_printer(1)
# Example of a dump file 
# https://github.com/hansalemaos/android_fragment_parser/blob/main/forpandascsv.csv
df = pd.read_csv(r"c:\forpandascsv.csv")
df.ds_color_print_all()
for key, item in df.iterrows():
    try:
        outfile = cropimage(imx, (item.START_X, item.START_Y, item.END_X, item.END_Y))
        filname = rf"C:\picsoutfile\{key}.png"
        cv2.imwrite(filname, outfile)
    except Exception as e:
        print(e)


```