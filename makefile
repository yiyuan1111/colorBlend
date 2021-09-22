colorBlend: main.c
	gcc -g -Werror  main.c -lm -o colorBlend

clean:
	rm colorBlend main.o -f
