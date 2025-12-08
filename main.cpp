#include <chrono>
#include <cmath>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "vect.hpp"

#define unpack2(arr2) arr2[0],arr2[1]
#define area2(arr2) arr2[0] * arr2[1]

// template <const int img_buf_size>
inline void frame_gen(FILE* f,
                      const int i,
                      const int dim[2],
                      const int vLenght, const int fps, const int T) {
    // param (const int i, const int dim[2],char buf[256])
    // char* img_buf = new char(img_buf_size);

    
    const vec2f r{dim[0],dim[1]};

    constexpr vec4f u_const{-1.f,1.f,2.f,0.f};
    const float frames = area2(dim);

    const float t = ((float)i/(fps*4.f))*2.f*M_PI;
    for (unsigned int y{0}; y < dim[1]; ++y) {
        for (unsigned int x{0}; x < dim[0]; ++x) {

            vec4f o;
            const vec2f FC {dim[0] - 1 - x, dim[1] -1 - y};

            const float scale = 1.f + 5.f * (1 - std::abs(i - frames * 0.5f) / (frames * 0.5f));
            // const float scale = (float)i/(frames*0.5) * 9.f + 1;

            // const vec2f p{(FC*2.f-r)/r.y()};
            const vec2f p{((FC*2.f-r)/r.y()) * scale};
            vec2f l,i,
            v {p*(l += 4.f - 4.f * std::abs(0.7f - dot(p, p)))};

            for (; i.y()++ < 8.f; o += (sin(v.xyyx()) + 1.f) * std::abs(v.x() - v.y()))
                v += cos(v.yx() * i.y() + i + t) / i.y() + 0.7f;

            o = tanh(5.f * exp(l.x() - 4.f - p.y() * u_const) / o);

            fputc(o.x()*255, f);
            fputc(o.y()*255, f);
            fputc(o.z()*255, f);
            // const size_t idx = (y * dim[0] + x) * 3;
            // img_buf[idx + 0] = (unsigned char)(o.x()*255.f);
            // img_buf[idx + 1] = (unsigned char)(o.y()*255.f);
            // img_buf[idx + 2] = (unsigned char)(o.z()*255.f);
        }
    }
}

int main (void){//int argc, char *argv[]) {
    
    const unsigned T = 2;//std::jthread::hardware_concurrency();
    constexpr int vLenght = 4;
    constexpr int fps     = 60;
    constexpr int scalar = 60;
    constexpr int dim[2] = {16*scalar,9*scalar};

    // tempalte of the output file
    const std::string templa = std::format("output-%0{}d.ppm", (int)std::log10(fps*vLenght) + 1);
    const char* outp = "output_main.mp4";
    // i forgot why i did this
    const char* templ = templa.c_str();

    std::printf("ouput template %s\n", templa.c_str());

    std::vector<std::jthread> pool;
    pool.reserve(T + 1);

    auto state = std::vector<int>(T);
    std::vector<int> work_ammount{};
    work_ammount.reserve(T);

    for (unsigned t = 0; t < T; ++t) {

        pool.emplace_back([=,&state,&work_ammount]{
            // for diagnostics
            work_ammount.emplace_back((vLenght*fps)/T);
            // placeholder for the output template rsults
            char buf[50];// local for each thread
            // std::array<unsigned char,  area2(dim) * 3> outp;

            for (int i = t; i < vLenght*fps; i += T) {
                // file initialazion

                snprintf(buf, sizeof(buf), templ, i); // create a file name and put to buf
                FILE *f = std::fopen(buf,"wb");
                
                // header of the file
                std::fprintf(f, "P6\n");
                std::fprintf(f, "%d %d\n", unpack2(dim));
                std::fprintf(f, "255\n");
                // creating the current frame
                frame_gen(f,i,dim,vLenght,fps,t);
                
                // clearing
                // fwrite(outp.data(), 1,  area2(dim) * 3, f);
                fclose(f);
                
                // update the status
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
            std::printf("\x1b[%dA\x1b[2K\rProgress: %7.3f%% with worker count %d\n", T+3, (float)vstate / (vLenght * fps) * 100, T);
            std::printf("   workers:\n");
            for (unsigned worker{0}; worker < T; worker ++) {
                std::printf("      worker_%02d: job %4d/%4d\n", worker, state[worker], work_ammount[worker]);
            }
            std::printf("\n      total work %4d/%4d   ", vstate, fps*vLenght);
            std::fflush(stdout);

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::printf("\nDone!\n"); 
    });
    pool.clear();
    state.clear();
    std::printf("Finished creating %d images\n", fps*vLenght);
    std::fflush(stdout);

    auto d = system(std::format("ffmpeg -y -r {} -i {} {} > /dev/null 2>&1", fps, templa, outp).c_str());

    std::printf("Finished cimpiling into video deleting images\n");
    std::fflush(stdout);
    
    (void)system("rm output-*.ppm");
    return 0;
}
