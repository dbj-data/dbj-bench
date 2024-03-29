/****************************************************************************************/
 // demo and benchmarking
 //https://godbolt.org/z/PsKT19
// this is one very crowded file

#include "common/ht_testing_common.h"
#include "../../../ubut/ubench.h"

// #include "slot_array_map_ustrings.h"
#include "common/dbj_ustrings.h"
#include "../../../uthash/src/uthash.h"

#include <string_view>

/****************************************************************************************/
// (c) 2021 by dbj@dbj.org
struct dbj_uss final {
	using handle = size_t;
	using store_t = dbj::ustring_pool_using_uniq_ptr;
	store_t store_{};

	handle add(const char* s) noexcept {
		return store_t::assign(store_, s);
	}

	//if handle is not valid null pointer is returned
	const char* cstring(handle h) const noexcept
	{
		for (auto&& hspair_ : store_.strings)
		{
			if (h == hspair_.first)
			{
				return hspair_.second.get();
			}
		}
		return nullptr;
	}

	bool remove(handle h) noexcept {
		return store_t::remove(store_, h);
	}
	size_t count() const noexcept {
		return store_.strings.size();
	}
}; // dbj_uss

/****************************************************************************************/
// #include "../../loki_assoc_vector.h"

//struct loki_assoc_vector_pool final
//{
//	using pool_type = Loki::AssocVector<size_t, std::string>;
//	using handle = pool_type::key_type;
//
//	pool_type pool_{};
//
//	handle add(const char* s)
//	{
//		static std::hash<std::string_view> hash_fun;
//		//
//		size_t hash = hash_fun(s);
//		auto it = pool_.find(hash);
//		// already in here
//		if (it != pool_.end())  return hash;
//		// new unique entry
//		pool_[hash] = pool_type::mapped_type(s);
//		return hash;
//	}
//	const char* cstring(handle h) {
//		auto it = pool_.find(h);
//		if (it == pool_.end())  return nullptr;
//		return (*it).second.c_str();
//	}
//
//	bool remove(handle h) noexcept {
//		return pool_.erase(h); // returns 0 or 1
//	}
//
//	size_t count() const {
//		return pool_.size();
//	}
//};
/****************************************************************************************/
// evergrowing ustring pool based on 
// vector of uniq_ptr<char[]>
// handle is the index of the vector
// (c) 2021 by dbj.org
//struct dbj_extreme final {
//	using handle = size_t;
//	using store_t = std::vector<dbj::string_ptr>;
//	store_t store_{};
//
//	handle add(const char* str_) noexcept {
//
//		handle idx = 0;
//		for (auto&& el_ : store_)
//		{
//			if (!strcmp(el_.get(), str_))
//				return idx;
//			idx++;
//		}
//
//		// else element not found
//		store_.push_back(dbj::string_ptr_make(str_));
//		return store_.size() - 1;
//	}
//
//	//handle is the vector index
//	// there is no removing thus indexes are unchanged
//	const char* cstring(handle h_) const noexcept
//	{
//		return store_[h_].get();
//	}
//
//	size_t count() const noexcept {
//		return store_.size();
//	}
//}; // dbj_extreme

/****************************************************************************************/
/*  (c) 2021 by dbj@dbj.org -- unique string with unordered_set */
/*  https://godbolt.org/z/1xTK4c */

#include <unordered_set>

struct dbj_uset_pool final
{
	using handle = size_t;

	struct S final {
		size_t hash{};
		std::string name{};
	};

	friend bool operator==(const S& lhs, const S& rhs) noexcept {
		return lhs.hash == rhs.hash;
	}

	// custom hash is the mechanism by which we enter
	// hash and string automagically
	struct MyHash final
	{
		std::size_t operator()(S const& s) const noexcept
		{
			return const_cast<S&>(s).hash = std::hash<std::string>{}(s.name);
			// return s.hash ;
		}
	};

	using set_of_unique_names = std::unordered_set<S, MyHash>;
	set_of_unique_names names{};

	static bool find
	(set_of_unique_names const& names, std::string& retval, size_t hash_)
		noexcept {
		for (auto&& s : names)
			if (s.hash == hash_) {
				retval = s.name;
				return true;
			}
		return false;
	}

	handle add(const char* s) noexcept {
		auto R = names.emplace(S{ 0, std::string(s) });
		return R.first->hash;
	}

	const char* cstring(handle h) const noexcept
	{
		static std::string rv_{};
		if (find(names, rv_, h)) return rv_.c_str();
		return nullptr;
	}

	bool remove(handle h)
	{
		for (auto it = names.begin(); it != names.end(); ++it)
			if (it->hash == h) {
				(void)names.erase(it);
				return true;
			}

		return false;
	}

	size_t count() const {
		return names.size();
	}

};

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer

// not-a-handle type for 
// Arthur's solutions
// it is std::string_view
//struct strings_set_pool final {
//	using handle = std::string_view;
//	std::set<std::string, std::less<> > set_;
//	handle add(const char* s) {
//		// just ignore the inserted
//		auto [it, inserted] = set_.emplace(s);
//		return *it;
//	}
//	const char* cstring(handle h) {
//		// handle should be always valid
//		// as there is no removal
//		return h.data();
//	}
//	size_t count() const {
//		return set_.size();
//	}
//};

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer
// (c) 2021 by dbj@dbj.org -- made it work with handle number
struct uthash_pool final {

	constexpr static auto  MAX_STR_LEN = 0xFFFF;
	using handle = int;

	struct Item final {
		int id; /* key */
		std::string data;
		UT_hash_handle hh;
	};

	Item* set_ = nullptr;

	static handle str_to_handle(const char* const str_) noexcept {
		assert(str_);
		int hash_value_ = 0;
		const int str_len_ = strnlen(str_, MAX_STR_LEN);
		HASH_VALUE(str_, str_len_, hash_value_);
		return hash_value_;
	}

	handle add(const char* str_) noexcept
	{
		struct Item* item = nullptr;

		handle handle_ = str_to_handle(str_);

		HASH_FIND_INT(set_, &handle_, item);
		if (item == nullptr) {
			item = new Item{ handle_, str_, {} };
			HASH_ADD_INT(set_, id, item); /* id: name of key field */
		}
		return item->id;
	}

	const char* cstring(handle handle_) {
		struct Item* item = nullptr;
		HASH_FIND_INT(set_, &handle_, item);
		if (item)
			return item->data.c_str();
		return nullptr;
	}

	bool remove(handle handle_) noexcept {
		Item* item = nullptr;
		HASH_FIND_INT(set_, &handle_, item);
		if (item) {
			HASH_DEL(set_, item);
			delete item;
			return true;
		}
		return false;
	}
	int count() const {
		return HASH_COUNT(set_);
	}
	~uthash_pool() {
		// we hold std::strings so this is OK
		HASH_CLEAR(hh, set_);
	}
};
/****************************************************************************************/

UBENCH(strpool, dbj_unique_strings) {
	test_removal<dbj_uss>();
}

//UBENCH(strpool, loki_assoc_vector_pool) {
//	test_removal<loki_assoc_vector_pool>();
//}

UBENCH(strpool, dbj_unordered_set_pool) {
	test_removal<dbj_uset_pool>();
}

UBENCH(strpool, uthash_pool_CPP) {
	test_removal<uthash_pool>();
}



