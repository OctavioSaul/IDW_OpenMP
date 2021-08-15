#include "common.h"
#include "Metodos.h"
#include "Raster.h"
int main() {
    clock_t start_global = clock();//inicia tiempo global
    std::string str;
    //------------------------------------------------------------------------objetos
    Raster objrast;
    metodos objMeth;
//------------------------------------------------------------------------variables
    int rows, cols;//tamaño matriz
    int row,col;//iteradores matriz
    int cell_null;//valor nulo mapa
    int scale;//escala del mapa
    int num_com;//numero de localidades en el mapa
    float exp = 1.005;//exponente IDW
    int cont = 0;//numero de localidades a explorar en el ciclo while
    position array; //almacenar movimientos de CD
    int i;
// ------------------------------------------------------------------------matrices
    float* fric_matrix; //mapa friccion
    float* localidad_matrix;//mapa con localidades (ubicacion)
    float* IDW_matrix;
// ------------------------------------------------------------------------mapas
    map<int, Raster::local> localidades;// mapa ubicacion localidades
    //estructura local contiene el no. de comunidad y su (x,y)
    std::map<int, Raster::local>::iterator ubicacion; //iterados mapa ubicacion localidades
    map<int, float> biomass_requerida;//mapa requisitos de localidades
    std::map<int, float>::iterator biomass; //iterados mapa requisitos localidad
    //---------------------mapa friccion
    printf("----matriz friccion\n");
    fric_matrix = objrast.read_tif_matrix("/home/sperez/modelos/Kenia/general/fricc_singeo0.tif", rows, cols, scale, cell_null);
    //---------------------mapa localidades
    printf("----matriz localidades\n");
    localidad_matrix = objrast.read_tif_matrix("/home/sperez/modelos/Kenia/20_comunidades/locs_20_int.tif", rows, cols, scale,cell_null);
    //obtenemos el numero de comunidades
    num_com = objrast.contar_comunidades(localidad_matrix, rows, cols, cell_null);
    //---------------------guardamos los requisitos de las comunidades en un mapa
    objrast.carga_requisitos("/home/sperez/modelos/Kenia/20_comunidades/fwuse_20.csv", biomass_requerida);

    // guardamos las localidades en un mapa para ordenarlas
    objrast.leer_localidades(localidad_matrix, rows, cols, localidades, cell_null, num_com);
    //valores iniciales
    IDW_matrix = objMeth.reset_Matrix(rows, cols, 0); //llena la matriz inicial del valor indicado
    float *CD_matrix = new float[rows*cols];

    //-------------------------------------------------------------------------------------------------------inicia calculo modelos
    biomass = biomass_requerida.begin();
    int start =int(biomass->first);
    biomass = biomass_requerida.end();
    int end =int(biomass->first);
    for(i=start;i<=end;i++) {
        if (biomass_requerida.find(i) != biomass_requerida.end()) {//existe la comunidad con ese numero?
            biomass = biomass_requerida.find(i);
            if (biomass->second != 0) {//requisitos distintos a cero
                if (localidades.find(i) != localidades.end()) { //existe la comunidad con ese numero?
                    ubicacion = localidades.find(biomass->first);//buscar ubicacion de la localidad
                    //ubicaion inicial
                    array.row = ubicacion->second.row;
                    array.col = ubicacion->second.col;
                    array.val_fricc = 0;
                    array.key=0;
                    cont++;//localidades calculadas
                    CD_matrix=objMeth.cost_distance(fric_matrix, rows, cols, array, CD_matrix);
                    //objrast.matrix_to_tiff(CD_matrix, rows, cols,cont,"CD_");
                    cout<<"costo distancia "<<i<< " calculado"<<endl;
                    objMeth.IDW_test(biomass->second, CD_matrix, IDW_matrix, rows, cols, exp, cell_null);

                }
            }
        }
    }
    // ---------------agregar valores nulos en la ubicacion de las localidade
    ubicacion = localidades.begin();
    while (ubicacion != localidades.end()) {
        IDW_matrix[(cols * ubicacion->second.row)+ubicacion->second.col] = cell_null;
        ubicacion++;
    }

    objrast.matrix_to_tiff(IDW_matrix, rows, cols,cont,"IDW_");//crea tiff de IDW de todas las localidades calculadas
    clock_t end_global = clock();//termina tiempo global
    double duration = (double)(end_global - start_global) / (double) CLOCKS_PER_SEC;//calcula tiempo de ejecucion
    printf("tiempo global: %lf segundos \n", duration);
    return 0;
}

