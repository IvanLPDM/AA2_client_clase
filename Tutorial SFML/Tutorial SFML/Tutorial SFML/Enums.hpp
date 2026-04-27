#pragma once

enum class GameState {
	SplashScreen,
	LoginMenu,
	MatchmakingMenu,
	Gameplay,
	GameOver,
	RankingMenu,
	None
};

enum class MessageType
{
	PLAYER_PROFILE,
	MOVE_REQUEST,
	TURN_CHANGE,
	GAME_OVER,
	PLAYER_DISCONNECTED,
};

enum class CellState {
	EMPTY,
	PLAYER1,
	PLAYER2,
	PLAYER3,
	PLAYER4
};