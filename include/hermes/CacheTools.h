#ifndef HERMES_CACHETOOLS_H
#define HERMES_CACHETOOLS_H

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "hermes/Common.h"
#include "hermes/Units.h"

namespace hermes {

typedef std::pair<double, double> tPairKey;
typedef std::array<double, 3> tArrayKey;
typedef std::array<int, 2> tArray2Key;
typedef std::tuple<double, double, double> tTupleKey;

template <class T>
inline void hash_combine(std::size_t &seed, const T &v) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash {
	std::size_t operator()(const tPairKey &p) const {
		auto h1 = std::hash<double>{}(p.first);
		auto h2 = std::hash<double>{}(p.second);

		hash_combine(h1, h2);
		return h1;
	}
};

struct pair_equal {
	bool operator()(const tPairKey &v0, const tPairKey &v1) const {
		return v0 == v1;
	}
};

struct array_hash {
	std::size_t operator()(const tArrayKey &k) const {
		auto h1 = std::hash<double>{}(std::get<0>(k));
		auto h2 = std::hash<double>{}(std::get<1>(k));
		auto h3 = std::hash<double>{}(std::get<2>(k));

		hash_combine(h1, h2);
		hash_combine(h1, h3);
		return h1;
	}
};

struct array_equal {
	bool operator()(const tArrayKey &v0, const tArrayKey &v1) const {
		return v0 == v1;
	}
};

struct array2_hash {
	std::size_t operator()(const tArray2Key &k) const {
		auto h1 = std::hash<int>{}(std::get<0>(k));
		auto h2 = std::hash<int>{}(std::get<1>(k));

		hash_combine(h1, h2);
		return h1;
	}
};

struct array2_equal {
	bool operator()(const tArray2Key &v0, const tArray2Key &v1) const {
		return (std::get<0>(v0) == std::get<0>(v1) && std::get<1>(v0) == std::get<1>(v1));
	}
};

struct tuple_hash {
	std::size_t operator()(const tTupleKey &k) const {
		auto h1 = std::hash<double>{}(std::get<0>(k));
		auto h2 = std::hash<double>{}(std::get<1>(k));
		auto h3 = std::hash<double>{}(std::get<2>(k));

		hash_combine(h1, h2);
		hash_combine(h1, h3);
		return h1;
	}
};

struct tuple_equal {
	bool operator()(const tTupleKey &v0, const tTupleKey &v1) const {
		return v0 == v1;
	}
};

template <typename Q1, typename Q2, typename V>
class CacheStorageWith2Args {
  private:
	using MutexType = std::mutex;
	typedef std::pair<double, double> tPairKey;
	std::unordered_map<tPairKey, V, pair_hash, pair_equal> cachedValues;
	// std::array<std::map<tPairKey, V>, 8> cachedValues;
	std::function<V(Q1, Q2)> f;
	std::size_t generation = 0;
	mutable MutexType mtx;

  public:
	CacheStorageWith2Args() = default;
	~CacheStorageWith2Args() = default;
	CacheStorageWith2Args(CacheStorageWith2Args &&other) {
		std::lock_guard<MutexType> guard(other.mtx);
		cachedValues = std::move(other.cachedValues);
		f = std::move(other.f);
		generation = other.generation;
	}
	CacheStorageWith2Args &operator=(CacheStorageWith2Args &&other) = delete;       // Move assignment
	CacheStorageWith2Args(const CacheStorageWith2Args &other) = delete;             // Copy declaration
	CacheStorageWith2Args &operator=(const CacheStorageWith2Args &other) = delete;  // Copy Assignment

	void setFunction(std::function<V(Q1, Q2)> f_) {
		std::lock_guard<MutexType> guard(mtx);
		f = std::move(f_);
		cachedValues.clear();
		++generation;
	}

	void cacheValue(const tPairKey &key, V value) {
		std::lock_guard<MutexType> guard(mtx);
		cachedValues.insert_or_assign(key, std::move(value));
	}

	V getValue(Q1 q1, Q2 q2) {
		const auto key = std::make_pair(static_cast<double>(q1), static_cast<double>(q2));
		std::function<V(Q1, Q2)> function;
		std::size_t functionGeneration;
		{
			std::lock_guard<MutexType> guard(mtx);
			const auto it = cachedValues.find(key);
			if (it != cachedValues.end()) return it->second;
			function = f;
			functionGeneration = generation;
		}

		V result = function(q1, q2);
		std::lock_guard<MutexType> guard(mtx);
		if (functionGeneration != generation) return result;
		return cachedValues.emplace(key, std::move(result)).first->second;
	}

