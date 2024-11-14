# TODO: Split LS7866 Code into Library and Application-Specific Code

We need to separate the LS7866 code into a library and the
application-specific code, including `setup()` and `loop()`, into
separate files. This will make it easier for us and others to use the
LS7866 code in other projects.

## Goals

- Create a reusable library for the LS7866 quadrature encoder chip.
- Move application-specific testing code, including `setup()` and `loop()`, into a separate file.
- Organize the code into separate subdirectories under `photon2/src`.

## Steps

1. **Create Library Directory**

   - Create a new directory for the LS7866 library code:
     ```
     photon2/src/library
     ```
   - Move the generic LS7866 code (`LS7866_Registers.h` and related functions) into this directory.

2. **Refactor Library Code**

   - In the library directory, create new files:
     - `LS7866.h` - Header file containing declarations.
     - `LS7866.cpp` - Source file containing definitions.
   - Encapsulate the LS7866 functionalities into a class or namespace for better modularity.
   - Ensure all LS7866-related functions are properly declared in `LS7866.h` and defined in `LS7866.cpp`.
   - Update includes within the library code to use relative paths if necessary.

3. **Create Application Directory**

   - Create a new directory for the application-specific code:
     ```
     photon2/src/app
     ```
   - Move the testing code, including `setup()` and `loop()`, into this directory (e.g., `main.cpp`).
   - Update the application code to include the LS7866 library header:
     ```cpp
     #include "../library/LS7866.h"
     ```

4. **Update Build Configuration**

   - Modify the project build configuration to include both the library and application directories.
     - If using a `project.properties` file or build script, update the source paths accordingly.
   - Ensure that the compiler can find the library during the build process.

5. **Adjust Include Paths**

   - In all source files, update `#include` directives to correctly reference headers.
   - Remove any redundant or obsolete includes.
   - Ensure that the paths are correct relative to the new directory structure.

6. **Test the New Structure**

   - Compile the project and upload it to the Particle Photon 2.
   - Verify that the application behaves as expected.
   - Debug and fix any issues that arise due to the code reorganization.

7. **Document the Library**

   - Write documentation for the LS7866 library, including:
     - Overview of functionalities.
     - Instructions on how to integrate and use the library in other projects.
     - API reference for public functions and classes.
   - Include example code demonstrating how to use the library.

8. **Prepare for Distribution**

   - Consider packaging the library according to Particle's library structure guidelines.
     - Include a `library.properties` file with metadata.
     - Follow best practices for naming and versioning.
   - Optionally, publish the library to the Particle Library Repository for public use.

9. **Version Control**

   - Ensure all changes are committed to version control (e.g., Git).
   - Use meaningful commit messages to document the changes.
   - Tag the version appropriately if releasing the library.

10. **Cleanup**

    - Remove any obsolete files or code fragments that are no longer needed.
    - Review code for consistency in formatting and style.
    - Update comments to reflect any changes made during refactoring.

## Additional Notes

- Ensure that the library code is as hardware-independent as possible.
- The library should handle all low-level communication with the LS7866 chip, providing a clean and simple API for users.
- Encapsulate I2C communications within the library to abstract complexity from the application code.
- Providing examples and thorough documentation will help others integrate the library into their projects more easily.

