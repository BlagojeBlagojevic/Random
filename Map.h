
#ifndef _MAP_H
#define _MAP_H
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct
{
    char** walling;
    int h;
    int w;
}
Map;


Map xmgen(const int w, const int h, const int grid, const int max);
Map xmgen_graph(const int w, const int h, const int num_rooms, const int min_size, const int max_size, const int extra_connections);


//Map xmgen_bsp(const int w, const int h, const int min_room_size);


Map xmgen_drunk(const int w, const int h, float floor_goal_percent);

//Map xmgen_cellular(const int w, const int h, const float wall_percent, const int iterations);

Map xmgen_brogue(const int w, const int h, const int max_rooms, const int min_size, const int max_size);


Map xmgen_maze(const int wR, const int hR, const int w, const int h);

Map xmgen_room_maze(const int wR, const int hR, const int w, const int h, const int num_rooms_to_try, const int min_room_size, const int max_room_size);


Map xmgen_modules(const int w, const int h, const int num_modules);



void xmclose(const Map);

void xmprint(const Map);








#endif

#define toss(t, n) ((t*) malloc((n) * sizeof(t)))
#define zero(a) (memset(&(a), 0, sizeof(a)))

typedef int (*const Direction)(const void*, const void*);

typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    Point* point;
    int count;
    int max;
} Points;

typedef struct {
    Point a;
    Point b;
    Point c;
} Tri;

typedef struct {
    Tri* tri;
    int count;
    int max;
} Tris;

typedef struct {
    Point zer;
    Point one;
} Flags;



typedef struct {
    char** tiles;
    int w;
    int h;
    // N, E, S, W doors
    int door_x[4];
    int door_y[4];
} BrogueRoom;





static void bomb(const char* const message, ...) {
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    exit(1);
}

static char** reset(char** block, const int h, const int w, const int blok) {
    for (int row = 0; row < h; row++)
        for (int col = 0; col < w; col++)
            block[row][col] = blok;
    return block;
}

static char** bnew(const int h, const int w, const int blok) {
    char** block = toss(char*, h);
    for (int row = 0; row < h; row++)
        block[row] = toss(char, w);
    return reset(block, h, w, blok);
}

static Map mnew(const int h, const int w) {
    Map map;
    zero(map);
    map.h = h;
    map.w = w;
    map.walling = bnew(map.h, map.w, '#');
    return map;
}

static int psame(const Point a, const Point b) {
    return a.x == b.x && a.y == b.y;
}

static int fl(const float x) {
    return (int)x - (x < (int)x);
}

static Point snap(const Point a, const int grid) {
    const Point out = {
        (float)fl(a.x / grid) * grid,
        (float)fl(a.y / grid) * grid,
    };
    return out;
}

static Points psnew(const int max) {
    const Points ps = { toss(Point, max), 0, max };
    return ps;
}

static Points psadd(Points ps, const Point p) {
    if (ps.count == ps.max)
        bomb("points size limitation reached\n");
    ps.point[ps.count++] = p;
    return ps;
}

static int psfind(const Points ps, const Point p) {
    for (int i = 0; i < ps.count; i++)
        if (psame(ps.point[i], p))
            return true;
    return false;
}

static Tris tsnew(const int max) {
    const Tris ts = { toss(Tri, max), 0, max };
    return ts;
}

static Tris tsadd(Tris tris, const Tri tri) {
    if (tris.count == tris.max)
        bomb("tris size limitation reached\n");
    tris.tri[tris.count++] = tri;
    return tris;
}

static int reveql(const Tri a, const Tri b) {
    return psame(a.a, b.b) && psame(a.b, b.a);
}

static int foreql(const Tri a, const Tri b) {
    return psame(a.a, b.a) && psame(a.b, b.b);
}

static int alligned(const Tri a, const Tri b) {
    return foreql(a, b) || reveql(a, b);
}

static void emark(Tris edges, const Flags flags) {
    for (int i = 0; i < edges.count; i++) {
        const Tri edge = edges.tri[i];
        for (int j = 0; j < edges.count; j++) {
            if (i == j)
                continue;
            const Tri other = edges.tri[j];
            if (alligned(edge, other))
                edges.tri[j].c = flags.one;
        }
    }
}

static Tris ejoin(Tris tris, const Tris edges, const Point p, const Flags flags) {
    for (int j = 0; j < edges.count; j++) {
        const Tri edge = edges.tri[j];
        if (psame(edge.c, flags.zer)) {
            const Tri tri = { edge.a, edge.b, p };
            tris = tsadd(tris, tri);
        }
    }
    return tris;
}

static int outob(const Point p, const int w, const int h) {
    return p.x < 0 || p.y < 0 || p.x >= w || p.y >= h;
}

static int incircum(const Tri t, const Point p) {
    const float ax = t.a.x - p.x;
    const float ay = t.a.y - p.y;
    const float bx = t.b.x - p.x;
    const float by = t.b.y - p.y;
    const float cx = t.c.x - p.x;
    const float cy = t.c.y - p.y;
    const float det =
        (ax * ax + ay * ay) * (bx * cy - cx * by) -
        (bx * bx + by * by) * (ax * cy - cx * ay) +
        (cx * cx + cy * cy) * (ax * by - bx * ay);
    return det > 0.0f;
}

static Tris ecollect(Tris edges, const Tris in, const Flags flags) {
    for (int i = 0; i < in.count; i++) {
        const Tri tri = in.tri[i];
        const Tri ab = { tri.a, tri.b, flags.zer };
        const Tri bc = { tri.b, tri.c, flags.zer };
        const Tri ca = { tri.c, tri.a, flags.zer };
        edges = tsadd(edges, ab);
        edges = tsadd(edges, bc);
        edges = tsadd(edges, ca);
    }
    return edges;
}

static Tris delaunay(const Points ps, const int w, const int h, const int max, const Flags flags) {
    Tris in = tsnew(max);
    Tris out = tsnew(max);
    Tris tris = tsnew(max);
    Tris edges = tsnew(max);
    Tri* dummy = tris.tri;
    const Tri super = { { (float)-w, 0.0f }, { 2.0f * w, 0.0f }, { w / 2.0f, 2.0f * h } };
    tris = tsadd(tris, super);
    for (int j = 0; j < ps.count; j++) {
        in.count = out.count = edges.count = 0;
        const Point p = ps.point[j];
        for (int i = 0; i < tris.count; i++) {
            const Tri tri = tris.tri[i];
            if (incircum(tri, p))
                in = tsadd(in, tri);
            else out = tsadd(out, tri);
        }
        edges = ecollect(edges, in, flags);
        emark(edges, flags);
        out = ejoin(out, edges, p, flags);
        tris = out;
    }
    free(dummy);
    free(in.tri);
    free(edges.tri);
    return tris;
}

static Points prand(const int w, const int h, const int max, const int grid, const int border) {
    Points ps = psnew(max);
    for (int i = ps.count; i < ps.max; i++) {
        const Point p = {
            (float)(rand() % (w - border) + border / 2),
            (float)(rand() % (h - border) + border / 2),
        };
        const Point snapped = snap(p, grid);
        ps = psadd(ps, snapped);
    }
    return ps;
}

static Point sub(const Point a, const Point b) {
    const Point out = { a.x - b.x, a.y - b.y };
    return out;
}

