#include <bits/stdc++.h>

using namespace std;

template <typename T>
class Vector {
    T* start;
    size_t size_;
    size_t capacity_;

    static T* allocate(size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    static void deallocate(T* ptr) {
        ::operator delete(ptr);
    }

    // copy constructs one T at the memory address (does not allocate)
    static void construct(T* ptr, const T &value) {
        new (ptr) T(value);
    }

    // destructs the T stored at ptr without freeing the memory
    static void destroy(T* ptr) {
        ptr->~T();
    }

    size_t next_power_of_two(size_t from) {
        size_t curr = 1;

        for (int i = 1; curr < ULONG_MAX;++i) {
            // dangerous if from is >= the largest power of two storable in the int type - this will explode.
            if (curr > from) return curr;
            curr *= 2;
        }

        throw bad_alloc();
    }

    void construct_fill(T* buf, size_t count, const T &val) {
        size_t i = 0;
        try {
            for (i = 0; i < count; ++i) {
                new (&buf[i]) T(val);
            }
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                buf[j].~T();    
            }
            throw;
        }
    }

    void construct_fill(T* buf, size_t count, const Vector &other) {
        size_t i = 0;
        try {
            for (i = 0; i < count; ++i) {
                new (&buf[i]) T(other[i]);
            }
        } catch(...) {
            for (size_t j = 0; j < i; ++j) {
                buf[j].~T();    
            }
            throw;
        }
    }

    void destroy_and_deallocate_current_buffer() {
        for (size_t i = 0; i < size_; ++i) {
            destroy(&start[i]);
        }
        deallocate(start);
    }

    void reallocate_and_copy_elements(size_t new_capacity) {
        T* new_start = allocate(new_capacity);
        try {
            construct_fill(new_start, size_, *this);
        } catch(...) {
            deallocate(new_start);
            throw;
        }

        destroy_and_deallocate_current_buffer();
        start = new_start;
        capacity_ = new_capacity;
    }
public:
    using iterator = T*;
    using const_iterator = const T*;
    ~Vector() {
        destroy_and_deallocate_current_buffer();
    }
    // constructors
    Vector() {
        capacity_ = 0;
        size_ = 0;
        start = nullptr;
    }
    Vector(initializer_list<T> init_list): size_(init_list.size()), capacity_(init_list.size()) {
        start = allocate(size_);
        size_t i = 0;
        try {
            for (const T& val : init_list) {
                new (&start[i++]) T(val);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                destroy(&start[j]);
            }
            deallocate(start);
            throw;
        }

    }

    explicit Vector(size_t count, const T &val = T()): size_(count), capacity_(count) {
        start = allocate(size_);
        try {
            construct_fill(start, size_, val);
        } catch(...) {
            deallocate(start);
            throw;
        }
    }

    Vector(const Vector &other): size_(other.size_), capacity_(other.capacity_)  {
        start = allocate(capacity_);
        try {
            construct_fill(start, size_, other);
        } catch(...) {
            deallocate(start);
            throw;
        }
    }
    Vector(Vector &&other) noexcept : start(other.start), size_(other.size_), capacity_(other.capacity_) {
        other.start = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }
    // copy assignment
    Vector &operator=(const Vector &other) {
        temp Vector(other);
        swap(start, temp.start);
        swap(size_, temp.size_);
        swap(capacity_, temp.capacity_);
        return *this;
    }
    // move assignment
    Vector &operator=(Vector &&other) noexcept {
        swap(start, other.start);
        swap(size_, other.size_);
        swap(capacity_, other.capacity_);
        return *this;
    }
    bool operator==(const Vector &other) const {
        // do we need to compare capacities?
        if (other.size_ != size_) return false;
        for (size_t i = 0; i < size_; ++i) {
            // comparign by value, or should we compare by actual object 
            // (p1==p2 if and only if they reference the same object)
            if (start[i] != other.start[i]) return false;
        }

        return true;
    }
    T &operator[](size_t idx) /*const? does this even matter since we already have the const const overload below*/ {
        if (idx >= size_) throw runtime_error("index out of bounds!");
        return start[idx];
    }
    const T &operator[](size_t idx) const {
        if (idx >= size_) throw runtime_error("index out of bounds!");
        return start[idx];
    }
    bool empty() const {
        return size_ == 0;
    }
    size_t size() const {
        return size_;
    }
    size_t capacity() const {
        return capacity_;
    }
    T &front() {
        if (size_ == 0) throw runtime_error("error: vector is empty");
        return start[0];
    }
    const T &front() const {
        if (size_ == 0) throw runtime_error("error: vector is empty");
        return start[0];
    }
    T &back() {
        if (size_ == 0) throw runtime_error("error: vector is empty");
        return start[size_ - 1];
    }
    const T &back() const {
        if (size_ == 0) throw runtime_error("error: vector is empty");
        return start[size_ - 1];
    }
    void push_back(const T &x) {
        if (capacity_ > size_) {
            new (&start[size_++]) T(x);
        } else {
            assert(capacity_ == size_);
            size_t new_capacity_ = max(size_t(1), capacity_ * 2);
            reallocate_and_copy_elements(new_capacity_);
            new (&start[size_++]) T(x);
        }
    }

