#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

uint64_t last_tick_add;

//Character Data
typedef struct character
{
    int x;
    int y;
    SDL_Texture *texture_character;
} Character;

//Star Data
typedef struct star
{
    bool appear;
    int x;
    int y;
    int star_index;
    SDL_Texture *texture_star;
} Star;

typedef enum getstar
{
    GET,
    NOTHING
} GetStar;

//draw out button
void draw_button(SDL_Renderer *renderer, SDL_Texture *button_texture, int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderCopy(renderer, button_texture, NULL, &rect);
}

//draw out character
void draw_character(SDL_Renderer *renderer, Character character)
{
    SDL_Rect rect;
    rect.x = character.x;
    rect.y = character.y;
    rect.w = 125;
    rect.h = 125;
    SDL_RenderCopy(renderer, character.texture_character, NULL, &rect);
}

//Draw out the text
void draw_text(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int x, int y)
{
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	int texW = 0, texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = {x, y, texW, texH};
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

//Draw out the stars
void draw_star(SDL_Renderer *renderer, Star *star_array)  
{
    for(int i = 0; i < 4; i++)
    {
        if (star_array[i].appear)
        {
            SDL_Rect starRect;
            starRect.x = star_array[i].x;
            starRect.y = star_array[i].y;
            starRect.w = 100;
            starRect.h = 100;
            SDL_RenderCopy(renderer, star_array[i].texture_star, NULL, &starRect);
        }
    }
}

//Logic of how user get star and earn score
GetStar character_get_star(Character character, Star *star_array)
{
    for(int i = 0; i < 4; i++)
    {
        if(star_array[i].appear)
        {
            //absolute value, 50 = half size of star, 62.5 = half size of character
            int distance_from_character_x = abs((star_array[i].x + 50) - (character.x + 62.5)); 
            int distance_from_character_y = abs((star_array[i].y + 50) - (character.y + 62.5));

            if ((distance_from_character_x < 20) && (distance_from_character_y < 20))
            {
                star_array[i].appear = false;
                return GET;
            }
        }

    }
    return NOTHING;
}

//Adding star every 5 seconds
void add_star(Star *star_array, SDL_Texture **texture_new_star, int *star_index)
{
    // Add food logic
    bool adding_star = false;

    //5000 millisecond = 5 seconds
    if (SDL_GetTicks() - last_tick_add > 5000)
    {
        adding_star = true;
        last_tick_add = SDL_GetTicks();
    }

    if (adding_star)
    {
        Star new_star;

        new_star.appear = true;
        new_star.star_index = rand() % 4;                //random get a number from 0 to 3
        new_star.texture_star = texture_new_star[new_star.star_index];  //change the image star based on number
        new_star.x = rand() % (1000 - 100);     //prevent star getting out of window
        new_star.y = rand() % (600 - 100);

        star_array[*star_index] = new_star;     //number of star

        if (*star_index < 4 - 1)      //-1 cause array start from 0
        {
            (*star_index) += 1;  //if less than four star in window, add
        }
        else
        {
            (*star_index) = 0;  //if more than 4, remove one add a new
        }
    }
}

//draw out background
void draw_background(SDL_Renderer *renderer, SDL_Texture *background)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 1000;
    rect.h = 600;
    SDL_RenderCopy(renderer, background, NULL, &rect);
}

