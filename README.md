# OpenMP Rabbits and Foxes

## Requirements

- OpenMP
- GCC
- CMake (used to facilitate the management of multiple executables, which is difficult to do with Makefile)
- Python3.8 (required to the extra package for executions and dataset generation, this package is optional)

## Artifacts

	.
	├── README.md
	├── Makefile # Makefile that compiles the program using CMake
	├── serial-rabbits-and-foxes # Serial version of the simulator program
	├── parallel-rabbits-and-foxes # Parallel version of the simulator using OpenMP
	├── src
	│   ├── errors.h # errors package
	│   ├── rabbits_fox.c # divisor function package
	│   ├── rabbits_fox.h # divisor function package header
	│   ├── main.c # main program
	│   ├── utils.c # utilities (e.g.,I/O)
	│   └── utils.h # utilities
	└── test # (extra package)
			├── dataset_generator.py # divisor function using a Python library to check results and validity of the program
			├── random_numbers.py # random generator of inputs to the program
			├── dataset_generator.py # generate datasets
			├── run_all_profile.py # run profile experiments
			├── run_all.py # run normal time experiments
			├── runs.sh # batch execution of algorithms
			├── utils.py # utilities
			└── print_all.py # Print time results of the executions

## Usage (examples)

	make
	./parallel-rabbits-and-foxes < data/in/example.txt
	./serial-rabbits-and-foxes < data/in/example.txt
	make release_time_debug
	./serial-rabbits-and-foxes < data/in/example.txt > out.txt
