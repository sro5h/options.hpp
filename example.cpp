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