static float mag(const Point a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

static float len(const Tri edge) {
    return mag(sub(edge.b, edge.a));
}

static int descending(const void* a, const void* b) {
    const Tri ea = *(const Tri*)a;
    const Tri eb = *(const Tri*)b;
    return len(ea) < len(eb) ? 1 : len(ea) > len(eb) ? -1 : 0;
}

static void sort(const Tris edges, const Direction direction) {
    qsort(edges.tri, edges.count, sizeof(Tri), direction);
}

static int connected(const Point a, const Point b, const Tris edges, const Flags flags) {
    Points todo = psnew(edges.max);
    Points done = psnew(edges.max);
    Tris reach = tsnew(edges.max);
    todo = psadd(todo, a);
    int connection = false;
    while (todo.count != 0 && connection != true) {
        const Point removed = todo.point[--todo.count];
        done = psadd(done, removed);
        reach.count = 0;
        for (int i = 0; i < edges.count; i++) {
            const Tri edge = edges.tri[i];
            if (psame(edge.c, flags.one))
                continue;
            if (psame(edge.a, removed))
                reach = tsadd(reach, edge);
        }
        for (int i = 0; i < reach.count; i++) {
            if (psame(reach.tri[i].b, b)) {
                connection = true;
                break;
            }
            if (!psfind(done, reach.tri[i].b))
                todo = psadd(todo, reach.tri[i].b);
        }
    }
    free(todo.point);
    free(reach.tri);
    free(done.point);
    return connection;
}

static void revdel(Tris edges, const int w, const int h, const Flags flags) {
    sort(edges, descending);
    for (int i = 0; i < edges.count; i++) {
        Tri* edge = &edges.tri[i];
        if (outob(edge->a, w, h)
            || outob(edge->b, w, h)) {
            edge->c = flags.one;
            continue;
        }
        edge->c = flags.one;
        if (!connected(edge->a, edge->b, edges, flags)) edge->c = flags.zer;
    }
}

static void mdups(const Tris edges, const Flags flags) {
    for (int i = 0; i < edges.count; i++)
        for (int j = 0; j < edges.count; j++) {
            if (psame(edges.tri[j].c, flags.one))
                continue;
            if (psame(edges.tri[i].c, flags.one))
                continue;
            if (reveql(edges.tri[i], edges.tri[j]))
                edges.tri[j].c = flags.one;
        }
}

static void mroom(const Map map, const Point where, const int w, const int h) {
    for (int i = -w; i <= w; i++)
        for (int j = -h; j <= h; j++) {
            const int xx = (int)where.x + i;
            const int yy = (int)where.y + j;
            if (yy >= 0 && yy < map.h && xx >= 0 && xx < map.w) {
                map.walling[yy][xx] = ' ';
            }
        }
}

static void mcorridor(const Map map, const Point a, const Point b) {
    const Point step = sub(b, a);
    const Point delta = {
        step.x > 0.0f ? 1.0f : step.x < 0.0f ? -1.0f : 0.0f,
        step.y > 0.0f ? 1.0f : step.y < 0.0f ? -1.0f : 0.0f,
    };
    const int sx = (int)fabsf(step.x);
    const int sy = (int)fabsf(step.y);
    const int dx = (int)delta.x;
    const int dy = (int)delta.y;
    int x = (int)a.x;
    int y = (int)a.y;
    for (int i = 0; i < sx; i++) {
        x += dx;
        if (y >= 0 && y < map.h && x >= 0 && x < map.w) {
            map.walling[y][x] = '+';
        }
    }
    for (int i = 0; i < sy; i++) {
        y += dy;
        if (y >= 0 && y < map.h && x >= 0 && x < map.w) {
            map.walling[y][x] = '+';
        }
    }
    if (y >= 0 && y < map.h && x >= 0 && x < map.w) {
        map.walling[y][x] = '*';
    }
}

static void mpillar(const Map map, const Point where, const int w, const int h) {
    int type = rand() % 3;
    if (type == 1) {
        int numPIllar = rand() % 10 + 2;
        for (int i = 0; i < numPIllar; i++) {
            int xx = (int)where.x + rand() % w;
            int yy = (int)where.y + rand() % h;
            if (yy >= 0 && yy < map.h && xx >= 0 && xx < map.w) {
                if (map.walling[yy][xx] == ' ') {
                    map.walling[yy][xx] = '#';
                }
            }
        }
    } else if (type == 2) {
        const int xx = (int)where.x + w / 2;
        const int yy = (int)where.y + h / 2;
        if (yy + 1 < map.h && xx + 1 < map.w && yy >= 0 && xx >= 0) {
            map.walling[yy][xx] = '#';
            map.walling[yy + 1][xx] = '#';
            map.walling[yy][xx + 1] = '#';
            map.walling[yy + 1][xx + 1] = '#';
        }
    }
}

static void bone(const Map map, const Tri e, const int w, const int h) {
    mroom(map, e.a, w, h);
    mpillar(map, e.a, w, h);
    mroom(map, e.b, w, h);
    mpillar(map, e.b, w, h);
    mcorridor(map, e.a, e.b);
}

static void carve(const Map map, const Tris edges, const Flags flags, const int grid) {
    for (int i = 0; i < edges.count; i++) {
        const Tri e = edges.tri[i];
        if (psame(e.c, flags.one))
            continue;
        const int min = 2;
        const int size = grid / 2 - min;
        const int w = min + rand() % (size > 0 ? size : 1);
        const int h = min + rand() % (size > 0 ? size : 1);
        bone(map, e, w, h);
    }
}

Map xmgen(const int w, const int h, const int grid, const int max) {
    srand((unsigned)time(0));
    const Flags flags = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
    const int border = 3 * grid;
    const Points ps = prand(w, h, max, grid, border);
    const Tris tris = delaunay(ps, w, h, 9 * max, flags);
    const Tris edges = ecollect(tsnew(27 * max), tris, flags);
    revdel(edges, w, h, flags);
    const Map map = mnew(h, w);
    mdups(edges, flags);
    carve(map, edges, flags, grid);
    free(tris.tri);
    free(ps.point);
    free(edges.tri);
    return map;
}

void xmclose(const Map map) {
    for (int row = 0; row < map.h; row++)
        free(map.walling[row]);
    free(map.walling);
}

void xmprint(const Map map) {
    for (int row = 0; row < map.h; row++)
        for (int col = 0; col < map.w; col++)
            printf("%c%s", map.walling[row][col], col == map.w - 1 ? "\n" : "");
    putchar('\n');
}

/* ===================== Rooms & BSP connectivity utils ===================== */

typedef struct {
    int x, y, w, h;
} Rect;
static Rect* room_list = NULL;
static int room_count = 0;

static void carve_room(Map map, Rect r) {
    int room_x = r.x + rand() % (r.w / 4 + (r.w/4==0));
    int room_y = r.y + rand() % (r.h / 4 + (r.h/4==0));
    int room_w = r.w - (room_x - r.x) * 5;
    int room_h = r.h - (room_y - r.y) * 5;
    if (room_w < 3 || room_h < 3) return;
    for (int y = room_y; y < room_y + room_h; y++) {
        for (int x = room_x; x < room_x + room_w; x++) {
            if (y > 0 && y < map.h - 1 && x > 0 && x < map.w - 1) {
                map.walling[y][x] = ' ';
            }
        }
    }
    room_list[room_count++] = (Rect){ room_x, room_y, room_w, room_h };
}

static void create_corridor(Map map, int x1, int y1, int x2, int y2) {
    int x = x1, y = y1;
    while (x != x2 || y != y2) {
        if (abs(x - x2) > abs(y - y2) && x != x2) {
            if (x < x2) x++; else x--;
        } else if (y != y2) {
            if (y < y2) y++; else y--;
        }
        if (y >= 0 && y < map.h && x >= 0 && x < map.w && map.walling[y][x] == '#') {
            map.walling[y][x] = '+';
        } else if (y >= 0 && y < map.h && x >= 0 && x < map.w && map.walling[y][x] == '+') {
            // keep corridor
        } else if (y >= 0 && y < map.h && x >= 0 && x < map.w && map.walling[y][x] == ' ') {
            // already floor, ok
        }
    }
}

/* ---------- New: generic room connectivity pass (nearest-first) ---------- */

static inline void room_center(Rect r, int* cx, int* cy) {
    *cx = r.x + r.w / 2;
    *cy = r.y + r.h / 2;
}

static inline int room_distance2(Rect a, Rect b) {
    int ax, ay, bx, by;
    room_center(a, &ax, &ay);
    room_center(b, &bx, &by);
    int dx = ax - bx, dy = ay - by;
    return dx * dx + dy * dy;
}

/* Connect all rooms in the given list to ensure full reachability */
static void connect_rooms(Map map, Rect* rooms, int count) {
    if (count <= 1) return;

    bool* connected_flags = (bool*)calloc(count, sizeof(bool));
    connected_flags[0] = true;
    int connected_num = 1;

    while (connected_num < count) {
        int bestA = -1, bestB = -1, bestDist2 = 2147483647;

        for (int i = 0; i < count; i++) if (connected_flags[i]) {
            for (int j = 0; j < count; j++) if (!connected_flags[j]) {
                int d2 = room_distance2(rooms[i], rooms[j]);
                if (d2 < bestDist2) {
                    bestDist2 = d2;
                    bestA = i;
                    bestB = j;
                }
            }
        }

        if (bestA == -1 || bestB == -1) break;

        int x1, y1, x2, y2;
        room_center(rooms[bestA], &x1, &y1);
        room_center(rooms[bestB], &x2, &y2);
        create_corridor(map, x1, y1, x2, y2);

        connected_flags[bestB] = true;
        connected_num++;
    }

    free(connected_flags);
}

/* ------------------------------ BSP generator ---------------------------- */

static void bsp_split(Map map, Rect r, int min_size) {
    if (r.w < min_size * 2 && r.h < min_size * 2) {
        carve_room(map, r);
        return;
    }
    int split_vertical = rand() % 2;
    if (r.w > r.h * 1.5) split_vertical = 1;
    if (r.h > r.w * 1.5) split_vertical = 0;
    if (split_vertical) {
        if (r.w < min_size * 2) {
            carve_room(map, r);
            return;
        }
        int split_x = r.x + (r.w / 4) + rand() % (r.w / 2);
        Rect r1 = { r.x, r.y, split_x - r.x, r.h };
        Rect r2 = { split_x, r.y, r.w - (split_x - r.x), r.h };
        bsp_split(map, r1, min_size);
        bsp_split(map, r2, min_size);
    } else {
        if (r.h < min_size * 2) {
            carve_room(map, r);
            return;
        }
        int split_y = r.y + (r.h / 4) + rand() % (r.h / 2);
        Rect r1 = { r.x, r.y, r.w, split_y - r.y };
        Rect r2 = { r.x, split_y, r.w, r.h - (split_y - r.y) };
        bsp_split(map, r1, min_size);
        bsp_split(map, r2, min_size);
    }
}

Map xmgen_bsp(const int w, const int h, const int min_room_size) {
    srand(time(0));
    Map map = mnew(h, w);
    room_list = toss(Rect, (w * h) / (min_room_size * min_room_size));
    room_count = 0;
    Rect initial_rect = { 1, 1, w - 2, h - 2 };
    bsp_split(map, initial_rect, min_room_size);
    for (int i = 0; i < room_count - 1; i++) {
        Rect r1 = room_list[i];
        Rect r2 = room_list[i + 1];
        int x1 = r1.x + r1.w / 2;
        int y1 = r1.y + r1.h / 2;
        int x2 = r2.x + r2.w / 2;
        int y2 = r2.y + r2.h / 2;
        create_corridor(map, x1, y1, x2, y2);
    }
    free(room_list);
    room_list = NULL;
    room_count = 0;
    return map;
}

/* ===================== BSP with Leaf Node Rooms ===================== */

static void bsp_split_and_room(Map map, Rect r, int max_size, int min_size) {
    // Stop splitting if the region is too small
    if (r.w < min_size * 2 || r.h < min_size * 2) {
        // Create a room in this leaf node
        int rw = min_size + rand() % (max_size - min_size);
        int rh = min_size + rand() % (max_size - min_size);
        int rx = r.x + rand() % (r.w - rw + 1);
        int ry = r.y + rand() % (r.h - rh + 1);
        
        for (int y = ry; y < ry + rh; y++) {
            for (int x = rx; x < rx + rw; x++) {
                if (x >= 0 && x < map.w && y >= 0 && y < map.h) {
                    map.walling[y][x] = ' ';
                }
            }
        }
        return;
    }

    bool split_h = (rand() % 2 == 0);
    if (r.w > r.h && r.w / r.h >= 1.2f) {
        split_h = false;
    } else if (r.h > r.w && r.h / r.w >= 1.2f) {
        split_h = true;
    }

    if (split_h) {
        int split_point = r.h / 4 + rand() % (r.h / 2);
        Rect r1 = {r.x, r.y, r.w, split_point};
        Rect r2 = {r.x, r.y + split_point, r.w, r.h - split_point};
        bsp_split_and_room(map, r1, max_size, min_size);
        bsp_split_and_room(map, r2, max_size, min_size);
    } else {
        int split_point = r.w / 4 + rand() % (r.w / 2);
        Rect r1 = {r.x, r.y, split_point, r.h};
        Rect r2 = {r.x + split_point, r.y, r.w - split_point, r.h};
        bsp_split_and_room(map, r1, max_size, min_size);
        bsp_split_and_room(map, r2, max_size, min_size);
    }
}

Map xmgen_bsp_leaf(const int w, const int h, const int max_size,  const int min_size) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);

    Rect root_rect = {0, 0, w, h};
    bsp_split_and_room(map, root_rect, max_size, min_size);

    return map;
}
/* ===================== Prefabricated Module Generator ===================== */

