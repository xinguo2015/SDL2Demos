
//Using SDL, SDL_image, SDL_ttf, standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h> 

#pragma warning(disable:4996)
typedef int BOOL;
#define FALSE 0  
#define TRUE  1  

typedef struct {

	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
}LTexture;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Globally used font
TTF_Font *gFont = NULL;

//Set text color as black
SDL_Color textColor = { 255, 255, 255, 255 };
//Current time start time
Uint32 startTime = 0;
//Start time string 
char startTimestr[100];
char timeText[100];
//Scene textures
LTexture gPromptTextTexture;
LTexture gTimeTextTexture;
LTexture gSplashTexture;
 
//Starts up SDL and creates window
BOOL init();
//Loads media
BOOL loadMedia();
//Frees media and shuts down SDL
void close();
//Loads individual image as texture
LTexture loadTexture(char* path);
//Loads text as texture
LTexture loadFromRenderedText(char *text, SDL_Color textColor);
//Free LTexture
void freeLTexture(LTexture ltexture);
//Render texture 
void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center);
//Our test callback function
Uint32 callback(Uint32 interval, void* param);

BOOL init()
{
	int imgFlags = IMG_INIT_PNG;
	//Initialization flag
	BOOL success = TRUE;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
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
				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
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

	//Load PNG texture 
	gSplashTexture = loadTexture("media/splash.png");
	if (gSplashTexture.mTexture == NULL)
	{
		printf("Failed to load splash image!\n");
		success = FALSE;
	}

	//Open the font
	gFont = TTF_OpenFont("media/myFont.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load myFont font! SDL_ttf Error: %s\n", TTF_GetError());
		success = FALSE;
	}
	else
	{
		//Render text
		gPromptTextTexture = loadFromRenderedText("Press Enter to Reset Start Time.", textColor);
		if (gPromptTextTexture.mTexture == NULL)
		{
			printf("Failed to render text texture!\n");
			success = FALSE;
		}
	}
	return success;
}

void close()
{
	//Free loaded image 
	freeLTexture(gPromptTextTexture);
	freeLTexture(gTimeTextTexture);
	freeLTexture(gSplashTexture);

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
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

LTexture loadFromRenderedText(char *text, SDL_Color textColor){

	LTexture textTexture;
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text, textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels 
		textTexture.mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (textTexture.mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			textTexture.mWidth = textSurface->w;
			textTexture.mHeight = textSurface->h;
		}
		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	return textTexture;
}

void freeLTexture(LTexture ltexture){

	if (ltexture.mTexture != NULL){

		SDL_DestroyTexture(ltexture.mTexture);
		ltexture.mWidth = 0;
		ltexture.mHeight = 0;
		ltexture.mTexture = NULL;
	}
}

void renderTexture(LTexture ltexture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center){

	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, ltexture.mWidth, ltexture.mHeight };
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, ltexture.mTexture, clip, &renderQuad, angle, center, SDL_FLIP_NONE);
}

Uint32 callback(Uint32 interval, void* param)
{
	//Print callback message
	printf("Callback called back with message: %s\n", (char*)param);
	return 0;
}

int main(int argc, char* args[])
{
	//Main loop flag
	BOOL quit = FALSE;
	//Event handler
	SDL_Event e;
	//Set callback
	SDL_TimerID timerID;
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
			timerID = SDL_AddTimer(3 * 1000, callback, "3 seconds waited!");
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
					//Reset start time on return keypress
					else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
					{
						startTime = SDL_GetTicks();
					}
				}

				//Set text to be rendered 
				strcpy(timeText, "Milliseconds since start time ");
				sprintf(startTimestr, "%d", SDL_GetTicks() - startTime); 
				strcat(timeText, startTimestr);

				//Render text
				freeLTexture(gTimeTextTexture);
				gTimeTextTexture = loadFromRenderedText(timeText, textColor);
				if (gTimeTextTexture.mTexture == NULL)
				{
					printf("Unable to render time texture!\n");
				}

				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 19, 57, 70, 0xFF);
				SDL_RenderClear(gRenderer);

				//Render text  
				renderTexture(gPromptTextTexture, (SCREEN_WIDTH - gPromptTextTexture.mWidth) / 2, 0, NULL, 0, NULL);
				renderTexture(gTimeTextTexture, (SCREEN_WIDTH - gPromptTextTexture.mWidth) / 2,
					(SCREEN_HEIGHT - gPromptTextTexture.mHeight) / 2, NULL, 0, NULL);
				/*
				renderTexture(gSplashTexture,0,0,NULL,0,NULL);
				*/
				//Update screen
				SDL_RenderPresent(gRenderer);
			}
			SDL_RemoveTimer(timerID);
		}
	}
	//Remove timer in case the call back was not called
	
	
	//Free resources and close SDL
	close();
	return 0;
}