int main(int argc, char ** argv)
{
    //SDL Init
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    TTF_Init();

    //create window
    SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    //music
    Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Music *music = Mix_LoadMUS("music/megalovania.mp3");
    Mix_PlayMusic( music, -1 );

    //earn score sound effect
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 1024 );
    Mix_Chunk *audio_earn = Mix_LoadWAV("music/earn.wav");

    //Text instruction
    TTF_Font *fontInstruction = TTF_OpenFont("font/arial.ttf", 20);
	SDL_Color colorInstruction = {255, 255, 255};

    //Coordinate X Y text
	TTF_Font *fontCoor = TTF_OpenFont("font/arial.ttf", 25);
	SDL_Color colorCoor = {236, 184, 39};

    //Score text
    TTF_Font *fontScore = TTF_OpenFont("font/arial.ttf", 35);
	SDL_Color colorScore = {255, 83, 73};

    //Load image
    SDL_Surface *pic_start = IMG_Load("image/start.png");
    SDL_Surface *pic_quit = IMG_Load("image/quit.png");
    SDL_Surface *pic_character = IMG_Load("image/sans.png");
    SDL_Surface *pic_sky = IMG_Load("image/sky.png");
    SDL_Surface *pic_star = IMG_Load("image/star1.png");
    SDL_Surface *pic_star1 = IMG_Load("image/star2.png");
    SDL_Surface *pic_star2 = IMG_Load("image/star3.png");
    SDL_Surface *pic_star3 = IMG_Load("image/star4.png");

    //Load surface
    SDL_Texture *texture_start = SDL_CreateTextureFromSurface(renderer, pic_start);
    SDL_Texture *texture_quit = SDL_CreateTextureFromSurface(renderer, pic_quit);
    SDL_Texture *texture_sky = SDL_CreateTextureFromSurface(renderer, pic_sky);
    SDL_Texture *texture_character = SDL_CreateTextureFromSurface(renderer, pic_character);
    
    SDL_Texture *texture_new_star[4] = {
        SDL_CreateTextureFromSurface(renderer, pic_star),
        SDL_CreateTextureFromSurface(renderer, pic_star1),
        SDL_CreateTextureFromSurface(renderer, pic_star2),
        SDL_CreateTextureFromSurface(renderer, pic_star3),
    };

    //Clear image
    SDL_FreeSurface(pic_start);
    SDL_FreeSurface(pic_quit);
    SDL_FreeSurface(pic_sky);
    SDL_FreeSurface(pic_character);
    SDL_FreeSurface(pic_star);
    SDL_FreeSurface(pic_star1);
    SDL_FreeSurface(pic_star2);
    SDL_FreeSurface(pic_star3);

    // Application State
    int star_renewal = 0;
    Star star_array[4] = {
        {.appear = false},
        {.appear = false},
        {.appear = false},
        {.appear = false}};

    Character character;
    character.texture_character = texture_character;
    character.x = 450;
    character.y = 250;

    //bool
    bool running = true;
    bool game = false;

    draw_background(renderer, texture_sky);
    //draw start button
    draw_button(renderer, texture_start, 300, 100, 400, 200);
    //draw quit button
    draw_button(renderer, texture_quit, 300, 300, 400, 120);
    
    SDL_Event event;

    //variable declare
    int score = 0;
	int hoverX;
	int hoverY;

    //Program start running repeatly
	while (running)
	{
        // Poll for event
        if (SDL_PollEvent(&event) > 0)
        {
            //Character movement
            switch (event.type)
            {
            //Click X on right top to quit
            case SDL_QUIT:
                running = false;
                break;
            //Press start and quit button at starting interface
            case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					int mouseX = event.motion.x;
    				int mouseY = event.motion.y;
					
					if(mouseX < 700 && mouseX > 300 && mouseY > 100 && mouseY < 300)
					{
						game = true;
					}
					else if(game == false && mouseX < 700 && mouseX > 300 && mouseY > 300 && mouseY < 420)
					{
						running = false;
                        break;
					}
				}
				break;
            case SDL_KEYDOWN:
                // Arrow keys for character movement
                switch(event.key.keysym.scancode)
                {
                    //Press Q to quit
                    case SDL_SCANCODE_Q:
                        running = false;
                        break;
                    //Press left/A to go left
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_A:
                        if(character.x > 0)
                        {
                            if (event.key.keysym.mod & KMOD_SHIFT)
                            {
                                character.x -= 50;
                            }
                            else
                            {
                                character.x -= 15;
                            }   
                        }
                        break;
                    //Press right/D to go right
                    case SDL_SCANCODE_RIGHT:
                    case SDL_SCANCODE_D:
                        if(character.x < 875)
                        {
                            if (event.key.keysym.mod & KMOD_SHIFT)
                            {
                                character.x += 50;
                            }
                            else
                            {
                                character.x += 15;
                            }   
                        }
                        break;
                    //Press up/W to go up
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_W:
                        if(character.y > 0)
                        {
                            if (event.key.keysym.mod & KMOD_SHIFT)
                            {
                                character.y -= 50;
                            }
                            else
                            {
                                character.y -= 15;
                            }   
                        }
                        break;
                    //Press down/S to go down
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        if(character.y < 475)
                        {
                            if (event.key.keysym.mod & KMOD_SHIFT)
                            {
                                character.y += 50;
                            }
                            else
                            {
                                character.y += 15;
                            }   
                        }
                        break;
                }
                break;
            }
        }
        //score
        char score_text[256];
        sprintf(score_text, "Score: %d", score);

        //Get Coordinate X Y
        SDL_GetMouseState(&hoverX, &hoverY);
        //Input X Y Coords as string
        char coorX[256];
        sprintf(coorX, "Cursor X: %d", hoverX);
        char coorY[256];
        sprintf(coorY, "Cursor Y: %d", hoverY);

        //Combine coordinate text
        char coordinate[256] = "";
        strcat(coordinate, coorX);
        strcat(coordinate, "      ");
        strcat(coordinate, coorY);

        //instruction
        char textInstruction[256];
        sprintf(textInstruction, "PRESS Q TO QUIT");

        char textInstruction1[256];
        sprintf(textInstruction1, "PRESS WASD BUTTON TO MOVE AROUND, PRESS WITH SHIFT TO MOVE FASTER");

        // Application logic
        add_star(star_array, texture_new_star, &star_renewal);

        // Scoring logic
        switch(character_get_star(character, star_array)){
            case GET:
                score += 1;
                Mix_PlayChannel( -1, audio_earn, 0 );
                break;
            default:
        }

        //When at game interface
        if(game == true)
        {
            // Draw background
            draw_background(renderer, texture_sky);

            // Draw star
            draw_star(renderer, star_array);

            // Draw character
            draw_character(renderer, character);

            //Draw score text
            draw_text(score_text, renderer, fontScore, colorScore, 825, 0);

            //Draw coordinate text
            draw_text(coordinate, renderer, fontCoor, colorCoor, 20, 560);

            //Draw instruction text
            draw_text(textInstruction, renderer, fontInstruction, colorInstruction, 0, 0);
            draw_text(textInstruction1, renderer, fontInstruction, colorInstruction, 0, 20);
        }
       
        SDL_RenderPresent(renderer);
	}
    
    //Cleanup
    SDL_DestroyTexture(texture_start);
    SDL_DestroyTexture(texture_quit);
    SDL_DestroyTexture(texture_sky);
    SDL_DestroyTexture(texture_character);

    for (int i = 0; i < 4; i++)
    {
        SDL_DestroyTexture(texture_new_star[i]);
    }

    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

    Mix_FreeMusic(music);
    Mix_FreeChunk(audio_earn);

    TTF_CloseFont(fontScore);
    TTF_CloseFont(fontCoor);
    TTF_CloseFont(fontInstruction);

	TTF_Quit();
	IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();

    return 0;
}
