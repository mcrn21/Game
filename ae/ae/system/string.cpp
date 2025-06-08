#include "string.h"

#include <cstring>

namespace ae {

String::String() {}

String::String(const std::string &ansi)
{
    decodeUtf8(ansi.c_str(), ansi.size());
}

String::String(const char *ansi)
{
    decodeUtf8(ansi, std::strlen(ansi));
}

String::String(const String &other)
    : m_string{other.m_string}
{}

String::String(String &&other)
    : m_string{std::move(other.m_string)}
{}

const uint32_t *String::getData() const
{
    return m_string.data();
}

int32_t String::getSize() const
{
    return m_string.size();
}

bool String::isEmpty() const
{
    return m_string.empty();
}

uint32_t String::operator[](int32_t index) const
{
    return m_string[index];
}

uint32_t &String::operator[](int32_t index)
{
    return m_string[index];
}

String::Iterator String::begin()
{
    return m_string.begin();
}

String::ConstIterator String::begin() const
{
    return m_string.begin();
}

String::Iterator String::end()
{
    return m_string.end();
}

String::ConstIterator String::end() const
{
    return m_string.end();
}

String &String::operator=(const String &right)
{
    m_string = right.m_string;
    return *this;
}

String &String::operator+=(const String &right)
{
    m_string += right.m_string;
    return *this;
}

void String::erase(int32_t pos, int32_t npos)
{
    m_string.erase(pos, npos);
}

String &String::operator+=(uint32_t codepoint)
{
    m_string += codepoint;
    return *this;
}

String &String::operator+=(const std::string &ansi)
{
    decodeUtf8(ansi.data(), ansi.size());
    return *this;
}

String &String::operator+=(const char *ansi)
{
    decodeUtf8(ansi, std::strlen(ansi));
    return *this;
}

void String::clear()
{
    m_string.clear();
}

void String::decodeUtf8(const char *ansi, int32_t size)
{
    int32_t i = 0;
    while (i < size) {
        uint8_t c = ansi[i];
        uint32_t cp = 0;

        if ((c & 0x80) == 0) {
            cp = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = ((ansi[i] & 0x1F) << 6) | (ansi[i + 1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = ((ansi[i] & 0x0F) << 12) | ((ansi[i + 1] & 0x3F) << 6) | (ansi[i + 2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            cp = ((ansi[i] & 0x07) << 18) | ((ansi[i + 1] & 0x3F) << 12)
                 | ((ansi[i + 2] & 0x3F) << 6) | (ansi[i + 3] & 0x3F);
            i += 4;
        } else {
            // invalid byte, skip
            i += 1;
        }

        m_string.insert(m_string.end(), cp);
    }
}

String operator+(const String &left, const String &right)
{
    String result = left;
    result += right;
    return result;
}

String operator+(const String &left, uint32_t codepoint)
{
    String result = left;
    result += codepoint;
    return result;
}

String operator+(uint32_t codepoint, const String &right)
{
    String result;
    result += codepoint;
    result += right;
    return result;
}

String operator+(const String &left, const std::string &right)
{
    String result = left;
    result += right;
    return result;
}

String operator+(const std::string &left, const String &right)
{
    String result;
    result += left;
    result += right;
    return result;
}

String operator+(const String &left, const char *right)
{
    String result = left;
    result += right;
    return result;
}

String operator+(const char *left, const String &right)
{
    String result;
    result += left;
    result += right;
    return result;
}

} // namespace ae
