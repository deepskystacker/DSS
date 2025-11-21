#if !defined(_NNCOMMON_H)
#define _NN_COMMON_H

typedef struct {
    double x;
    double y;
    double r;
} circle;

int circle_build1(circle* c, point* p0, point* p1, point* p2);
int circle_build2(circle* c, point* p0, point* p1, point* p2);
int circle_contains(circle* c, point* p);
void nn_quit(char* format, ...);

#endif /* _NN_COMMON_H */
