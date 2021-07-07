#include "math/FFT.h"
#include "math/constants.h"

#include <algorithm>
#include <vector>

// precalc table size = 1<<B (2 to the power of B)
int32_t B = 0;

// precalculation tables
std::vector<std::vector<std::complex<float> > > w;
std::vector<int32_t > invbit;

namespace math{
    
    void resize_precalc_tables(int32_t B_){

        B = std::min(30, std::max(B, B_));

        while((int32_t)w.size() <= B){
                    
            /*
               precalculate w[b][x] = e^(i*PI*x/(2^b))

               and the invbit array:
               examples in base 2 of the invbit array:

               invbit[1000111] = 1110001
               invbit[0] = 0
               invbit[10110111] = 11101101

               it reverses the bit order.
            */

            int32_t z = 1<<w.size(), b = w.size();
            
            w.resize(w.size()+1);
            w[b].resize(z);

            for(int32_t i=0; i<z; i++) w[b][i] = std::polar(1.0f, PIF*i/z);

            invbit.resize(z, 0);
            for(int32_t i=0; i<z/2; i++){
                invbit[i] <<= 1;
                invbit[i+z/2] = invbit[i]+1;
            }
        }
    }

    void fft(std::complex<float> *v, int32_t n, bool inv){
        
        /*
           let's examine how the recursive FFT changes
           the ordering of the input array:
           
           [000, 001, 010, 011, 100, 101, 110, 111]
           [000, 010, 100, 110], [001, 011, 101, 111]
           [000, 100], [010, 110], [001, 101], [011, 111]
           [000], [100], [010], [110], [001], [101], [011], [111]

           [000, 100, 010, 110, 001, 101, 011, 111]
          
           The index of element x in the resulting array
           is it's index in the inupt array, but with the binary
           representation flipped. This makes sense, since
           on each layer we sort the array by the least
           significant bit.

           Just reorder the array, then run the basic
           FFT algorithm on a bottom-to-top loop such that the results
           are calculated in the indices used by the next layer.

           O(1) extra memory used with O(N*log(N)) preclac,
           O(N*log(N)) time with a good constant compared to
           the recursive implementation.
        */

        int32_t b = 0;
        while(1<<b < n) b++;
        if(1<<b != n) return;
        
        resize_precalc_tables(b);

        int32_t shift = B-b;

        for(int32_t i=0; i<n; i++){
            if(i < invbit[i]>>shift) std::swap(v[i], v[invbit[i]>>shift]);
        }

        for(int32_t r=0; r<b; r++){
            int32_t rd = 1<<r;
            for(int32_t i=0; i<n; i+=2*rd){
                for(int32_t j=i; j<i+rd; j++){
                    std::complex<float> tmp = w[r][j-i]*v[j+rd];
                    v[j+rd] = v[j]-tmp;
                    v[j] = v[j]+tmp;
                }
            }
        }

        if(inv){
            std::reverse(v+1, v+n);
            for(int32_t i=0; i<n; i++) v[i] /= n;
        }
    }

    std::complex<float> *fft(float *v, int32_t n, bool inv){
        std::complex<float> *cv = new std::complex<float>[n];
        for(int32_t i=0; i<n; i++) cv[i] = v[i];
        fft(cv, n, inv);
        return cv;
    }
}
