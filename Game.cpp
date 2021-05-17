#include "Game.h"

const int thickness = 15;
const int paddleH = thickness*8;

Game::Game()
	:mWindow(nullptr), 
	mRenderer(nullptr),
	mTicksCount(0),
	mIsRunning(true) {}

// Initialize the game
bool Game::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"My First Game",
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);
	
	if (!mWindow) {
		return false;
	}

	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddlePos.x = 20.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;

	return true;
}

// Runs the game loop until the game is over
void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

// Shutdown the game
void Game::Shutdown() {
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}





// Helper functions for the game loop
void Game::ProcessInput() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	} 
	if (state[SDL_SCANCODE_W]) {
		mPaddleDir -= 1;
	} 
	if (state[SDL_SCANCODE_S]) {
		mPaddleDir += 1;
	}
}

void Game::UpdateGame() {

	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	//Delta time is the difference in ticks from last frame
	//(coverted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	if (deltaTime > .05f)
	{
		deltaTime = .05f;
	}

	//Paddle Collision
	if (mPaddleDir != 0) {
		mPaddlePos.y += mPaddleDir * 500.0f * deltaTime;
		mPaddleDir = 0;

		if (mPaddlePos.y < (paddleH / 2 + thickness)) {
			mPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness)) {
			mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	//Ball Physics
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
		mBallVel.y *= -1;
	}
	if (mBallPos.y >= 768 - thickness && mBallVel.y > 0.0f) {
		mBallVel.y *= -1;
	}
	if (mBallPos.x >= 1024 - thickness && mBallVel.x > 0.0f) {
		mBallVel.x *= -1;
	}

	if (abs(static_cast<int>(mBallPos.y - mPaddlePos.y)) < paddleH / 2.0f
		&& mBallPos.x <= 30.0f && mBallPos.x >= 20.0f
		&& mBallVel.x < 0.0f) {
		mBallVel.x *= -1.0f;
	}


	//Update tick counts
	mTicksCount = SDL_GetTicks();

}

void Game::GenerateOutput() {
	//Sets tje backgroudn color of the window
	SDL_SetRenderDrawColor(
		mRenderer,
		0,
		0,
		255,
		255
	);

	SDL_RenderClear(mRenderer); //Clear the back buffer to the current draw color

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	//bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	// Draw ball
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness
	};

	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x - thickness / 2),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		paddleH
	};

	SDL_RenderFillRect(mRenderer, &ball);
	SDL_RenderFillRect(mRenderer, &paddle);
	SDL_RenderPresent(mRenderer); //Swaps front and back buffer to the color to the window
}
