#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <algorithm>
#include <cstdio>
#include <cstring>

using namespace std;

const int N = 8;
const int M = 12;

struct City {
    const char* name;
    int x, y;
};

struct Road {
    int u, v;
    double km, time, traffic;
    bool open;
};

City cities[N] = {
    {"Central Station", 70, 100},
    {"City Hospital", 210, 60},
    {"University", 350, 90},
    {"Shopping Mall", 490, 60},
    {"Airport", 570, 180},
    {"Tech Park", 400, 240},
    {"Bus Terminal", 220, 240},
    {"Old Town", 70, 260}
};

Road roads[M] = {
    {0,1,4,8,1,true},
    {0,6,3,7,1,true},
    {0,7,2.5,6,1,true},
    {1,2,5,9,1,true},
    {1,6,4,8,1,true},
    {2,3,3.5,7,1,true},
    {2,5,4.5,9,1,true},
    {3,4,6,11,1,true},
    {3,5,4,8,1,true},
    {4,5,5.5,10,1,true},
    {5,6,5,9,1,true},
    {6,7,3,6,1,true}
};

HWND startBox, endBox, outputBox;
HWND distanceLabel, timeLabel, mainWindow;

vector<int> route;

int source = 0;
int destination = 4;
int scenario = 0;

double totalKm = 0;
double totalTime = 0;

bool found = false;

string number(double x) {
    ostringstream s;
    s << fixed << setprecision(1) << x;
    return s.str();
}

int roadIndex(int a, int b) {
    for (int i = 0; i < M; i++) {
        if ((roads[i].u == a && roads[i].v == b) ||
            (roads[i].u == b && roads[i].v == a)) {
            return i;
        }
    }
    return -1;
}

void resetTraffic() {
    for (int i = 0; i < M; i++) {
        roads[i].traffic = 1.0;
        roads[i].open = true;
    }
    scenario = 0;
}

void simulateTraffic() {
    resetTraffic();
    scenario = scenario + 1;

    if (scenario % 3 == 1) {
        roads[3].traffic = 2.2;
        roads[5].traffic = 1.8;
        roads[8].traffic = 1.7;
    }
    else if (scenario % 3 == 2) {
        roads[1].traffic = 2.0;
        roads[4].traffic = 1.8;
        roads[10].traffic = 2.3;
        roads[7].open = false;
    }
    else {
        roads[0].traffic = 1.6;
        roads[6].traffic = 2.1;
        roads[9].traffic = 1.8;
    }
}

bool isRouteRoad(int a, int b) {
    for (int i = 0; i + 1 < (int)route.size(); i++) {
        if ((route[i] == a && route[i + 1] == b) ||
            (route[i] == b && route[i + 1] == a)) {
            return true;
        }
    }
    return false;
}

