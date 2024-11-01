# Main logic to install packages based on platform and architecture ,
# pass on the packages need to be installed as arguments for each Darwin and Linux

# Packages for macOS (Darwin)
set(darwin_packages
        pkg-config
        gcc
        git
        boost
)

# Packages for Ubuntu x86_64
set(ubuntu_x86_64_packages
        build-essential
        pkg-config
        git
        libboost-all-dev
        libbz2-dev
        libssl-dev
        libasio-dev
)

# Packages for Ubuntu ARM (aarch64)
set(ubuntu_aarch64_packages
        pkg-config
        git
        libboost-all-dev
        libbz2-dev
        libssl-dev
        libasio-dev
)

# Packages for Amazon Linux ARM (aarch64)
set(amzn_aarch64_packages
        pkg-config
        git
        boost-devel
        bzip2-devel
        libcurl-devel
)

if (UNIX AND NOT APPLE)
    # Detect Linux distribution by reading /etc/os-release
    if (EXISTS "/etc/os-release")
        file(STRINGS "/etc/os-release" LINUX_DISTRO REGEX "^ID=.*")

        # Extract the distribution ID and remove any quotes
        string(REGEX REPLACE "ID=\"?([^\"]+)\"?" "\\1" DISTRO ${LINUX_DISTRO})

        message(STATUS "Detected Linux distribution: ${DISTRO}")
    else ()
        message(FATAL_ERROR "Cannot detect Linux distribution. /etc/os-release not found.")
    endif ()
endif ()

# Debugging - Display detected architecture and distribution
message(STATUS "Detected Architecture: ${ARCH}")
message(STATUS "Detected Distribution: ${DISTRO}")
message(STATUS "OS: ${OS}")
message(STATUS "ARCH: ${ARCH}")

# Function to install packages via Homebrew
function(install_brew_package package_name)
    message(STATUS "Checking for ${package_name}...")

    # Check if Homebrew is installed
    execute_process(
            COMMAND brew --version
            RESULT_VARIABLE brew_check
            OUTPUT_QUIET
            ERROR_QUIET
    )

    if (brew_check)
        message(STATUS "Homebrew is not installed. Installing Homebrew...")
        execute_process(
                COMMAND /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
                RESULT_VARIABLE brew_install_result
        )

        if (brew_install_result)
            message(FATAL_ERROR "Homebrew installation failed.")
        endif ()
    else ()
        message(STATUS "Homebrew is already installed.")
    endif ()

    # Check if the package is already installed
    execute_process(
            COMMAND brew list ${package_name}
            RESULT_VARIABLE package_check
            OUTPUT_QUIET
            ERROR_QUIET
    )

    if (package_check)
        message(STATUS "${package_name} is not installed. Installing...")
        execute_process(
                COMMAND brew install ${package_name}
                RESULT_VARIABLE install_result
        )

        if (install_result)
            message(FATAL_ERROR "Failed to install ${package_name}.")
        else ()
            message(STATUS "${package_name} installed successfully.")
        endif ()
    else ()
        message(STATUS "${package_name} is already installed.")
    endif ()
endfunction()

# Function to install packages via apt-get (Ubuntu)
function(install_apt_package package_name)
        execute_process(
            COMMAND sudo apt-get install -y ${package_name}
            RESULT_VARIABLE install_result
        )

        if (install_result)
            message(FATAL_ERROR "Failed to install ${package_name}.")
        else ()
            message(STATUS "${package_name} installed successfully.")
        endif ()
endfunction()

# Function to install packages via yum (CentOS/RHEL)
function(install_yum_package package_name)
    message(STATUS "Checking for ${package_name}...")

    # Check if the package is installed
    execute_process(
            COMMAND rpm -q ${package_name}
            RESULT_VARIABLE package_check
            OUTPUT_QUIET
            ERROR_QUIET
    )

    if (package_check)
        message(STATUS "${package_name} is not installed. Installing...")
        execute_process(
                COMMAND sudo yum install -y ${package_name}
                RESULT_VARIABLE install_result
        )

        if (install_result)
            message(FATAL_ERROR "Failed to install ${package_name}.")
        else ()
            message(STATUS "${package_name} installed successfully.")
        endif ()
    else ()
        message(STATUS "${package_name} is already installed.")
    endif ()
endfunction()


# Main logic to install packages based on platform and architecture
if (OS STREQUAL "Darwin")
    message(STATUS "Installing dependencies on macOS...")

    foreach (package ${darwin_packages})
        install_brew_package(${package})
    endforeach ()

elseif (OS STREQUAL "LINUX" AND ARCH STREQUAL "x86_64")
    message(STATUS "Detected Ubuntu/Linux with architecture: x86_64")

    if (DISTRO STREQUAL "ubuntu")
        message(STATUS "Installing dependencies on Ubuntu x86_64...")

        foreach (package ${ubuntu_x86_64_packages})
            install_apt_package(${package})
        endforeach ()

    elseif (DISTRO STREQUAL "amzn")
        message(STATUS "Installing dependencies on Amazon Linux x86_64...")

        # Now install group packages like "Development Tools"
        message(STATUS "Installing development tools group on Amazon Linux...")
        execute_process(
                COMMAND sudo yum groupinstall -y "Development Tools"
                RESULT_VARIABLE groupinstall_result
        )

        if (groupinstall_result)
            message(FATAL_ERROR "Failed to install Development Tools group.")
        else ()
            message(STATUS "Development Tools group installed successfully.")
        endif ()

        foreach (package ${amzn_aarch64_packages})
            install_yum_package(${package})
        endforeach ()

    else ()
        message(FATAL_ERROR "Unsupported Linux distribution for x86_64 architecture: ${DISTRO}.")
    endif ()

elseif (OS STREQUAL "LINUX" AND ARCH STREQUAL "aarch64")
    message(STATUS "Detected Linux ARM architecture")

    if (DISTRO STREQUAL "ubuntu")
        message(STATUS "Installing dependencies on Ubuntu ARM (aarch64)...")

        foreach (package ${ubuntu_aarch64_packages})
            install_apt_package(${package})
        endforeach ()

    elseif (DISTRO STREQUAL "amzn")
        message(STATUS "Installing dependencies on Amazon Linux ARM (aarch64)...")

        # Now install group packages like "Development Tools"
        message(STATUS "Installing development tools group on Amazon Linux...")
        execute_process(
                COMMAND sudo yum groupinstall -y "Development Tools"
                RESULT_VARIABLE groupinstall_result
        )

        if (groupinstall_result)
            message(FATAL_ERROR "Failed to install Development Tools group.")
        else ()
            message(STATUS "Development Tools group installed successfully.")
        endif ()

        foreach (package ${amzn_aarch64_packages})
            install_yum_package(${package})
        endforeach ()

    else ()
        message(FATAL_ERROR "Unsupported Linux distribution for ARM architecture: ${DISTRO}.")
    endif ()

else ()
    message(FATAL_ERROR "Unsupported OS or architecture: OS=${OS}, ARCH=${ARCH}.")
endif ()



