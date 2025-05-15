#include <stdio.h>
#include <stdlib.h>
#include "matrix.c"
// #include "util/sigmoid.c" 
#include "util/array.c"
#include <time.h>
#include <unistd.h>
#include "util/absolute.c"
#include "util/for_macro.c"


#define DEBUG 0
#define GRADIENT_PRINT 0

typedef double (*Activation)(double);

typedef struct Model
{
    int num_layers;
    int *layers; //number of nodes in a layer
    double learning_rate;
    Matrix *weights; // array of matrices
    Matrix *bias;
    Matrix* layer_output; 
    Matrix* layer_error; 
    Activation activate; 
    Activation dactivate;
} Model;


#define alloc_model(model, learning_rate, num_layers, layers) \
    model->learning_rate = learning_rate; \
    model->num_layers = num_layers; \
    model->layers[num_layers+1]; \
    for(int i =0; i <= num_layers; i++) \
        model->layers[i] = layers[i]; \
    Matrix weights[num_layers], bias[num_layers]; \

#define alloc_model_input_layer(model)\
    mat_alloc(model->layer_output[0], model->layers[0], 1, layer_output_0); \
    mat_alloc(model->layer_error[0], model->layers[0], 1, layer_error_0); \

#define alloc_model_layer(model, layer_num, layer_name) \
    if (layer_num < 1 || layer_num > model->num_layers) \
    {\
        printf("Cannot allocation layer (%d) where total number of layers are %d\n", layer_num, model->num_layers+1);\
        exit(EXIT_FAILURE);\
    }\
    mat_alloc(model->weights[layer_num-1], model->layers[layer_num], model->layers[layer_num-1],layer_name_1)\
    mat_alloc(model->bias[layer_num-1], model->layers[layer_num], 1, layer_name_2)\
    mat_alloc(model->layer_output[layer_num], model->layers[layer_num], 1, layer_name_3)\
    mat_alloc(model->layer_error[layer_num-], model->layers[layer_num], 1, layer_name_4)\


#define alloc_model_from_file(model, file)\
    fscanf(file, "%d\n", &model->num_layers) \
    model->layers[model->num_layers+1]; \
    for(int i =0; i <= num_layers; i++) \
        fscanf(file, "%d ", &model->layers[i]); \
    fscanf(file, "\n")\
    \
    fscanf(file, "%lf", &model->learning_rate); \
    Matrix weights[model->num_layers], bias[model->num_layers]; \


void write_model(Model *model, char* filename)
{
    FILE *file;     
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    //Structure of model
    //num_layers
    //layers
    //learning rate
    //weights
    //bias
    fprintf(file, "%d\n", model->num_layers); 
    for(int i = 0 ; i < model->num_layers+1; i++)
        fprintf(file, "%d;", model->layers[i]);
    fprintf(file, "\n");
    fprintf(file, "%f\n", model->learning_rate);

    for (int i = 0; i < model->num_layers; i++)
    {
        mat_write(&model->weights[i], file);
        mat_write(&model->bias[i], file);
    }
}

int read_model_matrices(Model* model, FILE* file)
{
    //TODO: add error handling
    for(int i = 0; i < model->num_layers; i++)
    {
        mat_read_size(&model->weights[i], file); 
        mat_read(&model->weights[i], file);

        mat_read_size(&model->weights[i], file); 
        mat_read(&model->bias[i], file);
    }
    return EXIT_SUCCESS; 

}

int initialize_model_weights(Model *model)
{

    for (int i = 0; i < model->num_layers; i++)
    {
        mat_rand_xavier(model->weights[i]);
        mat_normalize(model->weights[i]);

        // mat_rand(model->bias[i]);
        // mat_normalize(model->bias[i]);
        mat_zeros(model->bias[i]);
    }

    return EXIT_SUCCESS;
}
int forward_pass(Matrix* output, Matrix* input, Model* model)
{
    
    // arr_to_mat(&model->layer_output[0], input, model->layers[0]);
    mat_copy_vals(&model->layer_output[0], input);
    mat_normalize_inplace(&model->layer_output[0]);
    // mat_map(frontier, activation, 1);

    for(int i = 0; i < model->num_layers; i++)
    {

        //Matrix* frontier_t = mat_mul(model->weights[i], frontier); 
        mat_mul(&model->layer_output[i+1], &model->weights[i], &model->layer_output[i]); 

        // mat_add(frontier_t, model->bias[i], 1);
        // mat_map(frontier_t, activation, 1);
        mat_add(&model->layer_output[i+1], &model->layer_output[i+1], &model->bias[i]);
        mat_map_inplace(&model->layer_output[i+1], model->activate);


        if (DEBUG)
        {
            printf("\ni: %d\n", i);
            printf("\nFrontier (%d, %d)\n", model->layer_output[i].rows, model->layer_output[i].columns); 
            mat_print(&model->layer_output[i]);

            printf("\nWeights[%d] (%d, %d)\n", i, model->weights[i].rows, model->weights[i].columns);
            mat_print(&model->weights[i]);

            printf("\nBias[%d] (%d, %d)\n",i, model->bias[i].rows, model->bias[i].columns);
            mat_print(&model->bias[i]);

            printf("\nfrontier_t: sigmoid(mul(model->weights[%d], frontier) + model->bias[%d])\n", i, i);
            mat_print(&model->layer_output[i+1]);
        }


        // mat_free(frontier, 0);
        // frontier = frontier_t;

    }
    mat_copy_vals(output, &model->layer_output[model->num_layers]);
    // mat_to_arr(output, &model->layer_output[model->num_layers], model->layers[model->num_layers]);
    return EXIT_SUCCESS; 
}

