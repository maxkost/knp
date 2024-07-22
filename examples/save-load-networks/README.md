Saving and loading a simple neural network. 
==============


# Solution overview

Example implementation of a creating a simple neural network and using `sonata` namespace functions to save and load it.


# Implementation of saving and loading a neural network

`./main.cpp` contains implementation of saving and loading a neural network.

_To save and load a simple neural network_:

1.  Include the header files required for the neural network execution using the `#include` directive.
    If necessary, define aliases by means of the `using` statement.
2.  Implement a synapse generator with the desired properties or use an existing generator from the framework library.
3.  Implement a neuron generator with the desired properties or use an existing generator from the framework library.
4.  Create a function that returns a neural network.
5.  In the created function, do the following:
    1.  Create a population object and pass the neuron generator to the constructor.
    2.  Create a projection object that loops the output of the population to itself. Pass the associated population ID and the synapse generator to the constructor.
    3.  Create an input projection object. Pass the synapse generator and associated population ID to the constructor.
    4.  Create a neural network object and pass the created projections and population to it.
6.  Create a function that calls `sonata` namespace functions to save and load a network.
7.  In the created function, do the following:
    1.  Create the neural network by using the function created in step 4.
    2.  Save the network by using the `save_network` function.
    3.  Load the network by using the `load_network` function.


# Build

The CMake build system from the Kaspersky Neuromorphic Platform is used in a solution.

`./CMakeLists.txt` contains CMake commands for building a solution.

If you install the `knp-examples` package, the example binary file is located in the `/usr/bin` directoty. To execute the example binary file, run the following command:

`$ save-load-networks`

You can also build the example by using CMake. The example binary file will be located in the `/build/bin` directory. To execute the created binary file, run the following commands:

`$ cd /build/bin`
`$ ./save-load-networks`


# Information about third-party code

Information about third-party code is provided in the `NOTICES.txt` file located in the platform repository.


# Trademark notices

Registered trademarks and service marks are the property of their respective owners.

© 2024 AO Kaspersky Lab