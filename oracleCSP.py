from ortools.sat.python import cp_model
import time

#defines and solves the problem
def search_for_all_solutions():
    # get unique simulation data from oracle.txt
    fp = open("oracle.txt", "r")

    # set unchanging simulation data
    printResults = False
    horizon = 2000  # max time (minutes)
    # resources: energy, cpu, memory
    numResources = 3
    resourceInitial = [10, 100, 40]
    resourceMin = [0,0,0]
    resourceMax = [40,100,40]
    # states: heater, slew, battery, solar, data quality
    numStates = 5
    stateInitial = [2, 0, 1, 0, 1]
    state_max_bounds = [3, 3, 3, 1, 3]

    #placeholders
    numTasks = 0
    names = []
    duration = []
    releaseTime = []
    deadline = []
    relativeTasks = []
    resourceDemand = []
    for i in range(numResources):
        resourceDemand.append([])
    stateConstraints=[]
    stateImpacts=[]
    for i in range(numStates):
        stateConstraints.append([])
        stateImpacts.append([])

    for line in fp.readlines():
        if line[0]=="#":
            continue #skip lines starting with #
        data = line.split()
        names.append(str(data[0]))
        duration.append(float(data[1]))
        releaseTime.append(float(data[2]))
        if float(data[3]) < 0.0001:
            deadline.append(horizon)
        else:
            deadline.append(float(data[3]))
        relativeTasks.append(int(data[4]))
        temp=5
        for r in range(numResources):
            resourceDemand[r].append(float(data[temp]))
            temp+=1
        for s in range(numStates):
            stateConstraints[s].append(int(data[temp]))
            stateImpacts[s].append(int(data[temp+1]))
            temp +=2
        numTasks += 1
    fp.close()

    #change values to integers
    duration = [int(i * 1000000) for i in duration]
    releaseTime = [int(i * 1000000) for i in releaseTime]
    deadline = [int(i * 1000000) for i in deadline]
    horizon = int(horizon * 1000000)
    for r in range(numResources):
        resourceInitial[r] = int(resourceInitial[r] *1000000)
        resourceDemand[r] = [int(i * 1000000) for i in resourceDemand[r]]
        # ortools assumes starts at 0, so lower min and max by actual starting value
        resourceMin[r] = resourceMin[r]*1000000-resourceInitial[r]
        resourceMax[r] = resourceMax[r] * 1000000 - resourceInitial[r]

    # create the model
    model = cp_model.CpModel()

    # create tasks
    starts = []
    ends = []
    intervals = []
    start_states = []
    end_states = []

    for tasknum in range(numTasks):
        starts.append(model.new_int_var(releaseTime[tasknum], deadline[tasknum], names[tasknum]))
        ends.append(model.new_int_var(0, horizon, f"{tasknum}{"_end"}"))
        intervals.append(model.new_interval_var(starts[tasknum], duration[tasknum], ends[tasknum],f"{tasknum}{"_interval"}"))

        #model state constraints
        t_start_states = [model.new_int_var(0,state_max_bounds[s], f"t_{tasknum}_start_s{s}") for s in range(numStates)]
        t_end_states = [model.new_int_var(0, state_max_bounds[s], f"t_{tasknum}_start_s{s}") for s in range(numStates)]
        for s in range(numStates):
            #enforce state constraints
            if stateConstraints[s][tasknum] != -1:
                model.add(t_start_states[s]==stateConstraints[s][tasknum])
            #enforce state impacts
            if stateImpacts[s][tasknum] != -1:
                model.add(t_end_states[s]==stateImpacts[s][tasknum])
            # if no impact, state persists
            else:
                model.add(t_end_states[s]==t_start_states[s])
        start_states.append(t_start_states)
        end_states.append(t_end_states)

    # serial tasks
    overlap_con = model.add_no_overlap(intervals)

    #map sequence
    for i in range(numTasks):
        for j in range(numTasks):
            if i==j: continue
            immediate_next = model.new_bool_var(f"{i}_precedes_{j}")
            #j starts at or after i ends
            model.add(starts[j] >= ends[i]).only_enforce_if(immediate_next)
            #end state matches next task start state
            for s in range(numStates):
                model.add(start_states[j][s]==end_states[i][s]).only_enforce_if(immediate_next)
    #state constraints
    for i in range(numTasks):
        first = model.new_bool_var(f"{i}_first")
        #if first, no other tasks can go before it
        for j in range(numTasks):
            if i!=j:
                model.add(starts[i]<=starts[j]).only_enforce_if(first)
        #starting state
        for s in range(numStates):
            model.add(start_states[i][s]==stateInitial[s]).only_enforce_if(first)

    #tc constraint ordering (loose) (strict order: end == start)
    for i in range(numTasks):
        if relativeTasks[i] > 0:
            model.add(ends[i]==starts[i+1])

    # resource constraints
    resource_con = []
    for r in range(numResources):
        resource_con.append(model.add_reservoir_constraint(starts, resourceDemand[r], resourceMin[r], resourceMax[r]))

    # objective: minimize total time
    makespan = model.new_int_var(0,horizon,"makespan")
    model.add_max_equality(makespan, ends)
    model.minimize(makespan)

    # Create a solver and solve
    solver = cp_model.CpSolver()
    status = solver.solve(model)

    if status == cp_model.OPTIMAL or status == cp_model.FEASIBLE:
        if printResults:
            print(f"Total Project Duration: {solver.Value(makespan)/1000000} minutes")
            results=[]
            for i in range(numTasks):
                nextResult = [
                    starts[i],
                    solver.Value(starts[i])/1000000,
                    solver.Value(ends[i])/1000000,
                    [solver.Value(start_states[i][s]) for s in range(numStates)],
                    [solver.Value(end_states[i][s]) for s in range(numStates)]
                ]
                results.append(nextResult)

            results.sort(key=lambda x: x[1])
            for i, res in enumerate(results):
                print(f"{res[0]}: {res[1]} -> {res[2]}")

            #alternate printing method, maintain initial order for printing
            #for i in range(numTasks):
                #print(f"{starts[i]}: Start={solver.Value(starts[i])/1000000} -> End={solver.Value(ends[i])/1000000}")
        return solver.Value(makespan)/1000000
    else:
        if printResults:
            print("No valid schedule could be found under these constraints.")
        return -1, 0.0

def main():
    # record time to run
    print("Begin Oracle CSP Simulation")
    st = time.time()
    num1 = search_for_all_solutions()
    et = time.time()
    print(f"Total Simulation Run Time: {et - st} seconds")
    file = open("cspResults.txt","a")
    file.write(f"{num1} {et-st}\n")
    file.close()

main()
