# Kaspersky Neuromorphic Platform

Platform for the spiking neural network execution.

<div align="center">
  <img src="packaging/neuromorphic-platform.svg" alt="Logo">
</div>

The Kaspersky Neuromorphic Platform ("KNP" or "platform") is a software platform for developing, training and executing spiking neural networks on a variety of computers.

You can use Kaspersky Neuromorphic Platform to do the following:

- Create spiking neural networks and train these on various types of input data, such as telemetry, events, images, 3D data, audio, and tactile data.
- Convert artificial neural networks (ANN) into spiking networks and train these.
- Optimize the structure of the loaded neural networks.
- Conduct applied research in the field of input data classification and other application domains of neural networks.
- Develop new neural network topologies, for example, impulse analogs of convolutional neural networks that involve convolution in space and time.
- Develop new models of synaptic plasticity.
- Implement new neuron models.
- Implement application solutions based on neuromorphic spiking neural networks in the field of robotic manipulators, Internet of Things, unmanned systems, human-machine interaction, wearable devices, and optimization planning.
- Implement application solutions on devices with low power consumption or using neuromorphic processors.

You can use the C++ and Python languages to accomplish these tasks. The platform supports CPUs as well as the AltAI-1 neuromorphic processor unit designed for energy-efficient execution of neural networks in various types of intelligent devices.

For information on the platform concepts and architecture, installation instructions and platform use cases, see <a href="https://click.kaspersky.com/?hl=en-US&version=1.0&pid=KNP&link=online_help">Kaspersky Neuromorphic Platform Help</a>.


## Hardware and software requirements

For Kaspersky Neuromorphic Platform operation, the computer must meet the following minimum requirements.

Minimum hardware requirements:

- CPU: Intel Core i5 or higher compatible processor
- Neuromorphic processing unit (if needed): AltAI-1
- 8 GB of RAM
- 10 GB of available hard drive space

Supported operating systems:

- Debian GNU/Linux 12.5 or later
- Ubuntu 22.04 LTS or later
- Windows 7
- Windows 10

You can use the device running any other operating system from the Linux family, if the operating system distribution kit contains the Boost library version 1.83 or later.

To work with the platform, the following software must be installed on the device:

- Visual Studio 2022

  This software must be installed when building the platform or application solution in Windows.

- Boost 1.83 or later

  When working in Windows, it is recommended to install the library <a href="https://archives.boost.io/release/1.83.0/binaries/">precompiled</a> for Visual Studio 2022 (compiler version: 14.3).

- CMake 3.25 or later
- Before installing platform component deb packages for an ANN2SNN backend, make sure that the device has the following software:
    - TensorFlow 2.4.1 or later
    - Keras 2.3.1 or later
    - NumPy 2.4.1 or later

      The NumPy library version must match the TensorFlow library version.

- Before installing a whl or deb package for a Python framework without an ANN2SNN library on an SNN backend, make sure that the device has Python 3.10 or later.
- Before installing a whl or deb package for a Python framework with an ANN2SNN library on an ANN2SNN backend, make sure that the device has following software:
    - NumPy 1.24.3
    - TensorFlow 2.13.1
    - Loguru 0.7.2
    - PyYAML 6.0.1
    - NetworkX 3.1
    - Matplotlib 3.6.3
    - tqdm 4.66.5
- Before installing a deb package with a Python framework including an ANN2SNN library on an ANN2SNN backend, make sure that the device has the following software:
    - python3-pip
    - python3-numpy
    - python3-yaml
    - python3-networkx
    - python3-matplotlib
    - python3-loguru
    - python3-tqdm
    - TensorFlow 2.13.1


## Installing the platform

For the development of application solutions on Linux, you can install the platform in the following ways:

- Install deb packages.
- Install packages for the development of application solutions in Python.

When working with the platform source code on Linux or Windows, you can build the platform or an application solution using C++. The platform build can be used to develop the Kaspersky Neuromorphic Platform.


### Installing deb packages

We recommend using the `dpkg` package manager to install deb packages.

_To install the deb packages:_ 

1. To install the deb packages with backend binary code, run the following commands as root:

    ```
    dpkg -i <path-to-deb-package>/knp-cpu-single-threaded-backend_<version 1>_amd64.deb
    dpkg -i <path-to-deb-package>/knp-cpu-multi-threaded-backend_<version 1>_amd64.deb
    dpkg -i <path-to-deb-package>/knp-altai-backend_<version 2>_amd64.deb
    ```
    When installing the `knp-altai-backend_<version 2>_amd64.deb` package, accept the AltAI-1 backends and `ANN2SNN` library Terms of Use. Your acceptance of the Terms of Use is a prerequisite for installing the package. The package installation process will be interrupted unless you accept the Terms of Use.

