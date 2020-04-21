#ifndef HERMES_UNITSSIPREFIXES_H
#define HERMES_UNITSSIPREFIXES_H

namespace hermes { namespace units {

// SI prefixes
constexpr double yocto = 1e-24;
constexpr double zepto = 1e-21;
constexpr double atto  = 1e-18;
constexpr double femto = 1e-15;
constexpr double pico  = 1e-12;
constexpr double nano  = 1e-9;
constexpr double micro = 1e-6;
constexpr double milli  = 1e-3;
constexpr double centi = 1e-2;
constexpr double deci  = 1e-1;
constexpr double deca  = 1e1;
constexpr double hecto = 1e2;
constexpr double kilo  = 1e3;
constexpr double mega  = 1e6;
constexpr double giga  = 1e9;
constexpr double tera  = 1e12;
constexpr double peta  = 1e15;
constexpr double exa   = 1e18;
constexpr double zetta = 1e21;
constexpr double yotta = 1e24;  

#define GENERATE_PREFIXED_UNITS(_UNIT) \
	constexpr auto yocto##_UNIT = yocto * _UNIT; \
	constexpr auto zepto##_UNIT = zepto * _UNIT; \
	constexpr auto atto##_UNIT  = atto  * _UNIT; \
	constexpr auto femto##_UNIT = femto * _UNIT; \
	constexpr auto pico##_UNIT  = pico  * _UNIT; \
	constexpr auto nano##_UNIT  = nano  * _UNIT; \
	constexpr auto micro##_UNIT = micro * _UNIT; \
	constexpr auto milli##_UNIT = milli * _UNIT; \
	constexpr auto centi##_UNIT = centi * _UNIT; \
	constexpr auto deci##_UNIT  = deci  * _UNIT; \
	constexpr auto hecto##_UNIT = hecto * _UNIT; \
	constexpr auto kilo##_UNIT  = kilo  * _UNIT; \
	constexpr auto mega##_UNIT  = mega  * _UNIT; \
	constexpr auto giga##_UNIT  = giga  * _UNIT; \
	constexpr auto tera##_UNIT  = tera  * _UNIT; \
	constexpr auto peta##_UNIT  = peta  * _UNIT; \
	constexpr auto exa##_UNIT   = exa   * _UNIT; \
	constexpr auto zetta##_UNIT = zetta * _UNIT; \
	constexpr auto yotta##_UNIT = yotta * _UNIT;

// SI prefixes - short
#define GENERATE_SHORT_PREFIXED_UNITS(_UNIT, _SHORTUNIT) \
	constexpr auto y##_SHORTUNIT  = yocto * _UNIT; \
	constexpr auto z##_SHORTUNIT  = zepto * _UNIT; \
	constexpr auto a##_SHORTUNIT  = atto  * _UNIT; \
	constexpr auto f##_SHORTUNIT  = femto * _UNIT; \
	constexpr auto p##_SHORTUNIT  = pico  * _UNIT; \
	constexpr auto n##_SHORTUNIT  = nano  * _UNIT; \
	constexpr auto mu##_SHORTUNIT = micro * _UNIT; \
	constexpr auto m##_SHORTUNIT  = milli * _UNIT; \
	constexpr auto c##_SHORTUNIT  = centi * _UNIT; \
	constexpr auto d##_SHORTUNIT  = deci  * _UNIT; \
	constexpr auto h##_SHORTUNIT  = hecto * _UNIT; \
	constexpr auto k##_SHORTUNIT  = kilo  * _UNIT; \
	constexpr auto M##_SHORTUNIT  = mega  * _UNIT; \
	constexpr auto G##_SHORTUNIT  = giga  * _UNIT; \
	constexpr auto T##_SHORTUNIT  = tera  * _UNIT; \
	constexpr auto P##_SHORTUNIT  = peta  * _UNIT; \
	constexpr auto E##_SHORTUNIT  = exa   * _UNIT; \
	constexpr auto Z##_SHORTUNIT  = zetta * _UNIT; \
	constexpr auto Y##_SHORTUNIT  = yotta * _UNIT;

} // namespace units
} // namespace hermes

#endif // HERMES_UNITSSIPREFIXES_H
