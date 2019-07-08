#ifndef INCLUDE_kyut_BitStreamReader_hpp
#define INCLUDE_kyut_BitStreamReader_hpp

#include <cstdint>
#include <memory>
#include <string_view>
#include <vector>

namespace kyut {
    class BitStreamReader {
    public:
        explicit BitStreamReader(std::vector<std::uint8_t> data)
            : data_(std::move(data))
            , pos_read_(0) {}

        template <typename Iterator>
        explicit BitStreamReader(Iterator begin, Iterator end)
            : BitStreamReader({begin, end}) {}

        BitStreamReader(const BitStreamReader &) = delete;
        BitStreamReader(BitStreamReader &&) = delete;

        BitStreamReader &operator=(const BitStreamReader &) = delete;
        BitStreamReader &operator=(BitStreamReader &&) = delete;

        ~BitStreamReader() noexcept = default;

        static std::unique_ptr<BitStreamReader> fromString(std::string_view s) {
            return std::make_unique<BitStreamReader>(reinterpret_cast<const std::uint8_t *>(s.data()),
                                                     reinterpret_cast<const std::uint8_t *>(s.data() + s.size()));
        }

        [[nodiscard]] std::size_t tell() const noexcept {
            return pos_read_;
        }

        bool readBit() {
            if (data_.empty()) {
                return false;
            }

            const auto pos = pos_read_;
            pos_read_ = (pos_read_ + 1) % data_.size();

            return (data_[pos / 8] >> (pos % 8)) & 1;
        }

        std::uint64_t read(std::size_t countBits) {
            std::uint64_t value = 0;

            for (std::size_t i = 0; i < countBits; i++) {
                value <<= 1;
                value |= readBit();
            }

            return value;
        }

    private:
        std::vector<std::uint8_t> data_;
        std::size_t pos_read_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_BitStreamReader_hpp
