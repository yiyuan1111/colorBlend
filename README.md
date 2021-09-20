# colorBlend  
Blend color with different cases.  
  
Usage: ./colorBlend testCase [-s SRC_PATH] [-d DST_PATH] [-o OUT_PATH] [-l LIGHT] [-t SATURATION] [-h HUE] [-rgb RED GREEN BLUE]
  
        example:  
            ./colorBlend 1 -l 120 -t 240 -s src_24.bmp -d dst_24.bmp -o out_24.bmp  
            ./colorBlend 2 -rgb 233 0 233  
            ./colorBlend 3 -l 142 -t 120 -h 74  
  
        testCase:  
            0: print help!  
            1: Get src's color with light and saturation adjustable: get_src_with_light_and_saturation()              
            2: convert RGB to light saturation hue. rgb_2_color()  
            3: convert light saturation hue to RGB. color_2_rgb()  
  
Links:  
    1. https://docs.github.com/en/get-started/getting-started-with-git/managing-remote-repositories  
    2. https://blog.csdn.net/weixin_41010198/article/details/119698015  
    3. <怎样用 rgb 三元组理解色相、亮度和饱和度？>: https://www.zhihu.com/question/265265004  
    4. <BMP文件格式详解（BMP file format）>:  https://blog.csdn.net/u013066730/article/details/82625158  
  