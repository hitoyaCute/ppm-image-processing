#pragma once

/*
usage

vect4<float> d{};

d + 32.f;
d * d;
32.f / d;
dot(d,d);
sin(d);

*/
#include <cmath>
#include <array>

template<typename d> struct vect4;
template <typename d, int N>
struct vect {
    std::array<d,N> data = {0};
    constexpr vect() = default;
    constexpr vect(const std::array<d, N>& inp):data{inp} {}
    // constexpr vect(const vect<d,N>& inp): data{inp.data}{}
    
    template <class... Args>
    requires (sizeof... (Args) == N)
    constexpr vect(Args... inp): data{static_cast<d>(inp)...}{}

    // constexpr inline vect<d,N>& operator= (const vect<d,N>& rhs) {
    //     data = rhs.data;
    //     return *this;
    // }

    #define op(o) constexpr inline vect<d,N> operator o(const d rhs) const noexcept {\
        vect<d,N> outp;\
        for (int i{0}; i < N; ++i){\
            outp.data[i] = data[i] o rhs;\
        }\
        return outp;\
    }
    #define opf(o) friend constexpr inline vect<d,N> operator o (const d lhs, const vect<d,N>& inp) noexcept{\
        vect<d,N> outp;\
        for (int i{0}; i < N; ++i){\
            outp.data[i] = lhs o inp.data[i];\
        }\
        return outp;\
    }
    #define mop(o) constexpr inline vect<d, N> operator o(const vect<d, N>& rhs) const noexcept {\
        vect<d, N> outp;\
        for (int i{0}; i < N; ++i){\
            outp.data[i] = data[i] o rhs.data[i];\
        }\
        return outp;\
    }
    #define eop(o) constexpr inline vect<d, N>& operator o##=(const d rhs) {\
        for (int i{0}; i< N; ++i) {\
            this->data[i] o##= rhs;\
        }\
        return *this;\
    }
    #define eoop(o) constexpr inline vect<d, N>& operator o##=(const vect<d,N>& rhs) {\
        for (int i{0}; i< N; ++i) {\
            this->data[i] o##= rhs.data[i];\
        }\
        return *this;\
    }

    #define allOp(o) op(o)\
    opf(o)\
    mop(o)\
    eop(o)\
    eoop(o)

    allOp(+);
    allOp(-);
    allOp(*);
    allOp(/);

    #undef opf
    #undef op
    #undef mop
    #undef allOp
    
    constexpr inline d& x() noexcept {return data[0];}
    constexpr inline const d& x() const noexcept {return data[0];}

    constexpr inline d& y() noexcept {return data[1];}
    constexpr inline const d& y() const noexcept {return data[0];}

    vect<d,2> xx() const {return vect<d,2>{std::array{data[0],data[0]}};}
    vect<d,2> yy() const {return vect<d,2>{std::array{data[1],data[1]}};}
    vect<d,2> yx() const {return vect<d,2>{std::array{data[1],data[0]}};}
    vect<d,2> xy() const {return vect<d,2>{std::array{data[0],data[1]}};}
    vect4<d> xyyx() const {return {std::array{data[0],data[1],data[0],data[1]}};}
    
};


#define mfuncvect(size) template <typename d>\
constexpr inline d dot(const vect<d,size>& a, const vect<d,size>& b) noexcept {\
    d sum = 0;\
    for (int i = 0; i < size; ++i)\
        sum += a.data[i] * b.data[i];\
    return sum;\
}\
template <typename d>\
constexpr inline vect##size<d> sin(const vect<d,size>& obj) noexcept {\
    vect##size<d> outp;\
    for (int i{0}; i < size; i++) {\
        outp.data[i] = std::sin(obj.data[i]);\
    }\
    return outp;\
}\
template <typename d>\
constexpr inline vect##size<d> cos(const vect<d,size>& obj) noexcept {\
    vect##size<d> outp;\
    for (int i{0}; i < size; i++) {\
        outp.data[i] = std::cos(obj.data[i]);\
    }\
    return outp;\
}\
template <typename d>\
constexpr inline vect##size<d> exp(const vect<d,size>& obj) noexcept {\
    vect##size<d> outp;\
    for (int i{0}; i < size; i++) {\
        outp.data[i] = std::exp(obj.data[i]);\
    }\
    return outp;\
}\
template <typename d>\
constexpr inline vect##size<d> tanh(const vect<d,size>& obj) noexcept {\
    vect##size<d> outp;\
    for (int i{0}; i < size; i++) {\
        outp.data[i] = std::tanh(obj.data[i]);\
    }\
    return outp;\
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename d>
using vect2 = vect<d, 2>;
////////////////////////////////////////////////////////////////////////////////////////////////////////

mfuncvect(2);


////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename d>
struct vect3 : vect<d,3>{
    constexpr vect3():vect<d, 3>(){}
    constexpr vect3(const std::array<d, 3>& inp):vect<d, 3>{inp}{}
    // constexpr vect3(const vect<d,3>& inp):vect<d, 3>{inp.data}{}

    constexpr vect3(d x, d y, d z): vect<d, 4>{x,y,z}{}

    constexpr inline d& z() noexcept {return this->data[1];}
    constexpr inline const d& z() const noexcept {return this->data[0];}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
mfuncvect(3);

////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename d>
struct vect4 : vect<d, 4>{
    constexpr vect4():vect<d, 4>{}{}
    constexpr vect4(const std::array<d, 4>& inp):vect<d, 4>{inp}{}
    // constexpr vect4(const vect<d,4>& inp):vect<d, 4>{inp.data}{}

    constexpr vect4(d x, d y, d z, d h): vect<d, 4>{x,y,z,h}{}

    constexpr inline d& z() noexcept {return this->data[1];}
    constexpr inline const d& z() const noexcept {return this->data[0];}

    constexpr inline d& h() noexcept {return this->data[3];}
    constexpr inline const d& h() const noexcept {return this->data[0];}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
mfuncvect(4);

using vec2f = vect2<float>;
using vec2i = vect2<int>;
using vec2d = vect2<double>;

using vec3f = vect3<float>;
using vec3i = vect3<int>;
using vec3d = vect3<double>;

using vec4f = vect4<float>;
using vec4i = vect4<int>;
using vec4d = vect4<double>;