2. To install the deb package with C ++ framework binary code, run the following command as root:

    ```
    dpkg -i <path-to-deb-package>/knp-cpp-framework_<version 1>_amd64.deb
    ```

3. To install dev-deb packages, run the following commands as root:

    ```
    dpkg -i <path-to-deb-package>/knp-cpu-multi-threaded-backend-dev_<version 1>_amd64.deb
    dpkg -i <path-to-deb-package>/knp-cpu-single-threaded-backend-dev_<version 1>_amd64.deb
    dpkg -i <path-to-deb-package>/knp-altai-backend-dev_<version 2>_amd64.deb
    dpkg -i <path-to-deb-package>/knp-cpp-framework-dev_<version 1>_amd64.deb
    ```
    When installing the `knp-altai-backend-dev_<version 2>_amd64.deb` package, accept the AltAI-1 backends and `ANN2SNN` library Terms of Use. Your acceptance of the Terms of Use is a prerequisite for installing the package. The package installation process will be interrupted unless you accept the Terms of Use.

4. To install the deb package with the Python framework without `ANN2SNN` library, run the following command as root:

    ```
    dpkg -i <path-to-deb-package>/knp-python3-framework_<version 1>_amd64.deb
    ```
  
5. To install the deb package with `ANN2SNN` library, run the following as root:

    ```
    dpkg -i <path-to-deb-package>/knp_ann2snn.deb
    ```
    When installing the `knp_ann2snn.deb` package, accept the AltAI-1 backends and `ANN2SNN` library Terms of Use. Your acceptance of the Terms of Use is a prerequisite for installing the package. The package installation process will be interrupted unless you accept the Terms of Use.

6. To install the deb package with platform usage examples, run the following command as root:
  
    ```
    dpkg -i <path-to-deb-package>/knp-examples_<version 1>_amd64.deb
    ```

7. To install the deb package with the API reference guide, run the following command as root:

    ```
    dpkg -i <path-to-deb-package>/knp-documentation_<version 1>_all.deb
    ```

where:
- `<path-to-deb-package>` is the path to the deb package;
- `<version 1>` is the Kaspersky Neuromorphic Platform version.
- `<version 2>` is the version of the backends for the AltAI-1 neuromorphic processor and the `ANN2SNN` library.


### Installing Python development packages

The following DEB or WHL packages must be installed for Python development:

- deb packages:
    - `knp-python3-framework_<version 1>_amd64.deb`: a deb package with a Python framework without `ANN2SNN` library
    - `knp_ann2snn.deb`: a deb package with `ANN2SNN` library, backend binary code for the AltAI-1 neuromorphic processor, and a component for placing the neural network on the AltAI-1
- whl packages for installing from PyPI:
    - `knp-<version 1>-py3-none-any.whl`: a whl package with a Python framework without `ANN2SNN` library
    - `knp_ann2snn_x86_64_<version 2>-py3-none-any.whl`: a whl package with `ANN2SNN` library, backend binary code for the AltAI-1 neuromorphic processor, and a component for placing the neural network on the AltAI-1
      
      This package is installed on x86-based computers.

    - `knp_ann2snn_aarch64-<version 2>-py3-none-any.whl`: a whl package with `ANN2SNN` library and the backend binary code for the AltAI-1 neuromorphic processor
      
      This package is installed on ARM-based computers.

where: 
- `<version 1>` is the version of Kaspersky Neuromorphic Platform.
- `<version 2>` is the version of the backends for the AltAI-1 neuromorphic processor and the `ANN2SNN` library.

We recommend using the `dpkg` package manager to install the deb package.

_To install deb packages for Python development:_

1. To install the deb package with a Python framework without `ANN2SNN` library, run the following as root:
  
    ```
    dpkg -i <path-to-deb-package>/knp-python3-framework_<version 1>_amd64.deb
    ```

2. To install the deb package with `ANN2SNN` library, run the following as root:
  
    ```
    dpkg -i <path-to-deb-package>/knp_ann2snn.deb
    ```
    When installing the `knp_ann2snn.deb` package, accept the AltAI-1 backends and `ANN2SNN` library Terms of Use. Your acceptance of the Terms of Use is a prerequisite for installing the package. The package installation process will be interrupted unless you accept the Terms of Use.

