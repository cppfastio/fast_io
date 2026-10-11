#pragma once

namespace fast_io
{

namespace details
{

struct bitvec_rep
{
	char unsigned *begin_ptr{};
	::std::size_t curr_pos{}, end_pos{};
};

struct bitvec_split_bits_result
{
	::std::size_t full_bytes;
	::std::size_t rem;
};

template <::std::size_t underlying_digits>
inline constexpr ::fast_io::details::bitvec_split_bits_result bitvec_split_bits(::std::size_t bits) noexcept
{
	if constexpr (underlying_digits == 8)
	{
		return {bits >> 3u, bits & 7u};
	}
	else
	{
		return {bits / underlying_digits, bits % underlying_digits};
	}
}
template <typename R>
concept boolean_testable_for_range = ::std::ranges::range<R> && requires(std::ranges::range_reference_t<R> v) {
	static_cast<bool>(v);
};

} // namespace details

namespace containers
{

template <typename allocator>
class bitvec
{
public:
	using allocator_type = allocator;
	using underlying_type = char unsigned;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using underlying_pointer = underlying_type *;
	using underlying_const_pointer = underlying_type const *;
	static inline constexpr size_type underlying_digits{
		::std::numeric_limits<underlying_type>::digits};
	::fast_io::details::bitvec_rep imp{};
	constexpr bitvec() noexcept = default;

private:
	template <::fast_io::details::boolean_testable_for_range R>
	static inline constexpr bitvec construct_from_range(R &&r)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any ||
									   !noexcept(static_cast<bool>(*::std::declval<::std::ranges::iterator_t<R> &>())) ||
									   !noexcept(++::std::declval<::std::ranges::iterator_t<R> &>()) ||
									   !noexcept(::std::declval<::std::ranges::iterator_t<R> &>() != ::std::declval<::std::ranges::sentinel_t<R>>()))
	{
		// default empty
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type n = static_cast<size_type>(::std::ranges::size(r));
			bitvec tmp(n);

			underlying_pointer out = tmp.imp.begin_ptr;

			underlying_type acc{};
			size_type bit_count{};

			for (auto &&elem : r)
			{
				acc |= static_cast<underlying_type>(static_cast<underlying_type>(static_cast<bool>(elem)) << bit_count);
				++bit_count;

				if (bit_count == underlying_digits)
				{
					*out = acc;
					acc = 0u;
					bit_count = 0;
					++out;
				}
			}

			if (bit_count != 0)
			{
				*out = acc;
			}
			return tmp;
		}
		else
		{
			bitvec tmp;
			for (auto &&elem : r)
			{
				tmp.push_back(static_cast<bool>(elem));
			}
			return tmp;
		}
	}

public:
	template <::fast_io::details::boolean_testable_for_range R>
	explicit constexpr bitvec(::fast_io::freestanding::from_range_t, R &&r)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(construct_from_range(::std::declval<R &&>()))
		: bitvec(construct_from_range(::std::forward<R>(r)))
	{
	}
	template <typename T>
		requires ::fast_io::details::boolean_testable_for_range<::std::initializer_list<T>>
	explicit constexpr bitvec(::std::initializer_list<T> il)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any || !noexcept(static_cast<bool>(::std::declval<T const &>())))
		: bitvec(::fast_io::freestanding::from_range, il)
	{}

	static inline constexpr ::std::size_t max_size() noexcept
	{
		constexpr ::std::size_t szmx{::std::numeric_limits<::std::size_t>::max()};
		return szmx;
	}
	static inline constexpr ::std::size_t max_size_bytes() noexcept
	{
		constexpr ::std::size_t szbytesmx{::std::numeric_limits<::std::size_t>::max() /
										  ::std::numeric_limits<underlying_type>::digits};
		return szbytesmx;
	}

private:
	using typed_allocator = ::fast_io::typed_generic_allocator_adapter<allocator, underlying_type>;
	static inline constexpr bool throwing_allocation{typed_allocator::throws_on_allocation_failure};
	static inline constexpr bool throwing_violations{typed_allocator::throws_on_violations};
	static inline constexpr bool throwing_any{throwing_allocation || throwing_violations};
	inline constexpr void grow_to_new_capacity(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_allocation)
	{
		::std::size_t current_capacity{this->imp.end_pos};
		if constexpr (underlying_digits == 8)
		{
			current_capacity >>= 3u;
		}
		else
		{
			current_capacity /= underlying_digits;
		}

		auto [new_begin_ptr, new_capacity] = typed_allocator::reallocate_zero_n_at_least(this->imp.begin_ptr, current_capacity, n);
		constexpr ::std::size_t mxbytes{max_size_bytes()};
		if (mxbytes < new_capacity)
		{
			new_capacity = mxbytes;
		}
		this->imp.begin_ptr = new_begin_ptr;
		if constexpr (underlying_digits == 8)
		{
			new_capacity <<= 3u;
		}
		else
		{
			new_capacity *= underlying_digits;
		}
		this->imp.end_pos = new_capacity;
	}
	inline constexpr void grow_twice()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		::std::size_t current_capacity{this->imp.end_pos};
		if constexpr (underlying_digits == 8)
		{
			current_capacity >>= 3u;
		}
		else
		{
			current_capacity /= underlying_digits;
		}
		constexpr ::std::size_t mxbyteshalf{max_size() >> 1};
		if (mxbyteshalf < current_capacity)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		::std::size_t toallocate{current_capacity << 1u};
		if (current_capacity == 0)
		{
			toallocate = 1;
		}
		this->grow_to_new_capacity(toallocate);
	}
	inline static constexpr ::fast_io::details::bitvec_rep allocate_new_bytes(size_type to_allocate_bytes)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		constexpr ::std::size_t mxbytes{max_size_bytes()};
		if (to_allocate_bytes == 0)
		{
			return {};
		}
		if (mxbytes < to_allocate_bytes)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		auto [new_begin_ptr, new_capacity] = typed_allocator::allocate_zero_at_least(to_allocate_bytes);
		if (mxbytes < new_capacity)
		{
			new_capacity = mxbytes;
		}
		if constexpr (underlying_digits == 8)
		{
			new_capacity <<= 3u;
		}
		else
		{
			new_capacity *= underlying_digits;
		}

		return {new_begin_ptr, 0, new_capacity};
	}
	inline static constexpr ::fast_io::details::bitvec_rep allocate_new_bits(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		if (n == 0)
		{
			return {};
		}
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(n);
		size_type to_new_bytes{byte_index + (bit_index != 0u)};
		auto rep = allocate_new_bytes(to_new_bytes);
		rep.curr_pos = n;
		return rep;
	}
	inline static constexpr ::fast_io::details::bitvec_rep
	clone_imp(::fast_io::details::bitvec_rep const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		using U = underlying_type;
		size_type const n{other.curr_pos};
		if (n == 0)
		{
			return {};
		}

		auto [full_units, rem_bits] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(n);

		// number of underlying units we need to copy
		size_type to_copy_units{full_units + (rem_bits != 0u)};

		auto newrep{allocate_new_bytes(to_copy_units)};

		// copy all full units
		auto it{::fast_io::freestanding::non_overlapped_copy_n(
			other.begin_ptr, full_units, newrep.begin_ptr)};

		// handle partial last unit, if any
		if (rem_bits != 0u)
		{
			U last = other.begin_ptr[full_units];
			U mask = static_cast<underlying_type>((U{1} << rem_bits) - 1u);
			*it = static_cast<U>(last & mask);
		}

		newrep.curr_pos = n;
		return newrep;
	}

	inline constexpr void destroy_bitvec() noexcept
	{
		auto begin_ptr{imp.begin_ptr};
		if (begin_ptr == nullptr)
		{
			return;
		}
		typed_allocator::deallocate_n(imp.begin_ptr, imp.end_pos);
	}

