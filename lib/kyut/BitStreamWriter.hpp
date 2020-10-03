#ifndef INCLUDE_kyut_BitStreamWriter_hpp
#define INCLUDE_kyut_BitStreamWriter_hpp

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

namespace kyut {
    class BitStreamWriter {
    public:
        BitStreamWriter()
            : data_()
            , pos_bits_(0) {
        }

        // Uncopyable and unmovable
        BitStreamWriter(const BitStreamWriter&) = delete;
        BitStreamWriter(BitStreamWriter&&) = delete;

        BitStreamWriter& operator=(const BitStreamWriter&) = delete;
        BitStreamWriter& operator=(BitStreamWriter&&) = delete;

        ~BitStreamWriter() noexcept = default;

        void write(std::uint64_t x, std::size_t size_bits) {
            assert(size_bits <= 64);

            for (std::size_t i = 0; i < size_bits; i++) {
                const auto bit = (x >> (size_bits - i - 1)) & 1;
                write_bit(bit != 0);
            }
        }

        void write_bit(bool x) {
            const auto n = pos_bits_ >> 3;
            const auto k = pos_bits_ & 7;

            if (k == 0) {
                data_.emplace_back(0);
            }

            data_[n] |= (x ? 1 : 0) << (7 - k);
            pos_bits_++;
        }

        std::size_t position_bits() const noexcept {
            return pos_bits_;
        }

        const std::vector<std::uint8_t>& data() const noexcept {
            return data_;
        }

        std::string_view data_as_str() const noexcept {
            return std::string_view(reinterpret_cast<const char*>(data_.data()), data_.size());
        }

    private:
        std::vector<std::uint8_t> data_;
        std::size_t pos_bits_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_BitStreamWriter_hpp
