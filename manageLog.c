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

/*  Function initializeLog
    Clears previous log and records time of simulation
*/
void initializeLog()
{
    FILE *Log;
    Log = fopen("log.txt", "w");
    fprintf(Log, "GERRY Iterative Repair Log\n");
    time_t now = time(NULL);
    fprintf(Log, "%s", asctime(localtime(&now)));
    fprintf(Log, "Initial ");
    fclose(Log);
    
    FILE *Constraints;
    Constraints = fopen("constraints.txt", "w");
    fprintf(Constraints, "#Initial Constraint Information\n");
    fclose(Constraints);
    
    FILE *Oracle;
    Oracle = fopen("oracle.txt", "w");
    fprintf(Oracle, "#Oracle Constraint Information\n");
    fprintf(Oracle, "#name duration releaseTime deadline relativeTask energy cpu mem heaterC heaterI slewC slewI batteryC batteryI\n");
    fclose(Oracle);
    
    FILE *WCET;
    WCET = fopen("wcet.txt", "w");
    fprintf(WCET, "#WCET Task Information\n");
    fprintf(WCET, "#name wcetStartTime actualStartTime expectedExecutionTime actualExecutionTime\n");
    fclose(WCET);
    
}

/*  Function logSchedule
    Prints a full schedule to log.txt
    Parameter headS: pointer to the head of a schedule linked list
*/
void logSchedule(struct schedule * headS)
{
    //print to a log
    FILE *Log;
    Log = fopen("log.txt", "a");
    fprintf(Log, "Schedule:\n");
    struct schedule * current;
    current = headS;
    //printf("logging check\n");
    while(&current->T != NULL)
    {
        //printf("works %lf-%lf %s\n", current->startTime, current->startTime + current->executionTime, current->T.name);
        //printf("test log: current %s prev %s next %s\n", current->T.name, current->prev->T.name, current->next->T.name);
        fprintf(Log, "   %lf-%lf %s\n", current->startTime, current->startTime + current->executionTime, current->T.name);
        current = current->next;
    }
    //printf("end log test\n");
    fclose(Log);
}

/*  Function logRepair
    Prints details of operations done to the schedule in log.txt
*/
void logRepair(struct schedule *currentS, int repairType){
    //print to a log
    FILE *Log;
    Log = fopen("log.txt", "a");
    switch(repairType){
        case 0:
            fprintf(Log, "Updated %s with Monte Carlo Simulation\n", currentS->T.name);
            break;
        case 1:
            fprintf(Log, "Moving %s\n", currentS->T.name);
            break;
        case 2:
            fprintf(Log, "Adding %s\n", currentS->next->T.name);
            break;
        case 3:
            fprintf(Log, "Removing %s\n", currentS->T.name);
            break;
        case 4:
            fprintf(Log, "Requesting Aide\n");
            break;
    }
    fclose(Log);
}

/*  Function describeConflictType
    Returns a description of a conflict based on the input type integer
*/
const char* describeConflictType(int conflictType){
    switch(conflictType){
        case 0:
            return "Error, not a conflict type";
            break;
        case 1:
            return "Task failure";
            break;
        case 2:
            return "Halting problem";
            break;
        case 3:
            return "Previous task completed after the task's deadline (task failure)";
            break;
        case 4:
            return "Task completed before the next task's release time";
            break;
        case 5:
            return "Task completed before the next task's start time";
            break;
        case 6:
            return "Task completed after the next task's start time";
            break;
        case 7:
            return "TC violation: Min distance between tasks";
            break;
        case 14:
            return "TC violation: Relative Task (previous)";
            break;
        case 15:
            return "TC violation: Relative Task (next)";
            break;
        case 8:
            return "Task violates a resource minimum";
            break;
        case 9:
            return "Task violates a resource maximum";
            break;
        case 10:
            return "Task violates a resource preConstraint";
            break;
        case 11:
            return "Task violates a resource maintainConstraint";
            break;
        case 12:
            return "Task violates a state preConstraint";
            break;
        case 13:
            return "Task violates a state maintainConstraint";
            break;
        default:
            printf("Invalid case\n");
            abort();
    }
}

