/*
Entfernen Sie folgende Informationen auf keinen Fall: / Do not remove following text:
Source code based on the javascript by Arnold Barmettler, www.astronomie.info / www.CalSky.com
based on algorithms by Peter Duffett-Smith's great and easy book
'Practical Astronomy with your Calculator'.
*/
#include "SunMoon.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <limits>
#include <string>

namespace Astronomy
{

#define DEG (M_PI / 180.0)
#define RAD (180.0 / M_PI)
#define NAN_DOUBLE std::numeric_limits<double>::quiet_NaN()
#define NAN_INT std::numeric_limits<int>::quiet_NaN()
#ifndef isnan
	inline bool isnan(double x) {
		return x != x;
	}
	inline bool isnan(int x) {
		return x != x;
	}
#endif
	int Int(double x) { return (x < 0) ? (int)std::ceil(x) : (int)std::floor(x); }

	TimeSpan::TimeSpan(double tdiff)
		: m_hh(0), m_mm(0), m_ss(0), m_dv(tdiff)
	{
		if (tdiff == 0.0 || isnan(tdiff)) return;
		double m = (tdiff - std::floor(tdiff)) * 60.0;
		m_hh = Int(tdiff);
		double s = (m - std::floor(m)) * 60.0;
		m_mm = Int(m);
		if (s >= 59.5) { m_mm++; s -= 60.0; }
		if (m_mm >= 60) { m_hh++; m_mm -= 60; }
		m_ss = (int)std::round(s);
	}

	TimeSpan::TimeSpan()
		: m_hh(0), m_mm(0), m_ss(0), m_dv(0.0)
	{
	}

	TimeSpan::~TimeSpan() {}

	int TimeSpan::GetHour() { return m_hh; }
	int TimeSpan::GetMinute() { return m_mm; }
	int TimeSpan::GetSecond() { return m_ss; }

	float TimeSpan::GetTotalHour() { return (float)m_hh + ((float)m_mm + (float)m_ss / 60.0f) / 60.0f; }
	float TimeSpan::GetTotalMinute() { return (float)(m_hh * 60 + m_mm) + (float)m_ss / 60.0f; }
	float TimeSpan::GetTotalSecond() { return (float)((m_hh * 60 + m_mm) * 60 + m_ss); }

	double TimeSpan::GetFloat() { return m_dv; }
	TimeSpan::operator double() { return m_dv; }

	std::ostream& operator<<(std::ostream& os, const TimeSpan& ts)
	{
		if (ts.m_hh < 10) os << "0";
		os << ts.m_hh << ":";
		if (ts.m_mm < 10) os << "0";
		os << ts.m_mm << ":";
		if (ts.m_ss < 10) os << "0";
		os << ts.m_ss;
		return os;
	}

	SunMoon::SunMoon()
		: m_Lat(55.7), m_Lon(-12.5), m_Zone(1), m_DeltaT(65.0), m_JD(0.0), m_SunDistance(0.0), m_SunDistanceObserver(0.0), m_SunLon(0.0), m_SunDec(0.0), m_SunAz(0.0), m_SunAlt(0.0), m_SunDiameter(0.0), m_MoonDistance(0.0), m_MoonDistanceObserver(0.0), m_MoonLon(0.0), m_MoonLat(0.0), m_MoonDec(0.0), m_MoonAz(0.0), m_MoonAlt(0.0), m_MoonDiameter(0.0), m_MoonPhaseNumber(0.0), m_MoonAge(0.0)
	{
		time_t now = time(NULL);
#ifdef _MSC_VER
		struct tm gmt;
		memset(&gmt, 0, sizeof(struct tm));
		memset(&m_datumzeit, 0, sizeof(struct tm));
		localtime_s(&m_datumzeit, &now);
		gmtime_s(&gmt, &now);
#else
		m_datumzeit = *localtime(&now);
		struct tm gmt = *gmtime(&now);
#endif
		time_t utc = mktime(&gmt);
		if (gmt.tm_isdst == 1) utc -= 3600;
		m_Zone = (int)(difftime(now, utc) / 3600.0);
		SetInput(55.7, -12.5, &m_datumzeit, 65.0, m_Zone);
	}

	SunMoon::SunMoon(double latitude, double longitude, struct tm* datetime, double deltaT)
		: m_Lat(latitude), m_Lon(longitude), m_Zone(0), m_DeltaT(deltaT), m_JD(0.0), m_SunDistance(0.0), m_SunDistanceObserver(0.0), m_SunLon(0.0), m_SunDec(0.0), m_SunAz(0.0), m_SunAlt(0.0), m_SunDiameter(0.0), m_MoonDistance(0.0), m_MoonDistanceObserver(0.0), m_MoonLon(0.0), m_MoonLat(0.0), m_MoonDec(0.0), m_MoonAz(0.0), m_MoonAlt(0.0), m_MoonDiameter(0.0), m_MoonPhaseNumber(0.0), m_MoonAge(0.0)
	{
		SetInput(latitude, longitude, datetime, deltaT);
	}

	SunMoon::SunMoon(double latitude, double longitude, struct tm* datetime, double deltaT, int zone)
		: m_Lat(latitude), m_Lon(longitude), m_Zone(zone), m_DeltaT(deltaT), m_JD(0.0), m_SunDistance(0.0), m_SunDistanceObserver(0.0), m_SunLon(0.0), m_SunDec(0.0), m_SunAz(0.0), m_SunAlt(0.0), m_SunDiameter(0.0), m_MoonDistance(0.0), m_MoonDistanceObserver(0.0), m_MoonLon(0.0), m_MoonLat(0.0), m_MoonDec(0.0), m_MoonAz(0.0), m_MoonAlt(0.0), m_MoonDiameter(0.0), m_MoonPhaseNumber(0.0), m_MoonAge(0.0)
	{
		SetInput(latitude, longitude, datetime, deltaT, zone);
	}

	SunMoon::~SunMoon()
	{
	}

	void SunMoon::SetInput(double latitude, double longitude, struct tm* datetime, double deltaT)
	{
		SetInput(latitude, longitude, datetime, deltaT);
	}

