#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <type_traits>
/* Function definitions ---------------------------------------------------- */
std::vector<float> Generate_Sine(size_t freq,
                        size_t numSeconds,
                        size_t sampleRate,
                        size_t framesPerBuffer,
                        size_t numChannels,
                        float  amplitude)
{
    size_t size   = numSeconds * sampleRate * numChannels;
    double cycles = static_cast<double>(numSeconds) / (1. / static_cast<double>(freq));

    std::vector<float> data(size);

    /* initialise sinusoidal wavetable */
    for(size_t i = 0; i < size; i++)
    {
        data[i] = static_cast<float>(amplitude * sin((cycles / size) * 3.1415 * 2. * i));
    }

   

    return data;
}



int analyse_rythme(){
    std::vector<float> tableau=Generate_Sine(10, 1, 100, 1, 1, 2);
    int taille = tableau.size();
    float derive[taille-1];
    float volume[taille-1];
    float maximum=0;
    float derive_double[taille-1];
    bool attaque[taille-1];
    int distance[taille-1];
    int note=0;
    int j=0;
    for(int i = 0; i < taille; i++){
            std::cout << tableau[i] << std::endl;
    }
    std::cout << std::endl;
    for(int i = 0; i < taille-1; i++){
            derive[i]=tableau[i+1]-tableau[i];
            std::cout << derive[i] << std::endl;
    }
    std::cout << std::endl;
    for(int i = 0; i < taille-1; i++){
        if(COMPARE_FLOATS(derive[i],0.0f) && tableau[i]>0){ //comparaison avec marge d'erreur, utilise la fonction de pascal
           maximum = tableau[i+1];
        }
        volume[i]=maximum;
        std::cout << volume[i] << std::endl;
    }
    std::cout << std::endl;
    for(int i = 0; i < taille-1; i++){
            derive_double[i]=volume[i+1]-volume[i];
            std::cout << derive_double[i] << std::endl;
    }
     std::cout << std::endl;
    for(int i = 0; i < taille-1; i++){
        if(COMPARE_FLOATS(derive[i],0.0f) && tableau[i]>0){ //comparaison avec marge d'erreur, utilise la fonction de pascal
           attaque[i] = 1;
        }
        else{
            attaque[i] = 0;
        }
        std::cout << attaque[i] << std::endl;
    }
    std::cout << std::endl;
    for(int i = 0; i< taille-1; i++){
        if(attaque[i]==1){
            distance[j]=i-note;
            note=i;
            std::cout << distance[j] << std::endl;
            j++;
        }
        
    }
    return 0;
}
