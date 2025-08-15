.PHONY: all test build clean

# we default to just library Release
# w/ tests: make build TESTS=1
# w/ tests in Debug mode: make build TESTS=1 BUILD_TYPE=Debug
# w/ csound plugins: make build CSOUND=1
BUILD_TYPE?=Release
TESTS?=0
CSOUND?=0
UTILS?=0

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

clean:
	rm -rf ./build

build:
	echo building $(BUILD_TYPE)
	mkdir -p build \
	&& cd build \
	&& cmake .. $(CMAKE_OPTS) \
	&& cmake --build .

test:
	./build/tests/dsp_unit_tests

all: clean build
