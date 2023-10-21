CXXFLAGS = -O3 -std=c++2a
LIBS = -I /usr/local/include/boost/
OUTPUT_DIR = ../bin/
TESTCASE_DIR = ../testcase/
OUTPUT_DIR_PUBLIC = ../output/
PROGRAM_NAME = hw2

all:
	g++ $(CXXFLAGS) $(LIBS) main.cpp -o $(OUTPUT_DIR)$(PROGRAM_NAME)


clean:
	rm $(OUTPUT_DIR)$(PROGRAM_NAME)

run:
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

verify:
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

runall: all
	$(OUTPUT_DIR)$(PROGRAM_NAME) $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out
	$(OUTPUT_DIR)verify $(TESTCASE_DIR)public$(filter-out $@,$(MAKECMDGOALS)).txt $(OUTPUT_DIR_PUBLIC)public$(filter-out $@,$(MAKECMDGOALS)).out

%:
	@: