typedef enum {
    LEFT,
    RIGHT,
    UP,
    DOWN,
} Direction;

typedef struct {
    int x;
    int y;
} Position;

struct Snake {
    Position position;
    struct Snake *next;
};
typedef struct Snake Snake;
