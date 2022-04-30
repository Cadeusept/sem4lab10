#include <boost/program_options.hpp>
#include <hashing_db.hpp>
#include <iostream>
#include <boost/thread.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description opt_desc("Allowed options");
    opt_desc.add_options()
        ("source", po::value<std::string>(), "set source db, NECESSARY")
        ("log-level", po::value<int>(), "set log level")
        ("thread_count", po::value<unsigned int>(), "set number of threads")
        ("output", po::value<std::string>(), "set output db, NECESSARY")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opt_desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opt_desc << std::endl;
        return 1;
    }

    int log_level = 0;
    unsigned int thread_count = boost::thread::hardware_concurrency();

    if (!(vm.count("source") && vm.count("output"))) {
        throw std::runtime_error("One or more necessary args are missing");
        return 1;
    }

    std::string source = vm["source"].as<std::string>();
    std::string output = vm["output"].as<std::string>();

    

    if (!(source.substr(source.length() - 3,source.length()) == ".db" &&
        output.substr(output.length() - 3,output.length()) == ".db")) {
        throw std::runtime_error("Wrong file format");
        return 1;
    }

    if (vm.count("log_level")) {
        log_level = vm["log_level"].as<unsigned int>();
    }

    if (vm.count("log_level") &&
        vm["thread_count"].as<unsigned int>() < thread_count) {
        thread_count = vm["thread_count"].as<unsigned int>();
    } else {
        if (!vm.count("log_level") || vm["thread_count"].as<int>() < 0) {
            throw std::runtime_error("Wrong thread_count argument");
            std::cout << opt_desc << std::endl;
            return 1;
        }
    }
    //output
}