void calculateRoute() {
    int selectedStart =
        (int)SendMessageA(startBox, CB_GETCURSEL, 0, 0);

    int selectedEnd =
        (int)SendMessageA(endBox, CB_GETCURSEL, 0, 0);

    if (selectedStart == CB_ERR || selectedEnd == CB_ERR) {
        SetWindowTextA(outputBox, "Please select both locations.");
        return;
    }

    source = selectedStart;
    destination = selectedEnd;

    vector<double> dist(N, DBL_MAX);
    vector<int> previous(N, -1);
    vector<int> visited(N, 0);

    dist[source] = 0;

    // Dijkstra's algorithm: find the minimum travel-time route.
    for (int step = 0; step < N; step++) {
        int u = -1;
        double best = DBL_MAX;

        for (int i = 0; i < N; i++) {
            if (!visited[i] && dist[i] < best) {
                best = dist[i];
                u = i;
            }
        }

        if (u == -1)
            break;

        visited[u] = 1;

        if (u == destination)
            break;

        for (int r = 0; r < M; r++) {
            if (!roads[r].open)
                continue;

            int v = -1;

            if (roads[r].u == u)
                v = roads[r].v;
            else if (roads[r].v == u)
                v = roads[r].u;

            if (v == -1 || visited[v])
                continue;

            double weight = roads[r].time * roads[r].traffic;
            double newTime = dist[u] + weight;

            if (newTime < dist[v]) {
                dist[v] = newTime;
                previous[v] = u;
            }
        }
    }

    route.clear();
    found = false;
    totalKm = 0;
    totalTime = 0;

    if (source == destination) {
        route.push_back(source);
        found = true;
    }
    else if (dist[destination] != DBL_MAX) {
        int current = destination;

        while (current != -1) {
            route.push_back(current);

            if (current == source)
                break;

            current = previous[current];
        }

        if (!route.empty() && route.back() == source) {
            reverse(route.begin(), route.end());
            found = true;
        }
        else {
            route.clear();
        }
    }

    string report;

    if (found) {
        totalTime = dist[destination];

        for (int i = 0; i + 1 < (int)route.size(); i++) {
            int r = roadIndex(route[i], route[i + 1]);

            if (r >= 0)
                totalKm += roads[r].km;
        }

        report = "OPTIMIZED ROUTE\n\n";

        for (int i = 0; i < (int)route.size(); i++) {
            if (i > 0)
                report += " -> ";

            report += cities[route[i]].name;
        }

        report += "\n\nDistance: " + number(totalKm) + " km";
        report += "\nTravel time: " + number(totalTime) + " minutes";
        report += "\n\nROAD CONDITIONS\n";

        bool congestion = false;

        for (int r = 0; r < M; r++) {
            if (!roads[r].open || roads[r].traffic >= 1.7) {
                report += string(cities[roads[r].u].name) + " - ";
                report += cities[roads[r].v].name;

                if (roads[r].open)
                    report += " : Congestion\n";
                else
                    report += " : CLOSED\n";

                congestion = true;
            }
        }

        if (!congestion)
            report += "All roads operating normally.\n";

        SetWindowTextA(
            distanceLabel,
            ("Distance: " + number(totalKm) + " km").c_str()
        );

        SetWindowTextA(
            timeLabel,
            ("Estimated time: " + number(totalTime) + " min").c_str()
        );
    }
    else {
        report =
            "No route available.\r\n"
            "A road closure may have disconnected the locations.";

        SetWindowTextA(distanceLabel, "Distance: unavailable");
        SetWindowTextA(timeLabel, "Travel time: unavailable");
    }

    SetWindowTextA(outputBox, report.c_str());
    InvalidateRect(mainWindow, NULL, TRUE);
}

void drawMap(HDC dc) {
    RECT rect;
    GetClientRect(mainWindow, &rect);

    HBRUSH bg = CreateSolidBrush(RGB(247, 250, 252));

    RECT area = {10, 65, 660, rect.bottom - 10};

    FillRect(dc, &area, bg);
    DeleteObject(bg);

    int px[N], py[N];

    for (int i = 0; i < N; i++) {
        px[i] = 35 + cities[i].x;
        py[i] = 85 + cities[i].y;
    }

    // Draw roads.
    for (int r = 0; r < M; r++) {
        COLORREF color = RGB(180, 195, 210);
        int width = 2;

        if (!roads[r].open) {
            color = RGB(210, 40, 40);
            width = 3;
        }
        else if (isRouteRoad(roads[r].u, roads[r].v)) {
            color = RGB(0, 110, 240);
            width = 5;
        }
        else if (roads[r].traffic >= 1.7) {
            color = RGB(240, 130, 20);
            width = 3;
        }

        HPEN pen = CreatePen(
            roads[r].open ? PS_SOLID : PS_DASH,
            width,
            color
        );

        HPEN oldPen = (HPEN)SelectObject(dc, pen);

        MoveToEx(dc, px[roads[r].u], py[roads[r].u], NULL);
        LineTo(dc, px[roads[r].v], py[roads[r].v]);

        SelectObject(dc, oldPen);
        DeleteObject(pen);

        char km[16];
        sprintf(km, "%.1f", roads[r].km);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(70, 80, 95));

        int mx = (px[roads[r].u] + px[roads[r].v]) / 2;
        int my = (py[roads[r].u] + py[roads[r].v]) / 2;

        TextOutA(dc, mx, my, km, (int)strlen(km));
    }

    // Draw city nodes and names.
    for (int i = 0; i < N; i++) {
        COLORREF color = RGB(255, 255, 255);

        if (i == source)
            color = RGB(100, 235, 145);
        else if (i == destination)
            color = RGB(190, 150, 245);

        HBRUSH brush = CreateSolidBrush(color);
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(60, 80, 100));

        HBRUSH oldBrush = (HBRUSH)SelectObject(dc, brush);
        HPEN oldPen = (HPEN)SelectObject(dc, pen);

        Ellipse(dc, px[i] - 9, py[i] - 9,
                px[i] + 9, py[i] + 9);

        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);

        DeleteObject(brush);
        DeleteObject(pen);

        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(20, 35, 55));

        TextOutA(
            dc,
            px[i] - 35,
            py[i] + 12,
            cities[i].name,
            (int)strlen(cities[i].name)
        );
    }

    const char* legend =
        "Blue: route | Orange: congestion | Red dashed: closed road";

    SetTextColor(dc, RGB(50, 60, 75));

    TextOutA(
        dc, 20, rect.bottom - 28,
        legend, (int)strlen(legend)
    );
}

