#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>

template <typename T>
class Vector {
private:
    // BURAYI SEN DOLDURACAKSIN:
    // 1. T tipinde pointer (Data)
    // 2. size_t tipinde kapasite (Capacity)
    // 3. size_t tipinde boyut (Size)
    T *data;
    size_t capacity;
    size_t length;

    void resize(){
        size_t newCapacity = (capacity == 0) ? 1 : capacity * 2;
        T *newData = new T[newCapacity];
        for(size_t i = 0; i<length; i++){
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCapacity;

    }

public:
    // Constructor (Kurucu)
    Vector() {
        capacity = 10;        // Başlangıçta 10'luk yerimiz olsun
        length = 0;             // Şu an içi boş
        data = new T[capacity];
    }
    //DeepCopy 
    Vector(const Vector& other) {
            capacity = other.capacity;
            length = other.length;
            
            data = new T[capacity];
            
            for (size_t i = 0; i < length; i++) {
                data[i] = other.data[i];
            }
        }
    // Destructor (Yıkıcı)
    ~Vector() {
        delete[] data;
    };

    void push_back(const T& value) {
        if(length == capacity){
            resize();
        }

        data[length] = value;
        length++;
    }

    

    
    Vector& operator=(const Vector& other) {
        
        if (this != &other) {
            
            delete[] data;

            
            capacity = other.capacity;
            length = other.length;
            
            
            data = new T[capacity];
            for (size_t i = 0; i < length; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    T& operator[] (size_t index){

        if (index >= length){
            throw std::out_of_range("Index out of bonds");
        }

        return data[index];

    }

    const T& operator[](size_t index) const {
        if (index >= length) {
            throw std::out_of_range("Index out of bounds");
        }
        return data[index];
    }

    size_t size() const { return length; }
    
    bool empty() const { return (length == 0); }

    T* begin(){ return data; }

    T* end(){ return data + length; }
};

#endif