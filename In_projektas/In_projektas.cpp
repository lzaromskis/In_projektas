/*
* Lygiagretaus programavimo
* inžinerinis projektas
* Lukas Žaromskis IFF-7/5
*
* main.cpp
*/

#include <iostream>
#include <fstream>
#include <exception>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include "graph.h"
#include "vector2d.h"

// Function prototypes
void ReadPoints(std::vector<lukzar::Vector2D>* points, std::string filename, char separator);
void Benchmark(int repeat);

int main(int argc, char* argv[])
{
	int threads = 4;
	int resolution = 1000;
	bool printPoints = false;
	if (argc > 1)
	{
		int t = std::stoi(argv[1]);
		if (t > 0)
			threads = t;
	}
	if (argc > 2)
	{
		if (std::stoi(argv[1]) == -1)
		{
			Benchmark(std::stoi(argv[2]));
			return 0;
		}
		resolution = std::stoi(argv[2]);
	}
	if (argc > 3 && argv[3][0] == '1')
	{
		printPoints = true;
	}
	std::cout << "Running " << threads << " threads.\n";
	std::vector<lukzar::Vector2D> points = std::vector<lukzar::Vector2D>();
	ReadPoints(&points, "points.dat", ' ');
	
	lukzar::Graph graph = lukzar::Graph(points);
	std::cout << "Pre optimization:\n";
	std::cout << "Target function value: " << graph.Target() << std::endl;
	if (printPoints)
		graph.PrintPoints();
	graph.DrawGraph("1.png", resolution);
	auto start = std::chrono::high_resolution_clock::now();
	graph.Optimize(threads);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	std::cout << "Post optimization:\n";
	std::cout << "Target function value: " << graph.Target() << std::endl;
	if (printPoints)
		graph.PrintPoints();
	graph.DrawGraph("2.png", resolution);

}

void ReadPoints(std::vector<lukzar::Vector2D>* points, std::string filename, char separator)
{
	std::ifstream infile(filename);
	std::string line;
	while (getline(infile, line))
	{
		int sepPos = line.find(separator, 0);
		std::string s1 = std::string(line.substr(0, sepPos));
		std::string s2 = std::string(line.substr(sepPos + 1, line.size() - sepPos - 1));

		lukzar::Vector2D point = lukzar::Vector2D(std::stod(s1), std::stod(s2));
		points->push_back(point);
	}
	infile.close();
}

void Benchmark(int repeat)
{
	std::string benchdata[] =
	{
		"b50.dat",
		"b75.dat",
		"b100.dat",
		"b125.dat",
		"b150.dat",
		"b175.dat",
		"b200.dat",
		"b225.dat",
		"b250.dat",
		"b275.dat",
		"b300.dat"
	};

	std::ofstream outfile("benchmark.csv");
	outfile.write("BENCHMARK\n\n", 11);
	

	

	std::cout << "***Benchmark started***\n";
	for (int k = 0; k < 11; k++)
	{
		std::vector<lukzar::Vector2D> points = std::vector<lukzar::Vector2D>();
		ReadPoints(&points, benchdata[k], ' ');
		std::cout << "Benchmarking file " << benchdata[k] << std::endl;

		std::string header = "\n\nBenchmarking file " + benchdata[k] + "\nThreads;Runtime\n";
		outfile.write(header.c_str(), header.size());
		for (int threads = 1; threads <= 8; threads++)
		{
			double total = 0;

			std::cout << "Running " << threads << " threads.\n";
			for (int i = 0; i < repeat; i++)
			{
				lukzar::Graph graph = lukzar::Graph(points);
				auto start = std::chrono::high_resolution_clock::now();
				graph.Optimize(threads);
				auto finish = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> elapsed = finish - start;
				std::cout << "[" << i << "]Elapsed time: " << elapsed.count() << " s\n";
				total += elapsed.count();

				//std::string row = std::to_string(i) + ";" + std::to_string(elapsed.count()) + "\n";
				//outfile.write(row.c_str(), row.size());
			}
			std::cout << "Average: " << total / static_cast<double>(repeat) << " s\n";
			std::cout << "\n\n";

			std::string footer = std::to_string(threads) + ";" + std::to_string(total / static_cast<double>(repeat)) + "\n";
			outfile.write(footer.c_str(), footer.size());
		}
	}
	std::string end = "\n\nBENCHMARK END";
	outfile.write(end.c_str(), end.size());
	outfile.close();
	std::cout << "***Benchmark ended***\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