/*  Function logConflictList
    Prints a list of conflicts to log.txt
*/
void logConflictList(struct conflict* conflicts){
    FILE *Log;
    Log = fopen("log.txt", "a");
    int i = 0;
    int maxConflictList = 10;
    const char* conflictTypeDescription;
    fprintf(Log, "Identified Conflict List:\n");
    while (conflicts[i].conflictType != 0 && abs(conflicts[i].conflictType) <= 30 && i<conflicts[0].numConflicts)
    {
        conflictTypeDescription = describeConflictType(conflicts[i].conflictType);
        //printf("test1 manLog%d\n", i+1);
        //printf("test2 manLog%s\n", conflictTypeDescription);
        //printf("test3 manLog%s\n", conflicts[i].T->T.name);
        fprintf(Log, "%d. Conflict for %s: %s\n", i+1, conflicts[i].T->T.name, conflictTypeDescription);
        i+=1;
    }
    fclose(Log);
}

/*  Function logConflict
    Prints a conflict to log.txt
*/
void logConflict(struct conflict conflicts){
    FILE *Log;
    Log = fopen("log.txt", "a");
    const char* conflictTypeDescription = describeConflictType(conflicts.conflictType);
    fprintf(Log, "Repairing Conflict for %s: %s\n", conflicts.T->T.name, conflictTypeDescription);
    fclose(Log);
}

/*  Function logUpdates
    Prints parsing text to log.txt
*/
void logUpdates(int updateID){
    FILE *Log;
    Log = fopen("log.txt", "a");
    switch(updateID)
    {
        case 0:
            fprintf(Log, "Begin GERRY Iteration\n");
            break;
        case 1:
            fprintf(Log, "Schedule Validated by GERRY, execute next task\n\n");
            break;
    }
    fclose(Log);
}

/*  Function logSimData
    Prints collected metadata about the simulation to log.txt
    Parameter: struct containing simulation metadata, including the number of task failures
*/
void logSimData(struct simData data){
    FILE *Log;
    Log = fopen("log.txt", "a");
    fprintf(Log, "Simulation Complete\n");
    fprintf(Log, "Total Failures: %d\n", data.numFail);
    fprintf(Log, "Total Early Tasks: %d\n", data.numEarly);
    fprintf(Log, "Total Late Tasks: %d\n", data.numLate);
    fprintf(Log, "Total Resource Violations: %d\n", data.numResourceViolation);
    fprintf(Log, "Total State Violations: %d\n", data.numStateViolation);
    fprintf(Log, "Error Code: %d\n", data.errorCode);
    fclose(Log);
    
    FILE *SimResults;
    SimResults = fopen("simResults.txt", "a");
    fprintf(SimResults, "%d %d ", data.numFail, data.errorCode);
    fclose(SimResults);
}

