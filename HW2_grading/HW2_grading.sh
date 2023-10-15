#!/bin/bash
homeworkName=HW2
binaryName=hw2
testcasePool="public1 public2 public3 public4 public5 public6"
timeLimit=300

root=$(pwd)
outputDir=$root/output
studentDir=$root/student
testcaseDir=$root/testcase
verifyBin=$root/verifier/verify
csvFile=$root/${homeworkName}_grade.csv

chmod 500 $verifyBin

function executionCmd() {
	local argv="$testcaseDir/$1.txt $outputDir/$1.out"
	local log=$(timeout $timeLimit time -p ./$binaryName $argv 2>&1 >/dev/null)
	if [[ $log =~ "real " ]] && [[ $log =~ "user " ]] && [[ $log =~ "sys " ]]; then
		echo "$(echo "$log" | grep real | tail -1 | cut -d ' ' -f 2)"
	else
		echo "TLE"
	fi
}

function verifyCmd() {
	local argv="$testcaseDir/$1.txt $outputDir/$1.out"
	local log=$($verifyBin $argv | cat)
	if [[ $log =~ "[Success]" ]]; then
		echo "success"
	elif [[ $log =~ "[Error] Missing Cell!" ]]; then
		echo "Some cells in $1 are missing. "
	elif [[ $log =~ "[Error] Duplicated Cell!" ]]; then
		echo "Some cells in $1 are duplicated. "
	elif [[ $log =~ "[Error] Wrong Cell Number!" ]]; then
		echo "$1 has wrong cell number. "
	elif [[ $log =~ "[Error] Area utilization" ]]; then
		echo "$1 violates area utilization constraint. "
	elif [[ $log =~ "[Error] Wrong Cut Size!" ]]; then
		echo "$1 has wrong cut size. "
	else
		echo "$1 has some errors. "
	fi
}

echo "|------------------------------------------------|"
echo "|                                                |"
echo "|    This script is used for PDA $homeworkName grading.    |"
echo "|                                                |"
echo "|------------------------------------------------|"

csvTitle="student id"
for testcase in $testcasePool; do
	csvTitle="$csvTitle, $testcase cut size, $testcase runtime"
done
echo "$csvTitle, status" >$csvFile

cd $studentDir/
for studentId in *; do
	if [[ -d $studentId ]]; then
		printf "grading on %s:\n" $studentId
		cd $studentId

		rm -rf $(find . -mindepth 1 -maxdepth 1 -type d)
		tar -zxf CS6135_${homeworkName}_$studentId.tar.gz
		cd $homeworkName/

		cd src
		make clean >/dev/null
		rm -rf ../bin/*
		make >/dev/null
		cd ..

		rm -rf $outputDir/*
		cd bin/
		csvContent="$studentId"
		statusList=""
		printf "%10s | %10s | %10s | %s\n" testcase cutsize runtime status
		for testcase in $testcasePool; do
			printf "%10s | " $testcase

			cutSize=fail
			status="$testcase failed."
			runtime=$(executionCmd $testcase)
			if [[ $runtime != "TLE" ]]; then
				status=$(verifyCmd $testcase)

				if [[ $status == "success" ]]; then
					cutSize=$(cat $outputDir/$testcase.out | grep CutSize | cut -d ' ' -f 2)
				else
					statusList="$statusList $status"
				fi
			else
				statusList="$statusList $status"
			fi

			printf "%10s | %10s | %s\n" $cutSize $runtime "$status"
			csvContent="$csvContent, $cutSize, $runtime"
		done
		cd ..

		echo "$csvContent, $statusList" >>$csvFile
		cd $studentDir/
	fi
done

echo "|-----------------------------------------------------|"
echo "|                                                     |"
echo "|    Successfully generate grades to ${homeworkName}_grade.csv    |"
echo "|                                                     |"
echo "|-----------------------------------------------------|"