public:
	inline constexpr bitvec(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any) : imp{allocate_new_bits(n)}
	{
	}

	inline constexpr void push_back(bool v)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		if (this->imp.curr_pos == this->imp.end_pos) [[unlikely]]
		{
			this->grow_twice();
		}
		this->push_back_unchecked(v);
	}

	inline constexpr void push_back_unchecked(bool v) noexcept
	{
		if constexpr (underlying_digits == 8)
		{
			size_type bitpos{imp.curr_pos};
			size_type byte_index{bitpos >> 3}; // bitpos / 8
			size_type bit_index{bitpos & 7};   // bitpos % 8

			underlying_type &byteval = imp.begin_ptr[byte_index];
			underlying_type mask = static_cast<underlying_type>(1u << bit_index);

			// Branchless set/clear
			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(v) * mask);

			++imp.curr_pos;
		}
		else
		{
			size_type bitpos{imp.curr_pos};
			size_type byte_index{bitpos / underlying_digits};
			size_type bit_index{bitpos % underlying_digits};

			underlying_type &byteval = imp.begin_ptr[byte_index];
			underlying_type mask =
				static_cast<underlying_type>(1u << bit_index);

			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(v) * mask);

			++imp.curr_pos;
		}
	}

	inline constexpr bool test_unchecked(size_type pos) const noexcept
	{
		if constexpr (underlying_digits == 8)
		{
			// Compute which byte contains the bit
			size_type byte_index{pos >> 3}; // pos / 8
			// Compute which bit inside that byte
			size_type bit_index{pos & 7}; // pos % 8

			underlying_type byteval{imp.begin_ptr[byte_index]};

			// Extract the bit
			return (byteval >> bit_index) & 1u;
		}
		else
		{
			// Compute which byte contains the bit
			size_type byte_index{pos / underlying_digits};
			// Compute which bit inside that byte
			size_type bit_index{pos % underlying_digits};

			underlying_type byteval{imp.begin_ptr[byte_index]};

			// Extract the bit
			return (byteval >> bit_index) & 1u;
		}
	}
	inline constexpr bool test(size_type pos) const
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return this->test_unchecked(pos);
	}
	inline constexpr void set_unchecked(size_type pos, bool value = true) noexcept
	{
		if constexpr (underlying_digits == 8)
		{
			size_type byte_index{pos >> 3}; // pos / 8
			size_type bit_index{pos & 7};   // pos % 8

			underlying_type &byteval = imp.begin_ptr[byte_index];
			underlying_type mask = static_cast<underlying_type>(1u << bit_index);

			// Branchless set/clear
			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(value) * mask);
		}
		else
		{
			size_type byte_index{pos / underlying_digits};
			size_type bit_index{pos % underlying_digits};

			underlying_type &byteval = imp.begin_ptr[byte_index];
			underlying_type mask =
				static_cast<underlying_type>(1u << bit_index);

			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(value) * mask);
		}
	}
	inline constexpr void set(size_type pos, bool value = true)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->set_unchecked(pos, value);
	}

	inline constexpr void reset_unchecked(size_type pos) noexcept
	{
		if constexpr (underlying_digits == 8)
		{
			size_type byte_index{pos >> 3}; // pos / 8
			size_type bit_index{pos & 7};   // pos % 8

			underlying_type &byteval = imp.begin_ptr[byte_index];
			underlying_type mask = static_cast<underlying_type>(1u << bit_index);

			// Clear the bit (branchless)
			byteval &= static_cast<underlying_type>(~mask);
		}
		else
		{
			size_type byte_index{pos / underlying_digits};
			size_type bit_index{pos % underlying_digits};

			underlying_type mask =
				static_cast<underlying_type>(1u << bit_index);

			imp.begin_ptr[byte_index] &= static_cast<underlying_type>(~mask);
		}
	}
	inline constexpr void reset(size_type pos)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->reset_unchecked(pos);
	}

	inline constexpr bool pop_back_unchecked() noexcept
	{
		// Position of last bit (curr_pos > 0 assumed)
		size_type bitpos{imp.curr_pos - 1};
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);

		underlying_type &byteval = imp.begin_ptr[byte_index];
		underlying_type mask =
			static_cast<underlying_type>(1u << bit_index);

		bool old = (byteval >> bit_index) & 1u;

		byteval &= static_cast<underlying_type>(~mask);

		--imp.curr_pos;
		return old;
	}

	inline constexpr bool pop_back()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!(this->imp.curr_pos)) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return this->pop_back_unchecked();
	}

	inline constexpr bool test_front_unchecked() const noexcept
	{
		// front bit is always at pos 0
		return (*this->imp.begin_ptr) & 1u;
	}

	inline constexpr bool test_front() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return this->test_front_unchecked();
	}

	inline constexpr void set_front_unchecked(bool value = true) noexcept
	{
		underlying_type &byteval = *imp.begin_ptr;
		constexpr underlying_type mask = static_cast<underlying_type>(1u);
		constexpr underlying_type invmask = ~mask;

		byteval = (byteval & invmask) | (static_cast<underlying_type>(value));
	}
	inline constexpr void set_front(bool value = true)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->set_front_unchecked(value);
	}

	inline constexpr void reset_front_unchecked() noexcept
	{
		constexpr underlying_type mask = static_cast<underlying_type>(1u);
		constexpr underlying_type invmask = ~mask;
		(*imp.begin_ptr) &= invmask;
	}

	inline constexpr void reset_front()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->reset_front_unchecked();
	}

	inline constexpr bool test_back_unchecked() const noexcept
	{
		// last bit is at pos curr_pos - 1
		size_type bitpos{this->imp.curr_pos - 1};

		if constexpr (underlying_digits == 8)
		{
			size_type byte_index{bitpos >> 3}; // bitpos / 8
			size_type bit_index{bitpos & 7};   // bitpos % 8

			underlying_type byteval{this->imp.begin_ptr[byte_index]};
			return (byteval >> bit_index) & 1u;
		}
		else
		{
			size_type byte_index{bitpos / underlying_digits};
			size_type bit_index{bitpos % underlying_digits};

			underlying_type byteval{this->imp.begin_ptr[byte_index]};
			return (byteval >> bit_index) & 1u;
		}
	}

	inline constexpr bool test_back() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		return this->test_back_unchecked();
	}

	inline constexpr void set_back_unchecked(bool value = true) noexcept
	{
		size_type bitpos{this->imp.curr_pos - 1};

		if constexpr (underlying_digits == 8)
		{
			size_type byte_index{bitpos >> 3};
			size_type bit_index{bitpos & 7};

			underlying_type &byteval = this->imp.begin_ptr[byte_index];
			underlying_type mask = static_cast<underlying_type>(1u << bit_index);

			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(value) * mask);
		}
		else
		{
			size_type byte_index{bitpos / underlying_digits};
			size_type bit_index{bitpos % underlying_digits};

			underlying_type &byteval = this->imp.begin_ptr[byte_index];
			underlying_type mask = static_cast<underlying_type>(1u << bit_index);

			byteval = (byteval & static_cast<underlying_type>(~mask)) |
					  (static_cast<underlying_type>(value) * mask);
		}
	}

	inline constexpr void set_back(bool value = true)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->set_back_unchecked(value);
	}

	inline constexpr void reset_back_unchecked() noexcept
	{
		size_type bitpos{this->imp.curr_pos - 1};
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);
		underlying_type mask = static_cast<underlying_type>(1u << bit_index);
		(this->imp.begin_ptr[byte_index]) &= static_cast<underlying_type>(~mask);
	}

	inline constexpr void reset_back()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->reset_back_unchecked();
	}

	inline constexpr void flip_unchecked(size_type pos) noexcept
	{
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(pos);
		underlying_type &byteval = imp.begin_ptr[byte_index];
		underlying_type mask = static_cast<underlying_type>(1u << bit_index);

		// Toggle the bit (branchless)
		byteval ^= mask;
	}

	inline constexpr void flip(size_type pos)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos <= pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->flip_unchecked(pos);
	}

	inline constexpr void flip_front_unchecked() noexcept
	{
		// front bit is always bit 0 of byte 0
		constexpr underlying_type mask = static_cast<underlying_type>(1u);
		(*imp.begin_ptr) ^= mask;
	}
	inline constexpr void flip_front()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->flip_front_unchecked();
	}

	inline constexpr void flip_back_unchecked() noexcept
	{
		size_type bitpos{imp.curr_pos - 1};
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);
		underlying_type &byteval = imp.begin_ptr[byte_index];
		underlying_type mask = static_cast<underlying_type>(1u << bit_index);
		byteval ^= mask;
	}

	inline constexpr void flip_back()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!this->imp.curr_pos) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		this->flip_back_unchecked();
	}

	constexpr bitvec(bitvec &&other) noexcept : imp{other.imp}
	{
		other.imp = {};
	}
	constexpr bitvec &operator=(bitvec &&other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->destroy_bitvec();
		this->imp = other.imp;
		other.imp = {};
		return *this;
	}
	constexpr bitvec(bitvec const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any) : imp{clone_imp(other.imp)}
	{}
	constexpr bitvec &operator=(bitvec const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		auto newimp{clone_imp(other.imp)};
		this->destroy_bitvec();
		this->imp = newimp;
		return *this;
	}
	constexpr ~bitvec() noexcept
	{
		this->destroy_bitvec();
	}
	constexpr void swap(bitvec &other) noexcept
	{
		::std::swap(this->imp, other.imp);
	}
	constexpr underlying_pointer underlying_data() noexcept
	{
		return this->imp.begin_ptr;
	}
	constexpr underlying_const_pointer underlying_data() const noexcept
	{
		return this->imp.begin_ptr;
	}
	constexpr size_type size() const noexcept
	{
		return this->imp.curr_pos;
	}
	constexpr size_type size_bytes() const noexcept
	{
		size_type bitpos{this->imp.curr_pos};
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);
		return byte_index + (bit_index != 0u);
	}
	constexpr size_type capacity() const noexcept
	{
		return this->imp.end_pos;
	}
	constexpr size_type capacity_bytes() const noexcept
	{
		size_type bitpos{this->imp.end_pos};
		auto [byte_index, bit_index] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);
		return byte_index + (bit_index != 0u);
	}
	constexpr void clear() noexcept
	{
		this->imp.curr_pos = 0;
	}
	constexpr void clear_destroy() noexcept
	{
		this->destroy_bitvec();
		this->imp = {};
	}

