#ifndef AE_STRING_H
#define AE_STRING_H

#include <stdint.h>
#include <string>

namespace ae {

// Utf8 string
class String
{
public:
    using Iterator = std::basic_string<uint32_t>::iterator;
    using ConstIterator = std::basic_string<uint32_t>::const_iterator;

    String();
    String(const std::string &ansi);
    String(const char *ansi, int32_t size);
    String(const String &other);
    String(String &&other);
    ~String() = default;

    const uint32_t *getData() const;
    int32_t getSize() const;

    bool isEmpty() const;

    uint32_t operator[](int32_t index) const;
    uint32_t &operator[](int32_t index);

    Iterator begin();
    ConstIterator begin() const;

    Iterator end();
    ConstIterator end() const;

    String &operator=(const String &right);
    String &operator+=(const String &right);

    void clear();

    template<typename T>
    static String fromNumber(const T &number)
    {
        return String(std::to_string(number));
    }

private:
    void decodeUtf8(const char *ansi, int32_t size);

private:
    std::basic_string<uint32_t> m_string;
};

} // namespace ae

#endif // AE_STRING_H