typedef struct {
    char** pattern;
    int w, h;
} Module;

// Example modules (in a real game you would load these from a file)
Module create_example_module(int id) {
    if (id == 0) { // Small room
        Module m = {toss(char*, 5), 5, 5};
        m.pattern[0] = strdup("#####");
        m.pattern[1] = strdup("#   #");
        m.pattern[2] = strdup("#   #");
        m.pattern[3] = strdup("#   #");
        m.pattern[4] = strdup("#####");
        return m;
    } else if (id == 1) { // L-corridor
        Module m = {toss(char*, 4), 4, 4};
        m.pattern[0] = strdup("# # ");
        m.pattern[1] = strdup("# # ");
        m.pattern[2] = strdup("# # ");
        m.pattern[3] = strdup("### ");
        return m;
    }
    
    return (Module){NULL, 0, 0};
}
//Tbd conect
Map xmgen_modules(const int w, const int h, const int num_modules) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    
    reset(map.walling, h, w, '#');

    int current_x = w / 2;
    int current_y = h / 2;
    
    for (int i = 0; i < num_modules; i++) {
        Module m = create_example_module(rand() % 2);

        // Place the module
        int start_x = current_x - m.w / 2;
        int start_y = current_y - m.h / 2;
        for (int y = 0; y < m.h; y++) {
            for (int x = 0; x < m.w; x++) {
                int map_x = start_x + x;
                int map_y = start_y + y;
                if (map_x >= 0 && map_x < w && map_y >= 0 && map_y < h) {
                    if (m.pattern[y][x] == ' ') {
                        map.walling[map_y][map_x] = ' ';
                    }
                }
            }
        }
        
        current_x += (rand() % 2 == 0 ? 1 : -1) * (m.w / 2 + rand() % 5);
        current_y += (rand() % 2 == 0 ? 1 : -1) * (m.h / 2 + rand() % 5);
        if (current_x < 0) current_x = 0;
        if (current_y < 0) current_y = 0;
        if (current_x >= w) current_x = w - 1;
        if (current_y >= h) current_y = h - 1;

        for(int j = 0; j < m.h; j++) {
            free(m.pattern[j]);
        }
        free(m.pattern);
    }
    
    return map;
}