private:
	inline static constexpr size_type bits_to_blocks(size_type bits) noexcept
	{
		if constexpr (underlying_digits == 8)
		{
			return (bits + 7) >> 3; // ceil(bits / 8)
		}
		else
		{
			return (bits + (underlying_digits - 1)) / underlying_digits;
		}
	}

public:
	constexpr void reserve(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		if (max_size() - (underlying_digits - 1u) < n) [[unlikely]]
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::value_too_large);
		}
		if (this->imp.end_pos < n)
		{
			this->grow_to_new_capacity(bits_to_blocks(n));
		}
	}
	constexpr bool is_empty() const noexcept
	{
		return !this->imp.curr_pos;
	}
	constexpr bool empty() const noexcept
	{
		return !this->imp.curr_pos;
	}
	constexpr void shrink_to_fit()
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_allocation)
	{
		size_type currpos{this->imp.curr_pos};
		size_type endpos{this->imp.end_pos};
		if (!currpos)
		{
			if (endpos)
			{
				this->clear_destroy();
			}
			return;
		}
		size_type const curblocks{size_bytes()};
		size_type const endblocks{capacity_bytes()};
		if (curblocks != endblocks)
		{
			this->grow_to_new_capacity(curblocks);
		}
	}
	constexpr void reset_all() noexcept
	{
		::fast_io::freestanding::fill_n(this->imp.begin_ptr, this->size_bytes(), 0u);
	}

	constexpr void flip_all() noexcept
	{
		size_type bits = this->imp.curr_pos;
		size_type bytes = this->size_bytes();
		if (bytes == 0)
		{
			return;
		}

		underlying_pointer p = this->imp.begin_ptr;

		// Full bytes
		constexpr underlying_type mask{static_cast<underlying_type>(~static_cast<underlying_type>(0u))};
		auto [full_bytes, rem] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bits);
		underlying_pointer it = p;
		underlying_pointer end_full = p + full_bytes;
		for (; it != end_full; ++it)
		{
			*it ^= mask;
		}

		// Partial byte
		if (rem)
		{
			*it ^= static_cast<underlying_type>((1u << rem) - 1u);
		}
	}

	constexpr void set_all() noexcept
	{
		size_type bits = this->imp.curr_pos;
		size_type bytes = this->size_bytes();
		if (bytes == 0)
		{
			return;
		}

		underlying_pointer p = this->imp.begin_ptr;

		// Full bytes
		constexpr underlying_type mask{static_cast<underlying_type>(~static_cast<underlying_type>(0u))};
		auto [full_bytes, rem] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		::fast_io::freestanding::fill_n(p, full_bytes, mask);

		// Partial byte
		if (rem)
		{
			p[full_bytes] = static_cast<underlying_type>((1u << rem) - 1u);
		}
	}

	constexpr bitvec &operator&=(bitvec const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos != other.imp.curr_pos)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}

		std::size_t bits = this->imp.curr_pos;

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		auto *ap = this->imp.begin_ptr;
		auto const *bp = other.imp.begin_ptr;

		// full bytes (pointer loop)
		auto *it_a = ap;
		auto const *it_b = bp;
		auto *end_full = ap + full_bytes;

		for (; it_a != end_full; ++it_a, ++it_b)
		{
			*it_a &= *it_b;
		}

		// partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);

			// it_a now points exactly to the partial byte
			*it_a = static_cast<U>((*it_a & *it_b) & mask);
		}

		return *this;
	}


	constexpr bitvec &operator|=(bitvec const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos != other.imp.curr_pos)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}

		std::size_t bits = this->imp.curr_pos;

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		auto *ap = this->imp.begin_ptr;
		auto const *bp = other.imp.begin_ptr;

		auto *it_a = ap;
		auto const *it_b = bp;
		auto *end_full = ap + full_bytes;

		for (; it_a != end_full; ++it_a, ++it_b)
		{
			*it_a |= *it_b;
		}

		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			*it_a = static_cast<U>((*it_a | *it_b) & mask);
		}

		return *this;
	}

	constexpr bitvec &operator^=(bitvec const &other)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->imp.curr_pos != other.imp.curr_pos)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}

		std::size_t bits = this->imp.curr_pos;

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		auto *ap = this->imp.begin_ptr;
		auto const *bp = other.imp.begin_ptr;

		auto *it_a = ap;
		auto const *it_b = bp;
		auto *end_full = ap + full_bytes;

		for (; it_a != end_full; ++it_a, ++it_b)
		{
			*it_a ^= *it_b;
		}

		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			*it_a = static_cast<U>((*it_a ^ *it_b) & mask);
		}

		return *this;
	}

	constexpr bitvec &operator<<=(size_type shift) noexcept
	{
		size_type bits = this->imp.curr_pos;
		if (shift == 0 || bits == 0)
		{
			return *this;
		}

		if (shift >= bits)
		{
			this->reset_all();
			return *this;
		}

		// Compute byte_shift and bit_shift without division when underlying_digits == 8
		size_type byte_shift;
		size_type bit_shift;

		if constexpr (underlying_digits == 8)
		{
			byte_shift = shift >> 3u;
			bit_shift = shift & 7u;
		}
		else
		{
			byte_shift = shift / underlying_digits;
			bit_shift = shift % underlying_digits;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total_bytes = full_bytes + (rem != 0);

		auto *p = this->imp.begin_ptr;

		//
		// 1. Shift whole bytes upward
		//
		if (byte_shift != 0)
		{
			auto *dst = p + total_bytes;
			auto *src = dst - byte_shift;

			while (dst != p)
			{
				--dst;
				--src;
				*dst = (src >= p) ? *src : 0;
			}
		}

		//
		// 2. Shift bits inside bytes
		//
		if (bit_shift != 0)
		{
			using U = underlying_type;

			U carry{};
			auto *it = p;
			auto *end = p + total_bytes;

			for (; it != end; ++it)
			{
				U new_carry = static_cast<U>(*it >> (underlying_digits - bit_shift));
				*it = static_cast<U>((*it << bit_shift) | carry);
				carry = new_carry;
			}
		}

		//
		// 3. Mask partial byte
		//
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			p[full_bytes] &= mask;
		}

		return *this;
	}

	constexpr bitvec &operator>>=(size_type shift) noexcept
	{
		size_type bits = this->imp.curr_pos;
		if (shift == 0 || bits == 0)
		{
			return *this;
		}

		if (shift >= bits)
		{
			this->reset_all();
			return *this;
		}

		// Compute byte_shift and bit_shift without division when underlying_digits == 8
		auto [byte_shift, bit_shift] = ::fast_io::details::bitvec_split_bits<underlying_digits>(shift);

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total_bytes = full_bytes + (rem != 0);

		auto *p = this->imp.begin_ptr;

		//
		// 1. Shift whole bytes downward
		//
		if (byte_shift != 0)
		{
			auto *dst = p;
			auto *src = p + byte_shift;
			auto *end = p + total_bytes;

			for (; dst != end; ++dst, ++src)
			{
				*dst = (src < end) ? *src : 0;
			}
		}

		//
		// 2. Shift bits inside bytes
		//
		if (bit_shift != 0)
		{
			using U = underlying_type;

			U carry{};
			auto *it = p + total_bytes;

			while (it != p)
			{
				--it;
				U new_carry = static_cast<U>(*it << (underlying_digits - bit_shift));
				*it = static_cast<U>((*it >> bit_shift) | carry);
				carry = new_carry;
			}
		}

		//
		// 3. Mask partial byte
		//
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			p[full_bytes] &= mask;
		}

		return *this;
	}

	constexpr bitvec operator~() const
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		bitvec tmp(*this);
		tmp.flip_all();
		return tmp;
	}

	constexpr bitvec &rotl_assign(difference_type shift)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		size_type bits = this->imp.curr_pos;
		if (bits == 0)
		{
			return *this;
		}

		using U = std::make_unsigned_t<difference_type>;

		// Step 1: convert to unsigned (safe even for PTRDIFF_MIN)
		U u = static_cast<U>(shift);

		// Step 2: reduce modulo bits
		size_type s = static_cast<size_type>(u % bits);

		if (s == 0)
		{
			return *this;
		}

		// Step 3: detect sign WITHOUT negation (avoids UB)
		bool negative = (shift < 0);

		if (negative)
		{
			// rotl(-s) == rotr(s)
			return this->rotr_assign(static_cast<difference_type>(s));
		}

		// Positive rotation: rotl(s)
		bitvec tmp(*this);

		*this <<= s;
		tmp >>= (bits - s);
		*this |= tmp;

		return *this;
	}

	constexpr bitvec &rotr_assign(difference_type shift)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		size_type bits = this->imp.curr_pos;
		if (bits == 0)
		{
			return *this;
		}

		using U = std::make_unsigned_t<difference_type>;

		// Step 1: convert to unsigned (safe even for PTRDIFF_MIN)
		U u = static_cast<U>(shift);

		// Step 2: reduce modulo bits
		size_type s = static_cast<size_type>(u % bits);

		if (s == 0)
		{
			return *this;
		}

		// Step 3: detect sign WITHOUT negation (avoids UB)
		bool negative = (shift < 0);

		if (negative)
		{
			// rotr(-s) == rotl(s)
			return this->rotl_assign(static_cast<difference_type>(s));
		}

		// Positive rotation: rotr(s)
		bitvec tmp(*this);

		*this >>= s;
		tmp <<= (bits - s);
		*this |= tmp;

		return *this;
	}

	constexpr bool has_single_bit() const noexcept
	{
		auto *p = this->imp.begin_ptr;

		size_type bits = this->imp.curr_pos;
		if (bits == 0)
		{
			return false;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		bool found = false;

		// full bytes
		auto *it = p;
		auto *end_full = p + full_bytes;

		for (; it != end_full; ++it)
		{
			unsigned char v = *it;
			if (v == 0)
			{
				continue;
			}

			// more than one nonzero byte → definitely more than one bit
			if (found)
			{
				return false;
			}

			// count bits in this byte
			if ((v & (v - 1)) != 0)
			{
				return false; // more than one bit in this byte
			}

			found = true;
		}

		// partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(p[full_bytes] & mask);

			if (v != 0)
			{
				if (found)
				{
					return false;
				}

				if ((v & (v - 1)) != 0)
				{
					return false;
				}

				found = true;
			}
		}

		return found;
	}

	constexpr bitvec &bit_floor_assign() noexcept
	{
		size_type bits = this->imp.curr_pos;
		if (bits == 0)
		{
			return *this;
		}

		size_type highest = this->bit_width();
		if (highest == 0)
		{
			// value is zero → stays zero
			this->reset_all();
			return *this;
		}

		// Clear everything
		this->reset_all();

		// Set only the highest bit
		this->set_unchecked(highest - 1);

		return *this;
	}

	constexpr bool is_all_zero() const noexcept
	{
		size_type bits{this->imp.curr_pos};

		if (bits == 0)
		{
			return true;
		}

		auto [full_units, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		// --------------------------------------------------------
		// Constexpr fallback: manual loop
		// --------------------------------------------------------
		auto *p{this->imp.begin_ptr};
		auto it{p};
		auto end{p + full_units};

		for (; it != end; ++it)
		{
			if (*it)
			{
				return false;
			}
		}


		// ------------------------------------------------------------
		// Check partial word
		// ------------------------------------------------------------
		if (rem)
		{
			using U = underlying_type;
			U mask = static_cast<U>((U{1} << rem) - 1u);
			if ((*it & mask))
			{
				return false;
			}
		}

		return true;
	}


	constexpr bitvec &bit_ceil_assign() noexcept
	{
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return *this;
		}

		// If zero → becomes 1
		if (this->is_all_zero())
		{
			this->reset_all();
			this->set_unchecked(0);
			return *this;
		}

		// If already a power of two → unchanged
		if (this->has_single_bit())
		{
			return *this;
		}

		// Otherwise: set only the bit at bit_width()
		size_type w = this->bit_width();

		this->reset_all();
		this->set_unchecked(w - 1);

		return *this;
	}

	constexpr size_type bit_width() const noexcept
	{
		auto *p = this->imp.begin_ptr;

		size_type bits = this->imp.curr_pos;
		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total_bytes = full_bytes + (rem != 0);

		// Start from the highest byte
		auto *it = p + total_bytes;

		// Check partial byte first
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(p[full_bytes] & mask);

			if (v != 0)
			{
				// find highest bit in v
				size_type pos = 0;
				while (v >>= 1)
				{
					++pos;
				}

				return full_bytes * underlying_digits + pos + 1;
			}

			--it;
		}

		// Scan full bytes
		while (it != p)
		{
			--it;
			unsigned char v = *it;
			if (v != 0)
			{
				size_type pos = 0;
				while (v >>= 1)
				{
					++pos;
				}

				return (it - p) * underlying_digits + pos + 1;
			}
		}

		return 0; // all zero
	}

	constexpr size_type countl_zero() const noexcept
	{
		auto *p = this->imp.begin_ptr;
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total_bytes = full_bytes + (rem != 0);
		size_type count = 0;

		// Start at MSB side
		auto *it = p + total_bytes;

		// Partial byte first
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(p[full_bytes] & mask);

			if (v != 0)
			{
				// Count leading zeros inside partial byte
				size_type leading = 0;
				U bit = static_cast<U>(1u << (rem - 1u));
				while ((bit != 0) && ((v & bit) == 0))
				{
					++leading;
					bit >>= 1;
				}
				return leading;
			}

			count += rem;
			--it;
		}

		// Full bytes
		while (it != p)
		{
			--it;
			unsigned char v = *it;

			if (v == 0)
			{
				count += underlying_digits;
				continue;
			}

			// Count leading zeros in this byte
			unsigned char bit = static_cast<unsigned char>(1u << (underlying_digits - 1u));
			while ((bit != 0) && ((v & bit) == 0))
			{
				++count;
				bit >>= 1;
			}
			return count;
		}

		return count;
	}

	constexpr size_type countl_one() const noexcept
	{
		auto *p = this->imp.begin_ptr;
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total_bytes = full_bytes + (rem != 0);
		size_type count = 0;

		auto *it = p + total_bytes;

		// Partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(p[full_bytes] & mask);

			if (v != mask)
			{
				// Count leading ones inside partial byte
				size_type leading = 0;
				U bit = static_cast<U>(1u << (rem - 1u));
				while ((bit != 0) && ((v & bit) != 0))
				{
					++leading;
					bit >>= 1;
				}
				return leading;
			}

			count += rem;
			--it;
		}

		// Full bytes
		while (it != p)
		{
			--it;
			unsigned char v = *it;

			if (v == 0xFF)
			{
				count += underlying_digits;
				continue;
			}

			unsigned char bit = static_cast<unsigned char>(1u << (underlying_digits - 1u));
			while ((bit != 0) && ((v & bit) != 0))
			{
				++count;
				bit >>= 1;
			}
			return count;
		}

		return count;
	}

	constexpr size_type countr_zero() const noexcept
	{
		auto *p = this->imp.begin_ptr;
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type count = 0;

		// Start at LSB side
		auto *it = p;

		// Full bytes first
		for (size_type i = 0; i < full_bytes; ++i, ++it)
		{
			unsigned char v = *it;
			if (v == 0)
			{
				count += underlying_digits;
				continue;
			}

			// Count trailing zeros in this byte
			unsigned char bit = 1u;
			while ((bit != 0) && ((v & bit) == 0))
			{
				++count;
				bit <<= 1;
			}
			return count;
		}

		// Partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(*it & mask);

			if (v == 0)
			{
				return count + rem;
			}

			U bit = 1u;
			while ((bit < (1u << rem)) && ((v & bit) == 0))
			{
				++count;
				bit <<= 1;
			}
		}

		return count;
	}

	constexpr size_type countr_one() const noexcept
	{
		auto *p = this->imp.begin_ptr;
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type count = 0;

		auto *it = p;

		// Full bytes
		for (size_type i = 0; i < full_bytes; ++i, ++it)
		{
			unsigned char v = *it;
			if (v == 0xFF)
			{
				count += underlying_digits;
				continue;
			}

			unsigned char bit = 1u;
			while ((bit != 0) && ((v & bit) != 0))
			{
				++count;
				bit <<= 1;
			}
			return count;
		}

		// Partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(*it & mask);

			if (v == mask)
			{
				return count + rem;
			}

			U bit = 1u;
			while ((bit < (1u << rem)) && ((v & bit) != 0))
			{
				++count;
				bit <<= 1;
			}
		}

		return count;
	}

	constexpr size_type popcount() const noexcept
	{
		auto *p = this->imp.begin_ptr;
		size_type bits = this->imp.curr_pos;

		if (bits == 0)
		{
			return 0;
		}

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(bits);

		size_type total = 0;

		// Full bytes
		auto *it = p;
		auto *end_full = p + full_bytes;

		for (; it != end_full; ++it)
		{
			unsigned char v = *it;
			// classic popcount
			while (v)
			{
				v &= (v - 1);
				++total;
			}
		}

		// Partial byte
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			U v = static_cast<U>(p[full_bytes] & mask);

			while (v)
			{
				v &= (v - 1);
				++total;
			}
		}

		return total;
	}

	constexpr void resize(size_type n)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		size_type old = this->imp.curr_pos;

		if (n == old)
		{
			return;
		}

		auto *p = this->imp.begin_ptr;

		//
		// GROWING
		//
		if (n > old)
		{
			this->reserve(n);

			auto [old_full, old_rem] =
				::fast_io::details::bitvec_split_bits<underlying_digits>(old);

			auto [new_full, new_rem] =
				::fast_io::details::bitvec_split_bits<underlying_digits>(n);

			//
			// 1. Mask old partial byte
			//
			if (old_rem != 0)
			{
				using U = underlying_type;
				U mask = static_cast<U>((1u << old_rem) - 1u);
				p[old_full] &= mask;
			}

			//
			// 2. Zero full bytes between old and new
			//
			size_type start = old_full + (old_rem != 0);
			if (start < new_full)
			{
				::fast_io::freestanding::fill(
					p + start,
					p + new_full,
					static_cast<unsigned char>(0));
			}

			//
			// 3. Mask new partial byte
			//
			if (new_rem != 0)
			{
				using U = underlying_type;
				U mask = static_cast<U>((1u << new_rem) - 1u);
				p[new_full] &= mask;
			}

			this->imp.curr_pos = n;
			return;
		}

		//
		// SHRINKING
		//
		this->imp.curr_pos = n;

		auto [full_bytes, rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(n);

		//
		// Mask partial byte
		//
		if (rem != 0)
		{
			using U = underlying_type;
			U mask = static_cast<U>((1u << rem) - 1u);
			p[full_bytes] &= mask;
		}

		// No need to zero trailing bytes — they are logically out of range.
	}

	inline constexpr size_type erase_index_unchecked(size_type firstpos, size_type lastpos) noexcept
	{
		size_type const erase_count = lastpos - firstpos;
		if (erase_count == 0)
		{
			return firstpos;
		}

		size_type const old_size = this->imp.curr_pos;
		size_type const new_size = old_size - erase_count;

		using U = underlying_type;
		U *p = this->imp.begin_ptr;

		// ------------------------------------------------------------
		// Fast path: underlying_digits == 8 (byte-wise, no division)
		// ------------------------------------------------------------
		if constexpr (underlying_digits == 8)
		{
			// Compute byte indices using shifts/masks
			size_type src_byte = lastpos >> 3;
			size_type dst_byte = firstpos >> 3;

			size_type src_bit_offset = lastpos & 7;
			size_type dst_bit_offset = firstpos & 7;

			size_type bit_shift = erase_count & 7;   // erase_count % 8
			size_type byte_shift = erase_count >> 3; // erase_count / 8

			underlying_pointer bp = p;

			// Shift whole bytes first
			if (byte_shift != 0)
			{
				size_type total_bytes = (old_size + 7) >> 3;
				for (size_t i = src_byte; i < total_bytes; ++i)
				{
					bp[(i - src_byte) + dst_byte] = bp[i];
				}
			}

			// Now handle cross-byte bit shifting
			if (bit_shift != 0)
			{
				size_type total_bytes = (old_size + 7) >> 3;

				underlying_type carry = 0;
				underlying_type rshift = bit_shift;
				underlying_type lshift = 8 - bit_shift;

				size_type out = dst_byte;
				size_type in = dst_byte;

				// Adjust input pointer by byte_shift
				in += byte_shift;

				for (; in < total_bytes; ++in, ++out)
				{
					underlying_type cur = bp[in];
					underlying_type merged = (cur >> rshift) | (carry << lshift);
					carry = cur;
					bp[out] = merged;
				}
			}

			// Update size
			this->imp.curr_pos = new_size;

			// Mask final partial byte
			size_type full_bytes = new_size >> 3;
			size_type rem_bits = new_size & 7;

			if (rem_bits != 0)
			{
				underlying_type mask = static_cast<underlying_type>((1u << rem_bits) - 1u);
				bp[full_bytes] &= mask;
			}

			return firstpos;
		}
		else
		{
			// ------------------------------------------------------------
			// Generic path: underlying_digits != 8 (word-wise)
			// ------------------------------------------------------------
			size_type src_word = lastpos / underlying_digits;
			size_type dst_word = firstpos / underlying_digits;

			size_type bit_shift = erase_count % underlying_digits;
			size_type word_shift = erase_count / underlying_digits;

			size_type total_words = (old_size + underlying_digits - 1) / underlying_digits;

			size_type src = src_word;
			size_type dst = dst_word;

			if (bit_shift == 0)
			{
				// Pure word shift
				for (; src < total_words; ++src, ++dst)
				{
					p[dst] = p[src];
				}
			}
			else
			{
				// Cross-word merge
				size_type rshift = bit_shift;
				size_type lshift = underlying_digits - bit_shift;

				for (; src + 1 < total_words; ++src, ++dst)
				{
					U lo = p[src];
					U hi = p[src + 1];
					p[dst] = (lo >> rshift) | (hi << lshift);
				}

				// Last word: only right-shift part
				p[dst] = p[src] >> rshift;
			}

			this->imp.curr_pos = new_size;

			// Mask final partial word
			size_type full_words = new_size / underlying_digits;
			size_type rem_bits = new_size % underlying_digits;

			if (rem_bits != 0)
			{
				U mask = (U{1} << rem_bits) - 1;
				p[full_words] &= mask;
			}

			return firstpos;
		}
	}

	inline constexpr size_type erase_index(size_type firstpos, size_type lastpos)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		// [firstpos, lastpos) must be a valid subrange of [0, imp.curr_pos)
		if (lastpos < firstpos || this->imp.curr_pos < lastpos)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}

		return this->erase_index_unchecked(firstpos, lastpos);
	}
	inline constexpr size_type erase_index(size_type idx)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		// idx must be a valid bit index
		if (this->imp.curr_pos <= idx)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}

		// erase the single bit [idx, idx+1)
		return this->erase_index_unchecked(idx, idx + 1);
	}
	inline constexpr size_type erase_index_unchecked(size_type idx) noexcept
	{
		// erase the single bit [idx, idx+1)
		return this->erase_index_unchecked(idx, idx + 1);
	}

	template <::fast_io::details::boolean_testable_for_range R>
	inline constexpr void append_range(R &&r)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any ||
									   !noexcept(static_cast<bool>(*::std::declval<::std::ranges::iterator_t<R> &>())) ||
									   !noexcept(++::std::declval<::std::ranges::iterator_t<R> &>()) ||
									   !noexcept(::std::declval<::std::ranges::iterator_t<R> &>() != ::std::declval<::std::ranges::sentinel_t<R>>()))
	{
		using U = underlying_type;
		// Rollback guard created AFTER reserve succeeds
		struct rollback_guard
		{
			bitvec *self;
			size_type old_size;
			~rollback_guard() noexcept
			{
				if (self)
				{
					self->imp.curr_pos = old_size;
				}
			}
		};
		// ------------------------------------------------------------
		// Sized range fast path
		// ------------------------------------------------------------
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type add = static_cast<size_type>(::std::ranges::size(r));
			size_type old_size = this->imp.curr_pos;

			// Reserve first
			this->reserve(old_size + add);

			rollback_guard guard{this, old_size};
			U *p = this->imp.begin_ptr;
			size_type bitpos = old_size;

			underlying_pointer bp = p;

			auto [byte_index, bit_offset] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bitpos);

			underlying_type acc = bp[byte_index] & static_cast<underlying_type>((1u << bit_offset) - 1u);
			size_type out_byte = byte_index;

			for (auto &&elem : r)
			{
				bool b = static_cast<bool>(elem);
				acc |= static_cast<underlying_type>(static_cast<underlying_type>(b) << bit_offset);
				++bit_offset;
				++bitpos;

				if (bit_offset == 8)
				{
					bp[out_byte] = acc;
					++out_byte;
					acc = 0;
					bit_offset = 0;
				}
			}
			if (bit_offset != 0)
			{
				p[out_byte] = acc;
			}

			this->imp.curr_pos = bitpos;
			guard.self = nullptr; // success
		}
		else
		{
			rollback_guard guard{this, this->imp.curr_pos};
			for (auto &&elem : r)
			{
				this->push_back(static_cast<bool>(elem));
			}
			guard.self = nullptr;
		}
	}

	inline constexpr size_type insert_index(size_type idx, bool value)
		FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_any)
	{
		if (this->imp.curr_pos < idx)
		{
			::fast_io::containers::details::contract_violation_report<throwing_violations>(::std::errc::invalid_argument);
		}
		if (this->imp.curr_pos == this->imp.end_pos)
		{
			this->grow_twice();
		}
		using U = underlying_type;
		U *p = this->imp.begin_ptr;

		size_type old_size = this->imp.curr_pos;
		size_type new_size = old_size + 1;

		// Split insertion index
		auto [idx_full, idx_rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(idx);

		// Split old size
		auto [old_full, old_rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(old_size);

		// ------------------------------------------------------------
		// Step 1: shift insertion word using addc
		// ------------------------------------------------------------
		U w = p[idx_full];

		U low_mask = (idx_rem == 0) ? U{0} : ((U{1} << idx_rem) - 1);
		U high_mask = ~low_mask;

		U low = w & low_mask;
		U high = w & high_mask;

		// Extract initial carry from LSB of high
		bool carry = (high & U{1}) != 0;

		// Shift high right by 1 with carry using addc
		high = ::fast_io::intrinsics::addc(high, high, carry, carry);

		// Combine low + shifted high
		p[idx_full] = low | high;

		// Insert the new bit
		if (value)
		{
			p[idx_full] |= (U{1} << idx_rem);
		}
		else
		{
			p[idx_full] &= ~(U{1} << idx_rem);
		}

		// ------------------------------------------------------------
		// Step 2: propagate carry through subsequent full words
		// ------------------------------------------------------------
		for (size_type widx = idx_full + 1; widx < old_full; ++widx)
		{
			U cur = p[widx];
			cur = ::fast_io::intrinsics::addc(cur, cur, carry, carry);
			p[widx] = cur;
		}

		// ------------------------------------------------------------
		// Step 3: handle the final partial word (if any)
		// ------------------------------------------------------------
		if (old_rem != 0)
		{
			U cur = p[old_full];
			cur = ::fast_io::intrinsics::addc(cur, cur, carry, carry);
			p[old_full] = cur;
		}
		else
		{
			// No partial word existed; we need to create one
			p[old_full] = carry;
		}

		this->imp.curr_pos = new_size;

		return idx;
	}

#if 0
	inline constexpr size_type insert_index(size_type idx, size_type count, bool value) noexcept
	{
		using U = underlying_type;

		// ------------------------------------------------------------
		// 1. Bounds check FIRST
		// ------------------------------------------------------------
		if (this->imp.curr_pos < idx)
		{
			::fast_io::fast_terminate();
		}

		// ------------------------------------------------------------
		// 2. Now check count == 0
		// ------------------------------------------------------------
		if (count == 0)
		{
			return idx;
		}

		// ------------------------------------------------------------
		// 3. Overflow‑checked addition using addc
		// ------------------------------------------------------------
		size_type old_size = this->imp.curr_pos;

		bool carry{};
		size_type new_size = ::fast_io::intrinsics::addc(old_size, count, false, carry);
		if (carry)
		{
			::fast_io::fast_terminate(); // overflow
		}

		// ------------------------------------------------------------
		// 4. Ensure capacity BEFORE shifting bits
		// ------------------------------------------------------------
		this->reserve(new_size);

		// refresh pointer after possible reallocation
		U *p = this->imp.begin_ptr;

		// ------------------------------------------------------------
		// 5. Split positions using bitvec_split_bits
		// ------------------------------------------------------------
		auto [idx_full, idx_rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(idx);

		auto [old_full, old_rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(old_size);

		auto [new_full, new_rem] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(new_size);

		size_type old_words = old_full + (old_rem != 0);
		size_type new_words = new_full + (new_rem != 0);

		// ------------------------------------------------------------
		// 6. Split count into whole‑word and bit shifts
		// ------------------------------------------------------------
		auto [word_shift, bit_shift] =
			::fast_io::details::bitvec_split_bits<underlying_digits>(count);

		// ------------------------------------------------------------
		// 7. Shift by whole words
		// ------------------------------------------------------------
		if (word_shift != 0)
		{
			for (size_type i = old_words; i-- > idx_full;)
			{
				p[i + word_shift] = p[i];
			}

			for (size_type i = 0; i < word_shift; ++i)
			{
				p[idx_full + i] = U{};
			}
		}

		// ------------------------------------------------------------
		// 8. Shift by remaining bits using shiftright
		// ------------------------------------------------------------
		if (bit_shift != 0)
		{
			for (size_type i = new_words; i-- > idx_full;)
			{
				U low = p[i];
				U high = (i == 0) ? U{} : p[i - 1];

				p[i] = ::fast_io::intrinsics::shiftright<U>(
					low, high, static_cast<unsigned>(bit_shift));
			}
		}

		// ------------------------------------------------------------
		// 9. Fill inserted region [idx, idx+count) with value
		// ------------------------------------------------------------
		if (value)
		{
			size_type first = idx;
			size_type last = idx + count;

			auto [f_full, f_rem] =
				::fast_io::details::bitvec_split_bits<underlying_digits>(first);
			auto [l_full, l_rem] =
				::fast_io::details::bitvec_split_bits<underlying_digits>(last);

			if (f_full == l_full)
			{
				// All inside one word
				U mask = ((U{1} << (l_rem - f_rem)) - 1) << f_rem;
				p[f_full] |= mask;
			}
			else
			{
				// First partial word
				if (f_rem != 0)
				{
					U mask = ~((U{1} << f_rem) - 1);
					p[f_full] |= mask;
					++f_full;
				}

				// Full words
				for (size_type w = f_full; w < l_full; ++w)
				{
					p[w] = ~U{};
				}

				// Last partial word
				if (l_rem != 0)
				{
					U mask = (U{1} << l_rem) - 1;
					p[l_full] |= mask;
				}
			}
		}

		// ------------------------------------------------------------
		// 10. Commit new size
		// ------------------------------------------------------------
		this->imp.curr_pos = new_size;
		return idx;
	}
#endif
public:
#if 0
	template <typename R>
	inline constexpr size_type insert_range_index(size_type idx, R &&r)
	{
		using U = underlying_type;

		// ------------------------------------------------------------
		// 1. Bounds check FIRST
		// ------------------------------------------------------------
		if (idx > this->imp.curr_pos)
		{
			::fast_io::fast_terminate();
		}

		// ------------------------------------------------------------
		// 2. Sized-range fast path
		// ------------------------------------------------------------
		if constexpr (std::ranges::sized_range<R>)
		{
			size_type count = std::ranges::size(r);
			if (count == 0)
			{
				return idx;
			}

			// Create gap of size count (shifts tail, preserves prefix)
			this->insert_index(idx, count, false);

			U *p = this->imp.begin_ptr;

			// Bit position where we start writing
			auto [start_full, start_rem] =
				::fast_io::details::bitvec_split_bits<underlying_digits>(idx);

			unsigned bit_offset = static_cast<unsigned>(start_rem);
			size_type out_word = start_full;

			// Preserve bits before idx in the first word
			U acc{};
			if (bit_offset != 0)
			{
				U low_mask = (U{1} << bit_offset) - 1;
				acc = p[out_word] & low_mask;
			}

			// Stream bits from r into underlying words
			for (auto &&elem : r)
			{
				bool b = static_cast<bool>(elem);
				acc |= static_cast<U>(b) << bit_offset;
				++bit_offset;

				if (bit_offset == underlying_digits)
				{
					p[out_word] = acc;
					++out_word;
					acc = 0;
					bit_offset = 0;
				}
			}

			if (bit_offset != 0)
			{
				// Preserve upper bits after the inserted region in the last word
				U upper_mask = ~((U{1} << bit_offset) - 1);
				U upper = p[out_word] & upper_mask;
				p[out_word] = upper | acc;
			}

			return count;
		}
		else
		{
			if (!::std::ranges::empty(r))
			{
				bitvec temp(::fast_io::freestanding::from_range, r);
				this->insert_range_index(idx, temp);
			}
			return idx;
		}
	}
#endif
};

template <typename allocator>
inline constexpr bool operator==(
	::fast_io::containers::bitvec<allocator> const &a,
	::fast_io::containers::bitvec<allocator> const &b) noexcept
{
	using underlying_type = ::fast_io::containers::bitvec<allocator>::underlying_type;
	using size_type = ::fast_io::containers::bitvec<allocator>::size_type;
	if (a.imp.curr_pos != b.imp.curr_pos)
	{
		return false;
	}

	size_type bits = a.imp.curr_pos;

	auto [full_bytes, rem] = ::fast_io::details::bitvec_split_bits<::fast_io::containers::bitvec<allocator>::underlying_digits>(bits);

	// --- Compare full bytes ---
	if consteval
	{
		// constexpr path: byte-by-byte
		for (size_type i{}; i != full_bytes; ++i)
		{
			if (a.imp.begin_ptr[i] != b.imp.begin_ptr[i])
			{
				return false;
			}
		}
	}
	else
	{
		// runtime path: memcmp
		if (::fast_io::freestanding::my_memcmp(a.imp.begin_ptr, b.imp.begin_ptr, full_bytes) != 0)
		{
			return false;
		}
	}

	// --- Compare partial byte ---
	if (rem != 0)
	{
		underlying_type mask =
			static_cast<underlying_type>((1u << rem) - 1u);

		underlying_type a_last = a.imp.begin_ptr[full_bytes] & mask;
		underlying_type b_last = b.imp.begin_ptr[full_bytes] & mask;

		return a_last == b_last;
	}

	return true;
}

template <typename allocator>
inline constexpr ::std::strong_ordering operator<=>(
	::fast_io::containers::bitvec<allocator> const &a,
	::fast_io::containers::bitvec<allocator> const &b) noexcept
{
	using underlying_type = ::fast_io::containers::bitvec<allocator>::underlying_type;
	using size_type = ::fast_io::containers::bitvec<allocator>::size_type;
	size_type bits_a = a.imp.curr_pos;
	size_type bits_b = b.imp.curr_pos;

	size_type min_bits = bits_a < bits_b ? bits_a : bits_b;

	auto [full_bytes, rem] = ::fast_io::details::bitvec_split_bits<::fast_io::containers::bitvec<allocator>::underlying_digits>(min_bits);

	// --- Compare full bytes ---
	if (__builtin_is_constant_evaluated())
	{
		// constexpr path: byte-by-byte
		for (size_type i{}; i != full_bytes; ++i)
		{
			unsigned char ai = a.imp.begin_ptr[i];
			unsigned char bi = b.imp.begin_ptr[i];
			if (ai < bi)
			{
				return std::strong_ordering::less;
			}
			if (bi < ai)
			{
				return std::strong_ordering::greater;
			}
		}
	}
	else
	{
		// runtime path: memcmp
		int cmp = ::fast_io::freestanding::my_memcmp(a.imp.begin_ptr, b.imp.begin_ptr, full_bytes);
		if (cmp < 0)
		{
			return ::std::strong_ordering::less;
		}
		if (0 < cmp)
		{
			return ::std::strong_ordering::greater;
		}
	}

	// --- Compare partial byte ---
	if (rem != 0)
	{
		underlying_type mask =
			static_cast<underlying_type>((1u << rem) - 1u);

		underlying_type a_last = a.imp.begin_ptr[full_bytes] & mask;
		underlying_type b_last = b.imp.begin_ptr[full_bytes] & mask;

		if (a_last < b_last)
		{
			return ::std::strong_ordering::less;
		}
		if (b_last < a_last)
		{
			return ::std::strong_ordering::greater;
		}
	}

	// --- Compare lengths ---
	return bits_a <=> bits_b;
}

template <typename allocator>
constexpr void swap(::fast_io::containers::bitvec<allocator> &lhs, ::fast_io::containers::bitvec<allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> operator&(::fast_io::containers::bitvec<allocator> lhs, ::fast_io::containers::bitvec<allocator> const &rhs) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	lhs &= rhs;
	return lhs;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> operator|(::fast_io::containers::bitvec<allocator> lhs, ::fast_io::containers::bitvec<allocator> const &rhs) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	lhs |= rhs;
	return lhs;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> operator^(::fast_io::containers::bitvec<allocator> lhs, ::fast_io::containers::bitvec<allocator> const &rhs) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	lhs ^= rhs;
	return lhs;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> operator<<(::fast_io::containers::bitvec<allocator> lhs, typename ::fast_io::containers::bitvec<allocator>::size_type shift) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	lhs <<= shift;
	return lhs;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> operator>>(::fast_io::containers::bitvec<allocator> lhs, typename ::fast_io::containers::bitvec<allocator>::size_type shift) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	lhs >>= shift;
	return lhs;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> bitvec_rotl(::fast_io::containers::bitvec<allocator> v, typename ::fast_io::containers::bitvec<allocator>::difference_type shift) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	v.rotl_assign(shift);
	return v;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> bitvec_rotr(::fast_io::containers::bitvec<allocator> v, typename ::fast_io::containers::bitvec<allocator>::difference_type shift) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	v.rotr_assign(shift);
	return v;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> bitvec_bit_floor(::fast_io::containers::bitvec<allocator> v) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	v.bit_floor_assign();
	return v;
}

template <typename allocator>
constexpr ::fast_io::containers::bitvec<allocator> bitvec_bit_ceil(::fast_io::containers::bitvec<allocator> v) FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::containers::details::allocator_throws_any<allocator>)
{
	v.bit_ceil_assign();
	return v;
}

} // namespace containers

namespace details
{
template <::std::integral chtype, typename underlying>
inline constexpr chtype *pr_rsv_bin_full(chtype *outit, underlying const *first, underlying const *last) noexcept
{
#if 1
	constexpr bool endian_available{
		::std::endian::little == ::std::endian::native
#if 0
//i am not sure whether big endian would work, fallback to default
	|| ::std::endian::big == ::std::endian::native
#endif
	};
	if constexpr (endian_available && ::std::numeric_limits<char unsigned>::digits == 8 &&
				  ::std::numeric_limits<::std::uint_least32_t>::digits == 32 &&
				  ::std::numeric_limits<::std::uint_least64_t>::digits == 64 &&
				  (sizeof(chtype) == 1) &&
				  ::std::numeric_limits<underlying>::digits == 8 &&
				  64 <= ::std::numeric_limits<::std::size_t>::digits)
	{
		constexpr bool ebcdic{::fast_io::details::is_ebcdic<chtype>};
		for (; first != last; ++first)
		{
			::std::uint_least8_t b{*first};
			::std::uint_least64_t x{b};

			x = (((x & 0b01010101) * 0x02040810204081LL) | ((x & 0b10101010) * 0x02040810204081LL)) & 0x0101010101010101LL;
			// Step 3: convert 0/1 → '0'/'1'
			if constexpr (ebcdic)
			{
				x += 0xF0F0F0F0F0F0F0F0ULL;
			}
			else
			{
				x += 0x3030303030303030ULL;
			}

			// Write 8 bytes
			::fast_io::freestanding::my_memcpy(outit, __builtin_addressof(x), 8);
			outit += 8;
		}
		return outit;
	}
	else
#endif
	{
		constexpr ::std::uint_fast8_t digits{::std::numeric_limits<underlying>::digits};
		using unsignedchartype = ::std::make_unsigned_t<chtype>;
		for (; first != last; ++first)
		{
			auto e{*first};
			for (::std::uint_fast8_t i{}; i != digits; ++i)
			{
				*outit = static_cast<unsignedchartype>(static_cast<unsignedchartype>(e & 1u) +
													   static_cast<unsignedchartype>(::fast_io::char_literal_v<u8'0', chtype>));
				e >>= 1u;
				++outit;
			}
		}
		return outit;
	}
}

template <::std::integral chtype, typename allocator>
inline constexpr chtype *pr_rsv_bitvec(chtype *outit, ::fast_io::containers::bitvec<allocator> const &bv) noexcept
{
	constexpr auto underlying_digits{::fast_io::containers::bitvec<allocator>::underlying_digits};
	auto [full_units, rem_bits] = ::fast_io::details::bitvec_split_bits<underlying_digits>(bv.imp.curr_pos);
	auto begin_ptr{bv.imp.begin_ptr};
	auto end_ptr{begin_ptr + full_units};
	outit = ::fast_io::details::pr_rsv_bin_full(outit, begin_ptr, end_ptr);
	if (rem_bits)
	{
		auto e{*end_ptr};
		using unsignedchartype = ::std::make_unsigned_t<chtype>;
		for (::std::size_t i{}; i != rem_bits; ++i)
		{
			*outit = static_cast<unsignedchartype>(static_cast<unsignedchartype>(e & 1u) +
												   static_cast<unsignedchartype>(::fast_io::char_literal_v<u8'0', chtype>));
			e >>= 1u;
			++outit;
		}
	}
	return outit;
}
} // namespace details

namespace containers
{
template <::std::integral chtype, typename allocator>
inline constexpr ::std::size_t print_reserve_size(::fast_io::io_reserve_type_t<chtype, ::fast_io::containers::bitvec<allocator>>, ::fast_io::containers::bitvec<allocator> const &bv) noexcept
{
	return bv.size();
}

template <::std::integral chtype, typename allocator>
inline constexpr chtype *print_reserve_define(::fast_io::io_reserve_type_t<chtype, ::fast_io::containers::bitvec<allocator>>, chtype *it, ::fast_io::containers::bitvec<allocator> const &bv) noexcept
{
	return ::fast_io::details::pr_rsv_bitvec(it, bv);
}
} // namespace containers

} // namespace fast_io
