#include <iostream>
#include <vector>
#include <string>

// Simple generator replacement using a class with iteration
class StringGenerator
{
private:
    std::vector<std::string> strings_;
    size_t index_;

public:
    StringGenerator() : strings_({"Hello", "World", "!"}), index_(0) {}

    const std::string &current() { return strings_[index_]; }
    bool next()
    {
        ++index_;
        return index_ < strings_.size();
    }
    void reset() { index_ = 0; }
    bool done() const { return index_ >= strings_.size(); }
};

int main()
{
    StringGenerator gen;

    // Print first value
    std::cout << "next(" << gen.current() << ")" << std::endl;
    gen.next();

    // Iterate through remaining values
    while (!gen.done())
    {
        std::cout << gen.current() << std::endl;
        gen.next();
    }

    return 0;
}