/* ===================== RCD Generator ===================== */

Map xmgen_rcd(const int w, const int h, const int num_rooms, const int min_size, const int max_size) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);

    Rect* rooms = toss(Rect, num_rooms);
    int room_count = 0;
    int attempts = 0;
    while (room_count < num_rooms && attempts < num_rooms * 10) {
        Rect r;
        r.w = min_size + rand() % (max_size - min_size + 1);
        r.h = min_size + rand() % (max_size - min_size + 1);
        r.x = 1 + rand() % (w - r.w - 1);
        r.y = 1 + rand() % (h - r.h - 1);

        bool overlap = false;
        for (int i = 0; i < room_count; i++) {
            Rect existing = rooms[i];
            // Check for overlap, with a 1-tile buffer
            if (r.x < existing.x + existing.w + 1 && r.x + r.w + 1 > existing.x &&
                r.y < existing.y + existing.h + 1 && r.y + r.h + 1 > existing.y) {
                overlap = true;
                break;
            }
        }

        if (!overlap) {
            rooms[room_count++] = r;
        }
        attempts++;
    }

    for (int i = 0; i < room_count; i++) {
        Rect r = rooms[i];
        for (int y = r.y; y < r.y + r.h; y++) {
            for (int x = r.x; x < r.x + r.w; x++) {
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    map.walling[y][x] = ' ';
                }
            }
        }
    }

    for (int i = 0; i < room_count - 1; i++) {
        int x1, y1, x2, y2;
        room_center(rooms[i], &x1, &y1);
        room_center(rooms[i+1], &x2, &y2);
        create_corridor(map, x1, y1, x2, y2);
    }
    
    for (int i = 0; i < room_count - 1; i++) {
        int x1, y1, x2, y2;
        room_center(rooms[i], &x1, &y1);
        room_center(rooms[i+1], &x2, &y2);
        
        int mid_x = (x1 + x2) / 2;
        int mid_y = (y1 + y2) / 2;

        if (mid_x > 0 && mid_x < w-1 && mid_y > 0 && mid_y < h-1) {
            if (map.walling[mid_y][mid_x] == ' ') {
                if (map.walling[mid_y][mid_x-1] == '#' && map.walling[mid_y][mid_x+1] == '#') {
                    map.walling[mid_y][mid_x] = '+'; // Horizontal door
                } else if (map.walling[mid_y-1][mid_x] == '#' && map.walling[mid_y+1][mid_x] == '#') {
                    map.walling[mid_y][mid_x] = '+'; // Vertical door
                }
            }
        }
    }

    free(rooms);
    return map;
}


/* ===================== Subtractive Generator ===================== */

Map xmgen_subtractive(const int w, const int h, const int carve_count) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    
    // Start with a fully walled map
    reset(map.walling, h, w, '#');

    for (int i = 0; i < carve_count; i++) {
        // Start a random walk from a random point
        int x = rand() % w;
        int y = rand() % h;

        for (int j = 0; j < 500; j++) {
            if (x >= 0 && x < w && y >= 0 && y < h) {
                map.walling[y][x] = ' ';
            }

            int dir = rand() % 4;
            if (dir == 0) x++;
            else if (dir == 1) x--;
            else if (dir == 2) y++;
            else if (dir == 3) y--;
        }
    }
    
    Map new_map = mnew(h, w);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int walls = 0;
            if (y > 0 && map.walling[y-1][x] == '#') walls++;
            if (y < h-1 && map.walling[y+1][x] == '#') walls++;
            if (x > 0 && map.walling[y][x-1] == '#') walls++;
            if (x < w-1 && map.walling[y][x+1] == '#') walls++;

            if (map.walling[y][x] == '#' && walls >= 4) {
                new_map.walling[y][x] = '#';
            } else {
                new_map.walling[y][x] = ' ';
            }
        }
    }
    xmclose(map);
    return new_map;
}


/* ===================== Voronoi/Delaunay Generator ===================== */

static int get_closest_seed(Point p, const Points ps) {
    int closest_idx = -1;
    float min_dist_sq = INFINITY;

    for (int i = 0; i < ps.count; i++) {
        float dx = p.x - ps.point[i].x;
        float dy = p.y - ps.point[i].y;
        float dist_sq = dx * dx + dy * dy;
        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest_idx = i;
        }
    }
    return closest_idx;
}

Map xmgen_voronoi(const int w, const int h, const int num_seeds, const int min_room_size, const int max_room_size) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    
    Points ps = psnew(num_seeds);
    for (int i = 0; i < num_seeds; i++) {
        Point p = {(float)(rand() % w), (float)(rand() % h)};
        ps = psadd(ps, p);
    }

    int* cell_map = (int*)malloc(w * h * sizeof(int));
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            cell_map[y * w + x] = get_closest_seed((Point){(float)x, (float)y}, ps);
        }
    }

    Rect* rooms = toss(Rect, num_seeds);
    int room_count = 0;
    for (int i = 0; i < num_seeds; i++) {
        int min_x = w, max_x = 0, min_y = h, max_y = 0;
        bool found_cell = false;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (cell_map[y * w + x] == i) {
                    if (x < min_x) min_x = x;
                    if (x > max_x) max_x = x;
                    if (y < min_y) min_y = y;
                    if (y > max_y) max_y = y;
                    found_cell = true;
                }
            }
        }
        
        if (found_cell) {
            int room_w = min_room_size + rand() % (max_room_size - min_room_size + 1);
            int room_h = min_room_size + rand() % (max_room_size - min_room_size + 1);
            int room_x = min_x + (max_x - min_x) / 2 - room_w / 2;
            int room_y = min_y + (max_y - min_y) / 2 - room_h / 2;
            
            // Ensure room is within the cell and map boundaries
            if (room_x < min_x) room_x = min_x;
            if (room_y < min_y) room_y = min_y;
            if (room_x + room_w > max_x) room_x = max_x - room_w;
            if (room_y + room_h > max_y) room_y = max_y - room_h;

            rooms[room_count++] = (Rect){room_x, room_y, room_w, room_h};
            for (int y = room_y; y < room_y + room_h; y++) {
                for (int x = room_x; x < room_x + room_w; x++) {
                    if (y >= 0 && y < h && x >= 0 && x < w) {
                        map.walling[y][x] = ' ';
                    }
                }
            }
        }
    }

    const Flags flags = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
    const Tris tris = delaunay(ps, w, h, 9 * num_seeds, flags);
    const Tris edges = ecollect(tsnew(27 * num_seeds), tris, flags);
    
    for (int i = 0; i < edges.count; i++) {
        const Tri e = edges.tri[i];
        if (psame(e.c, flags.one)) continue;
        
        int x1, y1, x2, y2;
        room_center(rooms[get_closest_seed(e.a, ps)], &x1, &y1);
        room_center(rooms[get_closest_seed(e.b, ps)], &x2, &y2);
        
        create_corridor(map, x1, y1, x2, y2);
    }
    
    free(cell_map);
    free(ps.point);
    free(tris.tri);
    free(edges.tri);
    free(rooms);

    return map;
}

/* ===================== Dungeon-as-Graph Generator ===================== */

// Room struct with connectivity tracking
typedef struct {
    Rect rect;
    bool connected;
} GraphRoom;

