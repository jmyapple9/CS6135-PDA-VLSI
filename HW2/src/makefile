CXXFLAGS = -O3 -std=c++17 # -Wall -Wextra
LIBS = -I /usr/local/include/boost/
# LIBS_ON_LAPTOP = -I "C:\Users\jmyap\boost_1_83_0"
OUTPUT_DIR = ../bin/
TESTCASE_DIR = ../testcase/
OUTPUT_DIR_PUBLIC = ../output/
PROGRAM_NAME = hw2

all:
	g++ $(CXXFLAGS) main.cpp -o $(OUTPUT_DIR)$(PROGRAM_NAME)
#	g++ $(CXXFLAGS) $(LIBS_ON_LAPTOP) main.cpp -o $(OUTPUT_DIR)$(PROGRAM_NAME)


clean:
	rm $(OUTPUT_DIR)$(PROGRAM_NAME)

run: all
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

verify:
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

runall: all
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

sample:
	g++ $(CXXFLAGS) main.cpp -o $(OUTPUT_DIR)$(PROGRAM_NAME)
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)sample.txt $(OUTPUT_DIR_PUBLIC)sample.out
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)sample.txt $(OUTPUT_DIR_PUBLIC)sample.out

sample1:
	g++ $(CXXFLAGS) main.cpp -o $(OUTPUT_DIR)$(PROGRAM_NAME)
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)sample1.txt $(OUTPUT_DIR_PUBLIC)sample1.out
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)sample1.txt $(OUTPUT_DIR_PUBLIC)sample1.out
%:
	@: