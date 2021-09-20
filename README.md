# colorBlend
Blend color with different cases.

printf("Usage: ./colorBlend testCase [-s SRC_PATH] [-d DST_PATH] [-o OUT_PATH] [-l LIGHT] [-t SATURATION] [-h HUE] [-rgb RED GREEN BLUE]"
        "example: \n"
        "    ./colorBlend 1 -l 120 -t 240 -s src_24.bmp -d dst_24.bmp -o out_24.bmp \n"
        "    ./colorBlend 2 -rgb 233 0 233 \n"
        "    ./colorBlend 3 -l 142 -t 120 -h 74 \n"
        "0: print help! \n"
        "1: Get src's color with light and saturation adjustable: get_src_with_light_and_saturation() \n"
        "2: convert RGB to light saturation hue. rgb_2_color() \n"
        "3: convert light saturation hue to RGB. color_2_rgb()");

Links:
    https://docs.github.com/en/get-started/getting-started-with-git/managing-remote-repositories
    https://blog.csdn.net/weixin_41010198/article/details/119698015