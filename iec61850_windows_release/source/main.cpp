#ifdef __linux__
#include <cstdlib>
#include <memory>
#include <pthread.h>
#include <utility>
#include <string>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <boost/format.hpp>
#endif

#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <chrono>
#include <stdio.h>
#include <time.h>
#include <thread>
#include "goose_header.h"
#include "r_header.h"


#ifdef __linux__
#include<unistd.h>
#else
#include<windows.h>  
#endif

void run_server(std::string output_file)
{
    std::cout << "Starting server... Output file: " << output_file << "\n";
    try {
        boost::asio::io_service io_service;
        R_sub server{ io_service };
        io_service.run();
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

void run_client(std::string server_ip, std::string server_port) {

    std::cout << "Starting client... \n";
    while (true)
    {
        boost::asio::io_service io_service;

        R_pub iec61850_publisher(io_service, server_ip, server_port);

        std::cout << "Sending packet to " << server_ip << ":" << server_port << "\n";
        iec61850_publisher.send_rgoose();
        Sleep(1);
    }
    
}

void swap_stdout_to_file(std::string filename) {
    std::ofstream out(filename.c_str());
    std::cout.rdbuf(out.rdbuf());

    std::cout << "Experimental: writing output to file -> " << filename << "\n";
}

int main(int argc, char *argv[])
{
    std::string mode, output_file, server_ip;
    std::string server_port = "8080";

    mode = std::string(argv[1]);
    if (mode == "server") {
        output_file = argv[2];

        swap_stdout_to_file(output_file);
        run_server(output_file);
    }
    else if (mode == "client") {
        server_ip = argv[2];
        if (argc == 4) {
            server_port = argv[3];
        }
        run_client(server_ip, server_port);
    }
    else {
        std::cout << "Command format: \n";
        std::cout << "server \"output_file_name\"\n";
        std::cout << "client \"server_ip\" [\"server_port\"]\n";
    }

    return 0;


}
