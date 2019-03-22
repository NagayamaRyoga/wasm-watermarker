#ifndef INCLUDE_kyut_circular_bit_stream_hpp
#define INCLUDE_kyut_circular_bit_stream_hpp

#include <cassert>
#include <cstdint>

#include <memory>
#include <string_view>
#include <vector>

namespace kyut {
    class CircularBitStream {
    public:
        explicit CircularBitStream(const std::uint8_t *data, std::size_t size_bytes) noexcept
            : bytes_(data, data + size_bytes)
            , pos_bits_(0) {
            assert(data != nullptr || size_bytes == 0);
        }

        CircularBitStream(const CircularBitStream &) = delete;
        CircularBitStream(CircularBitStream &&) = delete;

        CircularBitStream &operator=(const CircularBitStream &) = delete;
        CircularBitStream &operator=(CircularBitStream &&) = delete;

        ~CircularBitStream() noexcept = default;

        [[nodiscard]] const std::vector<std::uint8_t> &bytes() const noexcept {
            return bytes_;
        }

        [[nodiscard]] std::size_t size_bytes() const noexcept {
            return bytes_.size();
        }

        [[nodiscard]] std::size_t size_bits() const noexcept {
            return size_bytes() * 8;
        }

        [[nodiscard]] std::size_t pos_bits() const noexcept {
            return pos_bits_;
        }

        bool read_bit() noexcept {
            if (size_bits() == 0) {
                return false;
            }

            const auto value = (bytes_[pos_bits_ >> 3] >> (pos_bits_ & 0x7)) & 0x1;

            if (++pos_bits_ >= size_bits()) {
                pos_bits_ = 0;
            }

            return value;
        }

        std::uint64_t read(std::size_t size_bits) {
            assert(size_bits <= 64);

            std::uint64_t value = 0;

            for (size_t i = 0; i < size_bits; i++) {
                if (read_bit()) {
                    value |= std::uint64_t{1} << i;
                }
            }

            return value;
        }

        static std::unique_ptr<CircularBitStream> from_string(std::string_view s) {
            return std::make_unique<CircularBitStream>(reinterpret_cast<const std::uint8_t *>(s.data()), s.length());
        }

    private:
        std::vector<std::uint8_t> bytes_;
        std::size_t pos_bits_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_circular_bit_stream_hpp
