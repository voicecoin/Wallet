#ifndef BCUS_UINT_256_H
#define BCUS_UINT_256_H

#include <string>
#include <vector>
#include "string_helper.h"

template<unsigned int BITS>
class base_blob
{
public:
    base_blob()
    {
        clear();
    }

    explicit base_blob(const std::vector<unsigned char>& vch)
    {
        memcpy(data, &vch[0], sizeof(data));
    }

    base_blob(uint64_t b)
    {
        data_[0] = (unsigned int)b;
        data_[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            data_[i] = 0;
    }

    base_blob& operator=(uint64_t b)
    {
        data_[0] = (unsigned int)b;
        data_[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            data_[i] = 0;
        return *this;
    }

    bool empty() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (data_[i] != 0)
                return false;
        return true;
    }

    void clear()
    {
        memset(data_, 0, sizeof(data_));
    }

    inline int compare(const base_blob& other) const { return memcmp(data_, other.data_, sizeof(data_)); }

    friend inline bool operator==(const base_blob& a, const base_blob& b) { return a.Compare(b) == 0; }
    friend inline bool operator!=(const base_blob& a, const base_blob& b) { return a.Compare(b) != 0; }
    friend inline bool operator<(const base_blob& a, const base_blob& b) { return a.Compare(b) < 0; }

    std::string get_hex() const
    {
        return string_helper::to_hex(std::reverse_iterator<const uint8_t*>(data + sizeof(data)), std::reverse_iterator<const uint8_t*>(data));
    }
    void set_hex(const char* psz)
    {
        memset(data, 0, sizeof(data));

        // skip leading spaces
        while (isspace(*psz))
            psz++;

        // skip 0x
        if (psz[0] == '0' && tolower(psz[1]) == 'x')
            psz += 2;

        // hex string to uint
        const char* pbegin = psz;
        while (string_helper::hex_to_digit(*psz) != -1)
            psz++;
        psz--;
        unsigned char* p1 = (unsigned char*)data;
        unsigned char* pend = p1 + WIDTH;
        while (psz >= pbegin && p1 < pend) {
            *p1 = string_helper::hex_to_digit(*psz--);
            if (psz >= pbegin) {
                *p1 |= ((unsigned char)string_helper::hex_to_digit(*psz--) << 4);
                p1++;
            }
        }
    }

    void set_hex(const std::string& str)
    {
        set_hex(str.c_str());
    }

    std::string to_string() const
    {
        return get_hex();
    }

    unsigned char* begin()
    {
        return &data_[0];
    }

    unsigned char* end()
    {
        return &data_[WIDTH];
    }

    const unsigned char* begin() const
    {
        return &data_[0];
    }

    const unsigned char* end() const
    {
        return &data_[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(data_);
    }

    uint64_t get_uint64(int pos) const
    {
        const uint8_t* ptr = data_ + pos * 8;
        return ((uint64_t)ptr[0]) | \
            ((uint64_t)ptr[1]) << 8 | \
            ((uint64_t)ptr[2]) << 16 | \
            ((uint64_t)ptr[3]) << 24 | \
            ((uint64_t)ptr[4]) << 32 | \
            ((uint64_t)ptr[5]) << 40 | \
            ((uint64_t)ptr[6]) << 48 | \
            ((uint64_t)ptr[7]) << 56;
    }

    template<typename Stream>
    void serialize(Stream& s) const
    {
        s.write((char*)data_, sizeof(data_));
    }

    template<typename Stream>
    void unserialize(Stream& s)
    {
        s.read((char*)data_, sizeof(data_));
    }

protected:
    enum { WIDTH = BITS / 8 };
    uint8_t data_[WIDTH];
};

typedef base_blob<160> uint160;
typedef base_blob<256> uint256;

#define UBEGIN(a)           ((unsigned char*)&(a))
#define UEND(a)             ((unsigned char*)&((&(a))[1]))

#endif // BCUS_UINT_256_H
