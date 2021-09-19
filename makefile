colorBlend: main.c
	gcc -Werror  main.c -o colorBlend

clean:
	rm colorBlend main.o -f
