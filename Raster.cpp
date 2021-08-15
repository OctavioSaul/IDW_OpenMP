//
// Created by otavio on 05/03/20.
//
#include "Raster.h"

//-------------------------------------------------------------------------------------------------------------------------

double adfGeoTransform[6];
//-------------------------------------------------------------------------------------------------------------------------
float* Raster::read_tif_matrix(std::string file,int &rows, int &cols, int &scale,int &cell_null){
    int row,col;//iteradores matriz
    GDALDataset *dataset;
    GDALAllRegister();
    string ds = file;
    dataset = (GDALDataset *) GDALOpen(ds.c_str(), GA_ReadOnly);
    GDALRasterBand  *poBand;

    poBand = dataset->GetRasterBand(1);
    dataset->GetGeoTransform( adfGeoTransform );

    cols = poBand->GetXSize();
    rows= poBand->GetYSize();
    scale = adfGeoTransform[1];

    dataset->GetGeoTransform( adfGeoTransform );
    scale = adfGeoTransform[1];
    cell_null = poBand->GetNoDataValue();  //read null value of cell
    float *matrix = new float[rows * cols];
    float *pBuf = new float[rows * cols];

    poBand->RasterIO(GF_Read, 0, 0, cols, rows, pBuf, cols, rows, GDT_Float32, 0, 0);

    int location;
    for (row = 0; row < rows; row++)
        for ( col = 0; col < cols; col++){
            location = (cols * (row)) + col;
            matrix[(cols*row)+col] = *(pBuf+location);
        }
    printf("Columnas: %d\n", cols);
    printf("Renglones: %d\n", rows);
    printf("valor nulo: %d\n", cell_null);
    return matrix;
}
//-------------------------------------------------------------------------------------------------------------------------
void Raster::print_raster(float *matrix, int rows, int cols) {
    printf("Imprimiento matriz \n");
    for(int row=0; row< rows; row++){
        for(int col =0; col < cols; col++){
            printf(" %f ",matrix[(cols*row)+col]);
        }
        printf("\n");
    }
}
//-------------------------------------------------------------------------------------------------------------------------
int Raster::contar_comunidades(float *mapa_local, int rows, int cols,int cell_null) {
    int count=0;
    for(int row=0;row<rows;row++)
        for(int col=0;col<cols;col++){
            if(mapa_local[(cols*row)+col] != cell_null) {
                count++;
            }
        }
    printf("Total de comunidades es %d\n",count);
    return count;
}

//-------------------------------------------------------------------------------------------------------------------------
void Raster::leer_localidades(float *map_local,int rows, int cols, map<int,local> &local_ord,int cell_null,int num_local) {
    local array;
    int local_1=0;
    for(int row=0;row<rows;row++){
        for(int col=0;col<cols;col++){
            if(map_local[(cols*row)+col]  != cell_null) {
                array.row = row;
                array.col = col;
                local_ord[(int)map_local[(cols*row)+col]] = array;
                local_1 ++ ;
            }
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------
int Raster::no_row(string name){
    int counter = 0;
    ifstream file(name.c_str());
    string data;
    while(getline(file,name))
        counter++;
    return counter;
}
//-------------------------------------------------------------------------------------------------------------------------
void Raster::carga_requisitos(string name,map <int, float> &req_map){

    int cont;
    ifstream file(name.c_str());
    stringstream buffer;
    buffer << file.rdbuf();
    string key;
    string val;
    //no nos interesa cargar los titulos de la tabla
    getline(buffer,key,',');
    getline(buffer, val, '\n');
    cont=no_row(name);
    cont--;
    while(cont>0) {
        getline(buffer,key,',');
        getline(buffer, val, '\n');
        req_map.insert(pair<int, float>(atof(key.c_str()),atof(val.c_str())));//se guarda en el mapa, el no. de localidad como llave y el requisito de biomasa
        cont--;
    }
}
//-------------------------------------------------------------------------------------------------------------------------
void Raster::matrix_to_tiff(float *output_raster, int rows, int cols, int count,string name) {
    int row,col;//iteradores
    GDALDataset *poDstDS;
    GDALDriver *poDriver;
    OGRSpatialReference oSRS;
    string fileName = name + to_string(count) + ".tif";
    cout << fileName << endl;
    poDriver = GetGDALDriverManager()->GetDriverByName("Gtiff");
    poDstDS = poDriver->Create( fileName.c_str(), cols, rows, 1, GDT_Float32, NULL);

    GDALRasterBand *poBand;
    float *pBuf = new float[rows * cols], maxVal = 0;
    for(row= 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            pBuf[(row * cols) + col] = output_raster[(cols*row)+col];
            if(output_raster[(cols*row)+col] <= 0.0)
                pBuf[row * cols + col] = -9999;
            if(output_raster[(cols*row)+col] > maxVal)
                maxVal = output_raster[(cols*row)+col];
        }
    }

    poBand = poDstDS->GetRasterBand(1);
    poDstDS->GetRasterBand(1)->SetNoDataValue(-9999);
    poBand->RasterIO( GF_Write, 0, 0, cols, rows,pBuf, cols, rows, GDT_Float32, 0, 0 );
    GDALClose( (GDALDatasetH) poDstDS );

}
