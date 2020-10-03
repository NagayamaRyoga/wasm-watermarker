#ifndef INCLUDE_kyut_CircularBitStreamReader_hpp
#define INCLUDE_kyut_CircularBitStreamReader_hpp

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

namespace kyut {
    class CircularBitStreamReader {
    public:
        explicit CircularBitStreamReader(std::vector<std::uint8_t>&& data)
            : data_(std::move(data))
            , pos_bits_(0) {
        }

        explicit CircularBitStreamReader(std::string_view data)
            : data_(reinterpret_cast<const std::uint8_t*>(data.data()), reinterpret_cast<const std::uint8_t*>(data.data() + data.size()))
            , pos_bits_(0) {
        }

        // Uncopyable and unmovable
        CircularBitStreamReader(const CircularBitStreamReader&) = delete;
        CircularBitStreamReader(CircularBitStreamReader&&) = delete;

        CircularBitStreamReader& operator=(const CircularBitStreamReader&) = delete;
        CircularBitStreamReader& operator=(CircularBitStreamReader&&) = delete;

        ~CircularBitStreamReader() noexcept = default;

        std::uint64_t read(std::size_t size_bits) {
            std::uint64_t x = 0;
            for (std::size_t i = 0; i < size_bits; i++) {
                x <<= 1;
                x |= read_bit() ? 1 : 0;
            }

            return x;
        }

        bool read_bit() {
            const auto n = pos_bits_ >> 3;
            const auto k = pos_bits_ & 7;

            const auto bit = (data_[n] >> (7 - k)) & 1;

            pos_bits_++;
            pos_bits_ %= size_bits();

            return bit != 0;
        }

        std::size_t size_bytes() const noexcept {
            return data_.size();
        }

        std::size_t size_bits() const noexcept {
            return size_bytes() * 8;
        }

        std::size_t position_bits() const noexcept {
            return pos_bits_;
        }

    private:
        std::vector<std::uint8_t> data_;
        std::size_t pos_bits_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_CircularBitStreamReader_hpp
