#include "Metodos.h"
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
float* metodos::reset_Matrix(int rows, int cols,float val1){
    int row,col;
    float *IDW = new float [rows * cols];//matriz IDW para todas las localidades
    for(row=0; row<rows; row++)
        for(col=0; col<cols; col++)
            IDW[(cols*row)+col]=val1;
    return(IDW);
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void metodos::IDW_test(float req, float *cost_dist, float *&IDW, int rows, int cols, float exp, int cell_null){
    int row,col;
    for(row = 0; row < rows; row++)
        for(col = 0; col < cols; col++){
            if(cost_dist[(cols*row)+col] <=0)
                IDW[(cols*row)+col] =cell_null;
            else
                IDW[(cols*row)+col]  += req / pow(cost_dist[(cols*row)+col] , exp);
        }
}
//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
float* metodos::cost_distance(float *fric_matrix, int rows, int cols, position array, float *CD_matrix) {
    priority_queue<position> CD_costos;
    int key = 1;
    int row_temp,col_temp,i;
    const int mov[2][8]={{1,1,0,-1,-1,-1,0,1},{0,1,1,1,0,-1,-1,-1}};
    for(row_temp=0;row_temp<rows;row_temp++)
        for(col_temp=0;col_temp<cols;col_temp++)
            CD_matrix[(cols*row_temp)+col_temp]=std::numeric_limits<float>::max();
    position inicial;
    CD_costos.push(array);
    //---------------------------------------------------------------inicia calculo
    while(!CD_costos.empty()){
        inicial=CD_costos.top();
        CD_costos.pop();
        for(i=1;i<9;i++){
            row_temp = mov[1][i - 1] + inicial.row;
            col_temp = mov[0][i - 1] + inicial.col;
            if (row_temp < rows && row_temp >= 0 && col_temp < cols && col_temp >= 0 && fric_matrix[(cols*row_temp)+col_temp]>0.0) {
                if (i % 2 != 0)//si es un movimiento lateral
                    array.val_fricc = inicial.val_fricc + (fric_matrix[(cols*row_temp)+col_temp]);
                else//si es un movimiento diagonal
                    array.val_fricc =  inicial.val_fricc + sqrt(2) * (fric_matrix[(cols*row_temp)+col_temp]);
                //se busca el menor valor de CD, es posible que se escriba varias veces en una celda
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
    return CD_matrix;
}