	namespace Helper
	{
		double sqr(double x) { return x * x; }
		int Int(double x) { return (x < 0) ? (int)std::ceil(x) : (int)std::floor(x); }
		double frac(double x) { return (x - std::floor(x)); }
		double Mod(double a, double b) { return (a - std::floor(a / b) * b); }
		double Mod2Pi(double x) { return Mod(x, 2.0 * M_PI); }
		double round100000(double x) { return (std::round(100000.0 * x) / 100000.0); }
		double round10000(double x) { return (std::round(10000.0 * x) / 10000.0); }
		double round1000(double x) { return (std::round(1000.0 * x) / 1000.0); }
		double round100(double x) { return (std::round(100.0 * x) / 100.0); }
		double round10(double x) { return (std::round(10.0 * x) / 10.0); }
		SIGN Sign(double lon)
		{
			//char* signs[] = { "Widder", "Stier", "Zwillinge", "Krebs", "Löwe", "Jungfrau", "Waage", "Skorpion", "Schütze", "Steinbock", "Wassermann", "Fische" };
			return (SIGN)((int)std::floor(lon * RAD / 30.0));
		}

		// Calculate Julian date: valid only from 1.3.1901 to 28.2.2100
		double CalcJD(int day, int month, int year)
		{
			double jd = 2415020.5 - 64; // 1.1.1900 - correction of algorithm
			if (month <= 2) { year--; month += 12; }
			jd += Int(((year - 1900)) * 365.25);
			jd += Int(30.6001 * ((1 + month)));
			return jd + day;
		}

		// Julian Date to Greenwich Mean Sidereal Time
		double CalcGMST(double JD)
		{
			double UT = frac(JD - 0.5) * 24.0; // UT in hours
			JD = std::floor(JD - 0.5) + 0.5;   // JD at 0 hours UT
			double T = (JD - 2451545.0) / 36525.0;
			double T0 = 6.697374558 + T * (2400.051336 + T * 0.000025862);
			return (Mod(T0 + UT * 1.002737909, 24.0));
		}

		// Convert Greenweek mean sidereal time to UT
		double GMST2UT(double JD, double gmst)
		{
			JD = std::floor(JD - 0.5) + 0.5;   // JD at 0 hours UT
			double T = (JD - 2451545.0) / 36525.0;
			double T0 = Mod(6.697374558 + T * (2400.051336 + T * 0.000025862), 24.0);
			return 0.9972695663 * ((gmst - T0));
		}

		// Local Mean Sidereal Time, geographical longitude in radians, East is positive
		double GMST2LMST(double gmst, double lon) { return Mod(gmst + RAD * lon / 15, 24.0); }

		// Transform ecliptical coordinates (lon/lat) to equatorial coordinates (RA/dec)
		std::map<std::string, double> Ecl2Equ(std::map<std::string, double> coor, double TDT)
		{
			double T = (TDT - 2451545.0) / 36525.0; // Epoch 2000 January 1.5
			double eps = (23.0 + (26 + 21.45 / 60.0) / 60.0 + T * (-46.815 + T * (-0.0006 + T * 0.00181)) / 3600.0) * DEG;
			double coseps = std::cos(eps);
			double sineps = std::sin(eps);

			double sinlon = std::sin(coor["lon"]);
			coor["ra"] = Mod2Pi(std::atan2((sinlon * coseps - std::tan(coor["lat"]) * sineps), std::cos(coor["lon"])));
			coor["dec"] = std::asin(std::sin(coor["lat"]) * coseps + std::cos(coor["lat"]) * sineps * sinlon);

			return coor;
		}

		// Transform equatorial coordinates (RA/Dec) to horizonal coordinates (azimuth/altitude)
		// Refraction is ignored
		std::map<std::string, double> Equ2Altaz(std::map<std::string, double> coor, double TDT, double geolat, double lmst)
		{
			double cosdec = std::cos(coor["dec"]);
			double sindec = std::sin(coor["dec"]);
			double lha = lmst - coor["ra"];
			double coslha = std::cos(lha);
			double sinlha = std::sin(lha);
			double coslat = std::cos(geolat);
			double sinlat = std::sin(geolat);

			double N = -cosdec * sinlha;
			double D = sindec * coslat - cosdec * coslha * sinlat;
			coor["az"] = Mod2Pi(std::atan2(N, D));
			coor["alt"] = std::asin(sindec * sinlat + cosdec * coslha * coslat);

			return coor;
		}

		// Transform geocentric equatorial coordinates (RA/Dec) to topocentric equatorial coordinates
		std::map<std::string, double> GeoEqu2TopoEqu(std::map<std::string, double> coor, std::map<std::string, double> observer, double lmst)
		{
			double cosdec = std::cos(coor["dec"]);
			double sindec = std::sin(coor["dec"]);
			double coslst = std::cos(lmst);
			double sinlst = std::sin(lmst);
			double coslat = std::cos(observer["lat"]); // we should use geocentric latitude, not geodetic latitude
			double sinlat = std::sin(observer["lat"]);
			double rho = observer["radius"]; // observer-geocenter in Kilometer

			double x = coor["distance"] * cosdec * std::cos(coor["ra"]) - rho * coslat * coslst;
			double y = coor["distance"] * cosdec * std::sin(coor["ra"]) - rho * coslat * sinlst;
			double z = coor["distance"] * sindec - rho * sinlat;

			coor["distanceTopocentric"] = std::sqrt(x * x + y * y + z * z);
			coor["decTopocentric"] = std::asin(z / coor["distanceTopocentric"]);
			coor["raTopocentric"] = Mod2Pi(std::atan2(y, x));

			return coor;
		}

		// Calculate cartesian from polar coordinates
		std::map<std::string, double> EquPolar2Cart(double lon, double lat, double distance)
		{
			std::map<std::string, double> cart;
			double rcd = std::cos(lat) * distance;
			cart["x"] = rcd * std::cos(lon);
			cart["y"] = rcd * std::sin(lon);
			cart["z"] = distance * std::sin(lat);
			return cart;
		}

