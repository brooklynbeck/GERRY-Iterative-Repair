#include "gerry.h"
#include "helpers.h"

/*  Function mcs
    
    Determines if a scheduled task fails and changes the execution time of a scheduled task in a monte carlo simulation
    
    Parameters 
        S - specific task to be simulated
        failureChance - chance of task failure
    
    Returns the schedule struct with updated execution time
*/
struct schedule * mcs(struct schedule * S, double failureChance)
{
    //ignore any tasks with a standard deviation of 100, flag for unalterable task (ex "wait" or "solar recharge")
    if(S->T.sdExecutionTime ==100) //strcmp(S->T.name, "Wait") == 0 || strcmp(S->T.name, "Solar Recharge") == 0)
        return S;
    // failure check uses flat distribution
    double checkFailure = (double)rand() / (double)RAND_MAX;
    if(checkFailure > 1-failureChance)
        S->executionTime = -1.00;
    // execution time uses a gaussian distribution
    else
        S->executionTime = gaussianDistribution(S->T.avExecutionTime, S->T.sdExecutionTime);
    return S;
}

/*  Function gaussianDistribution
    
    Generates a random number on a gaussian distribution using the Box–Muller transform
    Parameters: mean and standard deviation of desired distribution
    Returns a random number
*/
double gaussianDistribution(double mean, double stddev)
{
    if (stddev == 100) //standard value for unalterable value
        return mean;
    double u1 = (double)rand() / RAND_MAX;
    double u2 = (double)rand() / RAND_MAX;
    double z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    double gaussianNum = mean + stddev * z;
    return gaussianNum;
}

/* Function targetVector
    
    Generates a random 3D vector using preallocated memory (vector)
*/
void targetVector(double * vector)
{
    vector[0] = ((double)rand() *2 / RAND_MAX) - 1.0;
    vector[1] = ((double)rand() *2 / RAND_MAX) - 1.0;
    vector[2] = ((double)rand() *2 / RAND_MAX) - 1.0; //assume can be in different hemispheres
    //vector[2] = ((double)rand()/ RAND_MAX); //assume all in same hemisphere
    //double magnitude = sqrt(vector[0]*vector[0] + vector[1]*vector[1] +vector[2]*vector[2]);
}



