Execution of a neural network automatically uploaded to the backend.
==============


# Solution overview

Example implementation of a neural network containing a population with one BLIFAT neuron, an input projection and a loop projection with one delta synapse each. The neural network is executed for 20 steps.


# Implementation of a neural network automatic upload and execution

`./main.cpp` contains implementation of a neural network automatic upload and execution.

_To automatically upload a neural network to the backend and execute it_:

1.  Include the header files required for the neural network execution using the `#include` directive.
    If necessary, define aliases by means of the `using` statement.
2.  Implement a synapse generator with the desired properties or use an existing generator from the framework library.
3.  Implement a neuron generator with the desired properties or use an existing generator from the framework library.
4.  Create a function that creates the objects required for the neural network execution.
5.  In the created function, do the following:
    1.  Create a population object and pass the neuron generator to the constructor.
    2.  Create a projection object that loops the output of the population to itself. Pass the associated population ID and the synapse generator to the constructor.
    3.  Create an input projection object. Pass the synapse generator and associated population ID to the constructor.
    4.  Create a neural network object and pass the created projections and population to it.
    5.  Define the IDs of the input and output channels.
    6.  Create a model object. Pass the neural network object, channel IDs, population IDs, and input projection IDs to the model.
    7.  Create a spike generation functor.
    8.  Specify the path to the backend instance that executes the neural network.
    9.  Create a model executor object. Pass the model object, the path to the backend, the input channel ID, and the spike generation functor to the model executor. Run the model execution.
6.  If necessary, create a vector where to write the indices of the steps at which spikes are received by the output channel.


# Build

The CMake build system from the Kaspersky Neuromorphic Platform is used in a solution.

`./CMakeLists.txt` contains CMake commands for building a solution.

If you install the `knp-examples` package, the example binary file is located in the `/usr/bin` directoty. To execute the example binary file, run the following command:

`$ simple-network`

You can also build the example by using CMake. The example binary file will be located in the `/build/bin` directory. To execute the created binary file, run the following commands:

`$ cd /build/bin`
`$ ./simple-network`


# Information about third-party code

Information about third-party code is provided in the `NOTICES.txt` file located in the platform repository.


# Trademark notices

Registered trademarks and service marks are the property of their respective owners.

© 2024 AO Kaspersky Lab