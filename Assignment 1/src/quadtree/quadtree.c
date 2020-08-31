#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>





Nodo* nodo_init(int x, int y, int width, int height){
    Nodo* nodo = malloc(sizeof(Nodo));
    nodo -> x = x;
    nodo -> y = y;
    nodo -> width = width;
    nodo -> height = height;
    

    nodo -> hijos[0] = NULL;
    nodo -> hijos[1] = NULL;
    nodo -> hijos[2] = NULL;
    nodo -> hijos[3] = NULL;

    nodo -> L = 0;
    nodo -> a = 0;
    nodo -> b = 0;

    nodo -> mean_L = 0;
    nodo -> mean_a = 0;
    nodo -> mean_b = 0;

    nodo -> sd_a = 0;
    nodo -> sd_b = 0;
    nodo -> sd_L = 0;

    nodo -> gamma = 0;

    nodo -> hoja = 0;
    return nodo;
}




void poblar_qt(Nodo* nodo, Image* img){


    int width = nodo -> width;
    int height = nodo -> height;
    int x = nodo -> x;
    int y = nodo -> y;


    if(width/2 > 0 && height/2 > 0){

        nodo -> hijos[0] = nodo_init(x, y, width/2, height/2);
        poblar_qt(nodo -> hijos[0], img);

        nodo -> hijos[1] = nodo_init(x + width/2, y, width/2, height/2);
        poblar_qt(nodo -> hijos[1], img);

        nodo -> hijos[2] = nodo_init(x, y + height/2, width/2, height/2);
        poblar_qt(nodo -> hijos[2], img);

        nodo -> hijos[3] = nodo_init(x + width/2, y + height/2, width/2, height/2);
        poblar_qt(nodo -> hijos[3], img);

    }

    else
    {
 
        nodo -> L = img -> pixels[y][x].L;
        nodo -> a = img -> pixels[y][x].a;
        nodo -> b = img -> pixels[y][x].b;

        nodo -> mean_L = img -> pixels[y][x].L;
        nodo -> mean_a = img -> pixels[y][x].a;
        nodo -> mean_b = img -> pixels[y][x].b;

        nodo -> hoja = 1;
    }
}

int qt_height(Nodo* root){
    if (root == NULL){
        return 0;
    }
    else
    {
        int height1 = qt_height(root -> hijos[0]);
        int height2 = qt_height(root -> hijos[1]);
        int height3 = qt_height(root -> hijos[2]);
        int height4 = qt_height(root -> hijos[3]);

        if (height1 >= height2 && height1 >= height3 && height1 >= height4){
            return height1 + 1;
        }
        else if (height2 >= height3 && height2 >= height4 && height2 >= height1){
            return height2 +1;
        }

        else if (height3 >= height1 && height3 >= height2 && height3 >= height4){
            return height3 + 1;
        }
        else
        {
            return height4 + 1;
        }
    }
}

//tipo = 0 para L, 1 para a y 2 para b
void mean(Nodo* nodo, Image* img){
    if (nodo -> hijos[0] == NULL && nodo->hijos[1] == NULL && nodo->hijos[2] == NULL && nodo -> hijos[3]==NULL){
        nodo -> mean_L = img ->pixels[nodo->y][nodo->x].L;
        nodo -> mean_a = img ->pixels[nodo->y][nodo->x].a;       
        nodo -> mean_b = img ->pixels[nodo->y][nodo->x].b;
    }
    else
    {
        mean(nodo -> hijos[0], img);
        mean(nodo -> hijos[1], img);
        mean(nodo -> hijos[2], img);
        mean(nodo -> hijos[3], img);

        nodo -> mean_L = (nodo ->hijos[0]-> mean_L + nodo -> hijos[1] -> mean_L + nodo -> hijos[2] -> mean_L + nodo -> hijos[3] -> mean_L)/4;
        nodo -> mean_a = (nodo ->hijos[0]-> mean_a + nodo -> hijos[1] -> mean_a + nodo -> hijos[2] -> mean_a + nodo -> hijos[3] -> mean_a)/4;
        nodo -> mean_b = (nodo ->hijos[0]-> mean_b + nodo -> hijos[1] -> mean_b + nodo -> hijos[2] -> mean_b + nodo -> hijos[3] -> mean_b)/4;

    }
    

}

