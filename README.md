# HMDH

HMDH (Hierarchical Multi-Dimensional Histograms) is a software library designed for efficient data summarization, enabling rapid exploratory analysis on integer and floating-point data. HMDH provides fast, space- and time-efficient encoding of data characteristics across multiple orders of magnitude in a single pass.

This public version supports one- and two-dimensional data structures and includes a graphical interface for interactive exploration. It is intended for non-commercial research and evaluation only (please refer to the license).

A commercial version is available with expanded capabilities, a CLI for generating HMDH from parquet files, supporting higher-dimensional data, finer resolution per dimension, and advanced querying features (e.g., range queries, percentile calculations, etc.). This version can process hundreds of millions of data points into compact, searchable structures occupying just a fraction of a megabyte, retaining precise counts and addressing at each hierarchical level. The commercial library also supports parallel generation, merging, and comparison of histograms, facilitating the analysis of variations across datasets.
Some current clients are using up to four dimensions in operations.

For further information, contact: hmdh@airmettle.com

## Getting Started
To get started, see [Requirements](#requirements) and [Installation](#installation). See [Usage](#usage) for a full example.

## Requirements
The project has been built and tested on an AWS codebuild instance running `Ubuntu 22.04` with `3 GB memory` and `2 vCPUs` using the image `aws/codebuild/standard:7.0`.

### Installation
This describes the installation process using cmake. As pre-requisites, you'll need git and cmake installed.

```bash
# Clone the repository
$ git clone https://github.com/AirMettle/HMDH.git
# Go to the root of the project
$ cd HMDH
# Install dependencies using these steps
$ cd cmake
$ mkdir build
$ cd build
$ cmake ..
$ make
# Go back to the root of the project
$ cd ../..
# Build the project
$ mkdir build
$ cd build
$ cmake ..
$ make
# Install the project so it is globally available (This step will require sudo priviliges)
$ sudo make install
```

## Usage

Here is an example CMakeLists.txt to use this library with your project.
```cmake
include_directories(/usr/local/include/)
link_directories(/usr/local/lib)

# Add the main.cpp file
add_executable(Simple2DExample main.cpp)

# Link the library to the executable
target_link_libraries(Simple2DExample HMDH)
```

Construct a `FPHArray` type of your input data. For example:

```cpp
std::vector<double> array = ... // construct your input data as vector 

// pass the vector to `buildFPHArray` as shown below
FPHArray array0 = buildFPHArray(array.data(),
                                  static_cast<int>(array.size()));
```

Pass the constructed `FPHArray`(s) to your desired config (see [API docs](docs/api.md) to learn about other configs).

### Generate a 1D buffer

Here is an example to construct a 1D compact histogram buffer:

```cpp
// array0 is of type FPHArray constructed as shown above.
std::vector<char> compactHistogram = generate_1DxF(array0);
// Persist the compact histogram data on disk.
Write(compactHistogram, "compact1D.bin");
```

### Generate a 2D buffer

Here is an example to construct a 2D compact histogram buffer:

```cpp
// array0 and array1 are of type FPHArray constructed as shown above.
std::vector<char> compactHistogram = generate_2DxF(array0, array1);
// Persist the compact histogram data on disk.
Write(compactHistogram, "compact2D.bin");
```

### UI
To access the UI run the `hmdh_ui` application that should have been installed following the steps in [Installation](#installation).

```bash
$ hmdh_ui
(2024-10-09 03:02:34) [INFO    ] Crow/master server is running at http://0.0.0.0:18080 using 16 threads
(2024-10-09 03:02:34) [INFO    ] Call `app.loglevel(crow::LogLevel::Warning)` to hide Info level logs.

```

This starts a web server. Switch to your browser and go to `http://localhost:18080`.
This should open up the landing page as shown below.

![UI landing page](docs/assets/1-landing-page.png)

Click on `Choose File` and select the buffer you generated in either [Generate a 1D buffer](#generate-a-1d-buffer) or [Generate a 2D buffer](#generate-a-2d-buffer) step. Click on `Upload` to generate the plots for the buffer.

### 1D Plots

![UI 1D tle](docs/assets/1D/1D-1-TLE.png)
<p align="center">
Top level plot
</p>

![UI 1D level 1](docs/assets/1D/1D-2-level1.png)
<p align="center">
Level 1 plot
</p>

![UI 1D level 2](docs/assets/1D/1D-3-level3.png)
<p align="center">
Level 2 plot
</p>

![UI 1D level 3](docs/assets/1D/1D-4-level4.png)
<p align="center">
Level 3 plot
</p>

### 2D Plots

![UI 2D tle grid](docs/assets/2D/2D-1-TLE.png)
<p align="center">
Top Level plot
</p>

![UI 2D tle details](docs/assets/2D/2D-2-TLE-detailed.png)
<p align="center">
Top Level plot (detailed)
</p>

![UI 2D level 2](docs/assets/2D/2D-3-Level2.png)
<p align="center">
Level 2 plot
</p>

![UI 2D level 3](docs/assets/2D/2D-4-Level3.png)
<p align="center">
Level 3 plot
</p>

![UI 2D m and p bits](docs/assets/2D/2D-4-m_p_bits.png)
<p align="center">
M and P bits displayed for each dimension
</p>