Map xmgen_graph(const int w, const int h, const int num_rooms, const int min_size, const int max_size, const int extra_connections) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    
    GraphRoom* rooms = toss(GraphRoom, num_rooms);
    int room_count = 0;
    int attempts = 0;

    while (room_count < num_rooms && attempts < num_rooms * 5) {
        Rect r;
        r.w = min_size + rand() % (max_size - min_size + 1);
        r.h = min_size + rand() % (max_size - min_size + 1);
        r.x = 1 + rand() % (w - r.w - 1);
        r.y = 1 + rand() % (h - r.h - 1);

        bool overlap = false;
        for (int i = 0; i < room_count; i++) {
            Rect existing = rooms[i].rect;
            if (r.x < existing.x + existing.w + 1 && r.x + r.w + 1 > existing.x &&
                r.y < existing.y + existing.h + 1 && r.y + r.h + 1 > existing.y) {
                overlap = true;
                break;
            }
        }

        if (!overlap) {
            rooms[room_count].rect = r;
            rooms[room_count].connected = false;
            room_count++;
        }
        attempts++;
    }
    
    if (room_count > 0) {
        rooms[0].connected = true;
        int connected_count = 1;
        
        while (connected_count < room_count) {
            int best_room_idx = -1;
            int best_neighbor_idx = -1;
            int min_dist_sq = 10000000;

            for (int i = 0; i < room_count; i++) {
                if (!rooms[i].connected) {
                    for (int j = 0; j < room_count; j++) {
                        if (rooms[j].connected) {
                            int x1, y1, x2, y2;
                            room_center(rooms[i].rect, &x1, &y1);
                            room_center(rooms[j].rect, &x2, &y2);
                            int dist_sq = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
                            if (dist_sq < min_dist_sq) {
                                min_dist_sq = dist_sq;
                                best_room_idx = i;
                                best_neighbor_idx = j;
                            }
                        }
                    }
                }
            }

            if (best_room_idx != -1) {
                int x1, y1, x2, y2;
                room_center(rooms[best_room_idx].rect, &x1, &y1);
                room_center(rooms[best_neighbor_idx].rect, &x2, &y2);
                create_corridor(map, x1, y1, x2, y2);
                rooms[best_room_idx].connected = true;
                connected_count++;
            }
        }

        for (int i = 0; i < extra_connections; i++) {
            int room1 = rand() % room_count;
            int room2 = rand() % room_count;
            if (room1 != room2) {
                int x1, y1, x2, y2;
                room_center(rooms[room1].rect, &x1, &y1);
                room_center(rooms[room2].rect, &x2, &y2);
                create_corridor(map, x1, y1, x2, y2);
            }
        }
    }
    
    for (int i = 0; i < room_count; i++) {
        Rect r = rooms[i].rect;
        for (int y = r.y; y < r.y + r.h; y++) {
            for (int x = r.x; x < r.x + r.w; x++) {
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    map.walling[y][x] = ' ';
                }
            }
        }
    }

    free(rooms);
    return map;
}






static int count_wall_neighbors(Map map, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int nx = x + j;
            int ny = y + i;
            if (nx < 0 || ny < 0 || nx >= map.w || ny >= map.h || map.walling[ny][nx] == '#') {
                count++;
            }
        }
    }
    return count;
}

Map xmgen_cellular(const int w, const int h, const float wall_percent, const int iterations) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (x == 0 || x == w - 1 || y == 0 || y == h - 1) {
                map.walling[y][x] = '#';
            } else {
                map.walling[y][x] = ((float)rand() / RAND_MAX) < wall_percent ? '#' : ' ';
            }
        }
    }
    char** buffer_map = bnew(h, w, '#');
    for (int iter = 0; iter < iterations; iter++) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                if (x == 0 || x == w - 1 || y == 0 || y == h - 1) {
                    buffer_map[y][x] = '#';
                    continue;
                }
                int wall_neighbors = count_wall_neighbors(map, x, y);
                if (map.walling[y][x] == '#') {
                    if (wall_neighbors < 4) {
                        buffer_map[y][x] = ' ';
                    } else {
                        buffer_map[y][x] = '#';
                    }
                } else {
                    if (wall_neighbors > 4) {
                        buffer_map[y][x] = '#';
                    } else {
                        buffer_map[y][x] = ' ';
                    }
                }
            }
        }
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                map.walling[y][x] = buffer_map[y][x];
            }
        }
    }
    for (int row = 0; row < map.h; row++)
        free(buffer_map[row]);
    free(buffer_map);
    return map;
}

#define PERLIN_TABLE_SIZE 256
static int p[2 * PERLIN_TABLE_SIZE];

static void init_perlin() {
    int perm[PERLIN_TABLE_SIZE];
    for (int i = 0; i < PERLIN_TABLE_SIZE; i++) perm[i] = i;
    for (int i = 0; i < PERLIN_TABLE_SIZE; i++) {
        int swap_idx = rand() % PERLIN_TABLE_SIZE;
        int temp = perm[i];
        perm[i] = perm[swap_idx];
        perm[swap_idx] = temp;
    }
    for (int i = 0; i < PERLIN_TABLE_SIZE; i++) {
        p[i] = p[i + PERLIN_TABLE_SIZE] = perm[i];
    }
}

static float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
static float lerp(float t, float a, float b) { return a + t * (b - a); }
static float grad(int hash, float x, float y) {
    int h = hash & 3;
    float u = h < 2 ? x : y;
    float v = h < 2 ? y : x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
static float perlin(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floorf(x);
    y -= floorf(y);
    float u = fade(x);
    float v = fade(y);
    int A = p[X] + Y;
    int B = p[X + 1] + Y;
    return lerp(v, lerp(u, grad(p[A], x, y), grad(p[B], x - 1, y)),
        lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1)));
}

Map xmgen_perlin(const int w, const int h, const float threshold) {
    srand((unsigned)time(0));
    init_perlin();
    Map map = mnew(h, w);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float noise_val = perlin((float)x * 0.1f, (float)y * 0.1f);
            if (noise_val > threshold) {
                map.walling[y][x] = ' ';
            }
        }
    }
    return map;
}

/* ----------------------- Region tools for CA shapes ---------------------- */

static void ca_flood_fill(int x, int y, char** tiles, int w, int h, int region_id, int** regions) {
    if (x < 0 || y < 0 || x >= w || y >= h || regions[y][x] != 0 || tiles[y][x] == '#') {
        return;
    }
    regions[y][x] = region_id;
    ca_flood_fill(x + 1, y, tiles, w, h, region_id, regions);
    ca_flood_fill(x - 1, y, tiles, w, h, region_id, regions);
    ca_flood_fill(x, y + 1, tiles, w, h, region_id, regions);
    ca_flood_fill(x, y - 1, tiles, w, h, region_id, regions);
}

static void isolate_largest_region(char** tiles, int w, int h) {
    int** regions = toss(int*, h);
    for (int i = 0; i < h; i++) {
        regions[i] = toss(int, w);
        memset(regions[i], 0, w * sizeof(int));
    }

    int region_id = 1;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (tiles[y][x] == ' ' && regions[y][x] == 0) {
                ca_flood_fill(x, y, tiles, w, h, region_id, regions);
                region_id++;
            }
        }
    }

    if (region_id <= 2) {
        for (int i = 0; i < h; i++) free(regions[i]);
        free(regions);
        return;
    }

    int* region_sizes = (int*)calloc(region_id, sizeof(int));
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (regions[y][x] != 0) {
                region_sizes[regions[y][x]]++;
            }
        }
    }

    int largest_region_id = 0;
    int max_size = 0;
    for (int i = 1; i < region_id; i++) {
        if (region_sizes[i] > max_size) {
            max_size = region_sizes[i];
            largest_region_id = i;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (regions[y][x] != largest_region_id) {
                tiles[y][x] = '#';
            }
        }
    }

    free(region_sizes);
    for (int i = 0; i < h; i++) free(regions[i]);
    free(regions);
}