		// Calculate observers cartesian equatorial coordinates (x,y,z in celestial frame) 
		// from geodetic coordinates (longitude, latitude, height above WGS84 ellipsoid)
		// Currently only used to calculate distance of a body from the observer
		std::map<std::string, double> Observer2EquCart(double lon, double lat, double height, double gmst)
		{
			double flat = 298.257223563;        // WGS84 flatening of earth
			double aearth = 6378.137;           // GRS80/WGS84 semi major axis of earth ellipsoid
			std::map<std::string, double> cart;
			// Calculate geocentric latitude from geodetic latitude
			double co = std::cos(lat);
			double si = std::sin(lat);
			double fl = 1.0 - 1.0 / flat;
			fl = fl * fl;
			si = si * si;
			double u = 1.0 / std::sqrt(co * co + fl * si);
			double a = aearth * u + height;
			double b = aearth * fl * u + height;
			double radius = std::sqrt(a * a * co * co + b * b * si); // geocentric distance from earth center
			cart["y"] = std::acos(a * co / radius); // geocentric latitude, rad
			cart["x"] = lon; // longitude stays the same
			if (lat < 0.0) { cart["y"] = -cart["y"]; } // adjust sign
			cart = EquPolar2Cart(cart["x"], cart["y"], radius); // convert from geocentric polar to geocentric cartesian, with regard to Greenwich
			// rotate around earth's polar axis to align coordinate system from Greenwich to vernal equinox
			double x = cart["x"];
			double y = cart["y"];
			double rotangle = gmst / 24.0 * 2.0 * M_PI; // sideral time gmst given in hours. Convert to radians
			cart["x"] = x * std::cos(rotangle) - y * std::sin(rotangle);
			cart["y"] = x * std::sin(rotangle) + y * std::cos(rotangle);
			cart["radius"] = radius;
			cart["lon"] = lon;
			cart["lat"] = lat;
			return cart;
		}

		// Calculate coordinates for Sun
		// Coordinates are accurate to about 10s (right ascension) 
		// and a few minutes of arc (declination)
		std::map<std::string, double> SunPosition(double TDT, double geolat = NAN_DOUBLE, double lmst = NAN_DOUBLE)
		{
			double D = TDT - 2447891.5;

			double eg = 279.403303 * DEG;
			double wg = 282.768422 * DEG;
			double e = 0.016713;
			double a = 149598500; // km
			double diameter0 = 0.533128 * DEG; // angular diameter of Moon at a distance

			double MSun = 360 * DEG / 365.242191 * D + eg - wg;
			double nu = MSun + 360.0 * DEG / M_PI * e * std::sin(MSun);

			std::map<std::string, double> sunCoor;
			sunCoor["lon"] = Mod2Pi(nu + wg);
			sunCoor["lat"] = 0;
			sunCoor["anomalyMean"] = MSun;

			sunCoor["distance"] = (1 - sqr(e)) / (1 + e * std::cos(nu)); // distance in astronomical units
			sunCoor["diameter"] = diameter0 / sunCoor["distance"]; // angular diameter in radians
			sunCoor["distance"] = sunCoor["distance"] * a;                         // distance in km
			sunCoor["parallax"] = 6378.137 / sunCoor["distance"];  // horizonal parallax

			sunCoor = Ecl2Equ(sunCoor, TDT);

			// Calculate horizonal coordinates of sun, if geographic positions is given
			if (!isnan(geolat) && !isnan(lmst))
			{
				sunCoor = Equ2Altaz(sunCoor, TDT, geolat, lmst);
			}
			return sunCoor;
		}

		// Calculate data and coordinates for the Moon
		// Coordinates are accurate to about 1/5 degree (in ecliptic coordinates)
		std::map<std::string, double> MoonPosition(std::map<std::string, double> sunCoor, double TDT, std::map<std::string, double> observer = std::map<std::string, double>(), double lmst = NAN_DOUBLE)
		{
			double D = TDT - 2447891.5;

			// Mean Moon orbit elements as of 1990.0
			double l0 = 318.351648 * DEG;
			double P0 = 36.340410 * DEG;
			double N0 = 318.510107 * DEG;
			double i = 5.145396 * DEG;
			double e = 0.054900;
			double a = 384401; // km
			double diameter0 = 0.5181 * DEG; // angular diameter of Moon at a distance
			double parallax0 = 0.9507 * DEG; // parallax at distance a

			double l = 13.1763966 * DEG * D + l0;
			double MMoon = l - 0.1114041 * DEG * D - P0; // Moon's mean anomaly M
			double N = N0 - 0.0529539 * DEG * D;       // Moon's mean ascending node longitude
			double C = l - sunCoor["lon"];
			double Ev = 1.2739 * DEG * std::sin(2 * C - MMoon);
			double Ae = 0.1858 * DEG * std::sin(sunCoor["anomalyMean"]);
			double A3 = 0.37 * DEG * std::sin(sunCoor["anomalyMean"]);
			double MMoon2 = MMoon + Ev - Ae - A3;  // corrected Moon anomaly
			double Ec = 6.2886 * DEG * std::sin(MMoon2);  // equation of centre
			double A4 = 0.214 * DEG * std::sin(2 * MMoon2);
			double l2 = l + Ev + Ec - Ae + A4; // corrected Moon's longitude
			double V = 0.6583 * DEG * std::sin(2 * (l2 - sunCoor["lon"]));
			double l3 = l2 + V; // true orbital longitude;

			double N2 = N - 0.16 * DEG * std::sin(sunCoor["anomalyMean"]);

			std::map<std::string, double> moonCoor;
			moonCoor["lon"] = Mod2Pi(N2 + std::atan2(std::sin(l3 - N2) * std::cos(i), std::cos(l3 - N2)));
			moonCoor["lat"] = std::asin(std::sin(l3 - N2) * std::sin(i));
			moonCoor["orbitLon"] = l3;

			moonCoor = Ecl2Equ(moonCoor, TDT);
			// relative distance to semi mayor axis of lunar oribt
			moonCoor["distance"] = (1 - sqr(e)) / (1 + e * std::cos(MMoon2 + Ec));
			moonCoor["diameter"] = diameter0 / moonCoor["distance"]; // angular diameter in radians
			moonCoor["parallax"] = parallax0 / moonCoor["distance"]; // horizontal parallax in radians
			moonCoor["distance"] = moonCoor["distance"] * a; // distance in km

			// Calculate horizonal coordinates of sun, if geographic positions is given
			if (observer.size() > 0 && !isnan(lmst))
			{
				// transform geocentric coordinates into topocentric (==observer based) coordinates
				moonCoor = GeoEqu2TopoEqu(moonCoor, observer, lmst);
				moonCoor["raGeocentric"] = moonCoor["ra"]; // backup geocentric coordinates
				moonCoor["decGeocentric"] = moonCoor["dec"];
				moonCoor["ra"] = moonCoor["raTopocentric"];
				moonCoor["dec"] = moonCoor["decTopocentric"];
				moonCoor = Equ2Altaz(moonCoor, TDT, observer["lat"], lmst); // now ra and dec are topocentric
			}

			// Age of Moon in radians since New Moon (0) - Full Moon (pi)
			moonCoor["moonAge"] = Mod2Pi(l3 - sunCoor["lon"]);
			moonCoor["phase"] = 0.5 * (1 - std::cos(moonCoor["moonAge"])); // Moon phase, 0-1

			double mainPhase = 1.0 / 29.53 * 360 * DEG; // show 'Newmoon, 'Quarter' for +/-1 day arond the actual event
			double p = Mod(moonCoor["moonAge"], 90.0 * DEG);
			if (p < mainPhase || p > 90 * DEG - mainPhase) p = 2 * std::round(moonCoor["moonAge"] / (90.0 * DEG));
			else p = 2 * std::floor(moonCoor["moonAge"] / (90.0 * DEG)) + 1;
			moonCoor["moonPhase"] = (int)p;

			return moonCoor;
		}