    // void emplace_back(T &x);
    T pop_back() {
        if (size_ == 0) throw runtime_error("error: vector is empty");
        T ret = start[--size_];
        destroy(&start[size_]);
        return ret;
    }
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
            destroy(&start[i]);
        }
        size_ = 0;
    }
    void erase(iterator pos) {
        if (pos < start || pos >= start + size_) throw runtime_error("position is out of bounds");
        for (size_t i = pos - start; i < size_ - 1; ++i) {
            start[i] = std::move(start[i + 1]);
        }      
        destroy(&start[--size_]);
    }
    void insert(iterator pos, const T &val) {
        if (size_ == 0 || capacity_ == 0) {
            push_back(val);
            return;
        }
        if (pos < start || pos > start + size_) throw runtime_error("position is out of bounds");
        // cout << "Inserting at index " << pos - start << " value " << val << '\n';
        // cout << size_ << ' ' << capacity_ << '\n';

        if (size_ < capacity_) {
            new(&start[size_]) T(std::move(start[size_ - 1]));
            for (size_t i = size_; i > pos - start; --i) {
                start[i] = std::move(start[i - 1]);
            }

            start[pos - start] = val;
        } else {
            assert(size_ == capacity_);
            size_t idx = pos - start;
            size_t new_capacity = max(size_t(1), next_power_of_two(capacity_));
            reallocate_and_copy_elements(new_capacity);
            new (&start[size_]) T(std::move(start[size_ - 1]));
            for (size_t i = size_ - 1; i > idx; --i) {
                start[i] = std::move(start[i - 1]);
            }
            start[idx] = val;
            capacity_ = new_capacity;
        }

        ++size_;
    }
    void reserve(size_t n) {
        if (n <= capacity_) return;
        reallocate_and_copy_elements(n);
    }

    void resize(size_t n, T val = T()) {
        if (n == size_) return;
        if (n > size_) {
            if (n > capacity_) {
                reserve(n);
            }
            for (size_t i = size_; i < n; ++i) {
                new (&start[i]) T(val);
            }
        } else {
            for (size_t i = n; i < size_;++i) {
                destroy(&start[i]);
            }
        }

        size_ = n;
    }

    iterator begin() {
        return start;
    }
    const_iterator begin() const {
        return start;
    }
    iterator end() {
        return &(start[size_]);
    }
    const_iterator end() const {
        return &(start[size_]);
    }
};

int main(void) {
    Vector<int> v;
    v.reserve(5);
    v.push_back(1);
    v.push_back(2);
    v.insert(v.end(), 3);
    for (int x : v) {
        cout << x << " " << '\n';
    }
    v.resize(0);
    v.push_back(1);
    v.push_back(2);
    v.insert(v.end(), 3);
    int n = (size_t) v.size();
    for (int i = 0; i < n; ++i) {
        v.erase(v.begin());
    }
    cout << v.size() << endl;
    for (int i : v) {
        cout << i << '\n';
    }
}