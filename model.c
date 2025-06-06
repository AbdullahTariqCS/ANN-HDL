#include <stdio.h>
#include <stdlib.h>
#include "matrix.c"
// #include "util/sigmoid.c"
#include "util/array.c"
#include <time.h>
#include <unistd.h>
#include "util/absolute.c"
#include "util/for_macro.c"


typedef struct Model
{
    int num_layers;
    int *layers; // number of nodes in a layer
    double learning_rate;
    Matrix *weights; // array of matrices
    Matrix *bias;
    Matrix *layer_output;
    Matrix *layer_error;
    Activation activate;
    Activation dactivate;
} Model;

#define alloc_model(model, _learning_rate, _num_layers, _layers, _activate, _dactivate, weight_name, bias_name, layer_output_name, layer_error_name) \
    model.learning_rate = _learning_rate;                                                                                                            \
    model.num_layers = _num_layers;                                                                                                                  \
    model.layers = _layers;                                                                                                                          \
    Matrix weight_name[_num_layers], bias_name[_num_layers], layer_output_name[_num_layers + 1], layer_error_name[_num_layers + 1];                  \
    model.weights = weight_name;                                                                                                                     \
    model.bias = bias_name;                                                                                                                          \
    model.layer_output = layer_output_name;                                                                                                          \
    model.layer_error = layer_error_name;                                                                                                            \
    model.activate = _activate;                                                                                                                      \
    model.dactivate = _dactivate;

#define alloc_model_input_layer(model, output_name, error_name)        \
    mat_alloc(model.layer_output[0], model.layers[0], 1, output_name); \
    mat_alloc(model.layer_error[0], model.layers[0], 1, error_name);

#define alloc_model_layer(model, layer_num, weight_name, bias_name, output_name, error_name)                           \
    if (layer_num < 1 || layer_num > model.num_layers)                                                                 \
    {                                                                                                                  \
        printf("Cannot allocation layer (%d) where total number of layers are %d\n", layer_num, model.num_layers + 1); \
        exit(EXIT_FAILURE);                                                                                            \
    }                                                                                                                  \
    mat_alloc(model.weights[layer_num - 1], model.layers[layer_num], model.layers[layer_num - 1], weight_name);        \
    mat_alloc(model.bias[layer_num - 1], model.layers[layer_num], 1, bias_name);                                       \
    mat_alloc(model.layer_output[layer_num], model.layers[layer_num], 1, output_name);                                 \
    mat_alloc(model.layer_error[layer_num], model.layers[layer_num], 1, error_name);

#define alloc_model_from_file(model, file, weight_name, bias_name) \
    fscanf(file, "%d\n", &model->num_layers)                       \
        model->layers[model->num_layers + 1];                      \
    for (int i = 0; i <= num_layers; i++)                          \
        fscanf(file, "%d ", &model->layers[i]);                    \
    fscanf(file, "\n");                                            \
    fscanf(file, "%lf", &model->learning_rate);                    \
    Matrix weight_name[model->num_layers], bias_name[model->num_layers];

void write_model(Model *model, char *filename)
{
    FILE *file;
    file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Structure of model
    // num_layers
    // layers
    // learning rate
    // weights
    // bias
    fprintf(file, "%d\n", model->num_layers);
    for (int i = 0; i < model->num_layers + 1; i++)
        fprintf(file, "%d;", model->layers[i]);
    fprintf(file, "\n");
    fprintf(file, "%f\n", model->learning_rate);

    for (int i = 0; i < model->num_layers; i++)
    {
        mat_write(&model->weights[i], file);
        mat_write(&model->bias[i], file);
    }
}

