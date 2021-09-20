colorBlend: main.c
	gcc -Werror  main.c -lm -o colorBlend

clean:
	rm colorBlend main.o -f
