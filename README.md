# GERRY-Iterative-Repair
GERRY Iterative Repair Open Source Implementation for Small Satellite Power, Attitude and Mission Task Schedule Management

Default Test Cases: (select in the main function)
1. Four simple tasks, duration and calandar constraints only
2. Eight tasks with state variables, resources, duration, temporal constraints, and calandar constraints
3. Randomized gaussian tasks with state, resource, duration, and calandar constraints
4. Twelve templated tasks with energy resource and spread out release/deadlines
5. Twelve templated tasks with energy resources but no release times or deadlines

Default Resources:
1. Energy
2. CPU
3. Memory

Default State Variables:
1. Heater Status = {On, Heated, Off, Cool}
2. Slew Status = {Pointed, Tracking, Broken, Saturated}
3. Battery Status = {Charging, Discharging, Near Upper Limit, Near lower Limit}
4. Data Quality = {Poor, Good, Anomalous, New}
5. Solar Condition = {Nominal, Solar Flare}

Default Template Tasks: (used to add to schedule)
1. Wait
2. Request Scheduling Aide
3. Solar Recharge

Default Helper Functions: (used to apply constraints to tasks)
1. slew
2. solarRechargeEnergyFromTime
3. solarRechargeTimeFromEnergy
4. timeTillRequiredSunlight
5. sunlightStatus
6. findEclipse
7. trueAnomaly
8. timeSincePeriapse
9. nextTrueAnomaly
10. peri2Equatorial
11. equatorial2Peri
