#include <iostream>
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include <vector>
#include <random>

using namespace std;
using namespace Eigen;

void Remove_percent_from_image(MatrixXd img) {


}


int main(int argc, char** argv){
	ifstream file("C:\\Users\\mares\\Desktop\\EigenTemplate\\EigenTemplate\\mona_lisa.pgm");
	if (!file.is_open()) {
		cout << "File not found" << endl;
		return 1;
	}
	string line;
	getline(file, line);
	int width, height;
	file >> width >> height;
	cout << width << " " << height << endl;
	MatrixXd image(width, height);
	int counter = 0;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int pixel;
			counter++;
			file >> pixel;
			image(i, j) = pixel;
		}
		if (counter % 1000 == 0) {
			cout << counter << endl;
		}
	}


	file.close();


	return 0;
}