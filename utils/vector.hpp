//
// Created by simon on 14.04.17.
//

#ifndef DRDEMO_VECTOR_HPP
#define DRDEMO_VECTOR_HPP

/**
 * Generic simple fixed size vector template that supports math operations
 */

#include <cstddef>
#include <cassert>
#include <algorithm>

namespace Utils {

    template<typename T, size_t SIZE>
    class Vector {
    protected:
        // Stack allocated vector content
        T *elements;

    public:
        Vector();

        explicit Vector(T const &v);

        // Copy constructor
        Vector(Vector<T, SIZE> const &other);

        // Assignment operator
        Vector<T, SIZE> &operator=(Vector<T, SIZE> const &other);

        // Move constructor
        Vector(Vector<T, SIZE> &&other);

        // Move operator
        Vector<T, SIZE> &operator=(Vector<T, SIZE> &&other);

        virtual ~Vector() noexcept;

        // Get vector size
        constexpr inline size_t Size() const {
            return SIZE;
        }

        // Access vector element
        inline T const &operator[](size_t i) const {
#ifdef DEBUG
            assert(i < SIZE);
#endif
            return elements[i];
        }

        inline T &operator[](size_t i) {
#ifdef DEBUG
            assert(i < SIZE);
#endif
            return elements[i];
        }

        // Math operators on itself
        Vector<T, SIZE> &operator+=(Vector<T, SIZE> const &a) {
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] += a.elements[i];
            }

            return *this;
        }

        Vector<T, SIZE> &operator-=(Vector<T, SIZE> const &a) {
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] -= a.elements[i];
            }

            return *this;
        }

        Vector<T, SIZE> &operator*=(Vector<T, SIZE> const &a) {
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] *= a.elements[i];
            }

            return *this;
        }

        Vector<T, SIZE> &operator*=(T const &a) noexcept {
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] *= a;
            }

            return *this;
        }

        Vector<T, SIZE> &operator/=(Vector<T, SIZE> const &a) {
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] /= a.elements[i];
            }

            return *this;
        }

        Vector<T, SIZE> &operator/=(T const &a) {
#ifdef DEBUG
            assert(a != T(0));
#endif
            for (size_t i = 0; i < SIZE; i++) {
                elements[i] /= a;
            }

            return *this;
        }
    };

    template<typename T, size_t SIZE>
    Vector<T, SIZE>::Vector()
            : elements(new T[SIZE]) {}

    template<typename T, size_t SIZE>
    Vector<T, SIZE>::Vector(T const &v)
            : elements(new T[SIZE]) {
        for (size_t i = 0; i < SIZE; i++) {
            elements[i] = v;
        }
    }

    template<typename T, size_t SIZE>
    Vector<T, SIZE>::Vector(Vector<T, SIZE> const &other)
            : elements(new T[SIZE]) {
        std::copy(other.elements, other.elements + SIZE, elements);
    }

    template<typename T, size_t SIZE>
    Vector<T, SIZE> &Vector<T, SIZE>::operator=(Vector<T, SIZE> const &other) {
        if (this != &other) {
            for (size_t i = 0; i < SIZE; ++i) {
                elements[i] = other.elements[i];
            }
        }

        return *this;
    }

    template<typename T, size_t SIZE>
    Vector<T, SIZE>::Vector(Vector<T, SIZE> &&other)
            : elements(other.elements) {
        other.elements = nullptr;
    }

    template<typename T, size_t SIZE>
    Vector<T, SIZE> &Vector<T, SIZE>::operator=(Vector<T, SIZE> &&other) {
        if (this != &other) {
            delete[] elements;
            elements = other.elements;
            other.elements = nullptr;
        }

        return *this;
    }

    template<typename T, size_t SIZE>
    Vector<T, SIZE>::~Vector() {
        if (elements != nullptr) {
            delete[] elements;
        }
    }

    /**
     * Declare the mathematical operators and function that allow to write expression using Vector<T>/Vector<T> and
     * Vector<T> / T
     */

    // Sum of Vector<T> and Vector<T>
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator+(Vector<T, SIZE> const &a, Vector<T, SIZE> const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] + b[i];
        }
        return c;
    }

    // Subtraction of Vector<T> and Vector<T>
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator-(Vector<T, SIZE> const &a, Vector<T, SIZE> const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] - b[i];
        }
        return c;
    }

    // Multiplication of Vector<T> and Vector<T>
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator*(Vector<T, SIZE> const &a, Vector<T, SIZE> const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] * b[i];
        }
        return c;
    }

    // Multiplication of Vector<T> and T
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator*(Vector<T, SIZE> const &a, T const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] * b;
        }
        return c;
    }

    // Multiplication of T and Vector<T>
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator*(T const &a, Vector<T, SIZE> const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a * b[i];
        }
        return c;
    }

    // Division of Vector<T> and Vector<T>
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator/(Vector<T, SIZE> const &a, Vector<T, SIZE> const &b) {
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] / b[i];
        }
        return c;
    }

    // Division of Vector<T> and T
    template<typename T, size_t SIZE>
    Vector<T, SIZE> operator/(Vector<T, SIZE> const &a, T const &b) {
#ifdef DEBUG
        assert(b != T(0));
#endif
        Vector<T, SIZE> c;
        for (size_t i = 0; i < SIZE; i++) {
            c[i] = a[i] / b;
        }
        return c;
    }

} // Utils namespace

#endif //DRDEMO_VECTOR_HPP
