#ifndef VOCAL_H
#define VOCAL_H

#include <vector>
#include <complex>

class Vocal{
    
    /*
       I'm gambling quite a lot of effort here on this one observation:
       it seems that human vocal consists of a 
       fundamental frequency and it's subfrequencies.

       i.e everything's synced to the fundamental.
       Changing the pitch of a voice is quite easy with this assumption:
    */

    protected:

        int32_t size = 60;
        float fund = 100;
        std::vector<std::complex<float> > voc;

    public:

        Vocal();
        Vocal(int32_t size_, float fundamental_);

        void set_size(int32_t size_);
        int32_t get_size();

        void set_fundamental(float fundamental_);
        float get_fundamental();

        void set(int32_t freq, std::complex<float> state);
        std::complex<float> get(int32_t freq);

        // set all frequencies to 0
        void clear();

        // set the phase of the fundamental to 0 and adjust other frequencies accordingly 
        void normalize();

        // shift the fundamental by state and the subfrequencies accordingly
        void shift(std::complex<float> state);

        // set the to state and the subfrequencies accordingly
        void set_phase(std::complex<float> state);
};

#endif
