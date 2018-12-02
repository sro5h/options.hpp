# options.hpp

A simple command line option parser that supports short (e.g. `-h`) and long and long
(e.g. `--help`) options with and without values.
Everything that is not an option (doesn't start with '-') is considered an action and collected
in a separate `std::vector`.
The options class is provided as a single header file with no other dependencies besides the c++
standard library.

# Example

A simple example, can also be found in `example.cpp`.

```c++
#define OPTIONS_IMPLEMENTATION
#include "Options.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
        sro5h::Options options;

        sro5h::Options::Key help('h', "help");
        sro5h::Options::Key file('f', "file");

        options.add(help);
        options.add({ 'v' });
        options.add(file, 1);

        if (!options.parse(argc, argv)) {
                std::cout << "Couldn't parse options" << std::endl;
                return 1;
        }

        if (options.has(help)) {
                std::cout << "Usage: a.out [OPTION]" << std::endl;
                std::cout << "    -h, --help    show this help" << std::endl;
                std::cout << "    -v            show version information" << std::endl;
                std::cout << "    -f, --file    process <file>" << std::endl;
                return 0;
        }

        if (options.has({ 'v' })) {
                std::cout << "Version 0.0.1" << std::endl;
        }

        if (options.has(file)) {
                std::cout << "Processing '" << options.value(file) << "'" << std::endl;
        }

        for (const std::string& action: options.actions()) {
                std::cout << "Executing '" << action << "'" << std::endl;
        }

        return 0;
}
```