		// Rough refraction formula using standard atmosphere: 1015 mbar and 10°C
		// Input true altitude in radians, Output: increase in altitude in degrees
		double Refraction(double alt)
		{
			double altdeg = alt * RAD;
			if (altdeg < -2 || altdeg >= 90) return 0.0;

			double pressure = 1015;
			double temperature = 10;
			if (altdeg > 15) return (0.00452 * pressure / ((273 + temperature) * std::tan(alt)));

			double y = alt;
			double D = 0.0;
			double P = (pressure - 80.0) / 930.0;
			double Q = 0.0048 * (temperature - 10.0);
			double y0 = y;
			double D0 = D;

			for (int i = 0; i < 3; i++)
			{
				double N = y + (7.31 / (y + 4.4));
				N = 1.0 / std::tan(N * DEG);
				D = N * P / (60.0 + Q * (N + 39.0));
				N = y - y0;
				y0 = D - D0 - N;
				if ((N != 0.0) && (y0 != 0.0)) { N = y - N * (alt + D - y) / y0; }
				else { N = alt + D; }
				y0 = y;
				D0 = D;
				y = N;
			}
			return D; // Hebung durch Refraktion in radians
		}

		// returns Greenwich sidereal time (hours) of time of rise 
		// and set of object with coordinates coor.ra/coor.dec
		// at geographic position lon/lat (all values in radians)
		// Correction for refraction and semi-diameter/parallax of body is taken care of in function RiseSet
		// h is used to calculate the twilights. It gives the required elevation of the disk center of the sun
		std::map<std::string, double> GMSTRiseSet(std::map<std::string, double> coor, double lon, double lat, double hn = NAN_DOUBLE)
		{
			double h = isnan(hn) ? 0.0: hn; // set default value
			std::map<std::string, double> riseset;
			//  double tagbogen = std::acos(-std::tan(lat)*std::tan(coor["dec"])); // simple formula if twilight is not required
			double tagbogen = std::acos((std::sin(h) - std::sin(lat) * std::sin(coor["dec"])) / (std::cos(lat) * std::cos(coor["dec"])));

			riseset["transit"] = RAD / 15 * (+coor["ra"] - lon);
			riseset["rise"] = 24.0 + RAD / 15 * (-tagbogen + coor["ra"] - lon); // calculate GMST of rise of object
			riseset["set"] = RAD / 15 * (+tagbogen + coor["ra"] - lon); // calculate GMST of set of object

			// using the modulo function Mod, the day number goes missing. This may get a problem for the moon
			riseset["transit"] = Mod(riseset["transit"], 24);
			riseset["rise"] = Mod(riseset["rise"], 24);
			riseset["set"] = Mod(riseset["set"], 24);

			return riseset;
		}

		// Find GMST of rise/set of object from the two calculates 
		// (start)points (day 1 and 2) and at midnight UT(0)
		double InterpolateGMST(double gmst0, double gmst1, double gmst2, double timefactor)
		{
			return ((timefactor * 24.07 * gmst1 - gmst0 * (gmst2 - gmst1)) / (timefactor * 24.07 + gmst1 - gmst2));
		}