static void generate_ca_shape(char** tiles, int w, int h) {
    float wall_chance = 0.55f;
    for(int y=0; y<h; y++) for(int x=0; x<w; x++) {
        tiles[y][x] = (rand() % 100 < wall_chance * 100) ? '#' : ' ';
    }

    char** buffer = bnew(h, w, '#');
    for (int i = 0; i < 5; i++) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int neighbors = 0;
                for(int dy=-1; dy<=1; dy++) for(int dx=-1; dx<=1; dx++) {
                    if(dx==0 && dy==0) continue;
                    int nx = x+dx, ny = y+dy;
                    if(nx < 0 || ny < 0 || nx >= w || ny >= h || tiles[ny][nx] == '#') {
                        neighbors++;
                    }
                }
                if (tiles[y][x] == '#') {
                    buffer[y][x] = (neighbors >= 2) ? '#' : ' ';
                } else {
                    buffer[y][x] = (neighbors >= 5) ? '#' : ' ';
                }
            }
        }
        for(int y=0; y<h; y++) memcpy(tiles[y], buffer[y], w * sizeof(char));
    }
    for(int y=0; y<h; y++) free(buffer[y]);
    free(buffer);

    isolate_largest_region(tiles, w, h);
}

/* -------------------------- Brogue-style rooms --------------------------- */

static BrogueRoom create_brogue_room(int min_size, int max_size) {
    BrogueRoom room;
    room.w = min_size + rand() % (max_size - min_size);
    room.h = min_size + rand() % (max_size - min_size);
    room.tiles = bnew(room.h, room.w, '#');

    int type = rand() % 3;
    if (type == 0) {
        for (int y = 1; y < room.h - 1; y++) for (int x = 1; x < room.w - 1; x++) room.tiles[y][x] = ' ';
    } else if (type == 1) {
        int cx = room.w / 2, cy = room.h / 2;
        int radius = (int)fmin((double)cx, (double)cy) - 1;
        for (int y = 0; y < room.h; y++) for (int x = 0; x < room.w; x++) {
            if ((x - cx) * (x - cx) + (y - cy) * (y - cy) <= radius * radius) {
                room.tiles[y][x] = ' ';
            }
        }
    } else {
        generate_ca_shape(room.tiles, room.w, room.h);
    }

    room.door_x[0] = 1 + rand() % (room.w - 2); room.door_y[0] = 0;
    room.door_x[1] = room.w - 1;              room.door_y[1] = 1 + rand() % (room.h - 2);
    room.door_x[2] = 1 + rand() % (room.w - 2); room.door_y[2] = room.h - 1;
    room.door_x[3] = 0;                       room.door_y[3] = 1 + rand() % (room.h - 2);

    if (rand() % 100 < 15) {
        int hw = room.w + 6, hh = room.h + 6;
        char** hallway_tiles = bnew(hh, hw, '#');
        for(int y=0; y<room.h; y++) for(int x=0; x<room.w; x++) {
            hallway_tiles[y+3][x+3] = room.tiles[y][x];
        }

        int hall_dir = rand() % 4;
        if (hall_dir == 0) {
            for(int y=0; y<=3; y++) hallway_tiles[y][hw/2] = ' ';
            for(int i=0; i<4; i++) { room.door_x[i] = hw/2; room.door_y[i] = 0; }
        } else if (hall_dir == 1) {
            for(int x=room.w+3; x<hw; x++) hallway_tiles[hh/2][x] = ' ';
            for(int i=0; i<4; i++) { room.door_x[i] = hw-1; room.door_y[i] = hh/2; }
        } else if (hall_dir == 2) {
            for(int y=room.h+3; y<hh; y++) hallway_tiles[y][hw/2] = ' ';
            for(int i=0; i<4; i++) { room.door_x[i] = hw/2; room.door_y[i] = hh-1; }
        } else {
            for(int x=0; x<=2; x++) hallway_tiles[hh/2][x] = ' ';
            for(int i=0; i<4; i++) { room.door_x[i] = 0; room.door_y[i] = hh/2; }
        }

        for(int y=0; y<room.h; y++) free(room.tiles[y]); free(room.tiles);
        room.tiles = hallway_tiles;
        room.w = hw; room.h = hh;
    }

    return room;
}

/* ---------------------------- Maze generators ---------------------------- */

Map xmgen_maze(const int wR, const int hR, const int w, const int h) {
    int maze_w = (w % 2 == 0) ? w + 1 : w;
    if (maze_w < 3) maze_w = 3;
    int maze_h = (h % 2 == 0) ? h + 1 : h;
    if (maze_h < 3) maze_h = 3;

    srand((unsigned)time(0));
    Map map = mnew(wR, hR);

    int* stack_x = toss(int, maze_w * maze_h);
    int* stack_y = toss(int, maze_w * maze_h);
    int stack_top = 0;

    int cx = 1;
    int cy = 1;
    map.walling[cy][cx] = ' ';

    stack_x[stack_top] = cx;
    stack_y[stack_top] = cy;
    stack_top++;

    int dx[] = {0, 2, 0, -2};
    int dy[] = {-2, 0, 2, 0};
    int wall_dx[] = {0, 1, 0, -1};
    int wall_dy[] = {-1, 0, 1, 0};

    while (stack_top > 0) {
        cx = stack_x[stack_top - 1];
        cy = stack_y[stack_top - 1];

        int valid_neighbors[4];
        int neighbor_count = 0;
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            if (nx > 0 && nx < maze_w - 1 && ny > 0 && ny < maze_h - 1 && map.walling[ny][nx] == '#') {
                valid_neighbors[neighbor_count++] = i;
            }
        }

        if (neighbor_count > 0) {
            int rand_idx = rand() % neighbor_count;
            int chosen_dir = valid_neighbors[rand_idx];

            int nx = cx + dx[chosen_dir];
            int ny = cy + dy[chosen_dir];
            int wx = cx + wall_dx[chosen_dir];
            int wy = cy + wall_dy[chosen_dir];

            map.walling[wy][wx] = ' ';
            map.walling[ny][nx] = ' ';

            stack_x[stack_top] = nx;
            stack_y[stack_top] = ny;
            stack_top++;

        } else {
            stack_top--;
        }
    }

    map.walling[1][0] = ' ';
    map.walling[maze_h - 2][maze_w - 1] = ' ';

    free(stack_x);
    free(stack_y);

    return map;
}