	V operator[](const std::pair<double, double> &key) const {
		std::lock_guard<MutexType> guard(mtx);
		return cachedValues.at(key);
	}
};

template <typename Q1, typename Q2, typename Q3, typename V>
class CacheStorageWith3Args {
  private:
	using MutexType = std::mutex;
	// typedef std::tuple<double, double, double> tTupleKey;
	typedef std::array<double, 3> tTupleKey;
	// map is much slower than unordered_map (!)
	// std::array<std::unordered_map<tTupleKey, V, tuple_hash, tuple_equal>,
	// 8> cachedValues;
	std::unordered_map<tTupleKey, V, array_hash, array_equal> cachedValues;
	std::function<V(Q1, Q2, Q3)> f;
	std::size_t generation = 0;
	mutable MutexType mtx;

  public:
	CacheStorageWith3Args() = default;
	~CacheStorageWith3Args() = default;
	CacheStorageWith3Args(CacheStorageWith3Args &&other) {
		std::lock_guard<MutexType> guard(other.mtx);
		cachedValues = std::move(other.cachedValues);
		f = std::move(other.f);
		generation = other.generation;
	}
	CacheStorageWith3Args &operator=(CacheStorageWith3Args &&other) = delete;       // Move assignment
	CacheStorageWith3Args(const CacheStorageWith3Args &other) = delete;             // Copy declaration
	CacheStorageWith3Args &operator=(const CacheStorageWith3Args &other) = delete;  // Copy Assignment

	void setFunction(std::function<V(Q1, Q2, Q3)> f_) {
		std::lock_guard<MutexType> guard(mtx);
		f = std::move(f_);
		cachedValues.clear();
		++generation;
	}

	void cacheValue(const tTupleKey &key, V value) {
		std::lock_guard<MutexType> guard(mtx);
		cachedValues.insert_or_assign(key, std::move(value));
	}

	V getValue(Q1 q1, Q2 q2, Q3 q3) {
		const tTupleKey key = {{static_cast<double>(q1), static_cast<double>(q2), static_cast<double>(q3)}};
		std::function<V(Q1, Q2, Q3)> function;
		std::size_t functionGeneration;
		{
			std::lock_guard<MutexType> guard(mtx);
			const auto it = cachedValues.find(key);
			if (it != cachedValues.end()) return it->second;
			function = f;
			functionGeneration = generation;
		}

		V result = function(q1, q2, q3);
		std::lock_guard<MutexType> guard(mtx);
		if (functionGeneration != generation) return result;
		return cachedValues.emplace(key, std::move(result)).first->second;
	}
};

class CacheStorageIC2 {
  private:
	typedef std::array<double, 3> tArrayKey;
	std::unordered_map<tArrayKey, QGREmissivity, array_hash, array_equal> cachedValues;
	std::function<QGREmissivity(int, int, QEnergy)> f;
	std::size_t generation = 0;
	mutable std::mutex mtx;

  public:
	void setFunction(std::function<QGREmissivity(int, int, QEnergy)> f_) {
		std::lock_guard<std::mutex> guard(mtx);
		f = std::move(f_);
		cachedValues.clear();
		++generation;
	}

	void cacheValue(const tArrayKey &key, QGREmissivity value) {
		std::lock_guard<std::mutex> guard(mtx);
		cachedValues.insert_or_assign(key, value);
	}

	QGREmissivity getValue(int q1, int q2, QEnergy q3) {
		const tArrayKey key = {{static_cast<double>(q1), static_cast<double>(q2),
		                        static_cast<double>(q3)}};
		std::function<QGREmissivity(int, int, QEnergy)> function;
		std::size_t functionGeneration;
		{
			std::lock_guard<std::mutex> guard(mtx);
			const auto it = cachedValues.find(key);
			if (it != cachedValues.end()) return it->second;
			function = f;
			functionGeneration = generation;
		}

		const QGREmissivity result = function(q1, q2, q3);
		std::lock_guard<std::mutex> guard(mtx);
		if (functionGeneration != generation) return result;
		return cachedValues.emplace(key, result).first->second;
	}
};

typedef CacheStorageWith3Args<int, int, QEnergy, QGREmissivity> CacheStorageIC;
typedef CacheStorageWith2Args<QEnergy, QEnergy, QDiffCrossSection> CacheStorageCrossSection;
typedef CacheStorageWith3Args<QEnergy, QEnergy, QEnergy, QDiffCrossSection> CacheStorageCrossSection3Args;

}  // namespace hermes

#endif  // HERMES_CACHETOOLS_H
