#include "common.h"
#include "Metodos.h"
#include "Raster.h"
int main() {
    std::string str;
    double start2;
    double end2;
    start2 = omp_get_wtime();
    //------------------------------------------------------------------------objetos
    Raster objrast;
    metodos objMeth;
//------------------------------------------------------------------------variables
    int rows, cols;//tamaño matriz
    //int row,col;//iteradores matriz
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
    //printf("----matriz friccion\n");
    fric_matrix = objrast.read_tif_matrix("/home/sperez/modelos/Kenia/general/fricc_singeo0.tif", rows, cols, scale, cell_null);
    //---------------------mapa localidades
    //printf("----matriz localidades\n");
    localidad_matrix = objrast.read_tif_matrix("/home/sperez/modelos/Kenia/40_comunidades/locs_40_int.tif", rows, cols, scale,cell_null);
    //obtenemos el numero de comunidades
    num_com = objrast.contar_comunidades(localidad_matrix, rows, cols, cell_null);
    //---------------------guardamos los requisitos de las comunidades en un mapa
    objrast.carga_requisitos("/home/sperez/modelos/Kenia/40_comunidades/fwuse_40.csv", biomass_requerida);

    // guardamos las localidades en un mapa para ordenarlas
    objrast.leer_localidades(localidad_matrix, rows, cols, localidades, cell_null, num_com);
    //valores iniciales
    IDW_matrix = objMeth.reset_Matrix(rows, cols, 0); //llena la matriz inicial del valor indicado

    //-------------------------------------------------------------------------------------------------------inicia calculo modelos
    biomass = biomass_requerida.begin();
    int start =int(biomass->first);
    biomass = biomass_requerida.end();
    int end =int(biomass->first);

    const int mov[2][8]={{1,1,0,-1,-1,-1,0,1},{0,1,1,1,0,-1,-1,-1}};
    //omp_set_num_threads(10);
    #pragma omp parallel for default(shared) private(ubicacion,biomass,array)
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
                    float *CD_matrix = new float[rows*cols];
                    //CD_matrix=objMeth.cost_distance(fric_matrix, rows, cols, array, CD_matrix);
                    priority_queue<position> CD_costos;
                    int key = 1;
                    int row_temp,col_temp,h;
                    for(row_temp=0;row_temp<rows;row_temp++)
                        for(col_temp=0;col_temp<cols;col_temp++)
                            CD_matrix[(cols*row_temp)+col_temp]=std::numeric_limits<float>::max();
                    position inicial;
                    CD_costos.push(array);
                    //---------------------------------------------------------------inicia calculo
                    while(!CD_costos.empty()){
                        inicial=CD_costos.top();
                        CD_costos.pop();
                        for(h=1;h<9;h++){
                            row_temp = mov[1][h - 1] + inicial.row;
                            col_temp = mov[0][h - 1] + inicial.col;
                            if (row_temp < rows && row_temp >= 0 && col_temp < cols && col_temp >= 0 && fric_matrix[(cols*row_temp)+col_temp]>0.0) {
                                if (h % 2 != 0){//si es un movimiento lateral
                                    array.val_fricc = inicial.val_fricc + (fric_matrix[(cols * row_temp) + col_temp]);
                                }else {//si es un movimiento diagonal
                                    array.val_fricc =
                                            inicial.val_fricc + sqrt(2) * (fric_matrix[(cols * row_temp) + col_temp]);
                                }//se busca el menor valor de CD, es posible que se escriba varias veces en una celda
                                if (CD_matrix[(cols*row_temp)+col_temp]>array.val_fricc) {
                                    array.row = row_temp;
                                    array.col = col_temp;
                                    array.key=key;
                                    key++;
                                    CD_matrix[(cols*row_temp)+col_temp] = array.val_fricc;
                                    CD_costos.push(array);
                                }
                            }
                        }
                    }
                    //objrast.matrix_to_tiff(CD_matrix, rows, cols,cont,"CD_");
                    //cout<<"costo distancia "<<i<< " calculado"<<endl;
                    //IDW_matrix_tmp=objMeth.IDW_test(biomass->second, CD_matrix, IDW_matrix_tmp, rows, cols, exp, cell_null);
                    //---------------IDW
                    int row,col;
                    for(row = 0; row < rows; row++) {
                        for (col = 0; col < cols; col++) {
                            if (CD_matrix[(cols * row) + col] <= 0) {
                                IDW_matrix[(cols * row) + col] = cell_null;
                            } else {
                                IDW_matrix[(cols * row) + col] += biomass->second / pow(CD_matrix[(cols * row) + col], exp);
                            }
                        }
                    }
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
    objrast.matrix_to_tiff(IDW_matrix, rows, cols,end,"IDW_SIN_8_");//crea tiff de IDW de todas las localidades calculadas
    end2 = omp_get_wtime();
    double duration = (end2 - start2);//calcula tiempo de ejecucion
    //-----------liberar memoria
    delete IDW_matrix;
    delete fric_matrix;
    delete localidad_matrix;
    printf("tiempo global: %lf segundos \n", duration);
    return 0;
}

