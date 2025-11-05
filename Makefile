.PHONY: all test build clean coverage

# we default to just library Release
# w/ tests: make build TESTS=1
# w/ tests in Debug mode: make build TESTS=1 BUILD_TYPE=Debug
# w/ csound plugins: make build CSOUND=1
BUILD_TYPE?=Release
TESTS?=0
CSOUND?=0
UTILS?=0
COVERAGE?=0

CMAKE_OPTS = -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

ifeq ($(CSOUND), 1)
	CMAKE_OPTS += -DBUILD_CSOUND=ON
endif

ifeq ($(TESTS), 1)
	CMAKE_OPTS += -DBUILD_TESTS=ON
endif

ifeq ($(UTILS), 1)
	CMAKE_OPTS += -DBUILD_UTILS=ON
endif

ifeq ($(COVERAGE), 1)
	CMAKE_OPTS += -DCOVERAGE=ON
endif

clean:
	rm -rf ./build

build:
	echo building $(BUILD_TYPE)
	mkdir -p build \
	&& cd build \
	&& cmake .. $(CMAKE_OPTS) \
	&& cmake --build .

build-clean: clean build

coverage: clean
	$(MAKE) build TESTS=1 BUILD_TYPE=Debug COVERAGE=1
	# Run tests from build directory
	./build/tests/dsp_unit_tests
	# GCC / lcov branch
	@if [ "$(shell $(CC) -v 2>&1 | grep -c "gcc")" -gt 0 ]; then \
       	lcov --capture --directory build --output-file coverage.info; \
       	lcov --remove coverage.info '*/tests/*' '*/munit.c' '/usr/*' --output-file coverage.info; \
       	genhtml coverage.info --output-directory coverage; \
       	lcov --summary coverage.info; \
	else \
       	echo "Collecting coverage with llvm-cov..."; \
       	LLVM_PROFILE_FILE="coverage.profraw" ./build/tests/dsp_unit_tests; \
       	llvm-profdata merge -sparse coverage.profraw -o coverage.profdata; \
       	llvm-cov report ./build/tests/dsp_unit_tests -instr-profile=coverage.profdata; \
	fi

test:
	./build/tests/dsp_unit_tests

html-cov:
	open ./coverage/index.html

all: clean build