LRESULT CALLBACK WindowProcedure(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp
) {
    switch (msg) {
    case WM_CREATE: {
        mainWindow = hwnd;

        HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        HWND heading = CreateWindowA(
            "STATIC",
            "SMART URBAN ROUTE PLANNING SYSTEM",
            WS_CHILD | WS_VISIBLE,
            20, 15, 650, 30,
            hwnd, NULL, NULL, NULL
        );

        HWND lblStart = CreateWindowA(
            "STATIC", "Starting location:",
            WS_CHILD | WS_VISIBLE,
            680, 85, 250, 22,
            hwnd, NULL, NULL, NULL
        );

        startBox = CreateWindowA(
            "COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            680, 108, 290, 220,
            hwnd, (HMENU)(INT_PTR)101, NULL, NULL
        );

        HWND lblEnd = CreateWindowA(
            "STATIC", "Destination:",
            WS_CHILD | WS_VISIBLE,
            680, 145, 250, 22,
            hwnd, NULL, NULL, NULL
        );

        endBox = CreateWindowA(
            "COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
            680, 168, 290, 220,
            hwnd, (HMENU)(INT_PTR)102, NULL, NULL
        );

        for (int i = 0; i < N; i++) {
            SendMessageA(
                startBox, CB_ADDSTRING, 0,
                (LPARAM)cities[i].name
            );

            SendMessageA(
                endBox, CB_ADDSTRING, 0,
                (LPARAM)cities[i].name
            );
        }

        SendMessageA(startBox, CB_SETCURSEL, 0, 0);
        SendMessageA(endBox, CB_SETCURSEL, 4, 0);

        HWND btnFind = CreateWindowA(
            "BUTTON", "Find Best Route",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            680, 210, 290, 32,
            hwnd, (HMENU)(INT_PTR)201, NULL, NULL
        );

        HWND btnSimulate = CreateWindowA(
            "BUTTON", "Simulate Traffic",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            680, 250, 290, 32,
            hwnd, (HMENU)(INT_PTR)202, NULL, NULL
        );

        HWND btnReset = CreateWindowA(
            "BUTTON", "Reset Traffic",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            680, 290, 290, 32,
            hwnd, (HMENU)(INT_PTR)203, NULL, NULL
        );

        distanceLabel = CreateWindowA(
            "STATIC", "Distance: --",
            WS_CHILD | WS_VISIBLE,
            680, 340, 290, 25,
            hwnd, NULL, NULL, NULL
        );

        timeLabel = CreateWindowA(
            "STATIC", "Travel time: --",
            WS_CHILD | WS_VISIBLE,
            680, 370, 290, 25,
            hwnd, NULL, NULL, NULL
        );

        outputBox = CreateWindowA(
            "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL |
            ES_READONLY | WS_VSCROLL,
            680, 405, 290, 230,
            hwnd, NULL, NULL, NULL
        );

        HWND controls[] = {
            heading, lblStart, startBox, lblEnd, endBox,
            btnFind, btnSimulate, btnReset,
            distanceLabel, timeLabel, outputBox
        };

        for (int i = 0; i < 11; i++) {
            SendMessageA(
                controls[i], WM_SETFONT,
                (WPARAM)font, TRUE
            );
        }

        resetTraffic();
        calculateRoute();

        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 201:
            calculateRoute();
            break;

        case 202:
            simulateTraffic();
            calculateRoute();
            break;

        case 203:
            resetTraffic();
            calculateRoute();
            break;

        case 101:
        case 102:
            if (HIWORD(wp) == CBN_SELCHANGE)
                calculateRoute();
            break;
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);

        drawMap(dc);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(
    HINSTANCE instance, HINSTANCE, LPSTR, int show
) {
    const char CLASS_NAME[] = "UrbanRouteWindow";

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClassA(&wc))
        return 1;

    HWND hwnd = CreateWindowA(
        CLASS_NAME,
        "Smart Urban Route Planning",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1020, 720,
        NULL, NULL, instance, NULL
    );

    if (!hwnd)
        return 1;

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg;

    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}