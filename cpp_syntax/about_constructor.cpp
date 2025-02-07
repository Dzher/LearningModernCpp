#include <iostream>
#include <string>
#include <utility>

// Point 1: Can copy constructor's parameter just be const, not const reference?
// Answer: NO! if build success the copy-constructor will make infinite recursion
class NoRefCopyConstructor
{
public:
    explicit NoRefCopyConstructor(std::string name) : name_(std::move(name)) {
        std::cout << name_ << " NoRefCopyConstructor construct success!" << std::endl;
    }

    // !Attention here no reference! even build failed
    // NoRefCopyConstructor(const NoRefCopyConstructor other) {
    NoRefCopyConstructor(const NoRefCopyConstructor& other) {
        name_ = other.name_;
        std::cout << name_ << " NoRefCopyConstructor construct success!" << std::endl;
    }

public:
    std::string name_;
};

void testNoRefCopyConstructor() {
    NoRefCopyConstructor a("hello");
    NoRefCopyConstructor a_copy(a);
    a_copy.name_ = "world";
    std::cout << a_copy.name_ << std::endl;
}

// Point 2: how to write B's copy constructor if B has the point of class A?
// Answer: should think about deep and shallow copy of the point A
class A
{
public:
    std::string data_;
};

class BShallowCopy
{
public:
    explicit BShallowCopy(A* a) : a_ptr_(a) {
    }
    BShallowCopy(const BShallowCopy& other) {
        a_ptr_ = other.a_ptr_;
    }

public:
    A* a_ptr_ = nullptr;
};

class BDeepCopy
{
    // for deep copy should consider more, beside copy constructor & assign constructor, and destructor!
public:
    explicit BDeepCopy(A* a) : a_ptr_(a) {
    }
    BDeepCopy(const BDeepCopy& other) {
        if (other.a_ptr_) {
            a_ptr_ = new A();
            a_ptr_->data_ = other.a_ptr_->data_;
        }
    }
    BDeepCopy& operator=(const BDeepCopy& other) {
        if (this != &other) {
            delete a_ptr_;
            a_ptr_ = nullptr;
            if (other.a_ptr_) {
                a_ptr_ = new A();
                a_ptr_->data_ = other.a_ptr_->data_;
            }
        }
        return *this;
    }
    ~BDeepCopy() {
        delete a_ptr_;
    }

    // if need rvalue copy&assign construntor
    BDeepCopy(BDeepCopy&& other) noexcept : a_ptr_(other.a_ptr_) {
        other.a_ptr_ = nullptr;
    }
    BDeepCopy& operator=(BDeepCopy&& other) noexcept {
        if (this != &other) {
            delete a_ptr_;
            a_ptr_ = other.a_ptr_;
            other.a_ptr_ = nullptr;
        }
        return *this;
    }

public:
    A* a_ptr_ = nullptr;
};

int main() {
    testNoRefCopyConstructor();

    A a;
    a.data_ = "Proto A";
    std::cout << "address of a: " << &a << std::endl;
    BShallowCopy b_s(&a);
    std::cout << "address of a ptr in b_s: " << b_s.a_ptr_ << std::endl;
    BShallowCopy b_s_c(b_s);
    std::cout << "address of a ptr in b_s_c: " << b_s_c.a_ptr_ << std::endl;
    BDeepCopy b_d(&a);
    std::cout << "address of a ptr in b_d: " << b_d.a_ptr_ << std::endl;
    BDeepCopy b_d_c(b_d);
    std::cout << "address of a ptr in b_d_c: " << b_d_c.a_ptr_ << std::endl;

    return 0;
}