Map xmgen_room_maze(const int wR, const int hR, const int w, const int h, const int num_rooms_to_try, const int min_room_size, const int max_room_size) {
    int maze_w = (w % 2 == 0) ? w + 1 : w;
    if (maze_w < 15) maze_w = 15;
    int maze_h = (h % 2 == 0) ? h + 1 : h;
    if (maze_h < 15) maze_h = 15;

    srand((unsigned)time(0));
    Map map = mnew(wR, hR);

    Rect* rooms = toss(Rect, num_rooms_to_try);
    int room_count_local = 0;

    for (int i = 0; i < num_rooms_to_try && room_count_local < num_rooms_to_try; i++) {
        int rw = min_room_size + (rand() % (max_room_size - min_room_size + 1));
        if (rw % 2 == 0) rw++;
        int rh = min_room_size + (rand() % (max_room_size - min_room_size + 1));
        if (rh % 2 == 0) rh++;

        int rx = (rand() % ((maze_w - rw - 2) / 2 + ((maze_w - rw - 2) / 2==0))) * 2 + 1;
        int ry = (rand() % ((maze_h - rh - 2) / 2 + ((maze_h - rh - 2) / 2==0))) * 2 + 1;

        Rect new_room = {rx, ry, rw, rh};

        bool overlap = false;
        for (int j = 0; j < room_count_local; j++) {
            Rect other = rooms[j];
            if (new_room.x < other.x + other.w && new_room.x + new_room.w > other.x &&
                new_room.y < other.y + other.h && new_room.y + new_room.h > other.y) {
                overlap = true;
                break;
            }
        }

        if (!overlap) {
            rooms[room_count_local++] = new_room;
        }
    }

    for (int i = 0; i < room_count_local; i++) {
        Rect r = rooms[i];
        for (int y = r.y; y < r.y + r.h; y++) {
            for (int x = r.x; x < r.x + r.w; x++) {
                map.walling[y][x] = ' ';
            }
        }
    }

    for (int y = 1; y < maze_h; y += 2) {
        for (int x = 1; x < maze_w; x += 2) {
            if (map.walling[y][x] == '#') {
                int* stack_x = toss(int, maze_w * maze_h);
                int* stack_y = toss(int, maze_w * maze_h);
                int stack_top = 0;

                stack_x[stack_top] = x;
                stack_y[stack_top] = y;
                stack_top++;
                map.walling[y][x] = '.';

                int dx[] = {0, 2, 0, -2};
                int dy[] = {-2, 0, 2, 0};
                int wall_dx[] = {0, 1, 0, -1};
                int wall_dy[] = {-1, 0, 1, 0};

                while (stack_top > 0) {
                    int cx = stack_x[stack_top - 1];
                    int cy = stack_y[stack_top - 1];

                    int valid_neighbors[4];
                    int neighbor_count = 0;
                    for (int k = 0; k < 4; k++) {
                        int nx = cx + dx[k];
                        int ny = cy + dy[k];
                        if (nx > 0 && nx < maze_w - 1 && ny > 0 && ny < maze_h - 1 && map.walling[ny][nx] == '#') {
                            valid_neighbors[neighbor_count++] = k;
                        }
                    }

                    if (neighbor_count > 0) {
                        int rand_idx = rand() % neighbor_count;
                        int chosen_dir = valid_neighbors[rand_idx];
                        int nx = cx + dx[chosen_dir];
                        int ny = cy + dy[chosen_dir];
                        int wx = cx + wall_dx[chosen_dir];
                        int wy = cy + wall_dy[chosen_dir];

                        map.walling[wy][wx] = '.';
                        map.walling[ny][nx] = '.';

                        stack_x[stack_top] = nx;
                        stack_y[stack_top] = ny;
                        stack_top++;
                    } else {
                        stack_top--;
                    }
                }
                free(stack_x);
                free(stack_y);
            }
        }
    }

    for (int i = 0; i < room_count_local; i++) {
        Rect r = rooms[i];
        Point doors[r.w * 2 + r.h * 2];
        int door_count = 0;

        for (int x = r.x; x < r.x + r.w; x++) {
            if (r.y - 2 > 0 && map.walling[r.y - 2][x] == '.') doors[door_count++] = (Point){(float)x, (float)r.y - 1};
            if (r.y + r.h + 1 < maze_h && map.walling[r.y + r.h + 1][x] == '.') doors[door_count++] = (Point){(float)x, (float)r.y + r.h};
        }
        for (int y = r.y; y < r.y + r.h; y++) {
            if (r.x - 2 > 0 && map.walling[y][r.x - 2] == '.') doors[door_count++] = (Point){(float)r.x - 1, (float)y};
            if (r.x + r.w + 1 < maze_w && map.walling[y][r.x + r.w + 1] == '.') doors[door_count++] = (Point){(float)r.x + r.w, (float)y};
        }

        if (door_count > 0) {
            Point door = doors[rand() % door_count];
            map.walling[(int)door.y][(int)door.x] = '+';
        }
    }

    for (int y = 0; y < maze_h; y++) {
        for (int x = 0; x < maze_w; x++) {
            if (map.walling[y][x] == '.') {
                map.walling[y][x] = ' ';
            }
        }
    }

    connect_rooms(map, rooms, room_count_local);

    free(rooms);
    return map;
}

/* ---------------------- Connectivity helpers for Brogue ------------------ */

static int is_connected(Map map, Point start, Point end) {
    if (start.x < 0 || start.y < 0 || start.x >= map.w || start.y >= map.h) return 0;
    if (end.x < 0 || end.y < 0 || end.x >= map.w || end.y >= map.h) return 0;

    int** visited = toss(int*, map.h);
    for (int y = 0; y < map.h; y++) {
        visited[y] = toss(int, map.w);
        memset(visited[y], 0, map.w * sizeof(int));
    }

    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    Point* stack = toss(Point, map.w * map.h);
    int stack_top = 0;

    stack[stack_top++] = start;
    visited[(int)start.y][(int)start.x] = 1;

    while (stack_top > 0) {
        Point current = stack[--stack_top];

        if ((int)current.x == (int)end.x && (int)current.y == (int)end.y) {
            for (int y = 0; y < map.h; y++) free(visited[y]);
            free(visited);
            free(stack);
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            int nx = (int)current.x + dx[i];
            int ny = (int)current.y + dy[i];

            if (nx >= 0 && ny >= 0 && nx < map.w && ny < map.h &&
                !visited[ny][nx] && map.walling[ny][nx] != '#') {
                visited[ny][nx] = 1;
                stack[stack_top++] = (Point){(float)nx, (float)ny};
            }
        }
    }

    for (int y = 0; y < map.h; y++) free(visited[y]);
    free(visited);
    free(stack);
    return 0;
}

static Point find_closest_room_point(Map map, Point p, int room_x, int room_y, int room_w, int room_h) {
    Point closest = { -1, -1 };
    float min_dist = -1.0f;

    for (int y = room_y; y < room_y + room_h; y++) {
        for (int x = room_x; x < room_x + room_w; x++) {
            if (y < 0 || x < 0 || y >= map.h || x >= map.w) continue;
            if (map.walling[y][x] != ' ') continue;

            float dx = (float)x - p.x;
            float dy = (float)y - p.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (min_dist < 0.0f || dist < min_dist) {
                min_dist = dist;
                closest = (Point){ (float)x, (float)y };
            }
        }
    }
    return closest;
}

static void add_lake(Map* map, char tile, int x, int y,  int w, int h, float lakePercent){
    Map lake = xmgen_cellular(w, h, lakePercent, 200);
    for(int yL = 0; yL < lake.h; yL++){
        for(int xL = 0; xL < lake.w; xL++){
            if(lake.walling[yL][xL] == ' '){
                int xM = (xL + x)%map->w;
                int yM = (yL + y)%map->h;
                if(xM == 0){
                    xM = 1;
                }
                if(yM == 0){
                    yM = 1;
                }
                map->walling[yM][xM] = tile;
            }
        }
    }
    xmclose(lake);
}

void add_enviroment(Map* map, char tile, int x, int y,  int w, int h, float lakePercent){
    Map lake = xmgen_cellular(w, h, lakePercent, 200);
    for(int yL = 0; yL < lake.h; yL++){
        for(int xL = 0; xL < lake.w; xL++){
            if(lake.walling[yL][xL] == ' ' ){
                int xM = (xL + x)%map->w;
                int yM = (yL + y)%map->h;
                if(xM == 0){
                    xM = 1;
                }
                if(yM == 0){
                    yM = 1;
                }
                if(map->walling[yM][xM] == ' ')
                    map->walling[yM][xM] = tile;
            }
        }
    }
    xmclose(lake);
}


