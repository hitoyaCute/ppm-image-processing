#include <chrono>
#include <cmath>
#include <cstddef>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "vect.hpp"

#define unpack2(arr2) arr2[0],arr2[1]
#define area2(arr2) arr2[0] * arr2[1]


constexpr void frame_gen(const int i, const int dim[2],
                         char* buf, const int bufc,
                         const int vLenght, const int fps,
                         const char* templ,
                         char* img_buf, const int img_buf_size) {
    // param (const int i, const int dim[2],char buf[256])
    snprintf(buf, bufc, templ, i);
    const char* outp_file = buf;
    FILE *f = std::fopen(outp_file,"wb");
           
    std::fprintf(f, "P6\n");
    std::fprintf(f, "%d %d\n", unpack2(dim));
    std::fprintf(f, "255\n");
    
    const vec2d r{{(float)dim[0],(float)dim[1]}};

    constexpr vec4d u_const{-1.f,1.f,2.f,0.f};
    const float frames = area2(dim);

    const double t = ((float)i/(fps*4.f))*2.f*M_PI;
    for (int y{0}; y < dim[1]; ++y) {
        for (int x{0}; x < dim[0]; ++x) {

            vec4d o{{0.f,0.f,0.f,0.f}};
            
            const vec2d FC = {{(float)x,(float)(dim[1] - 1 - y)}};

            const vec2d p{(FC*2.f-r)/r.y() * ((1.f - (std::abs(frames/2.f - i)/(frames/2.f))) * 10.f +1.f)};
            // const vec2f p{(FC*2.f-r)/r.y() * 2.f};
            vec2d l,i,
            v {p*(l += 4.f - 4.f * std::abs(0.7f - dot(p, p)))};

            for (; i.y()++ < 8.f; o += (sin(v.xyyx()) + 1.f) * std::abs(v.x() - v.y()))
                v += cos(v.yx() * i.y() + i + t) / i.y() + 0.7f;

            o = tanh(5.f * exp(l.x() - 4.f - p.y() * u_const) / o);

            // fputc(o.x()*255, f);
            // fputc(o.y()*255, f);
            // fputc(o.z()*255, f);
            const size_t idx = (y * dim[0] + x) * 3;
            img_buf[idx + 0] = o.x()*255;
            img_buf[idx + 1] = o.y()*255;
            img_buf[idx + 2] = o.z()*255;
        }
    }
    fwrite(img_buf, 1, img_buf_size, f);
    fclose(f);
}

int main (void){//int argc, char *argv[]) {
    
    const unsigned T = std::jthread::hardware_concurrency();
    const int vLenght = 4;
    const int fps     = 60;

    const std::string templa = std::format("output-%0{}d.ppm", (int)std::log10(fps*vLenght) + 1);
    std::printf("ouput template %s\n", templa.c_str());

    std::vector<std::jthread> pool;
    pool.reserve(T + 1);

    auto state = std::vector<int>(T);    // state.reserve(T);
    std::vector<int> work_ammount{};
    work_ammount.reserve(T);

    for (unsigned t = 0; t < T; ++t) {
        pool.emplace_back([=,&state,&work_ammount]{

            work_ammount.emplace_back((vLenght*fps)/T);

            const char* templ = templa.c_str();
            constexpr int scalar = 60;
            constexpr int dim[2] = {18*scalar,10*scalar};
            char buf[50];

            char img_buf[area2(dim) * 3] {};

            for (unsigned int i = t; i < vLenght*fps; i += T) {
                frame_gen(i, dim, buf, sizeof(buf), vLenght,fps, templ, img_buf, sizeof(img_buf));
                state[t] += 1;
            }
        });
    }
    pool.emplace_back([=,&state,&work_ammount]{
        int vstate = 0;
        for (unsigned i{0}; i < T+3; i++)
            std::printf("\x1b[2K\n");
        while (vstate < (vLenght * fps)) {
            int total = 0;
            for (int i{0}; i < T; i++)
                total += state[i];

            vstate = total;
            std::printf("\x1b[%dA\x1b[2K\rProgress: %7.3f%% with worker count %d\n", T+3, (float)vstate / (vLenght * 60) * 100, T);
            std::printf("   workers:\n");
            for (unsigned worker{0}; worker < T; worker ++) {
                std::printf("      worker_%02d: job %4d/%4d\n", worker, state[worker], work_ammount[worker]);
            }
            std::printf("\n      total work %4d/%4d", vstate, fps*vLenght);
            std::fflush(stdout);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::printf("\nDone!\n"); 
    });
    pool.clear();
    state.clear();
    std::printf("Finished creating %d images\n", fps*vLenght);
    std::fflush(stdout);

    auto d = system(std::format("ffmpeg -y -r {} -i {} output.mp4 > /dev/null 2>&1", fps, templa).c_str());

    std::printf("Finished cimpiling into video deleting images\n");
    std::fflush(stdout);
    
    (void)system("rm output-*.ppm");
    return 0;
}

    // std::vector<std::jthread> pool;
    //
    // for (int i{0}; i<60; ++i)
    //     pool.emplace_back([&, i]{
    //         const int scalar = 60;
    //         const int dim[2] = {16*scalar,9*scalar};
    //         frame_gen(i, dim);
    //     });
    // }
    
    // for (int i{0}; i<240; i++) {
    //     // std::printf("file '%s' has been created\n", outp_file);
    //     snprintf(buf, sizeof(buf), "output-%04d.ppm",i);
    //     const char* outp_file = buf;
    //     FILE *f = std::fopen(outp_file,"wb");
    //
    //     std::fprintf(f, "P6\n");
    //     std::fprintf(f, "%d %d\n", unpack2(dim));
    //     std::fprintf(f, "255\n");
    //     for (int y{0}; y < dim[1]; y++) {
    //         for (int x{0}; x < dim[0]; x++) {
    //             if (((x+i)/60 + (y+i)/60)%2) {
    //                 fputc(0xff, f);
    //                 fputc(0x00, f);
    //                 fputc(0x00, f);
    //             } else {
    //                 fputc(0x00, f);
    //                 fputc(0x00, f);
    //                 fputc(0x00, f);
    //             }
    //         }
    //     }
    //     fclose(f);
    //
    // }
