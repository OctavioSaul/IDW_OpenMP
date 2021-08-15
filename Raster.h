#include "common.h"

class Raster {
public:
    typedef struct local{
        int row;
        int col;
        int num_local;
    }local;

    float* read_tif_matrix(string name,int &rows, int &cols, int &scale,int &cell_null);

    void print_raster(float *matrix, int rows, int cols);
    int contar_comunidades(float *mapa_local, int rows, int cols,int cell_null);
    void leer_localidades(float *map_local,int rows, int cols, map<int,local> &local_ord,int cell_null,int num_local);
    int no_row(string name);
    void carga_requisitos(string name,map <int, float> &req_map);
    void matrix_to_tiff(float *output_raster, int rows, int cols, int count, string name);

private:

};
