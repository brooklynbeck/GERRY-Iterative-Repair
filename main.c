#include "gerry.h"
#include "helpers.h"

/*  GERRY-Iterative-Repair
    Copyright (C) 2026  Brooklyn Beck

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see https://www.gnu.org/licenses/.
*/

/*  Function main
    Initializes, runs, and logs scheduling simulation using the GERRY algorithm and iterative repair
    GERRY algorithm: https://www.researchgate.net/publication/3114264_Scheduling_and_Rescheduling_with_Iterative_Repair
    Assumptions: 
        Tasks are not preemptive and cannot be parallelized
        Task failures are not caused by permanent hardware or software failure.
        A valid initial schedule is provided
        Safety-critical functions are independent of GERRY, so science task failure will not result in mission failure
        The domain S represents a closed world
*/

int main(int argc, char *argv[])
{
    //tracks cpu time over one test case
    int casenum; //see initializeDomain.c for case descriptions
    int seed1, seed2, schedulingHorizon, numTasks; 
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    if (argc == 1)
    {
        printf("For further use, please provide input integers according to use case:\n");
        printf("Usage 1: %s\nDefault GERRY run\n", argv[0]);
        printf("Usage 2: %s <casenum>\nSelect from prewritten initial schedules (Descriptions on GitHub)\n", argv[0]);
        printf("Usage 3: %s <seed1> <seed2> <schedulingHorizon> <numTasks>\nRandomize an initial schedule\n\n", argv[0]);
        casenum = 1;
        seed1 = 1;
        seed2 = 1;
        schedulingHorizon = 5;
        numTasks = 100;
        printf("Default GERRY run\n");
    }
    else if (argc == 2)
    {
        casenum = atoi(argv[1]);
        seed1 = 1;
        seed2 = 1;
        schedulingHorizon = 5;
        numTasks = 100;
    }
    else if (argc ==5)
    {
        casenum = 3;
        seed1 = atoi(argv[1]);
        seed2 = atoi(argv[2]);
        schedulingHorizon = atoi(argv[3]);
        numTasks = atoi(argv[4]);
    }
    else 
    {
        printf("Error: Please provide input integers according to use case:\n");
        printf("Usage 1: %s\nDefault GERRY run\n", argv[0]);
        printf("Usage 2: %s <casenum>\nSelect from prewritten initial schedules (Descriptions on GitHub)\n", argv[0]);
        printf("Usage 3: %s <seed1> <seed2> <schedulingHorizon> <numTasks>\nRandomize an initial schedule\n", argv[0]);
        return 1;
    }
    
    int generic = 1; //uses generic template tasks instead of helper functions if generic==1
    int printOn = 1; //1 to print results, 0 to turn off
    
    FILE *SimResults;
    SimResults = fopen("simResults.txt", "a");
    fprintf(SimResults, "%d %d ", seed1, seed2);
    fclose(SimResults);
    
    printf("Beginning Simulation %d:%d\n", seed1, seed2);

    // sim data
    
    double failureChance = 0.01; // 1% chance of failure for each task
    int timelineLength = 2000;
    
    // initialize rng, log, tasks, schedule, and simulation data
    srand(seed1); //set rng through program arguments for consistant testing
    initializeLog();
    struct domain Domain = initializeDomain(casenum, timelineLength, numTasks, generic);
    struct schedule * headS = initializeSchedule(Domain);
    struct schedule * currentS = headS;
    struct simData data = {0};
    
    if (printOn == 1)
    {
        printf("Initial ");
        printSchedule(headS);
        //optional timeline printing, before repair:
        //printResourceTimelines(Domain);
        //printStateTimelines(Domain);
    }
    
    double simTime = 0.0;
    logTaskConstraints(headS);
    
    srand(seed2);
    int iter=0;
    int maxIter = 1000; //need to increase once working fully TBD back to 1000
    // run monte carlo simulation and iterative repair until all tasks are complete
    while(currentS != NULL && iter < maxIter)
    {
        currentS = iterativeRepair(currentS, headS, &Domain, &data, failureChance, schedulingHorizon);
        simTime += currentS->executionTime;
        currentS = currentS->next;
        iter +=1;
    }
    // log and print final results
    logSimData(data);
    
    if (printOn == 1)
    {
        printf("Final ");
        printSchedule(headS);
        //optional timeline printing, after repair:
        //printResourceTimelines(Domain);
        //printStateTimelines(Domain);
    }
    
    logTaskConstraints(headS);
    logWCET(headS);
    logOracle(headS, &Domain);
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    if (printOn == 1)
        printf("CPU time used: %f seconds\n", cpu_time_used);
    
    printf("Completed Simulation %d:%d\n", seed1, seed2);
    
    SimResults = fopen("simResults.txt", "a");
    fprintf(SimResults, "%lf\n", cpu_time_used);
    fclose(SimResults);
    
    freeSchedule(headS);
    freeDomain(&Domain);
    
    return data.errorCode;
    //return 0;
}