		// JD is the Julian Date of 0h UTC time (midnight)
		std::map<std::string, double> RiseSet(double jd0UT, std::map<std::string, double> coor1, std::map<std::string, double> coor2, double lon, double lat, double timeinterval, double naltitude = NAN_DOUBLE)
		{
			// altitude of sun center: semi-diameter, horizontal parallax and (standard) refraction of 34'
			double alt = 0.0; // calculate 
			double altitude = isnan(naltitude) ? 0.0 : naltitude; // set default value

			// true height of sun center for sunrise and set calculation. Is kept 0 for twilight (ie. altitude given):
			if (altitude == 0.0) alt = 0.5 * coor1["diameter"] - coor1["parallax"] + 34.0 / 60 * DEG;

			std::map<std::string, double> rise1 = GMSTRiseSet(coor1, lon, lat, altitude);
			std::map<std::string, double> rise2 = GMSTRiseSet(coor2, lon, lat, altitude);

			std::map<std::string, double> rise;

			// unwrap GMST in case we move across 24h -> 0h
			if (rise1["transit"] > rise2["transit"] && std::abs(rise1["transit"] - rise2["transit"]) > 18) rise2["transit"] += 24.0;
			if (rise1["rise"] > rise2["rise"] && std::abs(rise1["rise"] - rise2["rise"]) > 18) rise2["rise"] += 24.0;
			if (rise1["set"] > rise2["set"] && std::abs(rise1["set"] - rise2["set"]) > 18) rise2["set"] += 24.0;
			double T0 = CalcGMST(jd0UT);
			//  var T02 = T0-zone*1.002738; // Greenwich sidereal time at 0h time zone (zone: hours)

			// Greenwich sidereal time for 0h at selected longitude
			double T02 = T0 - lon * RAD / 15 * 1.002738;
			if (T02 < 0) T02 += 24.0;

			if (rise1["transit"] < T02) { rise1["transit"] += 24.0; rise2["transit"] += 24.0; }
			if (rise1["rise"] < T02) { rise1["rise"] += 24.0; rise2["rise"] += 24.0; }
			if (rise1["set"] < T02) { rise1["set"] += 24.0; rise2["set"] += 24.0; }

			// Refraction and Parallax correction
			double decMean = 0.5 * (coor1["dec"] + coor2["dec"]);
			double psi = std::acos(std::sin(lat) / std::cos(decMean));
			double y = std::asin(std::sin(alt) / std::sin(psi));
			double dt = 240 * RAD * y / std::cos(decMean) / 3600; // time correction due to refraction, parallax

			rise["transit"] = GMST2UT(jd0UT, InterpolateGMST(T0, rise1["transit"], rise2["transit"], timeinterval));
			rise["rise"] = GMST2UT(jd0UT, InterpolateGMST(T0, rise1["rise"], rise2["rise"], timeinterval) - dt);
			rise["set"] = GMST2UT(jd0UT, InterpolateGMST(T0, rise1["set"], rise2["set"], timeinterval) + dt);

			return (rise);
		}

		// Find (local) time of sunrise and sunset, and twilights
		// JD is the Julian Date of 0h local time (midnight)
		// Accurate to about 1-2 minutes
		// recursive: 1 - calculate rise/set in UTC in a second run
		// recursive: 0 - find rise/set on the current local day. This is set when doing the first call to this function
		std::map<std::string, double> CalcSunRise(double JD, double deltaT, double lon, double lat, int zone, bool recursive)
		{
			double jd0UT = std::floor(JD - 0.5) + 0.5;   // JD at 0 hours UT
			std::map<std::string, double> coor1 = SunPosition(jd0UT + deltaT / 24.0 / 3600.0);
			std::map<std::string, double> coor2 = SunPosition(jd0UT + 1.0 + deltaT / 24.0 / 3600.0); // calculations for next day's UTC midnight

			std::map<std::string, double> risetemp;
			// rise/set time in UTC. 
			std::map<std::string, double> rise = RiseSet(jd0UT, coor1, coor2, lon, lat, 1);
			if (!recursive)
			{ // check and adjust to have rise/set time on local calendar day
				if (zone > 0)
				{
					// rise time was yesterday local time -> calculate rise time for next UTC day
					if (rise["rise"] >= 24 - zone || rise["transit"] >= 24 - zone || rise["set"] >= 24 - zone)
					{
						risetemp = CalcSunRise(JD + 1, deltaT, lon, lat, zone, true);
						if (rise["rise"] >= 24 - zone) rise["rise"] = risetemp["rise"];
						if (rise["transit"] >= 24 - zone) rise["transit"] = risetemp["transit"];
						if (rise["set"] >= 24 - zone) rise["set"] = risetemp["set"];
					}
				}
				else if (zone < 0)
				{
					// rise time was yesterday local time -> calculate rise time for next UTC day
					if (rise["rise"] < -zone || rise["transit"] < -zone || rise["set"] < -zone)
					{
						risetemp = CalcSunRise(JD - 1, deltaT, lon, lat, zone, true);
						if (rise["rise"] < -zone) rise["rise"] = risetemp["rise"];
						if (rise["transit"] < -zone) rise["transit"] = risetemp["transit"];
						if (rise["set"] < -zone) rise["set"] = risetemp["set"];
					}
				}

				rise["transit"] = Mod(rise["transit"] + zone, 24.0);
				rise["rise"] = Mod(rise["rise"] + zone, 24.0);
				rise["set"] = Mod(rise["set"] + zone, 24.0);

				// Twilight calculation
				// civil twilight time in UTC. 
				risetemp = RiseSet(jd0UT, coor1, coor2, lon, lat, 1, -6.0 * DEG);
				rise["cicilTwilightMorning"] = Mod(risetemp["rise"] + zone, 24.0);
				rise["cicilTwilightEvening"] = Mod(risetemp["set"] + zone, 24.0);

				// nautical twilight time in UTC. 
				risetemp = RiseSet(jd0UT, coor1, coor2, lon, lat, 1, -12.0 * DEG);
				rise["nauticalTwilightMorning"] = Mod(risetemp["rise"] + zone, 24.0);
				rise["nauticalTwilightEvening"] = Mod(risetemp["set"] + zone, 24.0);

				// astronomical twilight time in UTC. 
				risetemp = RiseSet(jd0UT, coor1, coor2, lon, lat, 1, -18.0 * DEG);
				rise["astronomicalTwilightMorning"] = Mod(risetemp["rise"] + zone, 24.0);
				rise["astronomicalTwilightEvening"] = Mod(risetemp["set"] + zone, 24.0);
			}
			return rise;
		}