/* -------------------------- Drunk & Cellular & Perlin -------------------- */

Map xmgen_drunk(const int w, const int h, const float floor_goal_percent) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);
    int floor_count = 0;
    const int total_tiles = w * h;
    const int floor_goal = (int)(total_tiles * floor_goal_percent);
    int walker_x = w / 2;
    int walker_y = h / 2;
    int max_iterations = total_tiles * 10;
    int iterations = 0;
    while (floor_count < floor_goal && iterations < max_iterations) {
        if (map.walling[walker_y][walker_x] == '#') {
            map.walling[walker_y][walker_x] = ' ';
            floor_count++;
        }
        int move = rand() % 4;
        switch (move) {
        case 0: walker_y--; break;
        case 1: walker_y++; break;
        case 2: walker_x--; break;
        case 3: walker_x++; break;
        }
        if (walker_x < 1) walker_x = 1;
        if (walker_x >= w - 1) walker_x = w - 2;
        if (walker_y < 1) walker_y = 1;
        if (walker_y >= h - 1) walker_y = h - 2;
        iterations++;
    }
    add_enviroment(&map, '"', 0, 0, w, h, 0.5);
    return map;
}



/* ----------------------------- xmgen_brogue ------------------------------ */

Map xmgen_brogue(const int w, const int h, const int max_rooms, const int min_size, const int max_size) {
    srand((unsigned)time(0));
    Map map = mnew(h, w);

    Rect* rooms = toss(Rect, max_rooms);
    int room_count_local = 0;

    BrogueRoom first_room = create_brogue_room(min_size, max_size);
    int start_x = (w / 2) - (first_room.w / 2);
    int start_y = (h / 2) - (first_room.h / 2);
    for (int y = 0; y < first_room.h; y++) {
        for (int x = 0; x < first_room.w; x++) {
            if (first_room.tiles[y][x] == ' ') {
                if(start_y + y >= 0 && start_y + y < h && start_x + x >= 0 && start_x + x < w)
                    map.walling[start_y + y][start_x + x] = ' ';
            }
        }
    }
    rooms[room_count_local++] = (Rect){start_x, start_y, first_room.w, first_room.h};

    for(int y=0; y<first_room.h; y++) free(first_room.tiles[y]); free(first_room.tiles);

    int rooms_placed = 1;
    int attempts = 0;

    while (rooms_placed < max_rooms && attempts < 2000) {
        attempts++;
        BrogueRoom new_room = create_brogue_room(min_size, max_size);

        Point* perimeter = toss(Point, w * h);
        int perimeter_count = 0;
        for (int y = 1; y < h - 1; y++) {
            for (int x = 1; x < w - 1; x++) {
                if (map.walling[y][x] == '#') {
                    if (map.walling[y - 1][x] == ' ' || map.walling[y + 1][x] == ' ' ||
                        map.walling[y][x - 1] == ' ' || map.walling[y][x + 1] == ' ') {
                        perimeter[perimeter_count++] = (Point){(float)x, (float)y};
                    }
                }
            }
        }

        if (perimeter_count == 0) {
            free(perimeter);
            for(int y=0; y<new_room.h; y++) free(new_room.tiles[y]); free(new_room.tiles);
            break;
        }

        bool placed = false;
        for (int p_idx = 0; p_idx < perimeter_count; p_idx++) {
            int rand_idx = p_idx + rand() % (perimeter_count - p_idx);
            Point temp = perimeter[p_idx]; perimeter[p_idx] = perimeter[rand_idx]; perimeter[rand_idx] = temp;
        }

        for (int p_idx = 0; p_idx < perimeter_count && !placed; p_idx++) {
            Point attach_point = perimeter[p_idx];

            for (int door_idx = 0; door_idx < 4; door_idx++) {
                int place_x = (int)attach_point.x - new_room.door_x[door_idx];
                int place_y = (int)attach_point.y - new_room.door_y[door_idx];

                if (place_x < 1 || place_y < 1 || place_x + new_room.w > w - 1 || place_y + new_room.h > h - 1) {
                    continue;
                }

                bool overlap = false;
                for (int y = 0; y < new_room.h && !overlap; y++) {
                    for (int x = 0; x < new_room.w; x++) {
                        if (new_room.tiles[y][x] == ' ' && map.walling[place_y + y][place_x + x] == ' ') {
                            overlap = true;
                            break;
                        }
                    }
                }

                if (!overlap) {
                    int placed_room_x = place_x;
                    int placed_room_y = place_y;
                    int placed_room_w = new_room.w;
                    int placed_room_h = new_room.h;

                    for (int y = 0; y < new_room.h; y++) {
                        for (int x = 0; x < new_room.w; x++) {
                            if (new_room.tiles[y][x] == ' ') {
                                map.walling[place_y + y][place_x + x] = ' ';
                            }
                        }
                    }

                    map.walling[(int)attach_point.y][(int)attach_point.x] = ' ';

                    rooms[room_count_local++] = (Rect){placed_room_x, placed_room_y, placed_room_w, placed_room_h};

                    Point room_center = { (float)(placed_room_x + placed_room_w/2), (float)(placed_room_y + placed_room_h/2) };
                    Point main_center = { w/2.0f, h/2.0f };

                    if (!is_connected(map, room_center, main_center)) {
                        Point room_point = { -1, -1 };
                        Point map_point = { -1, -1 };
                        float min_dist = -1.0f;

                        for (int ry = placed_room_y; ry < placed_room_y + placed_room_h; ry++) {
                            for (int rx = placed_room_x; rx < placed_room_x + placed_room_w; rx++) {
                                if (ry < 0 || rx < 0 || ry >= h || rx >= w) continue;
                                if (map.walling[ry][rx] != ' ') continue;

                                for (int y = 0; y < h; y++) {
                                    for (int x = 0; x < w; x++) {
                                        if (map.walling[y][x] != ' ') continue;

                                        if (y >= placed_room_y && y < placed_room_y + placed_room_h &&
                                            x >= placed_room_x && x < placed_room_x + placed_room_w) continue;

                                        float dx = (float)x - (float)rx;
                                        float dy = (float)y - (float)ry;
                                        float dist = dx*dx + dy*dy;
                                        if (min_dist < 0.0f || dist < min_dist) {
                                            min_dist = dist;
                                            room_point = (Point){ (float)rx, (float)ry };
                                            map_point  = (Point){ (float)x, (float)y };
                                        }
                                    }
                                }
                            }
                        }

                        if (min_dist >= 0.0f) {
                            create_corridor(map, (int)room_point.x, (int)room_point.y, (int)map_point.x, (int)map_point.y);
                        }
                    }

                    placed = true;
                    rooms_placed++;
                    break;
                }
            }
        }

        free(perimeter);
        for(int y=0; y<new_room.h; y++) free(new_room.tiles[y]); free(new_room.tiles);
    }

    int wallCount = 0;
    for(int y = 0; y < map.h; y++){
        for(int x = 0; x < map.w; x++){
            if(map.walling[y][x] == '#'){
                wallCount++;
            }
        }
    }
    if(wallCount == (map.h*map.w)){
        xmclose(map);
        free(rooms);
        return xmgen_brogue(w, h, max_rooms, min_size, max_size);
    }

    add_enviroment(&map, '\"', 0, 0, w, h, 0.6f);
    bool isLake = false;
    while(!isLake){
        int x = rand()%w;
        int y = rand()%h;
        if(map.walling[y][x] == ' '){
            // add_lake(&map, '~', x, y, 30, 30, 0.5f);
            // add_enviroment(&map, '~',  x, y, 20, 20, 0.5f);
            isLake = true;
        }
    }

    connect_rooms(map, rooms, room_count_local);

    free(rooms);
    return map;
}
