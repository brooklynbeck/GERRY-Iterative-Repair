#!/bin/bash
# This shell script runs GERRY and the CSP oracle code over 3 sets of variables: initial seed, initial number of tasks, and planning horizon
dk dir -p simulationResults
#set 1
for k in $(seq 1 10); do
	> cspResults.txt
	> simResults.txt
	horizon=5
	tasks=100
	countOuterStart=$((10 * k - 9))
	countOuterEnd=$((10 * k))
	countInner=100
	for i in $(seq $countOuterStart $countOuterEnd); do
		for j in $(seq $countInner); do
			timeout 10s ./gerry $i $j $horizon $tasks
			timeout 120s python3 oracleCSP.py
		done
	done
	cp cspResults.txt simulationResults/${tasks}_${horizon}cspResults${countOuterStart}-${countOuterEnd}.txt
	cp simResults.txt simulationResults/${tasks}_${horizon}simResults${countOuterStart}-${countOuterEnd}.txt
done
#set 2
for k in 10 50 100 150 200
do
	> cspResults.txt
	> simResults.txt
	horizon=5
	tasks=${k}
	countOuterStart=1
	countOuterEnd=20
	countInner=100
	for i in $(seq $countOuterStart $countOuterEnd); do
		for j in $(seq $countInner); do
			timeout 10s ./gerry $i $j $horizon $tasks
			timeout 120s python3 oracleCSP.py
		done
	done
	cp cspResults.txt simulationResults/tasks${tasks}_${horizon}cspResults${countOuterStart}-${countOuterEnd}.txt
	cp simResults.txt simulationResults/tasks${tasks}_${horizon}simResults${countOuterStart}-${countOuterEnd}.txt
done
#set 3
for k in 1 2 5 10 20
do
	
	> cspResults.txt
	> simResults.txt
	horizon=${k}
	tasks=100
	countOuterStart=1
	countOuterEnd=20
	countInner=100
	for i in $(seq $countOuterStart $countOuterEnd); do
		for j in $(seq $countInner); do
			timeout 10s ./gerry $i $j $horizon $tasks
			timeout 120s python3 oracleCSP.py
		done
	done
	cp cspResults.txt simulationResults/horizon${tasks}_${horizon}cspResults${countOuterStart}-${countOuterEnd}.txt
	simResults.txt simulationResults/horizon${tasks}_${horizon}simResults${countOuterStart}-${countOuterEnd}.txt
done