double backward_pass(
    Model* output, 
    Model* input, 
    Model* model
){    

    // Matrix* O = arr_to_mat(model->layers[model->num_layers], output, 1);
    // Matrix* frontier[model->num_layers+1];
    // frontier[0] = arr_to_mat(model->layers[0], input, 1);
    // mat_normalize(frontier[0]);
    // mat_map(frontier[0], activation, 1);

    mat_copy_vals(&model->layer_output[0], input);
    mat_normalize_inplace(&model->layer_output[0]);

    if (DEBUG)
    {
        printf("\nFrontier[%d]: \n", 0); 
        mat_print(&model->layer_output[0]);

        printf("\noutput: \n"); 
        mat_print(output);
    }

    for(int i = 0; i < model->num_layers; i++)
    {
        // frontier[i+1] = mat_mul(model->weights[i], frontier[i]); 
        mat_mul(&model->layer_output[i+1], &model->weights[i], &model->layer_output[i]);
        // mat_add(frontier[i+1], model->bias[i], 1);
        // mat_map(frontier[i+1], activation, 1);
        mat_add(&model->layer_output[i+1], &model->layer_output[i+1], &model->bias[i]); 
        mat_map_inplace(&model->layer_output[i+1], model->activate);

        if (DEBUG)
        {
            printf("\n model->weights[%d]\n", i); 
            mat_print(&model->weights[i]);

            printf("\n model->bias[%d] \n", i); 
            mat_print(&model->bias[i]);

            printf("\nfrontier[%d]: sigmoid(mat_mul(model->weights[%d], frontier[%d]) + model->bias[%d])\n", i+1, i, i, i);
            mat_print(&model->layer_output[i+1]);
        }
    }


    // Matrix* error[model->num_layers+1];
    // Matrix* neg_frontier = mat_scale(frontier[model->num_layers], -1, 0);
    // error[model->num_layers] = mat_add(O, neg_frontier, 0); 
    int last = model->num_layers; 
    Matrix neg_layer_output; 
    mat_alloc(neg_layer_output, model->layer_output[last].rows, model->layer_output[last].columns); 
    mat_copy_vals(&neg_layer_output, &model->layer_output[last]); 
    mat_add(&model->layer_error[last], &output, &neg_layer_output);

    if (DEBUG)
    {
        printf("\n neg_frontier = -frontier[%d: model->num_layers]\n", model->num_layers);
        mat_print(&neg_layer_output);
        printf("error[%d: model->num_layer] = O + neg_frontier\n", model->num_layers); 
        mat_print(&model->layer_error[last]);
    }


    for(int i = model->num_layers-1; i >= 0; i--)
    {
        //gradient = error[i+1] * lr * dsigmoid(output: frontier[i+1])
        //d_weights[i] = gradient x input: frontier[i]
        
        // Matrix* weights_t = mat_transpose(model->weights[i], 0);
        // error[i] = mat_mul(weights_t, error[i+1]);
        Matrix weights_t; 
        mat_alloc(weights_t, model->weights[i].columns, model->weights[i].rows);
        mat_transpose(&weights_t, &model->weights[i]);
        mat_mul(&model->layer_error[i], &weights_t, &model->layer_error[i+1]);


        // Matrix* gradient = mat_map(frontier[i+1], dactivation, 0);
        // mat_scalar_mul(gradient, error[i+1], 1);
        // mat_scale(gradient, model->learning_rate, 1);
        Matrix gradient; 
        mat_alloc(gradient, model->layer_output[i+1].rows, model->layer_output[i+1].columns);
        mat_map(&gradient, &model->layer_output[i+1], model->activate);

        // Matrix* neg_gradient = mat_scale(gradient, -1, 0);
        // mat_add(model->bias[i], gradient, 1);
        mat_add(&model->bias[i], &model->bias[i], &gradient);

        // Matrix* frontier_t =  mat_transpose(frontier[i], 0);
        // Matrix* d_weights = mat_mul(gradient, frontier_t);
        Matrix frontier_t, d_weights; 
        mat_alloc(frontier_t, model->layer_output[i].columns, model->layer_output[i].rows);
        mat_alloc(d_weights, gradient.rows, frontier_t.columns);


        // mat_scale(d_weights, -1, 1);
        // mat_add(model->weights[i], d_weights, 1);
        mat_add(&model->weights[i], &model->weights[i], &d_weights);

        if (GRADIENT_PRINT)
        {
            printf("\n");
            mat_print(&gradient);
        }

        if (DEBUG)
        {
            printf("\ni : %d\n", i);
            printf("\n weights_t= transpose(model->weights[%d])\n", i);
            mat_print(&weights_t); 

            printf("\n error[%d]\n", i); 
            mat_print(&model->layer_error[i]);

            printf("\nfrontier[%d]\n", i+1); 
            mat_print(&model->layer_output[i+1]);

            printf("\n gradient = map(frontier[%d], dsigmoid) * errror[%d] * lr: %f\n", i+1, i+1, model->learning_rate); 
            mat_print(&gradient);

        }
        
        // mat_free(gradient, 0);
        // mat_free(frontier_t, 0);
        // mat_free(weights_t, 0);
        // mat_free(d_weights, 0);
        // free(neg_gradient);
    }
    
    double e[model->layer_error[last].rows];
    mat_to_arr(e, &model->layer_error[last], model->layer_error[last].rows); 

    double mse = 0; 
    for (int i = 0 ; i < model->num_layers+1; i++)
    {
        mse += pow(e[i], 2);
    }

    return mse; 
}


