#include "containers.h"

typedef struct ss{
    Vector *val;
    Vector *key;
} SS;

void printvec(SS *line){
    Vector *vec = line->val;
    printf("%lu\n", iVector.Size(vec));
    unsigned long i;
    for(i = 0; i < iVector.Size(vec) - 1; i++){
        printf("%f, ", *(float *)iVector.GetElement(vec, i));
    }
    printf("%f\n", *(float *)iVector.GetElement(vec, i));
}

int main(void){
    Vector *AL = iVector.Create(sizeof(float), sizeof(float));
    float data = 4.5;
    int result = iVector.PushBack(AL, &data);
    data = 3.5;
    result = iVector.PushBack(AL, &data);
    if (result < 0) { /* Error handling */ }
    
    SS line;
    line.val = AL;

    printvec(&line);
    // Template version
    //~ #include "doubleVector.h"
    //~ doubleVector *AL = idoubleVector.Create();
    //~ int result = idoubleVector.Add(AL,4.5);
    //~ if (result < 0) { /* Error handling */ }

    return 0;
}
