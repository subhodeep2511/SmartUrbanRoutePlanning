# Smart Urban Route Planning System

A small C++ desktop project that finds a fast route between locations in a sample city. It uses Dijkstra's algorithm to choose a route based on estimated travel time, and lets you simulate traffic changes and road closures from the interface.

## What it does

- Shows a map of the sample city's locations and connecting roads.
- Lets you choose a starting point and destination.
- Finds the route with the lowest estimated travel time.
- Displays the route, total distance, and estimated travel time.
- Simulates different traffic conditions, including a road closure.
- Highlights the selected route, congested roads, and closed roads on the map.
- Lets you reset traffic to normal conditions.

## Locations included

The current map contains eight sample locations:

- Central Station
- City Hospital
- University
- Shopping Mall
- Airport
- Tech Park
- Bus Terminal
- Old Town

The roads and their distances are predefined in the source code. This is a demonstration project, so the map and traffic values are simulated rather than taken from live city data.

## How it works

The city is represented as a graph:

- **Locations** are the vertices.
- **Roads** are the edges connecting those locations.
- Each road stores its distance, base travel time, traffic multiplier, and open/closed status.

When a route is requested, the program runs **Dijkstra's algorithm** using the road's estimated travel time as its weight. The traffic multiplier increases the cost of a road when congestion is simulated. Closed roads are skipped during route calculation.

The interface is built with the Windows API (Win32), and the map is drawn directly in the application window.

## Requirements

- Windows
- A C++ compiler with Windows API support, such as MinGW-w64 or Visual Studio

This program uses `windows.h`, so it is not a standard cross-platform console application.

## Build and run

### Option 1: MinGW-w64

Save the source file as `route_planner.cpp`, open Command Prompt or PowerShell in that folder, and run:

```bash
g++ route_planner.cpp -o route_planner.exe -mwindows -lgdi32 -luser32
```

Then launch it:

```bash
.\route_planner.exe
```

If your compiler reports a Windows API or linker error, check that you are using a Windows-targeting compiler and that the required Windows libraries are available.

### Option 2: Visual Studio

1. Create a C++ Windows Desktop project.
2. Add `route_planner.cpp` to the project.
3. Build and run it on Windows.

The code defines `WinMain`, so configure the project as a Windows application rather than a console application.

## Using the application

1. Choose a starting location.
2. Choose a destination.
3. Click **Find Best Route** to calculate the fastest available route.
4. Click **Simulate Traffic** to apply the next predefined traffic scenario and recalculate the route.
5. Click **Reset Traffic** to reopen all roads and restore normal traffic values.

The map uses these colors:

- **Blue:** selected route
- **Orange:** congested road
- **Red dashed:** closed road
- **Green marker:** starting location
- **Purple marker:** destination

## Project structure

```text
Smart-Urban-Route-Planning/
├── route_planner.cpp
└── README.md
```

## Current limitations

- The city map is fixed in the source code.
- Traffic scenarios are predefined; the program does not use live traffic feeds.
- Travel times are estimates, not real-time predictions.
- Route selection minimizes estimated travel time, not distance.
- The interface is designed for Windows.

## Possible improvements

- Load locations and roads from a file instead of hard-coding them.
- Add live map data and traffic APIs.
- Let users add or edit locations and roads.
- Add route options for shortest distance, fewer turns, or accessibility.
- Improve map scaling and layout for different window sizes.
