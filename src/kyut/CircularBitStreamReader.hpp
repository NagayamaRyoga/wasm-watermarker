#ifndef INCLUDE_kyut_CircularBitStreamReader_hpp
#define INCLUDE_kyut_CircularBitStreamReader_hpp

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace kyut {
    class CircularBitStreamReader {
    public:
        explicit CircularBitStreamReader(std::vector<std::uint8_t> data)
            : data_(std::move(data))
            , pos_read_(0) {}

        template <typename Iterator>
        explicit CircularBitStreamReader(Iterator begin, Iterator end)
            : CircularBitStreamReader(std::vector<std::uint8_t>{begin, end}) {}

        CircularBitStreamReader(const CircularBitStreamReader &) = delete;
        CircularBitStreamReader(CircularBitStreamReader &&) = delete;

        CircularBitStreamReader &operator=(const CircularBitStreamReader &) = delete;
        CircularBitStreamReader &operator=(CircularBitStreamReader &&) = delete;

        ~CircularBitStreamReader() noexcept = default;

        static std::unique_ptr<CircularBitStreamReader> fromString(std::string_view s) {
            return std::make_unique<CircularBitStreamReader>(
                reinterpret_cast<const std::uint8_t *>(s.data()),
                reinterpret_cast<const std::uint8_t *>(s.data() + s.size()));
        }

        [[nodiscard]] const std::vector<std::uint8_t> &data() const noexcept {
            return data_;
        }

        [[nodiscard]] std::size_t tell() const noexcept {
            return pos_read_;
        }

        bool readBit() {
            if (data_.empty()) {
                return false;
            }

            const auto pos = pos_read_++;
            return ((data_[pos / 8 % data_.size()] << (pos % 8)) & 0x80) != 0;
        }

        template <typename UInt>
        UInt read(std::size_t countBits) {
            static_assert(std::is_unsigned_v<UInt>);
            assert(countBits <= sizeof(UInt) * 8);

            UInt value = 0;

            for (std::size_t i = 0; i < countBits; i++) {
                value <<= 1;
                value |= readBit() ? 1 : 0;
            }

            return value;
        }

    private:
        std::vector<std::uint8_t> data_;
        std::size_t pos_read_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_CircularBitStreamReader_hpp