		// Find local time of moonrise and moonset
		// JD is the Julian Date of 0h local time (midnight)
		// Accurate to about 5 minutes or better
		// recursive: 1 - calculate rise/set in UTC
		// recursive: 0 - find rise/set on the current local day (set could also be first)
		// returns '' for moonrise/set does not occur on selected day
		std::map<std::string, double> CalcMoonRise(double JD, double deltaT, double lon, double lat, int zone, bool recursive)
		{
			double timeinterval = 0.5;
			double jd0UT = std::floor(JD - 0.5) + 0.5;   // JD at 0 hours UT
			std::map<std::string, double> suncoor1 = SunPosition(jd0UT + deltaT / 24.0 / 3600.0);
			std::map<std::string, double> coor1 = MoonPosition(suncoor1, jd0UT + deltaT / 24.0 / 3600.0);

			std::map<std::string, double> suncoor2 = SunPosition(jd0UT + timeinterval + deltaT / 24.0 / 3600.0); // calculations for noon
			// calculations for next day's midnight
			std::map<std::string, double> coor2 = MoonPosition(suncoor2, jd0UT + timeinterval + deltaT / 24.0 / 3600.0);

			std::map<std::string, double> risetemp;

			// rise/set time in UTC, time zone corrected later.
			// Taking into account refraction, semi-diameter and parallax
			std::map<std::string, double> rise = RiseSet(jd0UT, coor1, coor2, lon, lat, timeinterval);

			if (!recursive)
			{ // check and adjust to have rise/set time on local calendar day
				if (zone > 0)
				{
					// recursive call to MoonRise returns events in UTC
					risetemp = CalcMoonRise(JD - 1.0, deltaT, lon, lat, zone, true);
					if (rise["transit"] >= 24.0 - zone || rise["transit"] < -zone)
					{ // transit time is tomorrow local time
						if (risetemp["transit"] < 24.0 - zone) rise["transit"] = NAN_DOUBLE; // there is no moontransit today
						else rise["transit"] = risetemp["transit"];
					}

					if (rise["rise"] >= 24.0 - zone || rise["rise"] < -zone)
					{ // rise time is tomorrow local time
						if (risetemp["rise"] < 24.0 - zone) rise["rise"] = NAN_DOUBLE; // there is no moontransit today
						else rise["rise"] = risetemp["rise"];
					}

					if (rise["set"] >= 24.0 - zone || rise["set"] < -zone)
					{ // set time is tomorrow local time
						if (risetemp["set"] < 24.0 - zone) rise["set"] = NAN_DOUBLE; // there is no moontransit today
						else rise["set"] = risetemp["set"];
					}

				}
				else if (zone < 0)
				{
					// rise/set time was tomorrow local time -> calculate rise time for former UTC day
					if (rise["rise"] < -zone || rise["set"] < -zone || rise["transit"] < -zone)
					{
						risetemp = CalcMoonRise(JD + 1.0, deltaT, lon, lat, zone, true);

						if (rise["rise"] < -zone)
						{
							if (risetemp["rise"] > -zone) rise["rise"] = NAN_DOUBLE; // there is no moonrise today
							else rise["rise"] = risetemp["rise"];
						}

						if (rise["transit"] < -zone)
						{
							if (risetemp["transit"] > -zone) rise["transit"] = NAN_DOUBLE; // there is no moonset today
							else rise["transit"] = risetemp["transit"];
						}

						if (rise["set"] < -zone)
						{
							if (risetemp["set"] > -zone) rise["set"] = NAN_DOUBLE; // there is no moonset today
							else rise["set"] = risetemp["set"];
						}

					}
				}

				if (rise["rise"] != NAN_DOUBLE) rise["rise"] = Mod(rise["rise"] + zone, 24.0);    // correct for time zone, if time is valid
				if (rise["transit"] != NAN_DOUBLE) rise["transit"] = Mod(rise["transit"] + zone, 24.0); // correct for time zone, if time is valid
				if (rise["set"] != NAN_DOUBLE) rise["set"] = Mod(rise["set"] + zone, 24.0);    // correct for time zone, if time is valid
			}
			return rise;
		}
	}

