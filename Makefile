all:
	gcc main.c utils.c game.c menu.c -lm -o m
run:
	./m