#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Core>
#include <omp.h>
#include <iomanip>

using namespace std;
using namespace Eigen;

int main(int argc, char** argv) {
	string filename = "C:\\Users\\mares\\Desktop\\EigenTemplate\\EigenTemplate\\mona_lisa.pgm";
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "File not found" << endl;
		return 0;
	}
	string line;
	getline(file, line);
	if (line != "P2") {
		cout << "Not a PGM file" << endl;
		return 0;
	}
	int width, height;
	file >> width >> height;
	cout << width << " " << height << endl;
	VectorXi img(width * height);
	int counter = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int pixel;
			counter++;
			file >> pixel;
			img(i * width + j) = pixel;
		}
		if (counter % 1000 == 0) {
			cout << "loaded # " << counter << " pixels" << endl;
		}
	}

	file.close();

	double percent = 100;
	double num_pixels = img.size();
	cout << num_pixels << endl;

	int size = width * height;
	int num_pixels_to_remove = num_pixels * percent / 100;
	cout << num_pixels_to_remove << "from total pixels: " << size << endl;

	//remove percent pixels

	int counter2 = 0;
	while (counter2 < num_pixels_to_remove) {
		int i = rand() % size;
		int row = i / width;
		int col = i % width;

		if (row == 0 || row == height - 1 || col == 0 || col == width - 1) {
			continue;
		}
		img(i) = 0;

		counter2++;
		if (counter2 % 10000 == 0) {
			cout << "removed # " << counter2 << "pixels" << endl;
		}
	}
	cout << "removing done" << endl;
	SparseMatrix<double, RowMajor> m(size, size);
	m.reserve(size * 5);

	for (size_t i = 0; i < size; i++) {
		if (img(i) == 0) {
			m.insert(i, i) = 4.0;

			if (i - 1 >= 0) {
				m.insert(i, i - 1) = -1.0;
			}
			if (i + 1 < size) {
				m.insert(i, i + 1) = -1.0;
			}
			if (i - width >= 0) {
				m.insert(i, i - width) = -1.0;
			}
			if (i + width < size) {
				m.insert(i, i + width) = -1.0;
			}
		}
		else {
			m.insert(i, i) = 1.0;
		}

		double progress = static_cast<double>(i + 1) / size * 100;
		printf("\r%.2f%%", progress);
		fflush(stdout);
	}

	//save the picture with removed pixels
	ofstream output_file1("C:\\Users\\mares\\Desktop\\EigenTemplate\\EigenTemplate\\output_removed_" + to_string(percent) + ".pgm");
	output_file1 << "P2" << endl;
	output_file1 << width << " " << height << endl;
	output_file1 << 255 << endl;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			output_file1 << img(i * width + j) << " ";	
		}
	}
	output_file1.close();


	cout << endl;
	initParallel();
	int n = 8;
	omp_set_num_threads(n);
	setNbThreads(n);

	BiCGSTAB <SparseMatrix<double, RowMajor>> solver;
	cout << "Computing BiCGSTAB..." << endl;
	solver.setMaxIterations(100000);
	solver.setTolerance(1E-8);
	
	solver.compute(m);
	cout << m.size() << endl;
	cout << "BiCGSTAB done" << endl;
	cout << "Solver initialized" << endl;

	cout << "casting done" << endl;
	VectorXd img_d = img.cast<double>();	
	VectorXd restored_d = solver.solve(img_d);
	img = restored_d.cast<int>();
	string output_filename = "C:\\Users\\mares\\Desktop\\EigenTemplate\\EigenTemplate\\output_" + to_string(percent) + ".pgm";
	ofstream output_file(output_filename);
	output_file << "P2" << endl;
	output_file << width << " " << height << endl;
	output_file << 255 << endl;

	
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			output_file << img(i * width + j) << " ";
		}
	}

	cout << "# Iterations:	" << solver.iterations();
	cout << "Estimated error: " << solver.error() << "-----------------\n";
	
	//img = restored_d.cast<int>();
	cout << "Solving Done" << endl;

	output_file.close();
	return 0;
}