	void SunMoon::SetInput(double latitude, double longitude, struct tm* datetime, double deltaT, int zone)
	{
		m_Lat = latitude;
		m_Lon = longitude;
		m_Zone = zone;
		m_DeltaT = deltaT;
		memcpy(&m_datumzeit, datetime, sizeof(struct tm));

		double JD0 = Helper::CalcJD(m_datumzeit.tm_mday, m_datumzeit.tm_mon + 1, m_datumzeit.tm_year + 1900);
		double jd = JD0 + (m_datumzeit.tm_hour - m_Zone + m_datumzeit.tm_min / 60.0 + m_datumzeit.tm_sec / 3600.0) / 24.0;
		double TDT = jd + m_DeltaT / 24.0 / 3600.0;
		double lat = m_Lat * DEG; // geodetic latitude of observer on WGS84
		double lon = m_Lon * DEG; // latitude of observer
		double height = 0 * 0.001; // altiude of observer in meters above WGS84 ellipsoid (and converted to kilometers)

		double gmst = Helper::CalcGMST(jd);
		double lmst = Helper::GMST2LMST(gmst, lon);

		std::map<std::string, double> observerCart = Helper::Observer2EquCart(lon, lat, height, gmst); // geocentric cartesian coordinates of observer

		std::map<std::string, double> sunCoor = Helper::SunPosition(TDT, lat, lmst * 15.0 * DEG);   // Calculate data for the Sun at given time
		std::map<std::string, double> moonCoor = Helper::MoonPosition(sunCoor, TDT, observerCart, lmst * 15.0 * DEG);    // Calculate data for the Moon at given time

		m_JD = Helper::round100000(jd);
		m_GMST = TimeSpan(gmst);
		m_LMST = TimeSpan(lmst);

		m_SunLon = Helper::round1000(sunCoor["lon"] * RAD);
		m_SunRA = TimeSpan(sunCoor["ra"] * RAD / 15);
		m_SunDec = Helper::round1000(sunCoor["dec"] * RAD);
		m_SunAz = Helper::round100(sunCoor["az"] * RAD);
		m_SunAlt = Helper::round10(sunCoor["alt"] * RAD + Helper::Refraction(sunCoor["alt"]));  // including refraction

		m_SunSign = Helper::Sign(sunCoor["lon"]);
		m_SunDiameter = Helper::round100(sunCoor["diameter"] * RAD * 60.0); // angular diameter in arc seconds
		m_SunDistance = Helper::round10(sunCoor["distance"]);

		// Calculate distance from the observer (on the surface of earth) to the center of the sun
		std::map<std::string, double> sunCart = Helper::EquPolar2Cart(sunCoor["ra"], sunCoor["dec"], sunCoor["distance"]);
		m_SunDistanceObserver = Helper::round10(std::sqrt(Helper::sqr(sunCart["x"] - observerCart["x"]) + Helper::sqr(sunCart["y"] - observerCart["y"]) + Helper::sqr(sunCart["z"] - observerCart["z"])));

		// JD0: JD of 0h UTC time
		std::map<std::string, double> sunRise = Helper::CalcSunRise(JD0, m_DeltaT, lon, lat, m_Zone, false);

		m_SunTransit = TimeSpan(sunRise["transit"]);
		m_SunRise = TimeSpan(sunRise["rise"]);
		m_SunSet = TimeSpan(sunRise["set"]);

		m_SunCivilTwilightMorning = TimeSpan(sunRise["cicilTwilightMorning"]);
		m_SunCivilTwilightEvening = TimeSpan(sunRise["cicilTwilightEvening"]);
		m_SunNauticalTwilightMorning = TimeSpan(sunRise["nauticalTwilightMorning"]);
		m_SunNauticalTwilightEvening = TimeSpan(sunRise["nauticalTwilightEvening"]);
		m_SunAstronomicalTwilightMorning = TimeSpan(sunRise["astronomicalTwilightMorning"]);
		m_SunAstronomicalTwilightEvening = TimeSpan(sunRise["astronomicalTwilightEvening"]);

		m_MoonLon = Helper::round1000(moonCoor["lon"] * RAD);
		m_MoonLat = Helper::round1000(moonCoor["lat"] * RAD);
		m_MoonRA = TimeSpan(moonCoor["ra"] * RAD / 15.0);
		m_MoonDec = Helper::round1000(moonCoor["dec"] * RAD);
		m_MoonAz = Helper::round100(moonCoor["az"] * RAD);
		m_MoonAlt = Helper::round10(moonCoor["alt"] * RAD + Helper::Refraction(moonCoor["alt"]));  // including refraction
		m_MoonAge = Helper::round1000(moonCoor["moonAge"] * RAD);
		m_MoonPhaseNumber = Helper::round1000(moonCoor["phase"]);
		//char* phases[] = { "Neumond", "Zunehmende Sichel", "Erstes Viertel", "Zunehmender Mond", "Vollmond", "Abnehmender Mond", "Letztes Viertel", "Abnehmende Sichel", "Neumond" };
		int phase = (int)moonCoor["moonPhase"];
		if (phase == 8) phase = 0;
		m_MoonPhase = (LUNARPHASE)phase;

		m_MoonSign = Helper::Sign(moonCoor["lon"]);
		m_MoonDistance = Helper::round10(moonCoor["distance"]);
		m_MoonDiameter = Helper::round100(moonCoor["diameter"] * RAD * 60.0); // angular diameter in arc seconds

		// Calculate distance from the observer (on the surface of earth) to the center of the moon
		std::map<std::string, double> moonCart = Helper::EquPolar2Cart(moonCoor["raGeocentric"], moonCoor["decGeocentric"], moonCoor["distance"]);
		m_MoonDistanceObserver = Helper::round10(std::sqrt(Helper::sqr(moonCart["x"] - observerCart["x"]) + Helper::sqr(moonCart["y"] - observerCart["y"]) + Helper::sqr(moonCart["z"] - observerCart["z"])));

		std::map<std::string, double> moonRise = Helper::CalcMoonRise(JD0, m_DeltaT, lon, lat, m_Zone, false);

		m_MoonTransit = TimeSpan(moonRise["transit"]);
		m_MoonRise = TimeSpan(moonRise["rise"]);
		m_MoonSet = TimeSpan(moonRise["set"]);
		std::cout << "AZ:   "<< m_MoonAz << "\n";
		moonPosAz = m_MoonAz;
		std::cout << "Alt:   " << m_MoonAlt << "\n";
		moonPosAlt = m_MoonAlt;
		/*std::cout << "lon:  " << m_Lon << "\n";
		std::cout << "lat:  " << m_Lat << "\n";
		std::cout << "DateTime:   " << m_Zone << "\n";
		std::cout << "RA:   " << m_MoonRA << "\n";
		std::cout << "Dec:   " << m_MoonDec << "\n";*/
	}

	double SunMoon::GetLat() { return m_Lat; }
	double SunMoon::GetLon() { return m_Lon; }
	struct tm SunMoon::GetDateTime() { return m_datumzeit; }
	double SunMoon::GetZone() { return m_Zone; }
	double SunMoon::GetDeltaT() { return m_DeltaT; }
	double SunMoon::GetJD() { return m_JD; }
	TimeSpan SunMoon::GetGMST() { return m_GMST; }
	TimeSpan SunMoon::GetLMST() { return m_LMST; }
	double SunMoon::GetSunDistance() { return m_SunDistance; }
	double SunMoon::GetSunDistanceObserver() { return m_SunDistanceObserver; }
	double SunMoon::GetSunLon() { return m_SunLon; }
	TimeSpan SunMoon::GetSunRA() { return m_SunRA; }
	double SunMoon::GetSunDec() { return m_SunDec; }
	double SunMoon::GetSunAz() { return m_SunAz; }
	double SunMoon::GetSunAlt() { return m_SunAlt; }
	double SunMoon::GetSunDiameter() { return m_SunDiameter; }
	TimeSpan SunMoon::GetSunAstronomicalTwilightMorning() { return m_SunAstronomicalTwilightMorning; }
	TimeSpan SunMoon::GetSunNauticalTwilightMorning() { return m_SunNauticalTwilightMorning; }
	TimeSpan SunMoon::GetSunCivilTwilightMorning() { return m_SunCivilTwilightMorning; }
	TimeSpan SunMoon::GetSunRise() { return m_SunRise; }
	TimeSpan SunMoon::GetSunTransit() { return m_SunTransit; }
	TimeSpan SunMoon::GetSunSet() { return m_SunSet; }
	TimeSpan SunMoon::GetSunCivilTwilightEvening() { return m_SunCivilTwilightEvening; }
	TimeSpan SunMoon::GetSunNauticalTwilightEvening() { return m_SunNauticalTwilightEvening; }
	TimeSpan SunMoon::GetSunAstronomicalTwilightEvening() { return m_SunAstronomicalTwilightEvening; }
	SIGN SunMoon::GetSunSign() { return m_SunSign; }
	double SunMoon::GetMoonDistance() { return m_MoonDistance; }
	double SunMoon::GetMoonDistanceObserver() { return m_MoonDistanceObserver; }
	double SunMoon::GetMoonLon() { return m_MoonLon; }
	double SunMoon::GetMoonLat() { return m_MoonLat; }
	TimeSpan SunMoon::GetMoonRA() { return m_MoonRA; }
	double SunMoon::GetMoonDec() { return m_MoonDec; }
	double SunMoon::GetMoonAz() { return m_MoonAz; }
	double SunMoon::GetMoonAlt() { return m_MoonAlt; }
	double SunMoon::GetMoonDiameter() { return m_MoonDiameter; }
	TimeSpan SunMoon::GetMoonRise() { return m_MoonRise; }
	TimeSpan SunMoon::GetMoonTransit() { return m_MoonTransit; }
	TimeSpan SunMoon::GetMoonSet() { return m_MoonSet; }
	double SunMoon::GetMoonPhaseNumber() { return m_MoonPhaseNumber; }
	double SunMoon::GetMoonAge() { return m_MoonAge; }
	LUNARPHASE SunMoon::GetMoonPhase() { return m_MoonPhase; }
	SIGN SunMoon::GetMoonSign() { return m_MoonSign; }

