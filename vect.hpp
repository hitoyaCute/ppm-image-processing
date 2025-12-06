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

template <typename d, int N>
struct vect {
    std::array<d,N> data = {0};
    vect() = default;
    vect(const std::array<d, N>& inp): data{inp}{};

    // vect<d,N>& operator= (const vect<d,N>& rhs) {
    //     this->data = rhs.data;
    //     return this;
    // }

    #define op(d,N,o) vect<d,N> operator o(const d& rhs) {\
        vect<d,N> outp;\
        for (int i{0}; i < N; i++){\
            outp.data[i] = data[i] o rhs;\
        }\
        return outp;\
    }
    #define opf(d,N,o) friend vect<d,N> operator o (d lhs, const vect<d,N>& inp) {\
        vect<d,N> outp;\
        for (int i{0}; i < N; i++){\
            outp.data[i] = lhs o inp.data[i];\
        }\
        return outp;\
    }
    #define mop(d,N,o) vect<d, N> operator o(const vect<d, N>& rhs) {\
        vect<d, N> outp;\
        for (int i{0}; i < N; i++){\
            outp.data[i] = data[i] o rhs.data[i];\
        }\
        return outp;\
    }
    #define eop(d, N, o) vect<d, N>& operator o##=(const d& rhs) {\
        for (int i{0}; i< N; i++) {\
            this->data[i] o##= rhs;\
        }\
        return *this;\
    }
    #define eoop(d, N, o) vect<d, N>& operator o##=(const vect<d,N>& rhs) {\
        for (int i{0}; i< N; i++) {\
            this->data[i] o##= rhs.data[i];\
        }\
        return *this;\
    }

    #define allOp(d,N,o) op(d,N,o)\
    opf(d,N,o)\
    mop(d,N,o)\
    eop(d,N,o)\
    eoop(d,N,o)

    allOp(d, N, +);
    allOp(d, N, -);
    allOp(d, N, *);
    allOp(d, N, /);

    #undef opf
    #undef op
    #undef mop
    #undef allOp
    
    const d& x() const {return data[0];}
    d& x() {return data[0];}
    const d& y() const {return data[1];}
    d& y() {return data[1];}
    
    d& h() {return this->data[3];}

    vect<d,2> xx(){return {{data[0],data[0]}};}
    vect<d,2> yy(){return {{data[1],data[1]}};}
    vect<d,2> yx(){return {{data[1],data[0]}};}
    vect<d,2> xy(){return {{data[0],data[1]}};}
    vect<d,4> xyyx(){return {{data[0],data[1],data[0],data[1]}};}

    
};


template <typename d, int N>
d dot(const vect<d, N>& a, const vect<d, N>& b) {
    return a.x()*b.x() + a.y()*b.y();
}
template <typename d, int N>
vect<d,N> sin(const vect<d, N>& obj){
    vect<d,N> outp;
    for (int i{0}; i < N; i++) {
        outp.data[i] = std::sin(obj.data[i]);
    }
    return outp;
}
template <typename d, int N>
vect<d,N> cos(const vect<d, N>& obj){
    vect<d,N> outp;
    for (int i{0}; i < N; i++) {
        outp.data[i] = std::cos(obj.data[i]);
    }
    return outp;
}
template <typename d, int N>
vect<d,N> exp(const vect<d, N>& obj){
    vect<d,N> outp;
    for (int i{0}; i < N; i++) {
        outp.data[i] = std::exp(obj.data[i]);
    }
    return outp;
}
template <typename d, int N>
vect<d,N> tanh(const vect<d, N>& obj){
    vect<d,N> outp;
    for (int i{0}; i < N; i++) {
        outp.data[i] = std::tanh(obj.data[i]);
    }
    return outp;
}


template <typename d>
using vect2 = vect<d, 2>;

template <typename d>
struct vect3 : vect<d,3>{
    vect3():vect<d, 3>(){}
    vect3(const std::array<d, 3>& inp):vect<d, 3>{inp}{}
    d& z() {return this->data[2];}
};
template <typename d>
struct vect4 :vect<d, 4>{
    vect4():vect<d, 4>(){}
    vect4(const std::array<d, 4>& inp):vect<d, 4>{inp}{}
    vect4(const vect<d,4>& inp):vect<d, 4>{inp.data}{}
    d& z() {return this->data[2];}
    d& h() {return this->data[3];}
};

using vec2f = vect2<float>;
using vec2i = vect2<int>;
using vec2d = vect2<double>;
