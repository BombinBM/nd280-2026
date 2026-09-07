#ifndef CONFIG
#define CONFIG

#pragma region CUTS

// Used in EventCharge, CubesEdep, EventHeatmap if true information is needed change value to:
#define MIN_CHARGE_CUT 80
#define MIN_ENERGY_CUT 0.3


// Used in EventCharge, CubesEdep, EventHeatmap
#define MIN_TIME_CUT 500

// Used in EventCharge, CubesEdep
#define MIN_LENGTH_BETWEEN_CUBES_CUT 30

// Used in DrawFibers
#define MAX_DIV_FOR_LINEAR_TRACK_CUT 160

// Used in DrawFibers
#define STANDARD_ENERGY_CUT 20

#pragma endregion END_OF_CUTS

#pragma region SETTINGS

#define MAX_CUBES 200

#endif