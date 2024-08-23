#include <cmath>

class Complex {
public:
    Complex(double real, double imaginary)
        : real(real), imaginary(imaginary) {}
    
    Complex(const Complex &c)
        : real(c.real), imaginary(c.imaginary) {}

    ~Complex(void) {}

    Complex operator+(const Complex &c) const {
        return Complex(this->real + c.real, this->imaginary + c.imaginary);
    }

    Complex squared(void) const {
        return Complex(this->real * this->real - this->imaginary * this->imaginary, 2 * this->real * this->imaginary);
    }

    double magnitudeSquared(void) const {
        return this->real * this->real + this->imaginary * this->imaginary;
    }

private:
    double real, imaginary;

};