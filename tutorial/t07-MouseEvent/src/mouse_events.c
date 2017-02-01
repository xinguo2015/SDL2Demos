
//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>  

typedef int BOOL;
#define FALSE 0  
#define TRUE  1 
#define TOTAL_BUTTONS 4

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Button constants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200; 

typedef enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
}LButtonSprite;

typedef struct {

	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
}LTexture;

typedef struct {
	
	int x;
	int y;
	//Currently used global sprite
	LButtonSprite mCurrentSprite;
}LButton;
 
//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL; 
//Displayed texture
LTexture gButtonSpriteTexture;
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL]; 
//Buttons objects
LButton gButtons[TOTAL_BUTTONS];
 
//Starts up SDL and creates window
BOOL init();
//Loads media
BOOL loadMedia();
//Frees media and shuts down SDL
void close();
//Loads individual image as texture
LTexture loadTexture(char* path);
//Free LTexture
void freeLTexture(LTexture ltexture);
//Render texture 
void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip);
//Set position of button
void setPosition(LButton *button, int x, int y); 
//Handle mouse event
void handleMouseEvent(LButton *button,SDL_Event* e);
//Shows button sprite
void renderButton(LButton *button);
 
BOOL init()
{
	int imgFlags = IMG_INIT_PNG;
	//Initialization flag
	BOOL success = TRUE;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = FALSE;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = FALSE;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = FALSE;
			}
			else
			{
				//Initialize PNG loading 
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = FALSE;
				}
			}
		}
	}
	return success;
}

BOOL loadMedia()
{
	//Loading success flag
	BOOL success = TRUE;
	int i;
	//Load PNG texture 
	gButtonSpriteTexture = loadTexture("media/button.png");
	if (gButtonSpriteTexture.mTexture == NULL)
	{
		printf("Failed to load button image!\n");
		success = FALSE;
	}
	else
	{
		//Set sprites
		for (i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
		{
			gSpriteClips[i].x = 0;
			gSpriteClips[i].y = i * 200;
			gSpriteClips[i].w = BUTTON_WIDTH;
			gSpriteClips[i].h = BUTTON_HEIGHT;
		}
		for (i = 0; i < TOTAL_BUTTONS; ++i) 
			gButtons[i].mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT; 

		//Set buttons in corners
		setPosition(gButtons, 0, 0);
		setPosition(gButtons + 1, SCREEN_WIDTH - BUTTON_WIDTH, 0);
		setPosition(gButtons + 2, 0, SCREEN_HEIGHT - BUTTON_HEIGHT);
		setPosition(gButtons + 3, SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
	}
	return success;
}

void close()
{
	//Free loaded image 
	freeLTexture(gButtonSpriteTexture); 

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

LTexture loadTexture(char* path){

	LTexture ltexture;
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s!SDL Error: %s\n", path, SDL_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
		//Create texture from surface pixels  
		ltexture.mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (ltexture.mTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		else{
			//Get image dimensions
			ltexture.mWidth = loadedSurface->w;
			ltexture.mHeight = loadedSurface->h;
		}
		//Get rid of old surface
		SDL_FreeSurface(loadedSurface);
	}
	//Return LTexture
	return ltexture;
}

void freeLTexture(LTexture ltexture){

	if (ltexture.mTexture != NULL){

		SDL_DestroyTexture(ltexture.mTexture);
		ltexture.mWidth = 0;
		ltexture.mHeight = 0;
		ltexture.mTexture = NULL;
	}
}

void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, ltexture.mWidth, ltexture.mHeight };
	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	//Render to screen
	SDL_RenderCopyEx(gRenderer, ltexture.mTexture, clip, &renderQuad, angle, center, flip);
}
    
void handleMouseEvent(LButton *button, SDL_Event* e)
{
	//Check if mouse is in button
	BOOL inside = TRUE;
	//Get mouse position
	int x, y;
	//If mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{ 
		SDL_GetMouseState(&x, &y);

		//Mouse is left of the button
		if (x < button->x)
		{
			inside = FALSE;
		}
		//Mouse is right of the button
		else if (x > button->x + BUTTON_WIDTH)
		{
			inside = FALSE;
		}
		//Mouse above the button
		else if (y < button->y)
		{
			inside = FALSE;
		}
		//Mouse below the button
		else if (y > button->y + BUTTON_HEIGHT)
		{
			inside = FALSE;
		}

		//Mouse is outside button
		if (!inside)
		{
			button->mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		//Mouse is inside button
		else
		{
			//Set mouse over sprite
			switch (e->type)
			{
			case SDL_MOUSEMOTION:
				button->mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;

			case SDL_MOUSEBUTTONDOWN:
				button->mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				button->mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}

void renderButton(LButton *button)
{
	//Show current button sprite
	renderTexture(gButtonSpriteTexture, button->x, button->y, &gSpriteClips[button->mCurrentSprite], 0, NULL, SDL_FLIP_NONE);
}

void setPosition(LButton *button, int x, int y)
{
	button->x = x;
	button->y = y;
}
 
int main(int argc, char* args[])
{
	int i;
	//Main loop flag
	BOOL quit = FALSE;
	//Event handler
	SDL_Event e;
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = TRUE;
					}
					//Handle mouse events
					for (i = 0; i < TOTAL_BUTTONS; ++i)
					{
						handleMouseEvent(gButtons+i, &e);
					}
				}
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);
				
				//Render buttons
				for (i = 0; i < TOTAL_BUTTONS; ++i)
				{ 
					renderButton(gButtons + i);
				}

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//Free resources and close SDL
	close();
	return 0;
	}