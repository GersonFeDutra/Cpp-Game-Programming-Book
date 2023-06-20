#define GRAPHICS "graphics/"
#define PLAYER_PNG GRAPHICS "player.png"

#define PLAYER_CENTER_X 25
#define PLAYER_CENTER_Y 25


/* Utils */
#define PI 3.14159265358979323846

#define clamp(X, MIN, MAX)                                                                         \
	{                                                                                              \
		if (X < (MIN))                                                                             \
			X = MIN;                                                                               \
		if (X > (MAX))                                                                             \
			X = MAX;                                                                               \
	}
