#ifndef INCLUDE_kyut_BitStreamWriter_hpp
#define INCLUDE_kyut_BitStreamWriter_hpp

#include <cassert>
#include <cstdint>
#include <string_view>
#include <vector>

namespace kyut {
    class BitStreamWriter {
    public:
        explicit BitStreamWriter()
            : data_()
            , pos_write_(0) {}

        BitStreamWriter(const BitStreamWriter &) = delete;
        BitStreamWriter(BitStreamWriter &&) = delete;

        BitStreamWriter &operator=(const BitStreamWriter &) = delete;
        BitStreamWriter &operator=(BitStreamWriter &&) = delete;

        ~BitStreamWriter() noexcept = default;

        [[nodiscard]] const std::vector<std::uint8_t> &data() const noexcept {
            return data_;
        }

        [[nodiscard]] std::string_view dataAsString() const {
            return std::string_view{
                reinterpret_cast<const char *>(data_.data()),
                data_.size(),
            };
        }

            [[nodiscard]] std::size_t tell() const noexcept {
            return pos_write_;
        }

        void writeBit(bool value) {
            if (pos_write_ % 8 == 0) {
                data_.emplace_back(0);
            }

            data_[pos_write_ / 8] |= (value ? 1 : 0) << (7 - pos_write_ % 8);
            pos_write_++;
        }

        template <typename UInt>
        void write(UInt value, std::size_t length) {
            static_assert(std::is_unsigned_v<UInt>);
            assert(length <= sizeof(UInt) * 8);

            for (std::size_t i = 0; i < length; i++) {
                writeBit((value & (1 << (length - i - 1))) != 0);
            }
        }

    private:
        std::vector<std::uint8_t> data_;
        std::size_t pos_write_;
    };
} // namespace kyut

#endif // INCLUDE_kyut_BitStreamWriter_hpp