void suma2(Nodo* nodo){
    if (nodo -> hijos[0] == NULL && nodo->hijos[1] == NULL && nodo->hijos[2] == NULL && nodo -> hijos[3]==NULL){
        nodo -> suma2_L = pow(nodo -> L, 2);
        nodo -> suma2_a = pow(nodo -> a, 2);
        nodo -> suma2_b = pow(nodo -> b, 2);
               
    }
    else{

        suma2(nodo -> hijos[0]);
        suma2(nodo -> hijos[1]);
        suma2(nodo -> hijos[2]);
        suma2(nodo -> hijos[3]);

        nodo -> suma2_L = nodo -> hijos[0] -> suma2_L + nodo -> hijos[1] -> suma2_L + nodo -> hijos[2] -> suma2_L + nodo -> hijos[3] -> suma2_L;
        nodo -> suma2_a = nodo -> hijos[0] -> suma2_a + nodo -> hijos[1] -> suma2_a + nodo -> hijos[2] -> suma2_a + nodo -> hijos[3] -> suma2_a;
        nodo -> suma2_b = nodo -> hijos[0] -> suma2_b + nodo -> hijos[1] -> suma2_b + nodo -> hijos[2] -> suma2_b + nodo -> hijos[3] -> suma2_b;
    }
    
}
void sd(Nodo* nodo){

    if (nodo -> hijos[0] == NULL && nodo->hijos[1] == NULL && nodo->hijos[2] == NULL && nodo -> hijos[3]==NULL){
        nodo -> sd_a = 0;
        nodo -> sd_b = 0;
        nodo -> sd_L = 0;
    }

    else
    {



        nodo -> sd_L = sqrt(( nodo -> suma2_L - pow(nodo -> width,2)*pow((nodo -> mean_L),2)) / pow(nodo -> width,2)) ;
        nodo -> sd_a = sqrt((nodo -> suma2_a - pow(nodo -> width,2)*pow((nodo -> mean_a),2))/pow(nodo -> width,2));
        nodo -> sd_b = sqrt((nodo -> suma2_b - pow(nodo -> width,2)*pow((nodo -> mean_b),2))/pow(nodo -> width,2));
        nodo -> gamma = (nodo -> sd_L + nodo -> sd_a + nodo -> sd_b)/3;

        sd(nodo -> hijos[0]);
        sd(nodo -> hijos[1]);
        sd(nodo -> hijos[2]);
        sd(nodo -> hijos[3]);


    }
    

}


Quadtree* quadtree_init(Image* img){
    Quadtree* qt = malloc(sizeof(Quadtree));


    int width = img -> width;
    int height = img -> height;
    qt -> root = nodo_init(0, 0, width, height);

    poblar_qt(qt->root, img);


    mean(qt -> root, img);
    suma2(qt->root);
    sd(qt -> root);

    


    return qt;


}


void colorear_alpha(Nodo* nodo, Image* img, double alpha){
    if (nodo -> gamma <= alpha)
    {
        img_square_paint(
            img,
            nodo -> y,
            nodo -> x,
            nodo -> width,
            (Color){.L = nodo -> mean_L, .a = nodo -> mean_a, .b = nodo -> mean_b}

        );
    }

    else
    {
        colorear_alpha(nodo -> hijos[0], img, alpha);
        colorear_alpha(nodo -> hijos[1], img, alpha);
        colorear_alpha(nodo -> hijos[2], img, alpha);
        colorear_alpha(nodo -> hijos[3], img, alpha);
    }
    
    
}

void recalcular_arbol(Nodo* nodo, double alpha){
    if (nodo -> gamma <= alpha)
    {
        nodo ->hoja =1;
    }
    else
    {
        recalcular_arbol(nodo -> hijos[0], alpha);
        recalcular_arbol(nodo -> hijos[1], alpha);
        recalcular_arbol(nodo -> hijos[2], alpha);
        recalcular_arbol(nodo -> hijos[3], alpha);
    } 
}

void resetear_arbol(Nodo* nodo){
    if (nodo -> hijos[0] == NULL && nodo -> hijos[1] == NULL && nodo -> hijos[2] == NULL && nodo -> hijos[3] == NULL){
        nodo ->hoja = 1;
    }
    else
    {
        nodo -> hoja = 0;
        resetear_arbol(nodo -> hijos[0]);
        resetear_arbol(nodo -> hijos[1]);
        resetear_arbol(nodo -> hijos[2]);
        resetear_arbol(nodo -> hijos[3]);
    } 
}


int cantidad_hojas(Nodo* root, double alpha){
    if (root -> gamma <= alpha)
    {
        return 1;
    }
    else
    {
        return cantidad_hojas(root->hijos[0], alpha) + cantidad_hojas(root->hijos[1], alpha) + cantidad_hojas(root->hijos[2], alpha) + cantidad_hojas(root->hijos[3], alpha);
    }    
}

int min_alpha(Quadtree* qt, int start, int end, int alpha, int h_max){
    int h = cantidad_hojas(qt ->root, alpha);


    if (h <= h_max)
    {
        if (cantidad_hojas(qt->root, alpha -1) <= h_max)
        {
            return min_alpha(qt, start, alpha, (alpha+start)/2, h_max);
        }
        else
        {
            return alpha;
        }
        
        

    }

    else if (h > h_max)
    {
        return min_alpha(qt, alpha, end, (alpha+end)/2, h_max);
    }

}

void free_qt(Nodo* nodo){
    if (nodo -> hijos[0] == NULL && nodo -> hijos[1] == NULL && nodo -> hijos[2] == NULL && nodo -> hijos[3] == NULL)
    {
        free(nodo);
    }
    else
    {
        free_qt(nodo -> hijos[0]);
        free_qt(nodo -> hijos[1]);
        free_qt(nodo -> hijos[2]);
        free_qt(nodo -> hijos[3]);

        free(nodo);
    }
    
    
}