void logTaskConstraints(struct schedule * headS){
    int additionalConstraints = 1; //set to 1 to print additional information
    FILE *Log;
    Log = fopen("constraints.txt", "a");
    fprintf(Log, "Task Constraints:\n");
    fprintf(Log, "name|startTime|executionTime|releaseTime|deadline\n");
    if (additionalConstraints!=0)
    {
        fprintf(Log, "TC constraints: minTimeAfter|relativePrev|relativeNext|relativeTask\n");
        fprintf(Log, "RR constraints: resource|preConstraint|maintainConstraint|preImpact|maintainImpact|postImpact\n");
        fprintf(Log, "SR constraints: stateVariable|preConstraint|maintainConstraint|preImpact|maintainImpact|postImpact\n");
    }
    struct schedule * current;
    current = headS;
    while(current->T.name != NULL)
    {
        fprintf(Log, "%s|", current->T.name);
        //wd
        fprintf(Log, "%lf|%lf|", current->startTime, current->executionTime);
        //wc
        fprintf(Log, " %lf|%lf\n", current->T.WC->releasetime, current->T.WC->deadline);
        //tc
        if (additionalConstraints!=0)
        {
            fprintf(Log, "  %d TC constraints\n", current->T.numTC);
            for (int i=0; i<current->T.numTC; i++)
            {
                fprintf(Log, "      %lf|%d|%d|%s\n", 
                    current->T.TC[i].minTimeAfter, current->T.TC[i].relativePrev, current->T.TC[i].relativeNext, current->T.TC[i].relativeTask->name);
            }
            //rr 
            fprintf(Log, "  %d RR constraints\n", current->T.numRR);
            for (int i=0; i<current->T.numRR; i++)
            {
                fprintf(Log, "      %s|%lf|%lf|%lf|%lf|%lf\n",
                    current->T.RR[i].R->name, current->T.RR[i].preConstraint, current->T.RR[i].maintainConstraint, current->T.RR[i].preImpact,
                    current->T.RR[i].maintainImpact, current->T.RR[i].postImpact);
            }
            //sr
            fprintf(Log, "  %d SR constraints\n", current->T.numSR);
            for (int i=0; i<current->T.numSR; i++)
            {
                fprintf(Log, "      %s|%d|%d|%d|%d|%d\n",
                    current->T.SR[i].SV->name, current->T.SR[i].preConstraint, current->T.SR[i].maintainConstraint, current->T.SR[i].preImpact,
                    current->T.SR[i].maintainImpact, current->T.SR[i].postImpact);
            }
        }
        
        current = current->next;
    }
    fclose(Log);
}

void logOracle(struct schedule * headS, struct domain *problemDomain) {
    FILE *Log;
    Log = fopen("oracle.txt", "a"); 
    
    double startTime = 0.0;
    struct schedule * currentS;
    currentS = headS;
    
    while(currentS->T.name != NULL)
    {
        if (strcmp(currentS->T.name, "Wait")==0)
            {
                currentS = currentS->next;
            }
        char name[80];
        strcpy(name, currentS->T.name);
        remove_spaces(name);
        
        if(currentS->executionTime < 0)
        {
            strcat(name, "(Failed)");
            fprintf(Log, "%s %lf %lf ", name, currentS->T.avExecutionTime, currentS->T.WC->releasetime);
        }
        else
            fprintf(Log, "%s %lf %lf ", name, currentS->executionTime, currentS->T.WC->releasetime);
        fprintf(Log, "%lf ", currentS->T.WC->deadline);
        //tc
        if (currentS->T.numTC >0)
        {
            if (currentS->T.TC->relativeNext == 1)
                fprintf(Log, "1 ");
            else
                fprintf(Log, "0 ");
        }
        else
            fprintf(Log, "0 ");
        //rr
        int numResources = problemDomain->numResources;
        double resources[numResources];
        for (int i=0; i<currentS->T.numRR; i++)
        {
            for (int j=0; j<numResources; j++)
            {
                if (strcmp(currentS->T.RR[i].R->name, problemDomain->resources[j].name) == 0)
                    resources[j]=currentS->T.RR[i].preImpact + currentS->T.RR[i].postImpact + currentS->executionTime*currentS->T.RR[i].maintainImpact/60;
            }
        }
        for (int i=0;i<numResources;i++)
        {
            fprintf(Log, "%lf ", resources[i]);
        }
        //sr
        int numStateVariables = problemDomain->numStateVariables;
        int stateConstraints[numStateVariables];
        int stateImpacts[numStateVariables];
        for (int i=0;i<numStateVariables;i++)
        {
            stateConstraints[i]=-1;
            stateImpacts[i]=-1;
        }
        for (int i=0; i<currentS->T.numSR; i++)
        {
            for (int j=0; j<numStateVariables; j++)
            {
                if ((currentS->executionTime > 0) && strcmp(currentS->T.SR[i].SV->name, problemDomain->stateVariables[j].name)==0)
                {
                    if(currentS->T.SR[i].preConstraint != -1)
                        stateConstraints[j]=currentS->T.SR[i].preConstraint;
                    if(currentS->T.SR[i].maintainConstraint != -1)
                        stateConstraints[j]=currentS->T.SR[i].maintainConstraint;
                    if(currentS->T.SR[i].preImpact != -1)
                        stateImpacts[j]=currentS->T.SR[i].preImpact;
                    if(currentS->T.SR[i].postImpact != -1)
                        stateImpacts[j]=currentS->T.SR[i].postImpact;
                }
            }
        }
        for (int i=0; i<numStateVariables; i++)
        {
            fprintf(Log, "%d %d ", stateConstraints[i], stateImpacts[i]);
        }
        fprintf(Log, "\n");
        currentS = currentS->next;
    }
    fclose(Log);
}

