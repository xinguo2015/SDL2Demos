
//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>  
#include <string.h> 

#pragma warning(disable:4996)

typedef int BOOL;
#define FALSE 0  
#define TRUE  1  

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Button constants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;

typedef struct {

	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
}LTexture;

typedef struct {

	//Window data
	SDL_Window* mWindow;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	BOOL mMouseFocus;
	BOOL mKeyboardFocus;
	BOOL mFullScreen;
	BOOL mMinimized;

}LWindow;

//The window we'll be rendering to
LWindow gWindow;
//The window renderer
SDL_Renderer* gRenderer = NULL;
//Scene textures
LTexture gSceneTexture;

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

//Intializes window
BOOL initWindow(LWindow* lw);
//Creates renderer from internal window
SDL_Renderer* createRenderer(LWindow* lw);
//Handles window events
void handleEvent(LWindow* lw, SDL_Event* e);
//Deallocates window
void freeLWindow(LWindow* lw);
//Window dimensions
int getWidth(LWindow* lw);
int getHeight(LWindow* lw);

BOOL hasMouseFocus(LWindow* lw);
BOOL hasKeyboardFocus(LWindow* lw);
BOOL isMinimized(LWindow* lw);

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
		if (!initWindow(&gWindow))
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = FALSE;
		}
		else
		{
			//Create renderer for window
			gRenderer = createRenderer(&gWindow);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = FALSE;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

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
	//Load PNG texture 
	gSceneTexture = loadTexture("media/window.png");
	if (gSceneTexture.mTexture == NULL)
	{
		printf("Failed to load window image!\n");
		success = FALSE;
	}
	return success;
}

void close()
{
	//Free loaded image 
	freeLTexture(gSceneTexture);

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	freeLWindow(&gWindow);

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

BOOL initWindow(LWindow* lw)
{
	//Create window
	lw->mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (lw->mWindow != NULL)
	{
		lw->mMouseFocus = TRUE;
		lw->mKeyboardFocus = TRUE;
		lw->mWidth = SCREEN_WIDTH;
		lw->mHeight = SCREEN_HEIGHT;
	}

	return lw->mWindow != NULL;
}

SDL_Renderer* createRenderer(LWindow* lw){

	return SDL_CreateRenderer(lw->mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void handleEvent(LWindow* lw, SDL_Event* e){

	//Caption string
	char caption[100];

	//Caption update flag
	BOOL updateCaption = FALSE;
	//Window event occured
	if (e->type == SDL_WINDOWEVENT)
	{
		switch (e->window.event)
		{
			//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			lw->mWidth = e->window.data1;
			lw->mHeight = e->window.data2;
			SDL_RenderPresent(gRenderer);
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			lw->mMouseFocus = TRUE;
			updateCaption = TRUE;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			lw->mMouseFocus = FALSE;
			updateCaption = TRUE;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			lw->mKeyboardFocus = TRUE;
			updateCaption = TRUE;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			lw->mKeyboardFocus = FALSE;
			updateCaption = TRUE;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			lw->mMinimized = TRUE;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			lw->mMinimized = FALSE;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			lw->mMinimized = FALSE;
			break;
		}

		//Update window caption with new data
		if (updateCaption)
		{
			strcpy(caption, "SDL Tutorial - MouseFocus: ");
			strcat(caption, (lw->mMouseFocus) ? "On" : "Off");
			strcat(caption, " KeyboardFocus:");
			strcat(caption, (lw->mKeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(lw->mWindow, caption);
		}
	}
	//Enter exit full screen on return key
	else if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_RETURN)
	{
		if (lw->mFullScreen)
		{
			SDL_SetWindowFullscreen(lw->mWindow, SDL_FALSE);
			lw->mFullScreen = FALSE;
		}
		else
		{
			SDL_SetWindowFullscreen(lw->mWindow, SDL_TRUE);
			lw->mFullScreen = TRUE;
			lw->mMinimized = TRUE;
		}
	}
}

void freeLWindow(LWindow* lw)
{
	if (lw->mWindow != NULL)
	{
		SDL_DestroyWindow(lw->mWindow);
	}

	lw->mMouseFocus = FALSE;
	lw->mKeyboardFocus = FALSE;
	lw->mWidth = 0;
	lw->mHeight = 0;
}

//Window dimensions
int getWidth(LWindow* lw){

	return lw->mWidth;
}
int getHeight(LWindow* lw){

	return lw->mHeight;
}

BOOL hasMouseFocus(LWindow* lw){

	return lw->mMouseFocus;
}
BOOL hasKeyboardFocus(LWindow* lw){

	return lw->mKeyboardFocus;
}
BOOL isMinimized(LWindow* lw){

	return lw->mMinimized;
}

int main(int argc, char* args[])
{
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

					//Handle window events
					handleEvent(&gWindow, &e);
				}
				//Only draw when not minimized
				if (!isMinimized(&gWindow))
				{
					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					renderTexture(gSceneTexture, (getWidth(&gWindow) - gSceneTexture.mWidth) / 2,(getHeight(&gWindow) - gSceneTexture.mHeight) / 2, NULL, 0, NULL, SDL_FLIP_NONE);

					//Update screen
					SDL_RenderPresent(gRenderer);
				}
			}
		}
	}
	//Free resources and close SDL
	close();
	return 0;
}