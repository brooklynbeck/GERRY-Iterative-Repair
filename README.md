# Overview: 

GERRY iteratively repairs a real-time task schedule with planning domain knowledge and constraints, building on the Waltz Algorithm to maintain arc consistency since repairs impact tasks and constraints beyond each repaired task. GERRY is a published algorithm with demonstrably high-quality results and a low-complexity computational footprint. GERRY relies on an initial working schedule. It integrates constraints from both scheduling and planning - namely task worst case execution time (WCET), priorities, other resources requirements e.g., energy, and states. Unlike planners, GERRY does not propagate a full domain model to maintain a computationally lightweight footprint.

The following assumptions are made for the implementation: 
- Tasks are non-preemptive and cannot be parallelized. 
- Task failures are not caused by permanent hardware or software failure. 
- A valid initial schedule is provided. 
- Safety-critical functions are independent of GERRY, so science task failure will not result in mission failure. 
- The domain 𝑆 represents a closed world so all tasks can be described without additional variables or resources.

The GERRY algorithm runs continuously, checking for constraint conflicts as new information becomes available from onboard sensors and task requests from ground control. If conflicts are detected, the algorithm repairs the conflicts iteratively, checking for new conflicts after repairing each. An initial schedule is input, and GERRY iteratively addresses each conflict by updating task schedule timelines. In simulation, physics-based models combined with Monte Carlo inputs adjust task execution time and outcomes according to a user-defined Gaussian distribution.

More details about the algorithm and the implementation can be found in the published paper, “Iterative Repair for Small Satellite Power, Attitude and Mission Task Schedule Management”

# Usage:

The following variables in main.c should be updated by the user before simulation:
- seed1: randomizing seed for generating an initial schedule, specific to casenum = 3.
- seed2: randomizing seed for simulating task execution of the initial schedule.
- schedulingHorizon: number of tasks considered when searching for conflicts.
- numTasks: number of initial tasks, specific to casenum = 3.
- generic: determines the types of template tasks available to the algorithm, set to 1 if casenum = 3.
- printOn: prints initial and final schedule values if printOn == 1. Printing additional values, such as tracked resource and state timelines, can be adjusted.
- casenum: specifies which of the default test cases to run. Set to values between 1 and 5 for predefined test cases. Modify or add new cases in initializeDomain.c.
- failureChance: determines the chance of task failure. Set at 0.01 = 1% failure rate initially.
- timelineLength: estimate of final execution time of the schedule. Initially set at 2000 as a conservative estimate, increase as needed.

# Output:

The implementation outputs five text documents with various result markers. Additionally, initial and final schedules can be printed directly if printOn ==1 in main.c. The output documents are described below:
- log.txt  - logs the schedule at every repair and task execution, along with detected constraint violations at every timestep
- constraints.txt - lists every task constraint in the initial schedule before the schedule is simulated and updated by GERRY
- wcet.txt - logs how the initial schedule would be executed as a static schedule assuming worst case task execution. Failed tasks are noted. Each task lists the wcet start time, the actual start time applying GERRY, the expected execution time, and the actual execution time after simulating updates.
- simResults.txt - adds one line for each simulation, tracking the randomizer seeds, the number of task failures, the error code, the WCET schedule length, the schedule length after applying GERRY, and the CPU time to run the simulation. This is the primary result for large scale testing over many randomizer seeds.
- oracle.txt - lists updated schedule constraints, excluding wait tasks, in the format {name duration releaseTime deadline relativeTask energy cpu mem heaterC heaterI slewC slewI batteryC batteryI}. This document is designed to be the input to a CSP scheduler implemented using Google’s OR-Tools, described in greater detail in a further section.

# Main features

The main usable features in this implementation are the default test cases, resources, state variables, template tasks, and helper functions. 

## Default Test Cases

Default test cases are predefined initial schedules with various constraints. A default test case can be selected in main.c with the integer value casenum. The third test case is a randomized schedule of an initial length defined by the integer numTasks in main.c, useful for larger scale testing of multiple schedules. This test case uses two randomizing seeds, seed1 and seed2 in main.c, the first to randomize the initial schedule and the second to randomize the schedule updates while running the simulation.

1. Four simple tasks, duration and calendar constraints only
2. Eight tasks with state, resource, duration, temporal, and calendar constraints
3. Randomized gaussian tasks with state, resource, duration, temporal, and calendar constraints
4. Twelve templated tasks with energy resource and spread out release/deadlines
5. Twelve templated tasks with energy resources but no release times or deadlines

