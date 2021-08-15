#include "common.h"
struct position{
    int row;
    int col;
    float val_fricc;
    int key;
    bool operator<(const position& cell)const {
        if(val_fricc>cell.val_fricc)
            return true;
        else{
            if(val_fricc<cell.val_fricc)
                return false;
            else
            if(key>cell.key)
                return true;
            else
                return false;
        }

    }
};

class metodos{
public:
	float* reset_Matrix(int rows,  int cols, float val1);
    void IDW_test(float req, float *cost_dist, float *&IDW, int rows, int cols, float exp, int cell_null);
    float* cost_distance(float *fric_matrix, int rows, int cols, position array, float *CD_matrix);

private:
};