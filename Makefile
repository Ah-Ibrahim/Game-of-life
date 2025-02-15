run: game
	@./game

game: game.c
	@gcc -pthread -o game game.c