## Resources

Resources are the source of many task constraints. Tasks can require a specific amount of a resource prior to execution, increase the resource before, during, or after execution, or decrease the resource similarly. Global constraints maintain resource levels above and below set thresholds. Default test cases 2-5 include resource constraints.

1. Energy
2. CPU
3. Memory

## State Variables

State variables are another source of task constraints, defining qualitative rather than quantitative domain features. Tasks can require a specific state variable before execution, or impact the state before, during, or after execution. State variables begin in a specified state. Default test cases 2 and 3 include state constraints.

1. Heater Status = {On, Heated, Off, Cool}
2. Slew Status = {Pointed, Tracking, Broken, Saturated}
3. Battery Status = {Charging, Discharging, Near Upper Limit, Near lower Limit}
4. Data Quality = {Poor, Good, Anomalous, New}
5. Solar Condition = {Nominal, Solar Flare}

## Template Tasks

Template tasks are used while running the schedule through the simulation to affect time, resources, or states if conflicts are detected. These tasks can be added to the schedule as needed. Template tasks 4-9 replace Solar Recharge if the integer generic in main.c is set to 1, allowing the scheduler to react to all resources. If the integer value generic is not 1, only the first three template tasks are included and the schedule fails if non-energy resources are depleted. The generic template tasks 4-9 do not link to helper functions, instead affecting the resource an arbitrary set amount taking an arbitrary set amount of time.

1. Wait
2. Request Scheduling Aide
3. Solar Recharge
4. Generic Increase Energy
5. Generic Decrease Energy
6. Generic Increase CPU
7. Generic Decrease CPU
8. Generic Increase Memory
9. Generic Decrease Memory

## Helper Functions

Helper functions are used in initial schedule generation for test cases 1, 2, 4, and 5, and in the template task Solar Recharge. To ground the simulation in realistic orbital dynamics, the scheduler uses helper functions to predict and simulate energy usage and execution time for specific physics-grounded tasks. The helper functions, their use, and their inputs are described in greater detail in helpers.c.

- slew
- solarRechargeEnergyFromTime
- solarRechargeTimeFromEnergy
- timeTillRequiredSunlight
- sunlightStatus
- findEclipse
- trueAnomaly
- timeSincePeriapse
- nextTrueAnomaly
- peri2Equatorial
- equatorial2Peri

# Example:

Set the following (other inputs irrelevant):

```
int seed2 = 1;
int schedulingHorizon = 5;
int printOn = 1;
int casenum = 1;
double failureChance = 0.01;
int timelineLength = 2000;
```

Returns the following:

```
Beginning Simulation 1:1
Initial Schedule:
   0.000000-0.800000 Point Satellite
   0.800000-2.200000 Run Heater
   2.500000-6.000000 Take Images
   6.000000-10.500000 Process Images
Final Schedule:
   0.000000-0.828166 Point Satellite
   0.828166-2.256004 Run Heater
   2.256004-2.500000 Wait
   2.500000-5.968479 Take Images
   5.968479-10.258798 Process Images
CPU time used: 0.000769 seconds
Completed Simulation 1:1
```

# Modification for other domains:

The GERRY algorithm can be modified to work with other domains, specifically by adjusting the file initializeDomain.c. Resources, state variables, initial task schedules, and template tasks can be modified, updated, or entirely rewritten. Though ideally the iterative repair implementation would be entirely independent of the initial schedule, some functions in the file iterativeRepair.c do rely on the domain as written. When modifying the domain, keep the wait and request scheduling aide template tasks in their order. Adding or removing resources will require updating the resource updating protocols in the function chooseMethod.

# Connecting with OR-Tools:

GERRY can be compared against a hypothetically perfect scheduler with complete knowledge of all task updates. A python based CSP scheduler is included as oracleCSP.py, which returns the file cspResults.txt, which has similar use as the GERRY output file simResults.txt. A linux based shell script that uses both the gerry implementation and the CSP implementation is included as montecarlosim.sh. Information about OR-Tools can be found at https://developers.google.com/optimization. To run the shell script, first the GERRY main.c file must be modified, removing commented out arguments in the function name and the commented section that applies those arguments. To compile gerry in linux, run the included shell script initializeGerry.sh. 
