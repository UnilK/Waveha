#include <iostream>

#include "wavestream.h"
#include "FFT.h"

int main(){
	
	int n;
	std::cin >> n;
	std::vector<float> x(n), y(n);
	for(float &i : x) std::cin >> i;
	for(float &i : y) std::cin >> i;

	FFT myFFT;

	std::vector<float> xy = myFFT.convolution(x, y);

	for(float i : xy) std::cout << i << ' ';
	std::cout << '\n';

	return 0;
}