void logWCET(struct schedule * headS) {
    FILE *Log;
    char name[80];
    Log = fopen("wcet.txt", "a");
    
    double startTime = 0.0;
    struct schedule * current;
    current = headS;
    
    while(current->T.name != NULL)
    {
        if (strcmp(current->T.name, "Wait")==0)
        {
            current = current->next;
        }
        //skip added tasks
        else if (strstr(current->T.name, "GENERIC") != NULL)
            current=current->next;
        //skip task that previously failed
        else if ((current->prev->T.name != NULL) && (strcmp(current->T.name, current->prev->T.name)==0))
        {
            //unless final task!!!
            if (current->next->T.name == NULL)
            {
                FILE *SimResults;
                SimResults = fopen("simResults.txt", "a");
                fprintf(SimResults, "%lf %lf ", startTime, current->startTime + current->executionTime);
                fclose(SimResults);
            }
            current = current->next;
        }
        else if ((current->next->T.name != NULL) && (strcmp(current->T.name, current->next->T.name)==0))
        {
            //note failed task
            fprintf(Log, "TASK FAILURE: %s\n", current->T.name);
            
            //still propogate startTime of first failure
            if (current->T.sdExecutionTime >= 100) //wait and solar recharge don't update based on sd
                startTime += current->T.avExecutionTime;
            else
                startTime += current->T.avExecutionTime + 2*current->T.sdExecutionTime;
            
            current = current->next;
            
        }
        else
        {
            if (startTime < current->T.WC->releasetime)
                startTime = current->T.WC->releasetime;
            strcpy(name, current->T.name);
            remove_spaces(name);
            
            fprintf(Log, "%s %lf %lf %lf %lf\n", name, startTime, current->startTime, current->T.avExecutionTime, current->executionTime);
    
            if (current->T.sdExecutionTime >= 100) //wait and solar recharge don't update based on sd
                startTime += current->T.avExecutionTime; //added tasks won't have an average execution time, 0 by design
            else
                startTime += current->T.avExecutionTime + 2*current->T.sdExecutionTime;
            
            if (current->next->T.name == NULL)
            {
                FILE *SimResults;
                SimResults = fopen("simResults.txt", "a");
                fprintf(SimResults, "%lf %lf ", startTime, current->startTime + current->executionTime);
                fclose(SimResults);
            }
            
            current = current->next;
        }
    }
    
    fclose(Log);
    
    
}

void remove_spaces(char *str) {
    int read_ptr = 0;
    int write_ptr = 0;

    // Loop through the entire string
    while (str[read_ptr] != '\0') {
        // If the current character is not a space, copy it
        if (str[read_ptr] != ' ') {
            str[write_ptr] = str[read_ptr];
            write_ptr++;
        }
        read_ptr++;
    }

    // Add the null terminator to mark the end of the new string
    str[write_ptr] = '\0';
}

