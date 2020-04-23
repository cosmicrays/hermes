#ifndef HERMES_CACHETOOLS_H
#define HERMES_CACHETOOLS_H

#include "hermes/Common.h"
#include "hermes/Units.h"

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace hermes {

typedef std::pair<double, double> tPairKey;
typedef std::array<double, 3> tArrayKey;
typedef std::array<int, 2> tArray2Key;
typedef std::tuple<double, double, double> tTupleKey;

template <class T> inline void hash_combine(std::size_t &seed, const T &v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash : public std::unary_function<std::size_t, std::size_t> {
    std::size_t operator()(const tPairKey &p) const {
	auto h1 = std::hash<double>{}(p.first);
	auto h2 = std::hash<double>{}(p.second);

	hash_combine(h1, h2);
	return h1;
    }
};

struct pair_equal : public std::binary_function<tPairKey, tPairKey, bool> {
    inline bool areEqual(double a, double b) const {
	return (std::fabs(a - b) < std::numeric_limits<double>::epsilon());
    }
    bool operator()(const tPairKey &v0, const tPairKey &v1) const {
	return (areEqual(v0.first, v1.first) && areEqual(v0.second, v1.second));
    }
};

struct array_hash : public std::unary_function<tArrayKey, std::size_t> {
    std::size_t operator()(const tArrayKey &k) const {
	auto h1 = std::hash<double>{}(std::get<0>(k));
	auto h2 = std::hash<double>{}(std::get<1>(k));
	auto h3 = std::hash<double>{}(std::get<2>(k));

	hash_combine(h1, h2);
	hash_combine(h1, h3);
	return h1;
    }
};

struct array_equal : public std::binary_function<tArrayKey, tArrayKey, bool> {
    bool areEqual(double a, double b) const {
	return (std::fabs(a - b) < std::numeric_limits<double>::epsilon());
    }
    bool operator()(const tArrayKey &v0, const tArrayKey &v1) const {
	return (areEqual(std::get<0>(v0), std::get<0>(v1)) &&
		areEqual(std::get<1>(v0), std::get<1>(v1)) &&
		areEqual(std::get<2>(v0), std::get<2>(v1)));
    }
};

struct array2_hash : public std::unary_function<tArray2Key, std::size_t> {
    std::size_t operator()(const tArray2Key &k) const {
	auto h1 = std::hash<int>{}(std::get<0>(k));
	auto h2 = std::hash<int>{}(std::get<1>(k));

	hash_combine(h1, h2);
	return h1;
    }
};

struct array2_equal
    : public std::binary_function<tArray2Key, tArray2Key, bool> {
    bool operator()(const tArray2Key &v0, const tArray2Key &v1) const {
	return (std::get<0>(v0) == std::get<0>(v1) &&
		std::get<1>(v0) == std::get<1>(v1));
    }
};

struct tuple_hash : public std::unary_function<tTupleKey, std::size_t> {
    std::size_t operator()(const tTupleKey &k) const {
	auto h1 = std::hash<double>{}(std::get<0>(k));
	auto h2 = std::hash<double>{}(std::get<1>(k));
	auto h3 = std::hash<double>{}(std::get<2>(k));

	hash_combine(h1, h2);
	hash_combine(h1, h3);
	return h1;
    }
};

struct tuple_equal : public std::binary_function<tTupleKey, tTupleKey, bool> {
    bool areEqual(double a, double b) const {
	return (std::fabs(a - b) < std::numeric_limits<double>::epsilon());
    }
    bool operator()(const tTupleKey &v0, const tTupleKey &v1) const {
	return (areEqual(std::get<0>(v0), std::get<0>(v1)) &&
		areEqual(std::get<1>(v0), std::get<1>(v1)) &&
		areEqual(std::get<2>(v0), std::get<2>(v1)));
    }
};

template <typename Q1, typename Q2, typename V> class CacheStorageWith2Args {
  private:
    using MutexType = std::mutex;
    typedef std::pair<double, double> tPairKey;
    std::unordered_map<tPairKey, V, pair_hash, pair_equal> cachedValues;
    // std::array<std::map<tPairKey, V>, 8> cachedValues;
    std::function<V(Q1, Q2)> f;
    mutable MutexType mtx;

  public:
    CacheStorageWith2Args(){};
    ~CacheStorageWith2Args() { cachedValues.clear(); };
    CacheStorageWith2Args(CacheStorageWith2Args &&other) { // Move declaration
	std::unique_lock<MutexType>(other.mtx);
    }
    CacheStorageWith2Args &
    operator=(CacheStorageWith2Args &&other) = delete; // Move assignment
    CacheStorageWith2Args(const CacheStorageWith2Args &other) =
	delete; // Copy declaration
    CacheStorageWith2Args &
    operator=(const CacheStorageWith2Args &other) = delete; // Copy Assignment

    void setFunction(std::function<V(Q1, Q2)> f_) { f = f_; }

    void cacheValue(const tPairKey &key, V value) { cachedValues[key] = value; }

    V getValue(Q1 q1, Q2 q2) {
	V result(0);
	auto key =
	    std::make_pair(static_cast<double>(q1), static_cast<double>(q2));
	auto it = cachedValues.find(key);
	if (it == cachedValues.end()) {
	    result = f(q1, q2);
	    std::lock_guard<std::mutex> guard(mtx);
	    cacheValue(key, result);
	}
	return cachedValues[key];
    }

    V operator[](const std::pair<double, double> &key) const {
	return cachedValues[key];
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
    mutable MutexType mtx;

  public:
    CacheStorageWith3Args(){};
    ~CacheStorageWith3Args() { cachedValues.clear(); };
    CacheStorageWith3Args(CacheStorageWith3Args &&other) { // Move declaration
	std::unique_lock<MutexType>(other.mtx);
    }
    CacheStorageWith3Args &
    operator=(CacheStorageWith3Args &&other) = delete; // Move assignment
    CacheStorageWith3Args(const CacheStorageWith3Args &other) =
	delete; // Copy declaration
    CacheStorageWith3Args &
    operator=(const CacheStorageWith3Args &other) = delete; // Copy Assignment

    void setFunction(std::function<V(Q1, Q2, Q3)> f_) { f = f_; }

    void cacheValue(const tTupleKey &key, V value) {
	cachedValues[key] = value;
    }

    V getValue(Q1 q1, Q2 q2, Q3 q3) {
	V result(0);
	/*auto key = std::make_tuple(
			static_cast<double>(q1),
			static_cast<double>(q2),
			static_cast<double>(q3));*/
	tTupleKey key = {{static_cast<double>(q1), static_cast<double>(q2),
			  static_cast<double>(q3)}};
	auto it = cachedValues.find(key);
	if (it == cachedValues.end()) {
	    result = f(q1, q2, q3);
	    cacheValue(key, result);
	}
	return cachedValues[key];
    }
};

class CacheStorageIC2 {
  private:
    typedef std::array<int, 2> tArray2Key;
    std::unordered_map<tArray2Key, QGREmissivity, array2_hash, array2_equal>
	cachedValues;
    std::function<QGREmissivity(int, int, QEnergy)> f;

  public:
    void setFunction(std::function<QGREmissivity(int, int, QEnergy)> f_) {
	f = f_;
    }

    void cacheValue(const tArray2Key &key, QGREmissivity value) {
	cachedValues[key] = value;
    }

    QGREmissivity getValue(int q1, int q2, QEnergy q3) {
	QGREmissivity result(0);
	tArray2Key key = {{q1, q2}};
	auto it = cachedValues.find(key);
	if (it == cachedValues.end()) {
	    result = f(q1, q2, q3);
	    cacheValue(key, result);
	}
	return cachedValues[key];
    }
};

typedef CacheStorageWith3Args<int, int, QEnergy, QGREmissivity> CacheStorageIC;
typedef CacheStorageWith2Args<QEnergy, QEnergy, QDiffCrossSection>
    CacheStorageCrossSection;
typedef CacheStorageWith3Args<QEnergy, QEnergy, QEnergy, QDiffCrossSection>
    CacheStorageCrossSection3Args;

} // namespace hermes

#endif // HERMES_CACHETOOLS_H