	std::ostream& operator<<(std::ostream& os, const SunMoon& sm)
	{
		os << "Östl. geografische Länge: " << sm.m_Lat << " Grad" << std::endl;
		os << "Geografische Breite: " << sm.m_Lon << " Grad" << std::endl;
		os << "Datum/Zeit: ";
		if (sm.m_datumzeit.tm_mday < 10) os << "0";
		os << sm.m_datumzeit.tm_mday << ".";
		if (sm.m_datumzeit.tm_mon < 9) os << "0";
		os << (sm.m_datumzeit.tm_mon + 1) << "." << (sm.m_datumzeit.tm_year + 1900) << " ";
		if (sm.m_datumzeit.tm_hour < 10) os << "0";
		os << sm.m_datumzeit.tm_hour << ":";
		if (sm.m_datumzeit.tm_min < 10) os << "0";
		os << sm.m_datumzeit.tm_min << ":";
		if (sm.m_datumzeit.tm_sec < 10) os << "0";
		os << sm.m_datumzeit.tm_sec << std::endl;
		os << "Zeitdifferenz zu Weltzeit: " << sm.m_Zone << " h" << std::endl;
		os << "deltaT: " << sm.m_DeltaT << " sek" << std::endl;
		os << "Julianisches Datum: " << sm.m_JD << " Tage" << std::endl;
		os << "Greenwich Sternzeit GMST: " << sm.m_GMST << " h" << std::endl;
		os << "Lokale Sternzeit LMST: " << sm.m_LMST << " h" << std::endl;
		os << "Entfernung der Sonne (Erdmittelpunkt): " << sm.m_SunDistance << " km" << std::endl;
		os << "Entfernung der Sonne (vom Beobachter): " << sm.m_SunDistanceObserver << " km" << std::endl;
		os << "Eklipt. Länge der Sonne: " << sm.m_SunLon << " Grad" << std::endl;
		os << "Rektaszension der Sonne: " << sm.m_SunRA << " h" << std::endl;
		os << "Deklination der Sonne: " << sm.m_SunDec << " Grad" << std::endl;
		os << "Azimut der Sonne: " << sm.m_SunAz << " Grad" << std::endl;
		os << "Höhe der Sonne über Horizont: " << sm.m_SunAlt << " Grad" << std::endl;
		os << "Durchmesser der Sonne: " << sm.m_SunDiameter << " '" << std::endl;
		os << "Astronomische Morgendämmerung: " << sm.m_SunAstronomicalTwilightMorning << " h" << std::endl;
		os << "Nautische Morgendämmerung: " << sm.m_SunNauticalTwilightMorning << " h" << std::endl;
		os << "Bürgerliche Morgendämmerung: " << sm.m_SunCivilTwilightMorning << " h" << std::endl;
		os << "Sonnenaufgang: " << sm.m_SunRise << " h" << std::endl;
		os << "Sonnenkulmination: " << sm.m_SunTransit << " h" << std::endl;
		os << "Sonnenuntergang: " << sm.m_SunSet << " h" << std::endl;
		os << "Bürgerliche Abenddämmerung: " << sm.m_SunCivilTwilightEvening << " h" << std::endl;
		os << "Nautische Abenddämmerung: " << sm.m_SunNauticalTwilightEvening << " h" << std::endl;
		os << "Astronomische Abenddämmerung: " << sm.m_SunAstronomicalTwilightEvening << " h" << std::endl;
		const char* signs[12];
		os << "Tierkreiszeichen: " << signs[(int)sm.m_SunSign] << std::endl;
		os << "Entfernung des Mondes (Erdmittelpunkt): " << sm.m_MoonDistance << " km" << std::endl;
		os << "Entfernung des Mondes (vom Beobachter): " << sm.m_MoonDistanceObserver << " km" << std::endl;
		os << "Eklipt. Länge des Mondes: " << sm.m_MoonLon << " Grad" << std::endl;
		os << "Eklipt. Breite des Mondes: " << sm.m_MoonLat << " Grad" << std::endl;
		os << "Rektaszension des Mondes: " << sm.m_MoonRA << " h" << std::endl;
		os << "Deklination des Mondes: " << sm.m_MoonDec << " Grad" << std::endl;
		os << "Azimut des Mondes: " << sm.m_MoonAz << " Grad" << std::endl;
		os << "Höhe des Mondes über Horizont: " << sm.m_MoonAlt << " Grad" << std::endl;
		os << "Durchmesser des Mondes: " << sm.m_MoonDiameter << " '" << std::endl;
		os << "Mondaufgang: " << sm.m_MoonRise << " h" << std::endl;
		os << "Mondkulmination: " << sm.m_MoonTransit << " h" << std::endl;
		os << "Monduntergang: " << sm.m_MoonSet << " h" << std::endl;
		os << "Mondphase: " << sm.m_MoonPhaseNumber << std::endl;
		os << "Mondalter: " << sm.m_MoonAge << " Grad" << std::endl;
		const char* phases[8] = {};
		os << "Mondphase: " << phases[(int)sm.m_MoonPhase] << std::endl;
		os << "Mondzeichen: " << signs[(int)sm.m_MoonSign] << std::endl;
		return os;
	}
}
