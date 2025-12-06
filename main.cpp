#include <algorithm>
#include <cmath>
#include <cstddef>
#include <execution>
#include <numeric>
#include <ranges>
#include <cstdio>
#include <cstdlib>
#include "vect.hpp"

#define unpack2(arr2) arr2[0],arr2[1]


void frame_gen(const int i, const int dim[2],char* buf, size_t bufc) {
    // param (const int i, const int dim[2],char buf[256])
    snprintf(buf, bufc, "output-%04d.ppm",i);
    const char* outp_file = buf;
    FILE *f = std::fopen(outp_file,"wb");
           
    std::fprintf(f, "P6\n");
    std::fprintf(f, "%d %d\n", unpack2(dim));
    std::fprintf(f, "255\n");
    
    const vect2<float> r{{(float)dim[0],(float)dim[1]}};

    auto t = ((float)i/240)*2*M_PI;
    for (int y{0}; y < dim[1]; y++) {
        for (int x{0}; x < dim[0]; x++) {
            const vect4<float> u_const{{-1.f,1.f,2.f,0.f}};

            vect4<float> o{{0.f,0.f,0.f,0.f}};
            vec2f FC = {{(float)x,(float)(dim[1] - 1 - y)}};


            vec2f p{(FC*2.f-r)/r.y()},
            l,i,
            v {p*(l += 4.f - 4.f * std::abs(0.7f - dot(p, p)))};

            for (; i.y()++ < 8.f; o += (sin(v.xyyx()) + 1.f) * std::abs(v.x() - v.y()))
                v += cos(v.yx() * i.y() + i + t) / i.y() + 0.7f;

            o=tanh(5.f * exp(l.x() - 4.f - p.y() * u_const) / o);

            fputc(o.x()*255, f);
            fputc(o.y()*255, f);
            fputc(o.z()*255, f);
        }
    }
    fclose(f);

}

int main (void){//int argc, char *argv[]) {
    char buf[256];
    const int scalar = 60;
    const int dim[2] = {16*scalar,9*scalar};

    auto range = std::views::iota(0,240);

    std::for_each(
        std::execution::par,
        std::ranges::begin(range),
        std::ranges::end(range),
        [&](int i){
            frame_gen(i, dim, buf, 256);
        }
    );
    
    // for (int i{0}; i<240; i++) {
    //     // std::printf("file '%s' has been created\n", outp_file);
    //     frame_gen(i, dim, buf, 256);
    // }
    system("ffmpeg -y -i output-%04d.ppm -r 60 output.mp4 > /dev/null 2>&1");
    
    system("rm output-*.ppm");
    return 0;
}


