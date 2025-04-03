# C-Custom-Program

This is a simple C custom program developed during my first-year university Computer Science course as part of a distinction task. It is a 2D game built using SDL2 in C. To run this program, ensure that SDL2 is installed on your device.

Use the following command in the terminal to compile and run the program:
gcc -o main.exe game.c -lSDL2

# **Brief Explaination:**
This program is a simple 2D game developed using SDL2 in C, where a character moves around the screen collecting stars to earn points. The simple program is built using various SDL2 components, including rendering, fonts, image loading, and audio, to create an engaging experience. The main character is controlled using keyboard inputs, with the ability to move in different directions and increase the speed using the Shift key. Stars appear randomly on the screen every five seconds, and when the character collides with a star, it disappears, and the score increases, accompanied by a sound effect. The game starts with a menu screen featuring a “Start” and “Quit” button, which allowed the user to enter the game or exit. The game uses a simple distance-based collision detection system to check if the character is close enough to collect a star. Throughout the game loop, the rendering functions ensure everything is continuously updated, from the background and character movements to the stars and score tracking. Additionally, the game maintains a limit on the number of stars displayed at once, which is maximum four.
