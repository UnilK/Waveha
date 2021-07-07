#include "math/FT.h"
#include "math/FFT.h"
#include "wave/voiceTransform.h"

#include <bits/stdc++.h>
using namespace std;

int main(){

    int n;
    cin >> n;

    float *a = new float[2*n]();
    for(int i=0; i<n; i++) cin >> a[i];

    complex<float> *b = math::fft(a, 2*n);
    for(int i=0; i<2*n; i++) b[i] *= b[i];

    math::fft(b, 2*n, 1);

    for(int i=0; i<2*n; i++) cout << b[i].real() << ' ';
    cout << '\n';

    delete[] a;
    delete[] b;

    return 0;
}