int read_model_matrices(Model *model, FILE *file)
{
    // TODO: add error handling
    for (int i = 0; i < model->num_layers; i++)
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
        // mat_rand(&model->weights[i]);
        mat_rand_xavier_inplace(&model->weights[i]);
        mat_normalize_inplace(&model->weights[i]);

        // mat_rand(model->bias[i]);
        // mat_normalize(model->bias[i]);
        mat_zeros_inplace(&model->bias[i]);
    }

    return EXIT_SUCCESS;
}
int forward_pass(Matrix *output, Matrix *input, Model *model, int debug)
{

    mat_copy_vals(&model->layer_output[0], input);
    mat_normalize_inplace(&model->layer_output[0]);
    mat_map_inplace(&model->layer_output[0], model->activate);

    for (int i = 0; i < model->num_layers; i++)
    {

        mat_mul(&model->layer_output[i + 1], &model->weights[i], &model->layer_output[i]);
        mat_add(&model->layer_output[i + 1], &model->layer_output[i + 1], &model->bias[i]);
        mat_map_inplace(&model->layer_output[i + 1], model->activate);

        if (debug)
        {
            printf("\ni: %d\n", i);
            printf("\nLayer_output[%d] (%d, %d)\n", i, model->layer_output[i].rows, model->layer_output[i].columns);
            mat_print(&model->layer_output[i]);

            printf("\nWeights[%d] (%d, %d)\n", i, model->weights[i].rows, model->weights[i].columns);
            mat_print(&model->weights[i]);

            printf("\nBias[%d] (%d, %d)\n", i, model->bias[i].rows, model->bias[i].columns);
            mat_print(&model->bias[i]);

            printf("\nlayer_output[%d]: sigmoid(mul(model->weights[%d], layer_output[%d]) + model->bias[%d])\n", i + 1, i, i, i);
            mat_print(&model->layer_output[i + 1]);
        }

    }
    mat_copy_vals(output, &model->layer_output[model->num_layers]);
    return EXIT_SUCCESS;
}

double backward_pass(
    Matrix *output,
    Matrix *input,
    Model *model,
    int debug)
{

    mat_copy_vals(&model->layer_output[0], input);
    mat_normalize_inplace(&model->layer_output[0]);
    mat_map_inplace(&model->layer_output[0], model->activate);

    if (debug)
    {
        printf("\nlayer_output[%d] (input layer): \n", 0);
        mat_print(&model->layer_output[0]);

        printf("\nExpected Output: \n");
        mat_print(output);
    }

    for (int i = 0; i < model->num_layers; i++)
    {
        if (debug)
        {
            printf("\nmodel->weights[%d]\n", i);
            mat_print(&model->weights[i]);

            printf("\nmodel->bias[%d] \n", i);
            mat_print(&model->bias[i]);
        }

        // frontier[i+1] = mat_mul(model->weights[i], frontier[i]);
        // mat_add(frontier[i+1], model->bias[i], 1);
        // mat_map(frontier[i+1], activation, 1);
        mat_mul(&model->layer_output[i + 1], &model->weights[i], &model->layer_output[i]);

        if (debug)
        {
            printf("\nlayer_output[%d]: mat_mul(model->weights[%d], layer_output[%d])\n", i + 1, i, i);
            mat_print(&model->layer_output[i + 1]);
        }

        mat_add(&model->layer_output[i + 1], &model->layer_output[i + 1], &model->bias[i]);

        if (debug)
        {
            printf("\nlayer_output[%d]: mat_mul(model->weights[%d], layer_output[%d]) + model->bias[%d]\n", i + 1, i, i, i);
            mat_print(&model->layer_output[i + 1]);
        }

        mat_map_inplace(&model->layer_output[i + 1], model->activate);

        if (debug)
        {
            printf("\nlayer_output[%d]: sigmoid(mat_mul(model->weights[%d], layer_output[%d]) + model->bias[%d])\n", i + 1, i, i, i);
            mat_print(&model->layer_output[i + 1]);
        }
    }

    // Matrix* error[model->num_layers+1];
    // Matrix* neg_frontier = mat_scale(frontier[model->num_layers], -1, 0);
    // error[model->num_layers] = mat_add(O, neg_frontier, 0);
    int last = model->num_layers;
    Matrix neg_layer_output;
    mat_alloc_from_matrix(neg_layer_output, model->layer_output[last], nlo);

    mat_scale_in_place(&neg_layer_output, -1);
    mat_add(&model->layer_error[last], output, &neg_layer_output);

    if (debug)
    {
        printf("\n neg_output = -neg_layer_output[%d: model->num_layers]\n", model->num_layers);
        // printf("\n neg_output = -output\n");
        mat_print(&neg_layer_output);

        printf("error[%d: model->num_layer] = O + neg_layer_output\n", model->num_layers);
        mat_print(&model->layer_error[last]);
    }

    for (int i = model->num_layers - 1; i >= 0; i--)
    {
        // gradient = error[i+1] * lr * dsigmoid(output: frontier[i+1])
        // d_weights[i] = gradient x input: frontier[i]

        // Matrix* weights_t = mat_transpose(model->weights[i], 0);
        // error[i] = mat_mul(weights_t, error[i+1]);
        Matrix weights_t;
        mat_alloc(weights_t, model->weights[i].columns, model->weights[i].rows, wt);
        mat_transpose(&weights_t, &model->weights[i]);
        mat_mul(&model->layer_error[i], &weights_t, &model->layer_error[i + 1]);

        // Matrix* gradient = mat_map(frontier[i+1], dactivation, 0);
        // mat_scalar_mul(gradient, error[i+1], 1);
        // mat_scale(gradient, model->learning_rate, 1);
        Matrix gradient;
        mat_alloc_from_matrix(gradient, model->layer_output[i + 1], g);
        mat_map(&gradient, &model->layer_output[i + 1], model->dactivate);
        mat_haddard_inplace(&gradient, &model->layer_error[i + 1]);
        mat_scale_in_place(&gradient, model->learning_rate);
        mat_add(&model->bias[i], &model->bias[i], &gradient);

        // Matrix* neg_gradient = mat_scale(gradient, -1, 0);
        // mat_add(model->bias[i], gradient, 1);
        // Matrix neg_gradient;
        // mat_alloc_from_matrix(neg_gradient, gradient, ng);
        // mat_scale_in_place(&neg_gradient, -1);
        // mat_add(&model->bias[i], &model->bias[i], &neg_gradient);

        // Matrix* frontier_t =  mat_transpose(frontier[i], 0);
        // Matrix* d_weights = mat_mul(gradient, frontier_t);
        Matrix frontier_t, d_weights;
        mat_alloc(frontier_t, model->layer_output[i].columns, model->layer_output[i].rows, ft);
        mat_alloc(d_weights, gradient.rows, frontier_t.columns, dw);

        mat_transpose(&frontier_t, &model->layer_output[i]);
        mat_mul(&d_weights, &gradient, &frontier_t);
        mat_scale_in_place(&d_weights, -1);

        // mat_scale(d_weights, -1, 1);
        // mat_add(model->weights[i], d_weights, 1);
        mat_add(&model->weights[i], &model->weights[i], &d_weights);

        if (debug)
        {

            printf("\ni : %d\n", i);
            printf("\n weights_t= transpose(model->weights[%d])\n", i);
            mat_print(&weights_t);

            printf("\n error[%d]\n", i);
            mat_print(&model->layer_error[i]);

            printf("\nlayer_output[%d]\n", i + 1);
            mat_print(&model->layer_output[i + 1]);

            printf("\n gradient = map(layer_output[%d], dsigmoid) * error[%d] * lr: %f\n", i + 1, i + 1, model->learning_rate);
            mat_print(&gradient);
        }
    }

    double e[model->layer_error[last].rows];
    mat_to_arr(e, &model->layer_error[last], model->layer_error[last].rows);

    double mse = 0;
    for (int i = 0; i < model->num_layers + 1; i++)
    {
        mse += pow(e[i], 2);
    }

    return mse;
}
