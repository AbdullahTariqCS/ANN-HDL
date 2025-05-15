#include "model.c"
#include "util/sigmoid.c"

int main(int argc, char* argv[])
{
    int num_layers = 3; 
    int layers[] = {2, 2, 1};
    double learning_rate = 0.1; 
    int epochs = 1; 
    int debug = 0; 

    if (argc > 1) epochs = atoi(argv[1]);
    if (argc > 2) debug = atoi(argv[2]); 


    Model model;
    //model hyper parameters and weights initialized in the main scope
    alloc_model(model, learning_rate, num_layers-1, layers, sigmoid, dsigmoid, w, b, o, e);
    alloc_model_input_layer(model, o0, e0);  

    alloc_model_layer(model, 1, w0, b0, o1, e1);
    alloc_model_layer(model, 2, w1, b1, o2, e2);

    initialize_model_weights(&model); 
    srand(time(NULL));
    
    for(int i = 0; i < epochs; i++)
    {
        int a = rand() % 2, b = rand() % 2; 
        // int a = 1, b = 1; 
        int o = a ^ b; 
        
        double input_arr[2] = {(double)a, (double)b};
        double output_arr[1] = {(double)o};
        Matrix input, output; 

        mat_alloc_from_arr(input, 2, 1, input_arr); 
        mat_alloc_from_arr(output, 1, 1, output_arr);

        backward_pass(&output, &input, &model, debug);
    }

    
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            double input_arr[2] = {(double)i, (double)j};
            Matrix input, result; 
            mat_alloc_from_arr(input, 2, 1, input_arr); 

            forward_pass(&result, &input, &model, debug);
            printf("Result for %d^%d: ", i, j); 
            mat_print(&result);
        }
    }
    return 0;
}