where: 
- `<path-to-deb-package>` is the path to the deb package;
- `<version 1>` is the Kaspersky Neuromorphic Platform version.

Use the `pip3` package manager to install whl packages from the PyPI platform.

_To install the whl packages:_

1. To install a whl package with a Python framework without `ANN2SNN` library, run:
  
    ```
    pip3 install --break-system-packages knp-<version 1>-py3-none-any.whl
    ```

2. To install a whl package with `ANN2SNN` library, do one of the following:
    - To install a package on an x86-based computer, run:
      
      ```
      pip3 install --break-system-packages knp_ann2snn_x86_64_<version 2>-py3-none-any.whl
      ```

    - If you need to install a package on an ARM-based computer, run:
      
      ```
      pip3 install --break-system-packages knp_ann2snn_aarch64-<version 2>-py3-none-any.whl.whl
      ```

where: 
- `<version 1>` is the version of Kaspersky Neuromorphic Platform.
- `<version 2>` is the version of the backends for the AltAI-1 neuromorphic processor and the `ANN2SNN` library.


### Scenario for building a platform project or an application solution

You can build a platform or an application solution in C++. The platform build can be used when developing the Kaspersky Neuromorphic Platform.

When developing application solutions, you can install the required deb packages instead of building the solution.

You can also use the Docker image `knp-build-image` included in the platform distribution kit to build the platform or an application.

A platform or application solution build script consists of the following stages:

1. Obtaining the platform source code

    You can get the platform source code in one of the following ways:
    - Download the archive with the source code from the platform repository and unpack it to the working directory.
    - Clone the platform repository.
2. Defining the Boost_ROOT setting in Windows

    In Windows, define the `Boost_ROOT` setting. To do this, create an environment variable or specify the path to the installed Boost library in the `CMakePresets.json` file located in the root directory with the platform source code.\
    You can define the path to the installed Boost library as a CMake invocation parameter using the following command:\
    ```
    cmake -DBOOST-ROOT=<path to installed Boost library>
    ```

3. Configuring the build settings for the application solution

    In case of an application solution build, describe the build process in the `CMakeLists.txt` file. Specify the directory with the application to be built using the `add_subdirectory` function, specify the executable file using the `add_executable` function, and specify the platform libraries to be connected to the project using the `target_link_libraries` function. You can use the following libraries:
    - `neuron_traits`
    - `synapse_traits`
    - `core`
    - `meta`
    - `devices`
    - `backends`
    - `framework`

4. Configuring a platform or an application solution build

    If you are building a platform or an application solution on Linux, configure the build using the CMake build system. For more details on configuring a build using CMake, refer to the <a href="https://cmake.org/documentation/">CMake documentation</a>.\
    If you are building the platform or application solution in Windows, configure the build in Visual Studio. To do this, open the platform or application project by selecting the required `CMakeLists.txt` file and configure the cache. In case of an application solution build, select the `CMakeLists.txt` file of the application solution. In case of a platform build, select the `CMakeLists.txt` file located in the root directory with the platform source code.\
    The first build configuration with CMake may take too long to complete and fail with network errors. If network errors occur, please run the build configuration again.

5. Starting the build

    Start the platform or application solution build. For more details on starting the build of projects, refer to the <a href="https://cmake.org/documentation/">CMake documentation</a> or the documentation of the integrated development environment (IDE) being used.


## Trademark notices

Registered trademarks and service marks are the property of their respective owners.

Ubuntu and LTS are registered trademarks of Canonical Ltd.\
Docker and the Docker logo are trademarks or registered trademarks of Docker, Inc. in the United States and/or other countries. Docker, Inc. and other parties may also have trademark rights in other terms used herein.\
TensorFlow and any associated designations are trademarks of Google LLC.\
Intel and Core are trademarks of Intel Corporation in the U.S. and/or other countries.\
Linux is the registered trademark of Linus Torvalds in the U.S. and other countries.\
Microsoft, Visual Studio and Windows are trademarks of the Microsoft group of companies.\
JavaScript is a registered trademark of Oracle and/or its affiliates.\
Python is a trademark or registered trademark of the Python Software Foundation.\
Debian is a registered trademark of Software in the Public Interest, Inc.

© 2024 AO Kaspersky Lab