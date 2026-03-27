.PHONY: all test build clean coverage

# we default to just library Release
# w/ tests: make build TESTS=1
# w/ tests in Debug mode: make build TESTS=1 BUILD_TYPE=Debug
# w/ csound plugins: make build CSOUND=1
BUILD_TYPE?=Release
TESTS?=0
CSOUND?=0
COVERAGE?=0

CMAKE_OPTS = -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

ifeq ($(CSOUND), 1)
	CMAKE_OPTS += -DDSP_BUILD_CSOUND=ON
endif

ifeq ($(TESTS), 1)
	CMAKE_OPTS += -DDSP_BUILD_TESTS=ON
endif

ifeq ($(COVERAGE), 1)
	CMAKE_OPTS += -DDSP_COVERAGE=ON
endif

clean:
	rm -rf ./build

# custom build -- set appropriate flags
build: clean
	echo building $(BUILD_TYPE)
	mkdir -p build \
	&& cd build \
	&& cmake .. $(CMAKE_OPTS) \
	&& cmake --build .

# build the csound plugin
csound: clean
	$(MAKE) build CSOUND=1 BUILD_TYPE=Release

# rebuild lib for the csound plugin and run the regression tests
regression: csound
	uv run ./tests/regression/test.py

# rebuild the lib in debug and coverage on and run coverage (lcov usually)
coverage: clean
	$(MAKE) build TESTS=1 BUILD_TYPE=Debug COVERAGE=1
	@if [ "$(shell $(CC) -v 2>&1 | grep -c "gcc")" -gt 0 ]; then \
        echo "Collecting coverage with lcov..."; \
	    ./build/tests/dsp_unit_tests --seed 0x526af79e --no-fork; \
        lcov --capture --directory build --output-file coverage.info; \
       	lcov --remove coverage.info '/tests/*' --output-file coverage.info; \
       	genhtml coverage.info --output-directory coverage; \
       	lcov --summary coverage.info; \
	else \
		echo "Collecting coverage with llvm-cov..."; \
        LLVM_COV='/opt/homebrew/opt/llvm/bin/llvm-cov'; \
        LLVM_PROFDATA='/opt/homebrew/opt/llvm/bin/llvm-profdata';\
       	LLVM_PROFILE_FILE="coverage.profraw" ./build/tests/dsp_unit_tests; --seed 0x526af79e --no-fork; \
        $$LLVM_PROFDATA merge -sparse coverage.profraw -o coverage.profdata; \
       	$$LLVM_COV report ./build/tests/dsp_unit_tests -instr-profile=coverage.profdata; \
        $$LLVM_COV show ./build/tests/dsp_unit_tests -instr-profile=coverage.profdata -format=html -output-dir=coverage; \
    fi

# view last coverage report
html-cov:
	open ./coverage/index.html
